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
// [EDIT 26-02-26 01:16pm] [added vector preallocation because its mem stuff]
//==================================================================================
// So there was one leetcode problem that I did, where I said that I wasnt sure
// why the optimal solution was faster, SO i decided to look into it.
//
// To start off, when you know the entire vector size, like 1 to n length, its
// faster to precalculate and reserve that memory becaues its like o(1) or o(n)
// or something, and it will never change, because you already know how much you
// need, whereas when you use the vector.push_back(n) operator, it has to
// contintually call a copy, or move, and delete operator, beceause if i
// remember right from what i read, it cant jsut dynamically update the
// locations in memory that it is using, which leads to far more compute time
// than would take by just declaring the entire length you know you will need.
//
// In a similar
// problem
// (https://github.com/Jennyfirrr/Learning_cpp/blob/main/projects/leetcode/missing_nums_1toN.cpp),
// I was using something like vector[nums[i]] to use the value of nums at index
// i, as the index for a counter in the vector array, so like, if nums[1] was 2,
// then the vector[2] array would get a +1 to the value, which while it works
// for finding duplicates, or missing numbers, it also takes far more compute in
// some situations, because you have to perform an entire math operation cycle
// to update the number, when for even faster operations, you can just use
// vector[nums[i]] = 1, to signify that the number of that value exists, this
// doesnt really work for counting the amount of times you see it, but for other
// problems where you just need to verify that that number has been seen, this
// is way faster because you just directly overwrite the existing value with 1,
// instead of performing a math operation cycle.

