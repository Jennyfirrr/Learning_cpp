// alrighty then, i cant fucking sleep, and its 4am lmao, so WERE GOING OVER
// MEMORY ALIGNMENT, MALLOC, AND OTHER STUFF :D
//
// Memory alighment basically just means placing data in memory addresses that
// are multiples of the data types size, in one of the files where i annotated
// the asm block in main, you can see this happen in the bit_packing_32.cpp, i
// believe it was the NOP alignment or something? So apparently this is a
// different type of alignment, nop alignment is more about instruction
// alignment, no memory alignment, anyways back to the topic. Basically like
// when you have a 4-byte int, its naturall aligned with addresses that are
// divisible by 4, a 8-byte double, at mulitples of 8, etc
//
// Why this matters in HFT SYSTEMS, and other low level programming where
// performance and latency are critical, when te CPU fetches memory, it does so
// in cache-line sized chunks, typically 64 bytes, which i probably referenced
// in one of my earlier files, like 03 i think? i went over L1/L2 caching alot
// in those, because thats where i first came to the conclusion that you can use
// the actual bits to encode information to stack up orders, and then found out
// that this is actually what people do lol, anyways, when a piece of data
// straddles two cache lines,(misaligned) the cpu has to do T W O  W H O L E  F
// E T C H E S, and stitch the result together( I C K Y ), in a hot path this
// would 100% cause you to lose trades that would otherwise be profitable lol,
// we dont like that, so were gonna learn to optimize this lmao
//
// For deterministic computing, on x86_64, misaligned access usually works but
// it still costs extra cycles that are non deterministic, depending on if you
// cross a cache line boundary. on ARM and other older architectures, i can
// actually fault lmao, wtf, wow modern computers have come a long way, For SST
// deterministic execution, you dont want this lmao,
//
// SIMD vectorization, AVX2 wants 32-byte alignment, and AVX-512 wants 64-byte
// alignment, if its not aligned, then the compuler either emits slower
// unaligned, load instructions or cant auto-vectorize AT ALL, when doing bit
// packing methods and batch feature computation across instrucments this is  H
// U G E,
//
// im gonna list some practical patters that im being shown here as a reference,
// and to study and just know about because this shit is lit yo, as the kids
// would say
//
/*
struct Bad {
        char a; 1 byte + 7 padding
        double b; 8 bytes
        char c; 1 byte + 7 padding
};

total 24 bytes
*/
// SO the reason this one is bad is beacuse char is a single byte, with 7 as
// padding, double b is 8 bytes, and char c is a single byte with 7 as padding,
// which totals 24 bytes lol, so not only will it potentially straddle
// boundaries, it will also be an ineffecient use of memory, bceause you would
// have to slot it inot a 32 byte register, and youd have 8bytes just DOING
// ABSOLUTELY NOTHING
//
// EDIT: registers arnt 32-bytes, theyre typically 8 bytes, or atleast the
// general purpose ones are,
//
/*
struct Good {
  double b; 8 bytes
  char a; 1 byte
  char c; 1 byte + 6 padding
}
total 16 bytes
        */
// so the reason this one is good, is because of how your ordering the variables
// within the struct, bceause you group the char values together, they can
// essentially share right? On a small scale, it doesnt really seem like that
// much, because were at the byte level, and playing around with our bits, but
// over millions of these, it ADDS UP, just like the mountain dew i buy
//
// EDIT: so this wasnt due to variable typing, this was due to the way the stuff
// is stored in memory, like, it something is taking up a single byte, and the
// next item is 8 bytes, you cant store 9 bytes together, so it has to pad the
// single character value with 7 empty bytes, leading to ineffecient storage
// layouts, apparently it can be BOTH LOL, the type determines the alignment
// requirement, and the alignment requirement determines the padding, so the
// padding is a function of the relationship betweem adjacent member types, and
// not just the storage layout in isolation, so i was like half right lol
//
// there is a thing called explicit alignment, using the alignas function call,
// so this apparently guarentees that it will start at a cache line, so the
// whole block fits in all snug and tight, it doesnt cause a cache line, it just
// ensures that the data maps to exactly ONE
/*
struct alignas(64) SstCacheLineAlignedBlock {
  double sst_feature_values[8]; this occupies exactly one cache line
}
        */
