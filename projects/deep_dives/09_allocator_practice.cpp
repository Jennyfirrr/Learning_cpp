//=================================================================================
// [TAGS] [for grep searching later]
//=================================================================================
// [de-bruijn] [PDEP , PEXT] [pool allocator , arena allocator] [imul] [tzcnt
// bsf] [g++ -O3 -S -o] [java-bad]
//=================================================================================
// included libraries
//=================================================================================
#include <cstdint>
#include <immintrin.h>
#include <iostream>
#include <random>
#include <x86intrin.h>

static std::mt19937_64 rng(123456);

//=================================================================================
// [GOALS and PLAN]
//=================================================================================
// basic idea behind this, use a pool allocator or the arena allocator, to track
// placed orders, using the 64 bit order packing w/ 4 buy and 4 sell orders at a
// time, instead of pre-generating orders, use a while loop and continuously
// feeding random numbers through, to analyze branching, once the allocator
// fills up, create an order manifest, then clear, and continue with updating
// order counts in terminal, take advantage of the toptimzations using the
// _pdep_u64() and _pext_u64 or _pext_u32() to count, also using popcount, plus
// better de-bruijn masking instead of the way i did it by shifting 8 every
// order in the last file, the compiler optimized it to multiplication by a
// magic number, ((1 << 8) + (1 << 16)), but i wanna try to manually implement
// that, this is just the plan for this file
//
// gonna try to avoid @PLT instructions, as well as using the tzcnt/bsf if
// needed, basic order booking and stuff is kinda down mostly solid, using for
// loops to build them, so more practice using the de-bruijn techniques is
// needed, for example the order books could be built in at minimum 6 cycles,
// using 2 imul instructions and converting the buy/sell orders into 32bit
// integers before packing, so that will be a goal for this as well, and
// optimizing the risk gate build furter using these same techniques
//=================================================================================
// [FUNCTIONS]
//=================================================================================
// [ORDER PACKING]
//=================================================================================
// so for this function i was kinda thinking about it some, and like the example
// for broadcasting this into multiple lanes at once with a single instruction
// may not work, since its technically gonna be ingesting 8 "uinque" order id's,
// so multiplying by like 0x0101010101010101, may be the correct approach here,
// because its technicaly multiplying each individual byte by 1, or at least
// thats what this means to me, one approach i referenced above is basically
// using two imul instructions, to pack a uint64_t, by doing two seperate
// uint32_t which is the imul instrucitons, and then using two seeprate
// multiplication paths like 0x0000000001010101 for the right half, and then
// 0x0101010100000000, for the left half, because i think this should basically
// work, but idk, i have another constant in the 07 file, thats for something
// similar i think, im just kinda thinking before actually coding, one think
// that was pointed out was instead of using multiplication, i could tehcnically
// just do this using a single ^ operator, im trying to avoid using PDEP and
// PEXT right now because those kinda take away understanding from the actual
// bit manipulation, even if they are technically faster, and the 2 instruction
// thing with the bit multiplication would essentially use 2 imulq instrucionts
// im pretty sure, for like ~6 cycles for the actual bit packing logic, so the
// real issue becomes packing the actual order ID's without using a for loop,
// because im kinda feeding them in one at a time, but this may be why FPGA's
// were developed to handle all this in like 3-4 instruction cycles, maybe i
// should look into multithreading for this, because a single core is sequential
// processing, whereas if you could orchestrate say like 8 cores into this
// operation, then you could do it all in a single clock cycle theoretically,
// hmmm, but then that gets to race conditions, apparently this introduces more
// overhead, like ~5000 cycles, so thats a no go, SIMD wins again, back to
// thinking, and then ill start actually coding i guess, there is a chance that
// by using 2 for loops the compiler unrolls them, its what would essentially be
// like, ~8 instructions total? so that would be acceptable because they would
// be predictable enough to probably avoid jumps, this way probably seperates
// the need to use a nested for loop, so it becomes O(n) instead of O(n^2), or
// something like that, where you would have a single cycle per order id,
// because we can just take advantage of shifting, using this method, 2 order
// streams could be used as well, so i think ill do that for things
//=================================================================================
// [EDIT [01-03-26 05:04am]]
//=================================================================================
// *sparkle emoji*S T R U C T S*sparkle emoji*, because this way each struct
// will fit in a single e** register, and avoid the heap and the need for
// storing orders in slower memory, because registers are even faster than the
// stack
//=================================================================================
// [EDIT [01-03-26 07:50pm]]
//=================================================================================
// so to actually use the struct you wanna do something like this:
//
// OrderPair = {
// 		{10, 20, 30, 40},
// 		{50, 60, 70, 80}
// }
//=================================================================================
// [ACTUAL CODE [FUNCTIONS]]
//=================================================================================
// [ORDER PACKING] [TAG-order_packing]
//=================================================================================
// below is an example of GOOD code, this is GOOD code because the 34ish lines
// you see below this, compile down to a single movq %rdi, rax -> ret
// instruction, its literally just a proof for the compiler so the compiler can
// be like, yup this is true, no further work needed, idk wtf the makers of java
// were thinking because this is the peak of computer science, literally a
// SINGLE INSTRUCTION, the fastest code is the code you never write lmao, i
// actually understand that now, like idk wtf java is fucking doing, like youve
// got the JIT(java is trash compiler) the JVM(i cant think of how to make this
// say java is bad tbh, maybe someone can help me out with that, my brain is  T
// O A S T E D, after discovering this converted to a single instruction today),
// like 40 lines of code to print 'Hello world', and c++ is like, hey, you like
// performance? OH WE GOT PERFORMANCE, just tell us how your data structure is
// true, and we gotchu
//=================================================================================

