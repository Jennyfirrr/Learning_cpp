//=================================================================================
// [TAGS] [for grep searching later]
//=================================================================================
// [de-bruijn] [PDEP , PEXT] [pool allocator , arena allocator] [imul] [tzcnt
// bsf] [g++ -O3 -S -o] [java-bad]
//=================================================================================
// included libraries
//=================================================================================
#include <array>
#include <cstdint>
#include <immintrin.h>
#include <iostream>
#include <vector>
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
// [ACTUAL CODE [FUNCTIONS]]
//=================================================================================
// [ORDER PACKING] [TAG-order_packing]
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

uint64_t order_packing_8byte(buy_side buys, sell_side sells) {
  uint64_t packed_orders = 0;

  packed_orders |= static_cast<uint64_t>(buys.order0b);
  packed_orders |= static_cast<uint64_t>(buys.order1b) << 8;
  packed_orders |= static_cast<uint64_t>(buys.order2b) << 16;
  packed_orders |= static_cast<uint64_t>(buys.order3b) << 24;
  packed_orders |= static_cast<uint64_t>(sells.order0s) << 32;
  packed_orders |= static_cast<uint64_t>(sells.order1s) << 40;
  packed_orders |= static_cast<uint64_t>(sells.order2s) << 48;
  packed_orders |= static_cast<uint64_t>(sells.order3s) << 56;

  return packed_orders;
}
// this is fucking cursed LOL, the compiler likes this more though
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
// language that shall not be named right? i think so
//=================================================================================
struct OrderPool {
  uint64_t *slots;
  uint64_t bitmap;
  uint32_t capacity;
};

void OrderPool_init(OrderPool *pool, uint32_t capacity) {
  pool->slots = (uint64_t *)calloc(capacity, sizeof(uint64_t));
  pool->bitmap = 0;
  pool->capacity = capacity;
}

uint64_t *OrderPool_Allocate(OrderPool *pool) {
  uint32_t index = __builtin_ctzll(~pool->bitmap);
  pool->bitmap |= (1ULL << index);
  return &pool->slots[index];
}

void OrderPool_Free(OrderPool *pool, uint64_t *slot_ptr) {
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
//=================================================================================
//[ORDER GENERATION] [TAG-order_gen]
//=================================================================================
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
//=================================================================================
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
//=================================================================================
// [ORDER POOL[ASM]]
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