// this guarentees no false sharing across cores if different threads are
// touching different blocks, which for my purposes is critical for
// deterministic parallel execution
//
// aligned allocation, std::aligned_alloc or posix_memalign for heap allocations
// when you need specific alignment beyond what new gives you
/*
void sst_allocate_aligned(size_t aligned, size_t size) {
  return std::aligned_alloc(alignment, size);
}
*/
// so one thing about this, is that it REQUIRES that size be a multiple of
// alignment, if its not its undefined behaviour(I C K Y)
/*
void *sst_allocate_aligned(size_t alignment, size_t size) {
  size_t sst_adjusted_size = (size + alignment - 1) & ~(alignment - 1);
  return std::aligned_alloc(alignment, sst_adjusted_size);
}
*/
// this is apparently a better and safer way to do that, this bitmasking trick
// rounds up one to the next multiple of alignment, probably worth going more
// into detail on this, but its 4:30 am lmao

// so apparently false sharing is a sneaky lil thing, when you have two threads
// writing to two different variables that happen to share a cache line, they
// will thrash eachothers caches, WHICH IS BAD, the fix for this is to use
// padding, or alignas(64), to ensure that each threads hot data lives on its
// own cache line, as a note, this doest cause incorrect results, but it does
// introduce latency(ICKY), which is bad for deterministic behaviour
//
// so some stuff abut malloc is next, its a userspace memory allocator that
// manages a pool of memory, between your program and the OS< basically softare
// will ask malloc for memory, but malloc doesnt call the kernal every time,
// because this would apparently be SLOW(ICKY), syscall in hotpath is almost as
// bad as java, instead it asks the os for big chunks of memory upfront, just
// like student loans, and other loans lol, and it ddoes this via sbrk, or mmap,
// and then carves them up into smaller pieces as you request them,
//
// just like the GC in java, malloc call is non deterministic timing, somtimes
// its fast, other times it has to search for a block, somtimes its gotta call
// mmap to get MORE memory, and it becomes this whole thing, and no one wants
// that, this is why im probably gonna go over custom allocators or pre
// allocating memory, which i kind of touched on in the vector preallocation
// file when i was analyzing why one leetcode solution was faster than another
//
// so when you use a call like malloc(32) the allocator doesnt just give you 32
// bytes, it stores a header which is just metadata, right before your pointer,
// typically containing the block size and if its free, so your acutally using
// something like 40-48 bytes, and when you call free(), it reads that header to
// know how much to release, i guess this is similar to using the bit packing to
// encode data types, in a similar concept
//
// Fragmentation is BAD, and there are TWO FLAVORS, external fragmentation,
// which is enough total free memory, but its non contiguous, which means its
// scattered across the memory, and internal fragmentation which is something
// like when you ask for 17 bytes, but it got rounded up to 32 bceause the
// computer doesnt like things that arnt mulitples of powers of 2, both of these
// waste cache lines, and they both hurt determinism(shocker right?)
//
// when you free() memory, the allocator puts it on a free list, and the next
// malloc call searches that list, and there are apparently different
// strategies, to manage this, first-fit, best-fit, buddy system, the buddy
// allocator is worth understanding, it splits and merges power of 2 blocks,
// which makes allocation, deallocation O(log n), and predictable, that
// predictability is why it shows up in kernal memory managment, and real time
// systems, the big SST takeaway is NEVER call malloc in a hotpath, pre allocate
// everything, use memory pools, and recycle fixed size blocks, which is how you
// get deterministic allocation timing
