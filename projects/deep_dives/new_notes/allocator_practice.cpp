//=================================================================================
// [TAGS] [for grep searching later]
//=================================================================================
// [de-bruijn] [PDEP , PEXT] [pool allocator , arena allocator] [imul] [tzcnt
// bsf] [g++ -O3 -S -o]
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
// streams could be used as well, so i think ill do that for this
//=================================================================================
//=================================================================================
// [ACTUAL CODE [FUNCTIONS]]
//=================================================================================
// [ORDER PACKING] [TAG-order_packing]
//=================================================================================
uint64_t order_packing_8byte(const std::vector<uint8_t> &buy_side_orders,
                             const std::vector<uint8_t> sell_side_orders) {
  uint64_t packed_orders = 0;

  for (int i = 0; i < 4; i++) {
    packed_orders |= static_cast<uint64_t>(buy_side_orders[i]) << (i * 8);
  }

  for (int i = 0; i < 4; i++) {
    packed_orders |= static_cast<uint64_t>(sell_side_orders[i])
                     << ((i * 8) + 32);
  }

  return packed_orders;
}
//=================================================================================
//=================================================================================
// [MAIN]
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
// bytes being loaded,
//=================================================================================
