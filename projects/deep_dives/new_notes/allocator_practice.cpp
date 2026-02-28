// place holder for playing around with different types of allocaters, also
// continuous order generation + routing
//
#include <array>
#include <cstdint>
#include <immintrin.h>
#include <iostream>
#include <vector>

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