//==================================================================================
// TODO[Buddy allocaters, memory managment, bitwise memory managment]
//==================================================================================
//[EDIT [27-02-26 09:56pm] BUDDY/ARENA ALLOCATORS, Memory managment, and bitwise
// memory managemnt]
//==================================================================================
// so, isnce its been like a few days since i last did anything actually
// different here instead of just consolidating my vector.push_back() and vector
// allocation notes, and since the last time we found out about how the push and
// pop functions actually influence the stack, and why we love the stack and the
// heap is ICKY, were gonna expand on memory allocation, so that we can avoid
// malloc calls on the hot path, because as we learned last time, malloc is this
// whole affiar and its basically the cpp version, of, and i cant believe im
// saying this, the garbage collector in the programming language that shall not
// be named, so LETS DIG IN
//
// so, the core problem that buddy and arena allocators solve are avoiding
// malloc and new, because these introduce non deterministic latency, and it
// actually takes more than a few clock cycles to resolve and finish those
// instructions, like theyre flexible, and thats the exact reason we dont like
// them, because they have to handle any memory size, from any thread, at any
// time, and free things in any order, this is BAD, we dont like this, we dont
// want this, and we absolutely dont wanna let that happen, you should dislike
// this as much as i do, but if you actually, god forbid it, *like* java(my skin
// crawled after saying that), then you know, its YOUR Life, but ok, lets
// continue
//
// so the arena allocator is the simpler one of the two, the idea is pretty
// straight forward, grab one BIG chunk of memory upfront, just like a business
// loan, and then use that to basically function as a custom malloc or new
// instruction, where it just hands out bits and pieces of it sequentially by
// bumping the pointer forward, kind of like using a single pointer to traverse
// a vector, and just updating the incrememts by i amount every time, or you can
// also think of it like a stack but for heap memory,
//
// [====================[Free Space]====================]
// 	^ <- pointer
//
// [===USED=====|=======[Free Space]====================]
//              ^
//
// [===USED=====|=======USED=======|====[Free Space]====]
// 							 	   ^
//
// see how simple that is, just like a sliding window or traversing a vector,
// like its literally just pointer += size;, which i heard is something alot of
// dudes are insecure about btw, no searching for free blocks, no metadata
// headers, no fragmentation, none of that icky stuff, one addition, completely
// deterministic, O(1) time complexity, just like the *sparkle emoji*P U R E  U
// N C U T*sparkle emoji* pointer management of a stack allocation, it takes the
// exact same number of cycles regardless of the state of memory, so its
// basically like, you preallocate all the memory you have calulated that you
// will nead in the heap, and then you just ration it out using a pointer, im
// sure its a bit more complex when actually going to implement this, but
// conceptually, its pretty simple, and now we get to the bad part, because if
// it sounds too good to be true, it probably is, or at least thats what my mom
// always said, so, the bad part is that you cant free individual blocks, its
// kind of an all or nothing type deal, but for trading, this apparently isnt a
// huge deal, because you keep allocating trade orders, memory gets full, then
// you just flush it in a single clock cycle, and start over, ez pz lemon
// squeezy, so, ot kinda recover all this, its basically define the block of
// memory you want, incrementally ration it out as you need it, then once it
// gets full, you clear it all at once in a single instruction, because the
// pointer is essentially a single address in a register, and then start over
//==================================================================================
// [ARENA ALLOCATOR [example]]
//==================================================================================
//==================================================================================
//
// SO, now that we have that concept started, and we gotta let that sit and
// fester in our minds for a bit, lets start on buddy allocators, and now bear
// with me, because these are a tad more complex, but they also have a wider
// array(pun intended) of uses,
//
// so what happens when you DO need to free individual blocks, thats where your
// friendly neighborhood buddy allocator comes to save the day, the main idea
// behind this, is that you start with a single big block of memory, and for
// this example were gonna use 256kb, and when you have someone request an
// allocation, you keep splitting blocks in half, until you find the smallest
// power of 2 block that fits that request, which is where function like n & n-1
// are useful, becaues you can use these to check a power of 2 in a single
// cycle, or shifting right and left, because shifts are always a power of 2 *
// i, where i is the amount of bits you shift over
//
// start: 256kb
// [==========================256==========================]
//
// [==========128==============|======128==================]
//
// [======64=====|======64=====|==========128==============]
//
// [==32==|==32==|======64=====|==========128==============]
//
// [==XX==|==32==|======64=====|==========128==============]
//     ^ <- what is allocated
//
// so now that we can see how this works using a *gasp* table, when you need to
// go to free a single block that go allocated, it checks if its buddy is also
// free, so you get the 32 and 32 as two buddies, then if the XX is free and the
// other 32 is free, you get the two 64|64 blocks that are buddies, and it kinda
// of just cascdades upwards, assuming the associated buddy is free, until you
// have the entire 256kb block free again, they just keep going until they hit
// the next pair, where one of the buddies isnt free, and what makes this REALLY
// special, is that theyre always powers of 2, and when using bit shifting and
// other bitwise operators, we like powers of 2, because its EASY, computers
// LOVE powers of 2, almost as much as like the austin powers moveies lol, but
// this also means that finding a buddy is just a bit flip, [address ^
// block_size], and if you get 0, BOOM, it fits bette than my ex, no searching,
// no linked list traversal, just a single, quick, painless xor check to find
// the buddies address, but now, for all good things theres also something that
// makes them a choice rather than use this every time, and in this case, since
// it splits memory like this, it tends to get fragmented, which we dont like,
// but this is the cost of keeping everything seperated and deterministic, which
// we accept, because we will do ANYTHING to avoid having to rely on the garbage
// collector in a dead language like java
//
//==================================================================================
// [BUDDY ALLOCATOR [example]]
//==================================================================================
// so apparently this is like a whole ass seperate file you would include
// or something or like an import lol, wtf is this shit, whatever i chose this
// path lmao, and LEARNING IS FUN(java gives me a visceral physical pain)
//
// EDIT: *sparkle emoji*W H A T  T H E  F U C K  I S  T H I S  L M A O*sparkle
// emoji*, like what the fuck did i sign up for, lmao, im stting here actually
// thinking "maybe java isnt so bad", and other lies like that, that my brain
// tells me when im learning something new, LOL APPARENTLY THIS IS A SIMPLE
// VERSION AND STUFF LIKE jemalloc or tcmalloc are like tens of thousands of
// lines lmao, why am i doing this, what is wrong with me, W H Y, do i give up
// on my dreams, do i learn to like java(lmao lets be real), do i play "whos
// that pokemon", who knows lol, i actually didnt expect to get this far lmao,
// like what the actual fuck is this
//
// EDIT2: im gonna finish this, just like give me a minute im having a whole ass
// existential crises and looking back on my life to figure out the exact
// choices that have led to me dsicovering buddy allocators lol
//==================================================================================
#include <array>
#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
//==================================================================================
// [CONSTANTS]
//==================================================================================
static constexpr uint32_t BUDDY_MIN_ORDER = 4;  // 16 bytes min block
static constexpr uint32_t BUDDY_MAX_ORDER = 20; // 1MB max block
static constexpr uint32_t BUDDY_NUM_ORDERS =
    BUDDY_MAX_ORDER - BUDDY_MIN_ORDER + 1;