struct buy_side {
  uint8_t order0b;
  uint8_t order1b;
  uint8_t order2b;
  uint8_t order3b;
};
static_assert(sizeof(buy_side) == 4, "struct must be 4 bytes");

struct sell_side {
  uint8_t order0s;
  uint8_t order1s;
  uint8_t order2s;
  uint8_t order3s;
};
static_assert(sizeof(sell_side) == 4, "struct must be 4 bytes");

struct OrderPair {
  buy_side buys;
  sell_side sells;
};
static_assert(sizeof(OrderPair) == 8, "struct must be 8 bytes");

uint64_t order_packing_8byte(OrderPair pair) {
  uint64_t packed_orders = 0;

  packed_orders |= static_cast<uint64_t>(pair.buys.order0b);
  packed_orders |= static_cast<uint64_t>(pair.buys.order1b) << 8;
  packed_orders |= static_cast<uint64_t>(pair.buys.order2b) << 16;
  packed_orders |= static_cast<uint64_t>(pair.buys.order3b) << 24;
  packed_orders |= static_cast<uint64_t>(pair.sells.order0s) << 32;
  packed_orders |= static_cast<uint64_t>(pair.sells.order1s) << 40;
  packed_orders |= static_cast<uint64_t>(pair.sells.order2s) << 48;
  packed_orders |= static_cast<uint64_t>(pair.sells.order3s) << 56;

  return packed_orders;
}
//=================================================================================
// this is fucking cursed LOL, the compiler likes this more though, and theres
// no risk of jumps, you can tell by how this would get you put on a PIP at a
// normal dev job, this is faster though lol, like 24 -> 18 instrucitons vs 2
// for loops, and probably like 10 -> 8 clock cycles
//=================================================================================
//[POOL ALLOCATOR] [TAG-pool_allocator]
//=================================================================================
// first define the struct that holds the information for the orderpool, example
// usage:
//
// OrderPool pool;
// OrderPool_Init(&pool, 64);
//
// uint64_t* slot = OrderPool_Allocate(&pool);
// *slot = order_packing_8byte(buy_orders, sell_orders);
//
// uint32_t active = OrderPool_CountActive(&pool);
//
// OrderPool_Free(&pool, slot);
//
// i have no idea how to actually implement this btw lol, this is all new to me,
// but idk, maybe this is how i learn? i have like 0 hope for myself and i
// constantly question why citadel and HRT actually sent my exams lol, like im
// stoopid, but anyways, my curiosity wont let me stop so we doin this tonight,
// ive never done this stuff before so its probably gonna be me using alot of
// hand holding while i actually learn this lol, remember, im just a girl, and a
// stupid one at that
//
// EDIT: I may not have what it takes to be a cs major lmao, this is so hard, i
// feel like i should just intuitively know this stuff like i have 30 years of
// experience in SWE and hardware design, apparnetly i can write papers on this
// stuff, but i cant actually code, makes sense though because im bad at this
//=================================================================================
// [POST IMPLEMENTATION]
//=================================================================================
// so i guess this was simpler than i thought, and i just like freak out when
// learning new things but w/e, weve all been there, anyways, i guess you define
// the actual slots within a struct, then you assign the bitmap using the same
// integer as the slots, and then you use a 32bit int to ttrack the capacity,
// because you arnt really using that to track anything else using bits, its
// just a normal int just like me,
//
// the pool->slots is new syntax to me, but i also havent used structs much so i
// guess im learning something new, writing about this stuff was easier than
// this lol, but ive decided to have a vendetta against *sparkle emoji*J A V
// A*sparkle emoji* so here we are, i guess that syntax is just declaring that
// its the actual thing from the struct, instead of like just declaring it like
// uint32_t or something, because the compiler likes structs for assigning
// custom tags to variables, i still have no clue how to actually use this with
// the order packing from above but im sure ill figure it out at some point, as
// far as im aware, you woul first wanna generate the packed order, then
// afterwards assign it to the OrderPool? idk, usage:
//
// 1. grab a slot
// uint64_t *slot = OrderPool_Allocate(&pool);
//
// 2. pack orders and store in that slot
// *slot = order_packing_8byte(buy_side_orders, sell_side_orders);
//
// that kinda makes sense, the *slot just derefernces the pointer, "write this
// value into the location the slot points to", the pool gives you where to put
// it, and the packing function gives you what to put there, to read it back
// later, you just wanna do something like:
//
// uint64_t packed = *slot;
//
// so its just a slightly more complex way of shifting variables around i guess,
// BUT HEY WE LEARNED TO USE CALLOC TODAY, AND POOL ALLOCATORS ARNT SCARY
// ANYMORE, how about that, i couldnt really find a way to fit a playing with
// your bits pun in this, but hey, this is WAY more interesting than the
// language that shall not be named right? i think something
//
// EDIT: this may not even be needed for this, but i wanna get used to these
// patterns anyways, and i hate leetcode, id rather write 20k words for a
// technical essay about hot path optimization than sit down and do a leetcode
// medium for 15 minutes, but i guess we all have to do things we dont like
//
// EDIT2: N E W  U S A G E:
//
// OrderPair *slot = OrderPool_Allocate(&pool);
// *slot = my_order_pair;
// uint64_t packed = order_packing_8byte(*slot);
//=================================================================================
struct OrderPool {
  OrderPair *slots;
  uint64_t bitmap;
  uint32_t capacity;
};
static_assert(sizeof(OrderPool) == 24, "struct must be 24 bytes");

