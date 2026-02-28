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
