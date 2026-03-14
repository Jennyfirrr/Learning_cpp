# Deep Dives

**NOT SAFE FOR HUMAN RESOURCES**

**IF YOURE FROM HR, JUST FORWARD THIS TO YOUR LEAD ENGINEER SO YOU DONT HAVE AN ANYUERISM READING IT, I PROMISE I KNOW WHAT IM TALKING ABOUT, BUT THE PROSE IN THESE FILES WILL MAKE YOU HAVE A HEADACHE THAT LASTS FOR 6+ MONTHS**

**TLDR: USE A STRUCT**
 - Thats literally what all these files conclude to, 44k words of prose + 5k words of annotated ASM, to basically just say, yous a well written struct

Explorations of hardware/binary-level topics, low-level optimization, and how C++ actually compiles down to assembly. Started with bitwise operators and spiraled into branchless programming, bit packing for HFT order books, hand-tracing binary math, and annotating compiler-generated x86 assembly.

**Java hate counter: 76 mentions across 15 files** (memory_reference.cpp leads with 21, tied for second: 05_bit_packing_orders.cpp and 09_allocator_practice.cpp with 11 each)

## Index

| # | File | Prose | ASM | Lines | Topics |
|---|------|-------|-----|-------|--------|
| 01 | `01_bitwise_operators.cpp` | 929 | — | 159 | OR, XOR, and bit shift operators — setting, toggling, and reading individual bits, how each gate works at the binary level |
| 02 | `02_bitmasking.cpp` | 2,129 | — | 201 | Bitmasking, bitfields, and bit shifts — storing 32 T/F flags in a single int, power-of-2 checks, L1/L2 cache benefits of compact data |
| 03 | `03_branchless_programming.cpp` | 5,690 | 6 | 577 | Branchless clamp, two's complement, branch prediction penalties, chaining operations, ternary vs if/else at the CPU pipeline level |
| 04 | `04_bit_state_checking.cpp` | 2,231 | 12 | 283 | Per-bit state extraction and normalization, kill switch logic using bitmasks, cmov vs jump compiler behavior, when if statements stay branchless vs trigger branch prediction |
| 05 | `05_bit_packing_orders.cpp` | 4,757 | 1,768 | 1,305 | Packing multiple 8-bit order IDs into 32-bit ints, kill masks with de Bruijn-style multiplication, loop unrolling, delta encoding with XOR, signed vs unsigned pitfalls, extensively annotated ASM output for the entire program |
| 06 | `06_kill_switch_asm.cpp` | 1,245 | 509 | 398 | Array-based kill switch using std::array, function-by-function ASM breakdown showing how the compiler converts bit operations to NOT+TEST+SETE, RDTSC cycle counting |
| 07 | `07_boolean_algebra.cpp` | 3,358 | — | 479 | Mycroft trick (haszero/hasless macro), lane-by-lane binary subtraction with borrow logic, full hand-traced 32-bit operation table, POPCNT as a single silicon instruction |
| 08 | `08_mycroft_implementation.cpp` | 4,874 | 1,941 | 1,386 | Mycroft trick implementation with 64-bit order packing, 4 buy + 4 sell per uint64, PDEP/PEXT bit deposit/extract, signed bit smearing pitfalls, BMI1/2 instructions |
| 09 | `09_allocator_practice.cpp` | 3,151 | 1,048 | 1,027 | Pool/arena allocator for order tracking, de Bruijn masking, PDEP/PEXT with immintrin.h, IMUL magic number optimization, TZCNT/BSF bit scanning |
| 10 | `10_memory_allocation_implementation.cpp` | 3,577 | — | 527 | Pool allocator order tracking, buy/sell risk gates, 8-lane order packing, branchless order routing, struct-based state machine design for single-ticker and cross-sectional flows |

### Standalone / Reference Files

| File | Prose | ASM | Lines | Topics |
|------|-------|-----|-------|--------|
| `asm_reference.cpp` | 4,954 | 123 | 538 | x86_64 ASM reference — registers (eax/rax/al), AT&T syntax ($literal, %register), instruction suffixes (b/w/l/q), condition codes, flags (ZF/CF/SF/OF), JNE/JE/JMP branch instructions |
| `memory_reference.cpp` | 5,654 | — | 918 | Memory alignment, malloc internals, struct padding, alignas(64), cache hierarchy (L1/L2/L3), stack vs heap allocation, false sharing, buddy allocators, vector preallocation — why pre-reserving beats push_back |
| `inline_asm_discovery.cpp` | 1,419 | — | 131 | The moment of discovering you can write ASM inline in C++ (and that Java definitely does not let you do this) |
| `fixed_point_math.cpp` | 339 | — | 27 | COBOL-inspired fixed point arithmetic — storing decimals as integers for deterministic financial math, why floating point comparison is icky |
| `FIX_protocol.cpp` | 15 | — | 6 | Placeholder for FIX protocol research |
| `buddy_allocator_ref.cpp` | 60 | — | 305 | Buddy allocator reference — power-of-2 block splitting, free list ops, bitmap tracking, coalescing on free, diagnostics snapshot |
| `data_structures.cpp` | 511 | — | 78 | Linear structures (arrays/vectors/linked lists), trees (BST, in-order/pre-order/post-order traversals), graphs and hash maps (stubs) |
| `lock_free_reference.cpp` | 975 | — | 110 | Lock-free concurrency, CAS (compare_exchange_weak vs strong), spin loops, cmpxchg on x86, mutex vs lock-free tradeoffs |