void OrderPool_init(OrderPool *pool, uint32_t capacity) {
  pool->slots = (OrderPair *)calloc(capacity, sizeof(OrderPair));
  pool->bitmap = 0;
  pool->capacity = capacity;
}

OrderPair *OrderPool_Allocate(OrderPool *pool) {
  uint32_t index = __builtin_ctzll(~pool->bitmap);
  pool->bitmap |= (1ULL << index);
  return &pool->slots[index];
}

void OrderPool_Free(OrderPool *pool, OrderPair *slot_ptr) {
  uint32_t index = (uint32_t)(slot_ptr - pool->slots);
  pool->bitmap &= ~(1ULL << index);
}

uint32_t OrderPool_CountActive(const OrderPool *pool) {
  uint32_t popcount = __builtin_popcountll(pool->bitmap);
  return popcount;
}
//=================================================================================
// [RISK GATE] [TAG-risk_gate]
//=================================================================================
// this struct may be bad because it would have to do a movzbl, and jump from a
// 8 or 16bit register to 32 THEN to 64bit, im not sure about this optimization,
// maybe combining it into a single uint64_t before passing it to the function
// would work better or using uint16_t or uint32_t im not sutre, movzbl is
// basically free though, so this may not REALLY matter but we dont like wasted
// clock cycles
//
// EDIT: to initialize this just use:
// risk_gate gate = {
// 		buy_risk, buy_risk, buy_risk, buy_risk,
// 		sell_risk, sell_risk, sell_risk,sell_risk
// };
//
// lmao,actually converts down to a single intruction XD, god java would never,
// god i love c++, why tell the cpu how to arrange the data, when you can just
// do it properly in the first place lmao, never tell a human to do what a cpu
// does better, silicon > meat suits
//=================================================================================
// [EDIT [01-03-26 08:06om]]
//=================================================================================
// so, this way has edge cases where it doesnt work if the order ID is above
// 127, im thinking through this because the full mycroft trick would work for
// things that are 0 -> 255, but this only works with 0 ->127, like maybe if
// this used like, the MSB to indicate something eles? idk, because 0x82 - 0x80
// is an edge case, apparently doing it this way leaves the MSB as a kill bit,
// so if an order has anything that is 0x8XXXXXXX, then that means its a dead
// order, you dont lose any meaningful information because the MSB still can
// communicate information as a kill bit, so youre still technically using all 8
// bits for information, this is just reserving one by implicit definition, this
// way if breach returns 0, then every lane passed, if it doesnt, then the MSB
// tells you exactly which lanes failed, im gonna need to think about this logic
// more because right now its just, forany lane that matches the risk gate,
// allow it through i think, idk my brain feels like mush, java bad, im tired,
//=================================================================================
// [EDIT [02-03-26 02:28am]]
//=================================================================================
// this struct is just repeating the same value 8 times so it loads directly
// from the register and just becomes a movq %rdi, %rax instead of something
// like %si -> %edi -> %rdi -> %rax, because thats just ineffecient, and were
// already using the register anyways, it basically just functions the same as
// the above struct, i just didnt format it with 2 seeperate structs because im
// lazy, and theres really no need since its all the same value right now
// anyways, check usage in main, idk this way saves like 4 instruction counts
// lol, so like 60-80% reduction in cycle count, this is kinda handwavey, but it
// kinda explains the concept well enough, basically we dont wanna end up
// jumping from like an 8 or 16bit register, to a 32bit to a 64bit register,
// because thats wasted instruction counts that could be spent doing something
// important, like uploading my technical manifesto against java to git hub, or
// like, verifying orders against a risk gate for a hypothetical use case
//=================================================================================

struct risk_gate {
  uint8_t buy_side_risk0;
  uint8_t buy_side_risk1;
  uint8_t buy_side_risk2;
  uint8_t buy_side_risk3;
  uint8_t sell_side_risk0;
  uint8_t sell_side_risk1;
  uint8_t sell_side_risk2;
  uint8_t sell_side_risk3;
};
static_assert(sizeof(risk_gate) == 8, "struct must be 8 bytes");

