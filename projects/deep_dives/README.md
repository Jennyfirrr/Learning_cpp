# Deep Dives

Explorations of hardware/binary-level topics, low-level optimization, and how C++ actually compiles down to assembly. Started with bitwise operators and spiraled into branchless programming, bit packing for HFT order books, hand-tracing binary math, and annotating compiler-generated x86 assembly.

## Index

| # | File | Topics |
|---|------|--------|
| 01 | `01_bitwise_operators.cpp` | OR, XOR, and bit shift operators — setting, toggling, and reading individual bits, how each gate works at the binary level |
| 02 | `02_bitmasking.cpp` | Bitmasking, bitfields, and bit shifts — storing 32 T/F flags in a single int, power-of-2 checks, L1/L2 cache benefits of compact data |
| 03 | `03_branchless_programming.cpp` | Branchless clamp, two's complement, branch prediction penalties, chaining operations, ternary vs if/else at the CPU pipeline level |
| 04 | `04_bit_state_checking.cpp` | Per-bit state extraction and normalization, kill switch logic using bitmasks, cmov vs jump compiler behavior, when if statements stay branchless vs trigger branch prediction |
| 05 | `05_bit_packing_orders.cpp` | Packing multiple 8-bit order IDs into 32-bit ints, kill masks with de Bruijn-style multiplication, loop unrolling, delta encoding with XOR, signed vs unsigned pitfalls, extensively annotated ASM output for the entire program |
| 06 | `06_kill_switch_asm.cpp` | Array-based kill switch using std::array, function-by-function ASM breakdown showing how the compiler converts bit operations to NOT+TEST+SETE, RDTSC cycle counting |
| 07 | `07_boolean_algebra.cpp` | Mycroft trick (haszero/hasless macro), lane-by-lane binary subtraction with borrow logic, full hand-traced 32-bit operation table, POPCNT as a single silicon instruction |

### Standalone Files

| File | Topics |
|------|--------|
| `asm_reference.cpp` | x86_64 ASM reference — registers (eax/rax/al), AT&T syntax ($literal, %register), instruction suffixes (b/w/l/q), condition codes, flags (ZF/CF/SF/OF), JNE/JE/JMP branch instructions |
| `inline_asm_discovery.cpp` | The moment of discovering you can write ASM inline in C++ (and that Java definitely does not let you do this) |
| `fixed_point_math.cpp` | COBOL-inspired fixed point arithmetic — storing decimals as integers for deterministic financial math, why floating point comparison is icky |
| `FIX_protocol.cpp` | Placeholder for FIX protocol research |

### `new_notes/`

| File | Topics |
|------|--------|
| `memory_stuff.cpp` | Memory alignment, malloc internals, struct padding, alignas(64), cache hierarchy (L1/L2/L3), stack vs heap allocation, false sharing, buddy allocators, vector preallocation — why pre-reserving beats push_back |

### `asm_outputs/`

| File | Source |
|------|--------|
| `05_bit_packing_orders.s` | Generated with `g++ -S -O2` from `05_bit_packing_orders.cpp` |
| `06_kill_switch_asm.s` | Generated with `g++ -S -O2` from `06_kill_switch_asm.cpp` |

## Future Deep Dives

Things I'm probably gonna eventually add here:

- SIMD/AVX-512[still need more on this]
- Bitset class[idk if i did this or not]
- Branchless programming[alot of this is done]
    - basics for this are mostly covered I think
    - need to add more advanced implementations, like the mycroft trick, and using techniques for memory managment
    - basic mycroft implementation done, why its not ideal in some cases, why signed bits are bad for order packing
- Memory alignment[probably gonna need to know more about asm]
    - this will probably cover things like malloc, heap, stack, and other stuff like that[these are pretty fleshed out]
    - Partially done, malloc, alignas, cachelines, etc, need to cover implementation more[mostly done, need use cases/examples]
    - heap and stack need more indepth notes, only briefly covered[updated]
    - maybe optimal ASM inline implementations if im really bored
    - buddy/pool allocators
- Lock Free Concurrency[this is important]
    - to put in memory_stuff or not to put it there, that is the question
- Branchless Backtester[already kind of have the logic laid out]
    - idk how useful this would be
- FPGA/kernal bypasses
    - nust pure curiosity, would mostly be writing about, not actual code
- Inline ASM
    - definiely need to know this
- ASM[registers, DOD, lower level optimizations]
    - being able to write all asm not as important, but DOD(data oriented design), big important
    - understandnig how ASM works is very important, especially manual pointer managment
    - need more references for how cpp converts to asm at compile time
    - added inline asm notes to point out registers for better explanations
    - added todo's[DWARF unwinder, @PLT, more suffix/instruction]
- Centralized time keeping
    - not hugely important, just a curisoty for node based trading systems
- ML library design, although the guy that made databricks or whatever it is has a great thing for that, like Matei Zaharia is his name or something i think, i may just follow that, idk I saw a link for something that covered the fundamentals of that and it looked interesting
- Actual cpu design[lol ambitious i know]
    - probably more related to FPGA
- BMI1/BMI2[bit manipulation instructions]
    - this will probably be under branchless programming or bitset class
    - gonna group PDEP/PEXT in which this because WHY NOT LOL[briefly covered, probably more indepth in a new reference file for hardware instructions or microcode calls]

**Keep in mind that these are more like things I wanna study, there isnt like a guarentee or anything ill every actually get to these or push anything about them, there just a list of things that im currently interested in**