static constexpr uint32_t BUDDY_POOL_SIZE_BYTES = (1u << BUDDY_MAX_ORDER);
static constexpr uint32_t BUDDY_SENTINEL = 0xFFFFFFFFu;

//==================================================================================
// [TYPES]
//==================================================================================
struct BuddyFreeNode {
  uint32_t next_offset;
  uint32_t prev_offset;
};

struct BuddyAllocatorState {
  alignas(64) uint8_t pool[BUDDY_POOL_SIZE_BYTES];
  uint32_t free_lists[BUDDY_NUM_ORDERS];
  uint8_t split_bitmap[BUDDY_POOL_SIZE_BYTES / (1 << BUDDY_MIN_ORDER) / 8];
  uint8_t alloc_bitmap[BUDDY_POOL_SIZE_BYTES / (1 << BUDDY_MIN_ORDER) / 8];
  uint64_t total_alloc_bytes;
  uint64_t total_free_bytes;
  uint32_t alloc_count;
  uint32_t free_count;
};

//==================================================================================
// [INTERNAL HELPERS] (buddy_internal_*)
//==================================================================================
[[nodiscard]] static inline uint32_t
buddy_internal_size_to_order(size_t const size) noexcept {
  uint32_t const min_size = (size < (1u << BUDDY_MIN_ORDER))
                                ? (1u << BUDDY_MIN_ORDER)
                                : static_cast<uint32_t>(size);
  // round up to the next power of 2
  uint32_t const rounded = std::bit_ceil(min_size);
  return static_cast<uint32_t>(std::countr_zero(rounded));
}

[[nodiscard]] static inline uint32_t
buddy_internal_order_to_size(uint32_t const order) noexcept {
  return 1u < order;
}

[[nodiscard]] static inline uint32_t
buddy_internal_buddy_offset(uint32_t const offset,
                            uint32_t const order) noexcept {
  return offset ^ buddy_internal_size_to_order(order);
}

[[nodiscard]] static inline uint32_t
buddy_internal_bitmap_index(uint32_t const offset,
                            uint32_t const order) noexcept {
  return (offset >> order);
}

[[nodiscard]] static inline uint32_t
buddy_internal_bitmap_set(uint8_t *const bitmap, uint32_t const idx) noexcept {
  bitmap[idx >> 3] |= static_cast<uint8_t>(1u << (idx & 7u));
}

static inline void buddy_internal_bitmap_clear(uint8_t *const bitmap,
                                               uint32_t const idx) noexcept {
  bitmap[idx >> 3] &= static_cast<uint8_t>(~(1u << (idx & 7u)));
}

[[nodiscard]] static inline bool
buddy_internal_bitmap_test(uint8_t const *const bitmap,
                           uint32_t const idx) noexcept {
  return (bitmap[idx >> 3] >> (idx & 7u)) & 1u;
}

//==================================================================================
// [FREE LIST OPS] (buddy_freelist_*)
//==================================================================================

//==================================================================================
//==================================================================================
//==================================================================================
//==================================================================================