uint64_t build_risk_gate(risk_gate sides) {
  uint64_t risk_gate_built = 0;

  risk_gate_built |= static_cast<uint64_t>(sides.buy_side_risk0);
  risk_gate_built |= static_cast<uint64_t>(sides.buy_side_risk1) << 8;
  risk_gate_built |= static_cast<uint64_t>(sides.buy_side_risk2) << 16;
  risk_gate_built |= static_cast<uint64_t>(sides.buy_side_risk3) << 24;
  risk_gate_built |= static_cast<uint64_t>(sides.sell_side_risk0) << 32;
  risk_gate_built |= static_cast<uint64_t>(sides.sell_side_risk1) << 40;
  risk_gate_built |= static_cast<uint64_t>(sides.sell_side_risk2) << 48;
  risk_gate_built |= static_cast<uint64_t>(sides.sell_side_risk3) << 56;

  return risk_gate_built;
}

uint64_t risk_gate_check(uint64_t packed_order, uint64_t risk_gate) {
  uint64_t breach = (risk_gate - packed_order) & 0x8080808080808080ULL;
  return breach;
}

//=================================================================================
//[ORDER GENERATION] [TAG-order_gen]
//=================================================================================
OrderPair order_generation() {
  uint64_t generated_order = rng();
  generated_order &= 0x7F7F7F7F7F7F7F7FULL;
  return *reinterpret_cast<OrderPair *>(&generated_order);
}
//=================================================================================
//=================================================================================
// [MAIN]
//=================================================================================
// so for main, probably intializing a while loop, and then generation an order,
// passing it through everything, then looping back to generating an order may
// be the best move here, althoguht using the twister within the while loop may
// introduce high latency, so genearting the order list before that may be the
// best way to actually simulate order pass through, idk im tired lol,
// apparently writing like 15-20k words like this in 8 days actually has a
// cognitive tax lmao, so idk, im alos learning as i go, but ive kinda got the
// asm breaking apart down now, so i gotta focus more on the actual other stuff
// i guess, i just actually like picking through the asm to see what the
// compiler does though, ALL HAIL THE COMPILER, we love the compiler, i may
// start analyzing differences between -O2 and -O3 outputs, but that may come
// later, i guess this has kind of turned into "build a branchless order system
// with Jennifer" LOL, w/e, these notes are pretty good if i say so myself
//
// EDIT: idk why i did the pool allocator, ill probably learn to use it in a
// seperate one, once i start adding logic to track states for buying/selling,
// instead of just simulated direct order flow with no state tracking
//=================================================================================
int main() {
  uint64_t duration, start, end;
  uint32_t risk_id;

  std::cout << "Please select how long to run this: ";
  std::cin >> duration;

  std::cout << "Please select risk gate id [0-127]: ";
  std::cin >> risk_id;

  const uint64_t total_duration = duration;

  uint8_t risk_val = static_cast<uint8_t>(risk_id);

  risk_gate risk_gate_id = {risk_val, risk_val, risk_val, risk_val,
                            risk_val, risk_val, risk_val, risk_val};

  uint64_t risk = build_risk_gate(risk_gate_id);
  uint64_t passed = 0;
  uint64_t total_failed = 0;

  uint64_t cycles_gen = 0;
  uint64_t cycles_pack = 0;
  uint64_t cycles_gate = 0;

  while (duration > 0) {
    __asm__ volatile("mfence" ::: "memory");
    uint64_t s1 = __rdtsc();
    OrderPair order = order_generation();
    uint64_t e1 = __rdtsc();
    __asm__ volatile("mfence" ::: "memory");

    uint64_t s2 = __rdtsc();
    uint64_t packed = order_packing_8byte(order);
    uint64_t e2 = __rdtsc();
    __asm__ volatile("mfence" ::: "memory");

    uint64_t s3 = __rdtsc();
    uint64_t breach = risk_gate_check(packed, risk);
    uint64_t failed = __builtin_popcountll(breach);
    uint64_t e3 = __rdtsc();
    __asm__ volatile("mfence" ::: "memory");

    cycles_gen += (e1 - s1);
    cycles_pack += (e2 - s2);
    cycles_gate += (e3 - s3);

    passed += 8 - failed;
    total_failed += failed;

    duration--;
  }
  // apparently these function are too fast and the overhead is just rdtsc
  // overhead lol, rdtsc is basically ~30 cycles lol, so this basically comes
  // out to 5 cycles for all those functions lol

  std::cout
      << "\n==============================================================="
         "=======\n";

  std::cout << "Average cycle count gen: "
            << static_cast<float>(cycles_gen) / total_duration << "\n";

  std::cout << "Average cycle count pack: "
            << static_cast<float>(cycles_pack) / total_duration << "\n";

  std::cout << "Average cycle count gate: "
            << static_cast<float>(cycles_gate) / total_duration << "\n";

  std::cout
      << "\n==============================================================="
         "=======\n";
  std::cout << "Total Orders Generated: " << passed + total_failed << "\n";
  std::cout << "==============================================================="
               "=======\n";

  std::cout << "Passed: " << passed << "\n";
  std::cout << "Failed: " << total_failed << "\n";
  std::cout << "==============================================================="
               "=======\n";

  std::cout << "Passed %: "
            << (static_cast<float>(passed) / (passed + total_failed)) * 100
            << "%\n";
  std::cout << "==============================================================="
               "=======\n";

  return 0;
}
//=================================================================================
// [ASM BREAKDOWN]
//=================================================================================
//[ORDER PACKING[ASM]]
//=================================================================================
/*
        movq	(%rdi), %rdx
        movzbl	1(%rdx), %eax
                [
        movzbl	2(%rdx), %ecx | these can apparently both be executed at the
   same time, using 2 instructions in a single cycle

        salq	$8, %rax
                ]
        salq	$16, %rcx
        orq	%rcx, %rax
        movzbl	(%rdx), %ecx
        movzbl	3(%rdx), %edx
        orq	%rcx, %rax
        movq	(%rsi), %rcx
        salq	$24, %rdx
        orq	%rdx, %rax
        movzbl	(%rcx), %edx
        salq	$32, %rdx
        orq	%rax, %rdx
        movzbl	1(%rcx), %eax
        salq	$40, %rax
        orq	%rdx, %rax
        movzbl	2(%rcx), %edx
        salq	$48, %rdx
        orq	%rax, %rdx
        movzbl	3(%rcx), %eax
        salq	$56, %rax
        orq	%rdx, %rax
        ret
*/
// yeah as expected the compiler actually broke this down into like 20
// instructions, a little more than 10, but im learning, and i started this like
// 10 days ago, while this is like 24 instructions, apparently due to
// instruction level parallelism, this will actually complete in ~10 cycles, i
// would need to add timing to this, which ill probably go back and do once this
// file is done, but this is kinda neat
//
// EDIT: holy crap this way is so much better lol, the nested for loop for order
// packing was like, 2-4x the amount of instructions and had so many jumps lol,
// crazy how actually thinking through this before can optimize it further, so
// one optimization that was pointed out to me is that trying to keep all this
// in the same set of registers, looking at this, the main register holding the
// output, is the %rdx register, with rax probably pulling in the data from the
// vectors, because its the accumulator, and im guessing the edx register is
// being used because its porbably not converting an 8bit int straight into a
// 64bit int, because the registers with the prefix e, are 32bit registers, so
// im guessing that its loading the actual byte from the order vectors, int a
// 4byte register, then when it shifts the values, or the static cast happens,
// it pushes it to a 8 byte register?, and apparently the movzbl + salq + orq
// chains on independent bytes can overlap in the pipeline, so it apparently
// isnt a 1:1 conversion of instructions to cycles, so that entire chain
// basically becomes maybe 1 or 2 actual clock cycles
//
// EDIT2: so apparently the chain referenced above [movzbl + salq + orq] can run
// on independent bytes simaultaneously, because of the CPU's out-of-order
// engine, idk ill probably go over this more at some point, to explain it like
// im stupid, each byte load shit or sequence is independent from the other
// bytes being loaded
//
// EDIT3: this version was bad, the new version using structs is way better
// because it doesnt have to derefernce the pointer that pointed to the vectors
// i was using, and even with arrays, those still load from a stack call,
// whereas the struct version both structs can fit directly into a register so
// the order flow goes directly into the cpu, without having to be passed from a
// list, like yeah it looks fucking cursed, and if you do this in a cs
// assignemnt the professor is probably gonna wanna have friendly chat with you
// about the fundamentals of clean code, but if your targeting the industry i
// am, clean code is an after thought, were going for *sparkle emoji*M E C H A N
// I C A L  S Y M P A T H Y*sparkle emoji*, this is basically a war crime in
// java btw, dont let corporate devs see this, asm code for reference:
/*
        .cfi_startproc
        movl	%esi, %ecx
        movzbl	%sil, %eax
        movl	%edi, %edi
        movzbl	%ch, %edx
        salq	$32, %rax
        salq	$40, %rdx
        orq	%rdx, %rax
        orq	%rdi, %rax
        movl	%esi, %edi
        shrl	$16, %edi
        movzbl	%dil, %edi
        salq	$48, %rdi
        orq	%rax, %rdi
        movl	%esi, %eax
        shrl	$24, %eax
        salq	$56, %rax
        orq	%rdi, %rax
        ret
*/
// it brings a tear to my eye to see myself growing so much through these, first
// i was using vectors like i actually wanted to code in java, then i thought
// about using arrays, then i rememberd struct padding and data alignment, and
// its like *galaxy brain*
//
// EDIT4: so, this new struct layout made the asm code just this, LMAO
//
/*
.LFB9693:
        .cfi_startproc
        movq	%rdi, %rax
        ret
        .cfi_endproc
*/
// the compuler was really like, dont worry girl, i gotchu
//=================================================================================
//[ORDER POOL[ASM]]
//=================================================================================
// im gonna tear this apart later, but it looks pretty clean, you can already
// see the dwarf unwinders giving us that sweet, succulent free register instead
// of having to use it to track the base pointer location, thanks mr.compiler,
// we love you btw <3
/*
        .cfi_startproc
        pushq	%rbp
        .cfi_def_cfa_offset 16
        .cfi_offset 6, -16
        pushq	%rbx
        .cfi_def_cfa_offset 24
        .cfi_offset 3, -24
        movq	%rdi, %rbx
        movl	%esi, %edi
        movl	$8, %esi
        movq	%rdi, %rbp
        subq	$8, %rsp
        .cfi_def_cfa_offset 32
        call	calloc@PLT
        movq	$0, 8(%rbx)
        movq	%rax, (%rbx)
        movl	%ebp, 16(%rbx)
        addq	$8, %rsp
        .cfi_def_cfa_offset 24
        popq	%rbx
        .cfi_def_cfa_offset 16
        popq	%rbp
        .cfi_def_cfa_offset 8
        ret
        .cfi_endproc
.LFE9694:
        .size	_Z14OrderPool_initP9OrderPoolj, .-_Z14OrderPool_initP9OrderPoolj
        .p2align 4
        .globl	_Z18OrderPool_AllocateP9OrderPool
        .type	_Z18OrderPool_AllocateP9OrderPool, @function
_Z18OrderPool_AllocateP9OrderPool:
.LFB9695:
        .cfi_startproc
        movq	8(%rdi), %rdx
        movq	%rdx, %rax
        notq	%rax
        rep bsfq	%rax, %rax
        btsq	%rax, %rdx
        cltq
        movq	%rdx, 8(%rdi)
        movq	(%rdi), %rdx
        leaq	(%rdx,%rax,8), %rax
        ret
        .cfi_endproc
.LFE9695:
        .size	_Z18OrderPool_AllocateP9OrderPool,
.-_Z18OrderPool_AllocateP9OrderPool .p2align 4 .globl
_Z14OrderPool_FreeP9OrderPoolPm .type	_Z14OrderPool_FreeP9OrderPoolPm,
@function _Z14OrderPool_FreeP9OrderPoolPm: .LFB9696: .cfi_startproc subq
(%rdi), %rsi movq	$-2, %rax movq	%rsi, %rcx sarq	$3, %rcx rolq	%cl,
%rax andq	%rax, 8(%rdi) ret .cfi_endproc .LFE9696: .size
_Z14OrderPool_FreeP9OrderPoolPm, .-_Z14OrderPool_FreeP9OrderPoolPm .p2align 4
        .globl	_Z21OrderPool_CountActivePK9OrderPool
        .type	_Z21OrderPool_CountActivePK9OrderPool, @function
_Z21OrderPool_CountActivePK9OrderPool:
.LFB9697:
        .cfi_startproc
        subq	$8, %rsp
        .cfi_def_cfa_offset 16
        movq	8(%rdi), %rdi
        call	__popcountdi2@PLT
        addq	$8, %rsp
        .cfi_def_cfa_offset 8
        ret
        .cfi_endproc
*/
// im gonna go over this stuff later but it looks pretty clean and fast tbh
//=================================================================================
// [RISK GATE [ASM]]
//=================================================================================
// de-bruijn strikes again lmao, apparently the value 16843009 is literally just
// 0x0101010101010101 LMAO, so like you could also do just a like 2 loc for the
// same cursed shit i did, using like this format, i may go back and test that,
// because i like that formatting more, but idk right now its like 5:37 am, and
// i havent slept lmao, it really is a shame java was built from the ground up
// to hide stuff like this from you, it takes all the fun out of programming
// lol, like this is some hand crafted, artisinal, purpose built to fit in the
// cpu hot paths, youd have to like go to a farmers market or something and pay
// like a 400% markup for shit lke this, god i love how it makes me feel when i
// snort a fat line of code like this
/*
uint64_t build_risk_gate(risk_gate sides) {
  uint64_t buy = sides.buy_side_risk * 0x0101010101010101ULL;
  uint64_t sell = sides.sell_side_risk * 0x0101010101010101ULL;
  return (buy & 0x00000000FFFFFFFF) | (sell & 0xFFFFFFFF00000000);
}
*/
/*
        .cfi_startproc
        movl	%edi, %ecx
        movzbl	%dil, %eax
        movzbl	%ch, %edi
        movq	%rax, %rdx
        movq	%rax, %rcx
        imulq	$16843009, %rdi, %rdi
        salq	$32, %rdx
        salq	$40, %rcx
        orq	%rdx, %rdi
        movq	%rax, %rdx
        salq	$56, %rax
        orq	%rdi, %rcx
        salq	$48, %rdx
        orq	%rcx, %rdx
        orq	%rdx, %rax
        ret
*/
//=================================================================================
// yeah, that code snippet above, and reorganizing the layout of the struct made
// this so much better, now its just 2 de-bruijn operations lol
//=================================================================================

