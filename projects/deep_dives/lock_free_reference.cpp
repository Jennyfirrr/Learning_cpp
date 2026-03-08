//=================================================================================
// [LOCK FREE CONCURRENCY]
//=================================================================================
// [WHY THIS IS IMPORTANT]
//=================================================================================
// why this exists, so in FoxML_Core, i used file locking along with atomic
// writes, because i ran into issues where a model evaluation phase in the stage
// 0 and stage 1 of the pipline, were attempting to write to files before the
// file existed, and the file would essentially be created AFTER the model
// attempted to write, so nothing actually got output, so like a minor race
// condition issue, but were gonna go way more in depth here, so i actually
// understand this stuff now, this is also gonna be a seperate reference file
// from the other stuff because this involves quite a few siubjects that i
// couldnt really think of how they would fit into other reference files, and im
// sure that once i get into like data feed reading, order evaluation, risk
// gating, order tracking, this will become very important, and as a goal for
// the next actual file, i wanted to implement a basic order tracking system i
// feel like its good to go ahead and start learning about htis, anyways the
// version used in FoxML_Core was likely a mutex pattern, where one thread grabs
// the lock, does it work, and then releases it, everything else has to wait, it
// works but it has a cost, and this cost wasnt a super huge deal, beacuse this
// was just a model training pipeline, so like determinism was important and
// stuff, but latency wasnt a super critical factor when designing it, but the
// execution engine and harness, this is where this stuff actually becomes
// important,but anyways, if the thread holding the lock gets preempted by the
// OS scheduler, which just means the context gets swtiched out, then everything
// else just sits there, doing nothing, like i do in my java class, and in a
// trading system this is actually somehow worse then the garbace collector in
// java lmao, imagine that, we finally found something worse, java devs rejoice,
// lock free basically says, what if nobody every waits, which is perfect lol,
// what if instead of grab the lock, you just try to do the thing, and if
// someone else changed something with the data you were using, you just like,
// try again, just like the little engine that could,
//
// the actual primitive that makes this possible is [std::atmoic] and
// specifically the CAS operation, [Compare and Swap], its as single cpu
// instruction(we like this btw, this is good), (cmpxchg on x86 architecture)
// that does this atomically:
//
// "Look at this memory location, if it still contains the value i expect,
// replace it with my new value, if it changed tell me and ill retry", kinda
// like how i pushed bug testing on end users sometimes for FoxML_Core, <3 my
// unpaid testers, anyways, this is kinda the basic foundation for this, ill
// probably continue to expand this file similar to the other reference files as
// i discover new things, but for now this may be where i stop idk
//=================================================================================
// [BASIC CAS LOOP]
//=================================================================================
#include <atomic>

std::atomic<int> sst_counter{0};

void sst_increment_counter() {
  int expected = sst_counter.load();
  while (!sst_counter.compare_exchange_weak(expected, expected + 1)) {
    // CAS failed - someone else changed the value
    // "expected" is automatically updated to the current value
    // so we just loop and try again
  }
}
//=================================================================================
// SO how this works:
//
// [1] Load the current value into expected
// [2] Call compare_exchange_weak (this is CAS), it basically says, is
// sst_counter still equals expected, swap it to expected + 1
// [3] If it succeeds, we're done, and then we increment it automatically
// [4] IF it fails, that means that another thread changed the value between our
// load, and our CAS,if this happens apparently expected gets automatically
// udpated to whatever the current value actually is now, so you dont even have
// to reload it, it just loops and tries again lol, this is neat, just like a
// try except loop in *sigh* java(I C K Y)
//
// on x86 architectures, this basically just compiles down to a cmpxchg
// instruciton, which is *sparkle emoji*A  S I N G L E  I N S T R U C T I O
// N*sparkle emoji*, no mutex, no syscall on a hotpath(lul imagine), no OS
// scheduler involvment, nothing, just like my dads involvment in my life, and
// the lock prefix basically just means the CPU locks that associated cache line
// for the duration of the instruction, which ties directly back to the
// alignas(64) thing(i need to study that more, im not super clear on that yet),
// but basically you want the atomics on their own cache line, so that the lock
// doesnt accidentally block unrelated data,
//
// compare_exchange_weak vs compare_exchange_strong :
//
// weak can spuriously fail, so like sometimes itll just be like nope, for no
// fucking reason, just like me sometimes lol, #relateable, because on some
// architectures(ARM especially(we dont like arm(intel or amd <3))), the
// underlying instruction is LL/SC(load linked/store conditional), which can
// fail due to the cache-line eviction(i need to study this too), on x86 this
// apparenly doesnt really happen, but who knows, as much as i love the CPU,
// they arnt perfect and silicon does degrade eventually, but weak is apparently
// the preferred operation because the compiler(Praise be), can optimize it
// better, strong ensures that it doesnt just randomly decide to fail, but the
// tax to that is that it has a higher instruction count, so as a general rule,
// weak in loops, strong for one shot attempts,
//
// i wonder if this introduces any variance like branchless vs branch, but thats
// another tangent, anyways this is kinda called a spin loop, if CAS keeps
// failing, you just keep trying, no sleeping(wow this stuff is super
// relateable), no yielding, no wirintg, for a hot path this is exactly what you
// would want or so im told, because sleeping means the OS scheduler gets
// invovled which would introduce so much latency and burn so many clock cycles
// on uneeded operations, but if contention is high, youre burning CPU cycles
// spinning, and thats the trade off, so OS scheduler is I C K Y,
//=================================================================================
// [TODO] [memory_order_relaxed , memory_order_acquire , memory_order_release ,
// memory_order_seq_cst] [Example of CAS in C++], expand on how these connect
// back to cache lines, and fale sharing knowledge due to memory ordering
//=================================================================================