//==================================================================================
//==================================================================================
// so, within the context of HFT, which is what i love, alot of systems use
// a hybrid approach apparently in actual production code bases, arena
// allocators for the hot path, where youre processing batches of orders,
// allocate is always forward, never free individually, reset when the batch
// is done, and this actually makes sense, because every operation,
// including the reset, is O(1), wheras the buddy system is O(log n), just
// like a binary search, and you could probably kinda think of this like a
// binary serch for the actual memory needed, then a reverse binary search
// when freeing it, so while fast, its not SUPER fast, but it works for
// avoid the icky heap, some examples of what to track within this is like
// position tracking, or connection state, you wanna store longer lived
// objects here, that need individual lifetime managment
//
// one key note is to pick which allocator to use is based on the lifetime
// patter of your data, and not just size(similar to guys), orders that live
// for microseconds? A R E N A, Positions that live for hours? B U D D Y or
// a P O O L allocator, the schema that you define for your data dictates
// which allocator is a better selection
//
// now, we gonna talk about pool allocators because i know we all love the
// pool, and i was told this one would feel natural to me, because ive been
// using a similar concept to pack orders into integers, anyways a pool
// alocator is for when you have a bunch of objects that are the same size,
// kind of like a parking lot, or order ID's, because theyre *sparkle* A L W
// A Y S 8 bits, but anyways the reason they are all the same size, is so
// you can beasically check if one is used by using a single bit to track
// the state of it, you pre allocate a large block of memory, and divide it
// into N equal sized slots, so you get something that kinda looks like
// this,
//
// Slots: [0][1][2][3][4][5][6][7]
// bitmap: 1  1  1  1  1  1  1  1  (ones represent free here)
//
// Allocate slot:
// index = __builtin_cztll(bitmap); (find first free)
// bitmap &= ~(1ULL << index); (mark used)
//
// Free slot:
// bitmap |= (1ULL << index); (mark free)
//
// so, its probably kinda obvous why this one is ALREADY my favorite lol,
// finding a free slot is a single intrinsic, allocating is one AND
// operator, and freeing a spot is a single | operator, All O(1), all
// branchless, all deterministic, and with a single uint64_t integer, you
// can track a WHOLE GRADE A 64 SLOTS, for the price of a single register,
// isnt that amazing, usually you can only get deals that good at costco,
// but here we are, god i wish i could split this stuff into lines and snort
// it i love it so much, imagine if you could do this in java lol, id
// probably pay atteniton in class, so where i mentioned that its a single
// intrinsic, this basically compiles down to a single tzcnt or bsf
// instruction, A SINGLE CYCLE to find a free slot out of 64 WHOLE ASS
// MEMORY LOCATIONS, javas garbage collection would probably be actually
// good if it used this kinda tech, but we know that will never happen lul
//==================================================================================
// [POOL ALLOCATOR[example]]
//==================================================================================
// [EDIT [02-03-26 02:18am]]
//==================================================================================
// so, when using the example below, you can do something like replacing the
// uint64_t *slots, with something else
/*
struct OrderPool {
  OrderPair *slots;
  uint64_t bitmap;
  uint32_t capacity;
};
static_assert(sizeof(OrderPool) == 24, "struct must be 24 bytes");
*/

// so like you can see how I used another struct to make up part of this
// struct, to pass the actual order values you would want to store right,
// and where you would have the *, like in the example below, the
// pool->slots line, you would replace the size of blocks, and the uint64_t
// * slots with OrderPair, idk just an example of how to actually pass real
// order values to this, im eventually gonna get around to storing real
// data, but thats probably a ways away, and ill need to figure out a way to
// encode order values, and add a reading harness where the orderstream data
// is actually read, and decisions are made, this will probably start with
// basic startegies like SMA cross over, with cutoff points like 30, and 70,
// or simple macd strategies just to serve as examples, you would also
// replace the parts of the below where it says uint64_t *slot_ptr, with the
// associated struc that contains the values you wanna track, and in the
// example in file 09, im using a simple 8 byte struct to track 4 buy and 4
// sell orders, and there is a more indepth explanation in the actual file,
// but i wanted to touch on that here, because this is gonna lead up into
// the next file im gonna try, which is actually a primitive order storage
// and tracking system that executes based on like 1- 2 different variables
// or something, nothing too crazy, and definitely not actual full blown
// models that like FoxML_Core would produce, but i have some things i need
// to rework in that anyways, because ive had some new thoughts and stuff
// and learned some new things after doing something else for a while, im
// also probably gonna add exmaples of the arena allocator and the buddy
// allocator here at some point, but i learn best when i acutally have a
// reason to try to and learn these, instead of making an example an using
// it later, it just sticks better when i Have a real reason to use it,
// which is what makes java so hard lol, like cmon seriously? me? write
// java?, lets be for real
//==================================================================================
/*
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
*/
//==================================================================================
// to use this you wanna do something like this
//==================================================================================
// 1. grab a slot
// uint64_t *slot = OrderPool_Allocate(&pool);
//
// 2. pack orders and store in that slot
// *slot = order_packing_8byte(buy_side_orders, sell_side_orders);
//
// that kinda makes sense, the *slot just derefernces the pointer, "write
// this value into the location the slot points to", the pool gives you
// where to put it, and the packing function gives you what to put there, to
// read it back later, you just wanna do something like:
//
// uint64_t packed = *slot;
//==================================================================================
// [NEXT] [filling this in as i go]
//==================================================================================
//==================================================================================
//==================================================================================
