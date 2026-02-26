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
// just like the GC in java, malloc call is non deterministic timing, we dont
// hate malloc though because we LIKE cpp, java is I C K Y, somtimes its fast,
// other times it has to search for a block, somtimes its gotta call mmap to get
// MORE memory, and it becomes this whole thing, and no one wants that, this is
// why im probably gonna go over custom allocators or pre allocating memory,
// which i kind of touched on in the vector preallocation file when i was
// analyzing why one leetcode solution was faster than another
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
//
//==================================================================================
// EDIT[25-02-26 06:28pm]: heap, stack, probably relationship between this and
// the above, maybe some asm sprinkled in for funsies
//==================================================================================
//
// A L R I G H T Y, lets get this show on the road,
// so, stack is usually the "hot memory", like i referenced in another file, and
// i think i said something like this is true 80% of the time or so, but i said
// that because the way it was described to me, this will be better notes
//
// so stack allocations are fast and usually cache-friendly because the stack
// grows and shrinks in a predictable linear fashion, and as i stated before,
// the CPU L O V E S, things that are consistent and predictable, so this data
// tends to stay warm in the L1/l2 cache, which for DOD or data oriented design,
// and the stuff im pursuing, this is, we love this, i think when i referenced
// this in on of the earlier files, i said something like stack being on the
// right and heap being on the left, which isnt completely right, they arnt
// physically seperate on different ends of a bus, they both live in the same
// physical hardware, same memory bus, and stuff, the difference mainly lies in
// how they are managed, and WHERE they grow in the virtual space, the way i
// kind of interpreted the right left thing was probably because i was tired,
// thinking about how much i hate java, and not completely internalized how the
// stack vs heap allocations work, but for a better visual model, this is how it
// was described to me, think of your processes virtual address space as one
// long strip, at the bottom, you have code, static data, and things that dont
// really move around much, the heap starts right above that on the strip, and
// it grows U P W A R D, towards higher addresses, while the stack starts at the
// top, and grows D O W N W A R D, towards lower addresses, so basically they
// grow together, like the pointers in a classic 2 pointer problem, like the
// whole while(left < right) where left = index 0, and right = vector.size() -
// 1, and if they meet, well then your shits about to get fucked lol, and your
// GONNA have a bad day
//
// the reason that the stack is so much faster isnt really about physical
// location, so much as the allocation strategy, because allocating on the stack
// is literally just moving the stack pointer around, like so,
// [sub rsp, N] instruction, like literally just one of those, no searching free
// lists, no metadata headers, no fragmentation, just P U R E  U N C U T pointer
// managment, and freeing is the reverse, the function returns, stack pointer
// moves back, and your HOT TO GO(chapelle roan - good music btw), thats why its
// deterministic, and this is why you wanna use stack allocation on hot paths,
// because as stated before we LOVE THE SMELL OF DETERMINISTIC REPRODUCIBLE
// OUTPUT, which is also part of why i hate java so much, and heap allocation
// tends to be managed through malloc, and as stated above, this is similar to
// the garbage collector in java(icky), but we allow it because cpp is A M A Z I
// N G.
//
// So now that we know why we love the stack, and why the heap is I C K Y for
// alot of what im learning about, were gonna go over why you would EVER use the
// heap, because there are valid reasons to use it, unlike java, dont do java
// kids, java is wack, so, the stack has a few limitations, even though its
// faster than sonic, first, when your compiling your code, YOU HAVE TO KNOW HOW
// BIG IT WILL BE, you cant dynamically cast a size to it at runtime like you
// can with a vector or an array, Mr.Compiler HAS to know how much to move the
// stack pointer, second, the lifetime of the stack is tied to the scope of what
// your using it for, which is a fancy way of saying when the function returns
// the thing you coded it to do, everything on its stack frame is just gone,
// poof, dissappeared, just like my dad, so you cant allocate something in
// function A, AND have function B use it after A returns, the stack pointer was
// already like CYA L8R NERDS, also its small, like typically 1-8mb, depending
// on the OS, if you try to put more than the 1-8mb in the stack, you get the
// mythical *sparkle emoji*S T A C K  O V E R F L O W*sparkle emoji*, but this
// one wont yell at you with elitest ReAd ThE mAn PaGeS, but you also cant
// really copy code from it, so the heap exists for things that need dynamic
// sizing, which is why its managed by *sigh* malloc, and for things that need
// to outlive the function the created them, like using the risk gate functions
// i made where we packed bits into it so manage order flow, or like the order
// book vectors, like in a real system, this would be WAY more complex because
// your building an orderlist at runtime, based on tick level stremaing data,
// instead of building an order book, and then cycling through it, but for the
// example in the code i made, this explains it well enough at the time of
// writing for me, basically if you HAVE to use malloc in a hot path, you wanna
// make damn sure you pre allocate, so you can avoid I C K Y malloc calls during
// trading hours, because that is NO BUENO
//
// so a little side note about the L1 cache, its usually like 32-64kb per core,
// L2 is a little bigger at like 256k-1MB per core, and then L3 is the big daddy
// at like 8-64mb, depending on the processor, the hierarcy for this stuff kinda
// goes as the following:
//
// [Registers]: - bytes, the fastest of the fast, like ricky bobby fast
// [L1 cache]: - 32-64kb - a few clock cycles, which is why we use this for
// orders [L2 cache] - 256kb - 1MB - roughly 10ish cycles [L3 cache]: - 8-64mb -
// like a whopping 100+ cycle(E W)
//
// and the stack gets allocated to these things, based on access patterns, and
// the reason that it stays HOT TO GO in the L1/L2 cache isnt because its
// smaller than the cache, its because of just how predictable the access
// pattern is, and how localized it is, basically, if you continually use the
// data loaded into these, it stays hot, and the CPU keeps track of how often
// where what is used, so it stays at low cycle count, which is SO FIRE YO, and
// to expand on this, since the stack is a top -> down memory access, youre
// ALWAYS working with the data at the top, so the same small region continually
// gets accessed, which means its kept warmer than your lap with a cat on it, so
// now, that we know this, we can go back and take a look at the asm annotations
// that i left in other files, if you looked at those, you may have noticed
// things like push, rbp, mov rbp, rsp,sub rsp, N, these are literally just
// setting up the frame pointer, and reserving space for local variables, and
// then the reverse on return, and part of the reason that using the stack is so
// fast too, is because push and pop are DIRECT instructions to the cpu, its not
// like when you call std::cout, or like another thing that you import from a
// library, that has a whole function behind it, these are LITERAL ASM
// INSTRUCTIONS, and they just move the stack pointer, and because the cpu knows
// where the stack pointer is AT ALL TIMES, unlike your cats, its super duper
// easy peasy to move around, and fast, usually in the rsp register, and just
// writes the value, pop reads the value, and increments rsp, done. damn when i
// write this out like this, asm seems WAY less scary lol, like, when i started
// all this, a for loop used to scare me, and now look at me lol, ive found MY
// PASSION, hopefully you can find yours too, by comparison, push_back() is Y U
// C K Y, especially on a hot path, because its like, you gotta set the stage
// up, you gotta get everyone in their costumes, make sure they all know their
// lines, and then by the time the actual main event starts, youve ALREADY LOST
// THE TRADE, and the stack is just Last in, first out, easy breazy cover girl,
// and its fast and easy, because of the CONSTRAINTS that the stack has to live
// by, whereas heap is just like "yo dude, just done break shit", stack has all
// the rules, and regulations, just like the SEC, in short, the more the CPU
// knows ahead of time, the less work it has to do at runtime, just like me when
// i actually study for am exam, instead of waiting until the night before lmao,
// this is also why java sucks, because you cant do this
//==================================================================================
// TODO[Buddy allocaters, memory managment, bitwise memory managment]
//==================================================================================
//==================================================================================