### `libraries/`

Reusable header-only libraries extracted from the deep dives. All built on fixed-point arithmetic — no floating point, no heap allocation surprises.

| File | Topics |
|------|--------|
| `FixedPoint16.h` | 16-bit fractional fixed-point arithmetic — int32 backed, add/sub/mul/div, full comparison suite, branchless abs and sign via bit smearing |
| `LinearRegressionSimple.h` | Single-feature ordinary least squares using fixed-point math — y = mx + b, predict from fitted model |
| `LinearRegression3X.h` | Linear regression with ring buffer feeder (8-sample window), R² goodness-of-fit, OLS with 5-sum accumulator, price prediction pipeline — all fixed-point |
| `PoolAllocator.h` | Bitmap-based pool allocator for order tracking — TZCNT slot finding, POPCNT active count, calloc-backed with 64-slot bitmap capacity |

### `asm_outputs/`

| File | Source |
|------|--------|
| `05_bit_packing_orders.s` | Generated with `g++ -S -O2` from `05_bit_packing_orders.cpp` |
| `06_kill_switch_asm.s` | Generated with `g++ -S -O2` from `06_kill_switch_asm.cpp` |
| `08_mycroft_implementation.s` | Generated from `08_mycroft_implementation.cpp` |
| `09_allocator_practice.s` | Generated from `09_allocator_practice.cpp` |

## Deep Dive Progress

### Done / Mostly Done

- **Branchless programming** — basics, two's complement, branch prediction, cmov vs jump, mycroft trick, signed bit pitfalls (03, 04, 07, 08)
- **Bitwise operators & bitmasking** — gates, shifts, bitfields, flags, power-of-2 checks (01, 02)
- **Bit packing & order encoding** — 8-bit order IDs packed into 32/64-bit ints, kill masks, delta encoding, SWAR (05, 08, 09, 10)
- **ASM reference** — x86_64 registers, AT&T syntax, instruction suffixes, condition codes, flags, branch instructions (asm_reference.cpp)
- **Memory alignment & allocation** — malloc internals, struct padding, alignas(64), cache hierarchy, stack vs heap, false sharing, vector preallocation (memory_reference.cpp)
- **Pool/arena allocators** — pool allocator for order tracking, bitmap-based slot management, TZCNT/BSF bit scanning (09, 10)
- **Buddy allocator** — reference implementation with power-of-2 splitting, free lists, coalescing (buddy_allocator_ref.cpp)
- **BMI1/BMI2** — PDEP/PEXT bit deposit/extract, IMUL magic numbers, de Bruijn masking (08, 09)
- **Inline ASM** — discovery and basic usage, register annotations (inline_asm_discovery.cpp)
- **Fixed point math** — COBOL-inspired integer storage for deterministic financial math (fixed_point_math.cpp)
- **Data structures** — linear structures, BST, tree traversals (data_structures.cpp)

### In Progress / Continuing

- **Lock free concurrency** — basic reference file added (CAS, spin loops, weak vs strong), still need memory ordering (relaxed/acquire/release/seq_cst), false sharing, cache line implications (lock_free_reference.cpp)
- **Order routing & execution** — pool allocator state machine, buy/sell risk gates, branchless order routing, single-ticker done, cross-sectional flow design in progress (10)
- **ASM breakdowns** — function-by-function annotated ASM for 05, 06, 08, 09, need more coverage for newer files
- **Branchless backtester** — logic mostly laid out in 10, need actual implementation with data stream reading

### Not Started / Future

- SIMD/AVX-512
- Bitset class
- FPGA/kernel bypasses — pure curiosity, mostly writing not code
- Centralized time keeping — curiosity for node based trading systems
- ML library design
- Actual CPU design — probably more related to FPGA
- FIX protocol — placeholder exists (FIX_protocol.cpp)
- ASM deep dive — DWARF unwinder, @PLT, DOD (data oriented design), more instruction coverage

**Keep in mind that these are more like things I wanna study, there isnt like a guarentee or anything ill every actually get to these or push anything about them, there just a list of things that im currently interested in**

---

**Portfolio: [Jennyfirr.me](https://Jennyfirr.me)**

**Connect with me on LinkedIn if you like my content: [LinkedIn](https://www.linkedin.com/in/jennifer-lewis-b601a73a8)**

**Email: jenn.lewis5789@gmail.com**