/*
.LFB9698:
        .cfi_startproc
        movabsq	$72340172838076673, %rdx
        movl	%edi, %ecx
        movzbl	%dil, %eax
        movzbl	%ch, %edi
        imull	%edx, %eax
        imulq	%rdx, %rdi
        movabsq	$-4294967296, %rdx
        andq	%rdx, %rdi
        orq	%rdi, %rax
        ret
        .cfi_endproc
*/
// annnnnnnnnnnddddddddd we won, L I T E R A L L Y just a move quad instruction
// lol, wtf
/*
_Z15build_risk_gate9risk_gate:
.LFB9698:
        .cfi_startproc
        movq	%rdi, %rax
        ret
        .cfi_endproc
*/
//=================================================================================
// [RISK GATE CHECK [ASM]]
//=================================================================================
// just lmao, like what the fuck, you build the packed order in 1 instruction,
// build the risk gate in 1 instruction, and then check the order against the
// risk gate in 3 instructions, thats like, 2-3 cycles, to do all this, like 30
// lines of code, and it takes like 3 clock cycles, *sparkle emoji*W H A T  T H
// E  F U C K*sparkle emoji*
/*
.LFB9699:
        .cfi_startproc
        movabsq	$-9187201950435737472, %rax
        subq	%rsi, %rdi
        andq	%rdi, %rax
        ret
*/
//=================================================================================
// [MAIN [ASM]] [Im gonna annotate this tomorrow during the lecture]
//=================================================================================
/*
main:
.LFB11003:
        .cfi_startproc
        pushq	%r15
        .cfi_def_cfa_offset 16
        .cfi_offset 15, -16
        leaq	_ZSt4cout(%rip), %rdi
        pushq	%r14
        .cfi_def_cfa_offset 24
        .cfi_offset 14, -24
        pushq	%r13
        .cfi_def_cfa_offset 32
        .cfi_offset 13, -32
        pushq	%r12
        .cfi_def_cfa_offset 40
        .cfi_offset 12, -40
        pushq	%rbp
        .cfi_def_cfa_offset 48
        .cfi_offset 6, -48
        pushq	%rbx
        .cfi_def_cfa_offset 56
        .cfi_offset 3, -56
        subq	$56, %rsp
        .cfi_def_cfa_offset 112
        movq	%fs:40, %rsi
        movq	%rsi, 40(%rsp)
        leaq	.LC5(%rip), %rsi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0
        leaq	32(%rsp), %rsi
        leaq	_ZSt3cin(%rip), %rdi
        call	_ZNSi10_M_extractImEERSiRT_@PLT
        leaq	.LC6(%rip), %rsi
        leaq	_ZSt4cout(%rip), %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0
        leaq	28(%rsp), %rsi
        leaq	_ZSt3cin(%rip), %rdi
        call	_ZNSi10_M_extractIjEERSiRT_@PLT
        movq	32(%rsp), %r14
        testq	%r14, %r14
        je	.L34
        movzbl	28(%rsp), %eax
        movq	2496+_ZL3rng(%rip), %r13
        xorl	%r12d, %r12d
        xorl	%ebx, %ebx
        movabsq	$4311810305, %rcx
        leaq	_ZL3rng(%rip), %rbp
        movabsq	$6148914691236517205, %r15
        imulq	%rax, %rcx
        movq	%rax, %rsi
        salq	$40, %rsi
        orq	%rcx, %rsi
        movq	%rax, %rcx
        salq	$56, %rax
        salq	$48, %rcx
        orq	%rsi, %rcx
        orq	%rcx, %rax
        movq	%rax, 8(%rsp)
        jmp	.L28
        .p2align 4,,10
        .p2align 3
.L27:
        leaq	1(%rax), %r13
        movq	0(%rbp,%rax,8), %rax
        movq	8(%rsp), %rdi
        movabsq	$8202884508482404352, %rdx
        movabsq	$-2270628950310912, %rcx
        movq	%r13, 2496(%rbp)
        movq	%rax, %rsi
        shrq	$29, %rsi
        andq	%r15, %rsi
        xorq	%rsi, %rax
        movq	%rax, %rsi
        salq	$17, %rsi
        andq	%rdx, %rsi
        movabsq	$9187201950435737471, %rdx
        xorq	%rsi, %rax
        movq	%rax, %rsi
        salq	$37, %rsi
        andq	%rcx, %rsi
        xorq	%rsi, %rax
        movq	%rax, %rsi
        shrq	$43, %rsi
        xorq	%rsi, %rax
        andq	%rdx, %rax
        subq	%rax, %rdi
        movabsq	$-9187201950435737472, %rax
        andq	%rax, %rdi
        call	__popcountdi2@PLT
        cltq
        subq	%rax, %rbx
        addq	%rax, %r12
        addq	$8, %rbx
        subq	$1, %r14
        movq	%r14, 32(%rsp)
        je	.L26
.L28:
        movq	%r13, %rax
        cmpq	$311, %r13
        jbe	.L27
        movq	%rbp, %rdi
        call
_ZNSt23mersenne_twister_engineImLm64ELm312ELm156ELm31ELm13043109905998158313ELm29ELm6148914691236517205ELm17ELm8202884508482404352ELm37ELm18444473444759240704ELm43ELm6364136223846793005EE11_M_gen_randEv
        movq	2496(%rbp), %rax
        jmp	.L27
.L34:
        xorl	%r12d, %r12d
        xorl	%ebx, %ebx
        .p2align 4
        .p2align 3
.L26:
        leaq	.LC7(%rip), %rsi
        leaq	_ZSt4cout(%rip), %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0
        leaq	(%r12,%rbx), %rbp
        movl	$24, %edx
        leaq	.LC8(%rip), %rsi
        leaq	_ZSt4cout(%rip), %rdi
        call
_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
        movq	%rbp, %rsi
        leaq	_ZSt4cout(%rip), %rdi
        call	_ZNSo9_M_insertImEERSoT_@PLT
        leaq	.LC9(%rip), %rsi
        movq	%rax, %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0
        leaq	.LC10(%rip), %rsi
        leaq	_ZSt4cout(%rip), %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0
        movl	$8, %edx
        leaq	.LC11(%rip), %rsi
        leaq	_ZSt4cout(%rip), %rdi
        call
_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
        movq	%rbx, %rsi
        leaq	_ZSt4cout(%rip), %rdi
        call	_ZNSo9_M_insertImEERSoT_@PLT
        leaq	.LC9(%rip), %rsi
        movq	%rax, %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0
        movl	$8, %edx
        leaq	.LC12(%rip), %rsi
        leaq	_ZSt4cout(%rip), %rdi
        call
_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
        movq	%r12, %rsi
        leaq	_ZSt4cout(%rip), %rdi
        call	_ZNSo9_M_insertImEERSoT_@PLT
        leaq	.LC9(%rip), %rsi
        movq	%rax, %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0
        leaq	.LC10(%rip), %rsi
        leaq	_ZSt4cout(%rip), %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0
        movl	$10, %edx
        leaq	.LC13(%rip), %rsi
        leaq	_ZSt4cout(%rip), %rdi
        call
_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
        testq	%rbx, %rbx
        js	.L29
        pxor	%xmm0, %xmm0
        cvtsi2ssq	%rbx, %xmm0
.L30:
        testq	%rbp, %rbp
        js	.L31
        pxor	%xmm1, %xmm1
        cvtsi2ssq	%rbp, %xmm1
.L32:
        divss	%xmm1, %xmm0
        leaq	_ZSt4cout(%rip), %rdi
        mulss	.LC14(%rip), %xmm0
        cvtss2sd	%xmm0, %xmm0
        call	_ZNSo9_M_insertIdEERSoT_@PLT
        leaq	.LC15(%rip), %rsi
        movq	%rax, %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0
        leaq	.LC10(%rip), %rsi
        leaq	_ZSt4cout(%rip), %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0
        movq	40(%rsp), %rax
        subq	%fs:40, %rax
        jne	.L38
        addq	$56, %rsp
        .cfi_remember_state
        .cfi_def_cfa_offset 56
        xorl	%eax, %eax
        popq	%rbx
        .cfi_def_cfa_offset 48
        popq	%rbp
        .cfi_def_cfa_offset 40
        popq	%r12
        .cfi_def_cfa_offset 32
        popq	%r13
        .cfi_def_cfa_offset 24
        popq	%r14
        .cfi_def_cfa_offset 16
        popq	%r15
        .cfi_def_cfa_offset 8
        ret
.L29:
        .cfi_restore_state
        movq	%rbx, %rax
        andl	$1, %ebx
        pxor	%xmm0, %xmm0
        shrq	%rax
        orq	%rbx, %rax
        cvtsi2ssq	%rax, %xmm0
        addss	%xmm0, %xmm0
        jmp	.L30
.L31:
        movq	%rbp, %rax
        andl	$1, %ebp
        pxor	%xmm1, %xmm1
        shrq	%rax
        orq	%rbp, %rax
        cvtsi2ssq	%rax, %xmm1
        addss	%xmm1, %xmm1
        jmp	.L32
.L38:
        call	__stack_chk_fail@PLT
        .cfi_endproc
.LFE11003:
        .size	main, .-main
        .p2align 4
        .type	_GLOBAL__sub_I__Z19order_packing_8byte9OrderPair, @function
_GLOBAL__sub_I__Z19order_packing_8byte9OrderPair:
.LFB11750:
        .cfi_startproc
        movq	$125124, _ZL3rng(%rip)
        movl	$125124, %ecx
        movl	$1, %edx
        leaq	_ZL3rng(%rip), %rdi
        movabsq	$6364136223846793005, %rsi
        .p2align 6
        .p2align 4
        .p2align 3
.L40:
        movq	%rcx, %rax
        shrq	$62, %rax
        xorq	%rcx, %rax
        imulq	%rsi, %rax
        leaq	(%rax,%rdx), %rcx
        movq	%rcx, (%rdi,%rdx,8)
        addq	$1, %rdx
        cmpq	$312, %rdx
        jne	.L40
        movq	$312, 2496+_ZL3rng(%rip)
        ret
        .cfi_endproc
.LFE11750:
        .size	_GLOBAL__sub_I__Z19order_packing_8byte9OrderPair,
.-_GLOBAL__sub_I__Z19order_packing_8byte9OrderPair .section	.init_array,"aw"
        .align 8
        .quad	_GLOBAL__sub_I__Z19order_packing_8byte9OrderPair
        .local	_ZL3rng
        .comm	_ZL3rng,2504,32
        .section	.rodata.cst16,"aM",@progbits,16
        .align 16
.LC3:
        .quad	-5403634167711393303
        .quad	-5403634167711393303
        .section	.rodata.cst4,"aM",@progbits,4
        .align 4
.LC14:
        .long	1120403456
        .globl	__popcountdi2
        .ident	"GCC: (GNU) 15.2.1 20260103"
        .section	.note.GNU-stack,"",@progbits
*/
