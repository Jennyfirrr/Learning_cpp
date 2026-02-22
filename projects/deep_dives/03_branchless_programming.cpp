// ============================================================
// Branchless Programming
// (combined from: branchless_programming.cpp, chaining.cpp,
//  return_operations.cpp)
// ============================================================

#include <cstdint>
#include <iostream>
#include <vector>
#include <x86intrin.h> // header for __rdtsc()

// -----------------------------------------------------------
// Branchless Clamp + Two's Complement (branchless_programming.cpp)
// -----------------------------------------------------------

// were expanding on bitwise operators with branchless programming today, i may
// go back and make notes for all the operators, like &, |, ^, <<, >>, ?, and a
// few others before I do this, idk, im seeing where the rabbit hole takes me
//
// N O T E, ? isnt a bitwise operator, i was tired when i added the placeholder,
// its just the correct way to write if/else statements

// branch way, using methods like this are bad in latency critical systems
// because the CPU essentially tries to predcit which way the if/else statement
// will fall, and if it guesses wrong, your down 15-20 clock cycles or so, which
// for most things isnt that big of a deal, but when something as minor as a few
// nano seconds can cost you money, this is critical. The reason this happens is
// because if the predictor is wrong, the entire pipeline has to flush, and
// essentially rebuild these operations until it gets it right, which is B A D
//
// I also found out that another reason branching systems are B A D, is because
// the expose vulnerabilities that can be exploited by the flush cycles to get
// around secuirty measures, but cyber secuirty isnt exactly my interest so I
// probably wont go too into detail for that, just making a note of it here in
// case I wanna go back and study that one day, or someeone else finds it
// interesting. If your interested in that, i suggest looking up the spectre and
// meltdown exploits

/*
int clamp(int x) {
  if (x < 0) {
    return 0;
  }
  if (x > 255) {
    return 255;
  }
  return x;
}
*/

// note to add rdtsc to check how many cycles these actually take to complete,
// im gonna do that at some point, but for now the rabbit hole is telling me to
// watch pokemon

// thankfully, someone smarter than me laid the groundwork to use bitwise
// operators to avoid this issue by using *gasp* M A T H, so by using bit
// manipulation techniques, we can just not use if statements, and mathmatically
// figure it out in a fraction of the time, the example below is also why you
// use unsigned integers, because if you use a signed integer, negative numbers
// have a 1 at the leftmost bit, which removes a bit that could other be used
// for data, and the weird bugs that a signed integer overflow causes, which ill
// probably eventually add to this file, we dont like undefined behavior, almost
// as much as we dont like java, im gonna keep beating this dead horse btw, if
// your using my notes get used to it

int clamp_branchless(int x) {
  x &= ~(x >> 31); // if x is negative, x >> 31 is all 1's, ~ is all 0's
  x -= 255;
  x &= (x >> 31);
  // the inverse again
  x += 255;
  return x;
} // to avoid magic numbers, you could use the parameters passed to the function
  // as the example below

/*
int math_clamp_branchless_i32(int value, int low, int hi) {
  // rest of function, this prevents magic numbers and its more readable and
  // more geared towards a maintainable codebase
} */

// Visual representation of how this works
// Ok, so lets assume x = -5, x >> 31, gives you 0xFFFFFFFF, which is all ones,
// because the shift fills the entire register with the sign bit,
// ~0xFFFFFFFF(not) flips these to all 0's, because its not 0xFFFFFFFF, which is
// 0x00000000, then it compares with the and operator, so -5 & 0 = 0, because
// its clamped to 0, in case 2, x = 300, so x >> 31 returns 0x00000000, ~0 =
// 0xFFFFFFFF, 300 & 0xFFFFFFFF = 300, so it passes through, x -= 255 = 45, x &=
// (x >> 31), 45 is positive, so x >> 31 = 0x00000000, 45 & 0 = 0, x += 255 =
// 255, x = 100, 100 is positive, so it passes through the first gate as 100,
// then x -= 255 = -155, x &= (x >> 31), is -155 >> 31, which ends up being
// 0xFFFFFFFF, so -155 & 0xFFFFFFFF = - 155, then -155 += 255 = 100, THIS IS SO
// COOL WTF LOL, literally just directly manipulating bits, and removing the
// need for if statements, actually W I L D, ill probably come back and add a
// better description, but i added a main function to see how it actually works
// step by step, i gotta let it M A R I N A T E some in mah brain
//
// More indepth explanation without using hexcode: This basically works because
// of Two's complement, and sign extension in a 32-bit system, a positive 5 is
// represented as 00000000_00000000_00000000_00000101, to get the negative
// value, your cpu uses the two's complement, which inverts the bits, then adds
// 1 to the end, so you end up with 11111111_11111111_11111111_11111010, then it
// adds one so you get 11111111_11111111_11111111_11111011 which represents -5,
// Now the reason this reads as a negative number, is because the very first bit
// is the sign bit, and because its a 1, the number is negative. So when you
// tell the computer to perform the x >> 31, if the number is positive, the new
// empty slots on the left fill with 0's, and if it is negative, the fill with
// 1's, so in the example where I use -5, the computer essentially pushes all
// the bits to the right, and the "fall off", and because the origianl bit on
// the left, was a 1, it fills all the empty bit slots with 1's. so after
// shifting 31 times, you end up with 11111111_11111111_11111111_11111111 which
// is 0xFFFFFFFF, as seen above, I just wanted to add an explanation that
// actually shows the binary representation of why this works, so because of the
// way this works, for any SIGNED integer, it will essentially take the sign bit
// and smear it across the entire register, leading to the clamping behavior
// seen in the function above.
//
// Two's complement was created to solve the issue of representing the same
// hardware adder for both positive AND negative numbers. The ideal was getting
// -5 + 5 = 0 to work on the same circuit paths that the positive integers used.
// using 5 as an example, 5 is 00000101, each bit is a power of 2, 00000100 is
// 4, and 00000001 is 1, so those 2 added together using the |= operator, is 5,
// or 00000101, at first thought, using the left most bit to represent a
// negative or positive, like 10000101 for -5 was thought, but then you end up
// with a negative AND a positive zero lol, which apparently just completely
// breaks math, so to represent negative numbers, engineers decided to just flip
// all the bits to the inverse, and add 1, 5 = 00000101, so flipped it becomes,
// 11111010, +1 would be 11111011, when you add these you take 00000101 +
// 11111011, which apparently leaves 10000000, and the carry overflows out,
// which leaves you with 00000000, which is 0 lol, and in the explanation above,
// the left most bit isnt just a flag, it represents -128, in 8-bit, and -2^31
// in 32-bit, and as explained above, it just smears across the entire register
// when using the x >> 31, as outlined there, the way the addition works, is
// like so, using 5 + -5 as an example: 00000101 + 11111011
//
// [1 + 1] = 0, carry a 1
// [0 + 1] + carry 1 = 0 carry a 1
// [1 + 0] + carry 1 = 0 carry a 1
// [0 + 1] + carry 1 = 0 carry a 1
// [0 + 1] + carry 1 = 0 carry a 1
// [0 + 1] + carry 1 = 0 carry a 1
// [0 + 1] + carry 1 = 0 carry a 1
// [0 + 1] + carry 1 = 0 carry a 1
//
// and the final carried one has nowhere to go, so it basically just falls off
// the edge, and youre left with 00000000, using 8-bit as an example here
// because i dont wanna copy the same lines 32 times, the carry falling off is
// integer overflow, and two's complement is designed so that when that happens,
// it A L W A Y S produces 0, this removes the need for software having to know
// or care if a number is negative or positive, because it just takes advantage
// of the limitations of how much information can be stored in a finite space
// within memory
//
// its absolutely C R A Z Y when you realize that all programming languages
// essnetially compile down to operation chains that just do this faster than i
// fall asleep in a java lecture
//
// the __rdtscp() works by initializing the unsigned int aux, and passing it
// through the library call, this stores the processor id, and only lets it
// count up if the id matches due to the & operator

int main() {
  unsigned int aux;
  uint64_t start = __rdtscp(&aux);

  // for better benchmarking, using pthread_setaffinity_np before timing,
  // otherwise it measures scheduler noise as well, once again, small actions
  // like this can happen too foten to actually see in a monitoring tool, and
  // each one adds tons of uneeded cycles

  int result = 0;
  for (int i = 0; i < 1000000; i++) {
    result ^= clamp_branchless(i % 500);
  }

  // this doesnt work because the compiler over optimizes it, gonna add
  // something here later to prevent that using random number selection so the
  // loops isnt removed probably with the flag -o0, or something because i dont
  // really wanna add a ton of code just to benchmark something, FU compiler,
  // the compiler is smart, and essnetially sees that this will calculate the
  // same thing over and over again, so it removes "dead code", because it is
  // optimizing it, as compilers were designed to optimize production code, and
  // this is technically not optimal for production use, but this isnt
  // "production code", its a benchmark, and the compiler is like why waste
  // clock cycles on that, according to what im reading about this, the compiler
  // is smart enough to convert the predicatble i/o into a lookup table during
  // compile time, rather than calculating it during run time,this is why high
  // variance data is important, so the answer cant be guessed ahead of time, by
  // using random numbers as well, the branch predictor cant ever be faster than
  // the branchless version, so it will ALWAYS be slower, and the branchless
  // version will have far less variance in run time, because its just math,
  // instead of adding the pipeline flush when the predictor is wrong, as stated
  // above on why this adds latency.

  uint64_t end = __rdtscp(&aux);

  std::cout << "Average cycles: " << (end - start) / 1000000.0 << "\n";

  return 0;
}

// -----------------------------------------------------------
// Bit Extraction + Branchless Comparator (chaining.cpp)
// -----------------------------------------------------------

int32_t check_first(int32_t num) {
  // this function extracts the bit from the from the bit index[1], while
  // leaving the original number untouched
  int32_t result = num & (1 << 1);
  // the cout here is so you can see what is happening
  std::cout << result << "\n";
  return result;
}

int32_t check_second(int32_t num) {
  // this function extracts the bit from the bit index [2], while leaving the
  // original untouched just like the first
  int32_t result = num & (1 << 2);
  std::cout << result << "\n";
  return result;
}

int32_t smallest(int32_t num1, int32_t num2) {
  // this compares the two bits to find which is the higher bit, sort of a small
  // signal extractor that would be used to unpack data from a bit
  // Apparently the name for this is a branchless comparator chain for comparing
  // bits, there are two ways to kind of use the data from this i guess, the one
  // im showing here is using the positional encoding that s foundin the bit,
  // where result from the prior 2 functions returns the actual value that the
  // bit represents, like 2, from check first, and 4 from check second, you
  // could also return a 0 or 1 by masking it, but im just kind of toying around
  // with this atm, according to what im reading, this is essentially the same
  // pattern used like i stated above or w/e, like imagine that the number 255
  // is an orderbook state, and you have the binary code 0x000000FF, so
  // 11111111, you basically have both the 2, and the 4 position considered
  // active, where these could represent different things, like, "is this bid
  // aggressive" and "is this spread wide", the positional value of the bit that
  // is active could also preserve where the bit is, not just is it set or not,
  // and this is apparently called implicit encoding, where the data carries its
  // own metadata, which further allows for even more functionality in the L1
  // cache memory limitations, because the more data you can encode into a
  // single bit, the faster your system will be, because no ICKY ram hits, we
  // dont like those, ram is for training models, not executing trades lol
  // im probably gonna keep expaning on this file for a bit, but its like 3am
  // lol on a sunday, and im procrastinating doing my java homework(shocker) by
  // experimenting with this lol, using this method though, you basically build
  // an if / else branch, without using branches that essntially do whatever is
  // encoded in num1, assuming that it is bigger than num2, if num2 is bigger,
  // do num2, im sure that there are versions of these out there that use the
  // entire 32bit or even 64 bit register and have it all running in tandem
  // using SIMD or AVX instructions but im just a girl, and a baby HFT(hopeful)
  // engineer, so im L E A R N I N G, there are probably methods to use the
  // outputs from the first 2 functions, to output to a vector<vector<int>> or
  // something, where you can basically preload deicisons or something assuming
  // there is memory for it, to ensure that the hotpath is continually fed and
  // never has down time, but this probably requires a network, or multiple
  // colocated servers operating together, with a central order routing "brain",
  // im not sure how the latency part is figured out, but if your continually
  // caching orders like that, it becomes more about ensuring that you are
  // generating potential deicisons fast enough, so that the vector is never
  // empty(?), im just kind of thinking here, so i have no clue if this is right
  // or not, but saturating an instruction list like that kind of seems optimal,
  // given just how fast these systems are
  //
  int32_t diff = -(num1 < num2);
  std::cout << diff << "\n";
  int32_t mask = diff >> 31;
  std::cout << mask << "\n";

  return num2 ^ ((num1 ^ num2) & mask);
}

// and to expand further on that, since saturating the actual order execution
// pipeline becomes the main challenge, you would probably want a buy vector,
// sell vector, and maybe a hold vector(?), maybe just 2 though so that you can
// retain the benefits of the speed of L1 caching since being able to represent
// things with either a t/f, 1/0 or encoding whatever else information is key,
// and that would make that significantly more difficult, you would also want a
// watcher that keeps track of what is in each vector right? because the goal
// isnt to buy 1 tick and sell 1 tick later, thats a way to get eaten alive by
// slippage cost, and even if your making positive pnl trades on paper, the
// broker fees and taxes per trade would significantly cut into that, so you
// would probably want sell under x conditions, or a target thats like sell x
// ticks later assuming the price is above or below a certain thresh-hold, and
// using these same encoding techniques to watch whats in the vectors and
// compare with that? In that sense it wouldnt really be a pipeline, but you
// could probably group orders by 8 using a 64 bit int, but that leaves alot of
// information out, so the way you pack bits to make that work would have to
// have each bit essentially represent huge amounts of information i think, but
// it would lead to extremely fast execution vs using all 64 bits or 32 bits to
// represent the conditions for a single order, because using a 256 or 512 bit
// register you could fit like, 4 to 8 order books all exeucting and operating
// at once, with each one storing the information for 8 trades, this is fucking
// NUTS lol wtf, how would you even use a model to control this? Would this all
// be just hard coded instructions like buy vs sell given a hard cutoff thats
// programmed? like an inference module wouldnt even be able to keep up with
// that level of data flow lol, like for longer durations, maybe like 1m for
// just a few stocks, and like up to maybe 10-50 stocks, on 5m data, but for
// tick level LOL, the hardware limitations wouldnt allow for inference that
// fast would it? like maybe though, idk, id have to look more into that but
// holy shit thats insane lol, so i guess you use a model to control the hard
// constraints? How often would those change, do you only change them when you
// notice a decrease in performance? OMG THIS IS SO FUCKING COOL LOL I LOVE THIS
//
// like wtf, would you have like an overseer module that monitors performance,
// beacause when missing an order execution by like 3ns could lose you millions
// of dollars, like a human cant monitor that lmao, holy shit this is insane,
// how would you even optimize this lol, like i wanna know SO BAD
//
// if only leetcode was this entertaining lol, this shit is A M A Z I N G, like
// how am i expected to sit there and draw shapes in J A V A(ICK), when stuff
// like this exists lol, this isnt fair to me, can you spell A D D I C T I N G,
// lol like, im H O O K E D
//

#if 0 // main from chaining.cpp
int main() {
  int32_t num;
  int32_t num2;

  std::cout << "Please choose a num to see how this works: ";
  std::cin >> num;
  std::cout << "Please choose a num2: ";
  std::cin >> num2;

  std::cout << smallest(check_first(num), check_second(num2)) << "\n";

  return 0;
}
#endif

// -----------------------------------------------------------
// Branchless Min + ISA + Variance Analysis (return_operations.cpp)
// -----------------------------------------------------------

// im gonna start experimenting with different applications of bitmasking at
// branchless programming here, expect actual use cases and examples for how
// things actually work

int32_t smallest_of_two(int32_t a, int32_t b) {
  int32_t diff = a - b;
  // one thing to note, is that this can technically overflow in some edge
  // cases, so for the mask, ideally you would want to use
  //
  // int32_t mask = -(a < b); this has to be coupled with this return statement,
  // return b ^ ((a ^ b) & mask); apparently this is the industry standard,
  // updated
  //
  // so the way that the -( a < b) works is the a < b returns a bool or either
  // 0, false, or 1 true, so a > b returns a value like 0x00000000, and b > a
  // returns a value like 11111111, i got a little mixed up here, it basically
  // applies like this, (-1)( a < b), if a is bigger, it becomes (-1)(0), and
  // when b is bigger it becomes (-1)(1), my bad, ive been studying alot and got
  // a tad mixed up lol, i was thinking binary bit flipping instead of just
  // normal multiplication, bit manipulation is rotting my brain lmao
  //
  // so a ^ b is
  //
  // 0 0 0 1 0 0 0 0
  // 0 0 0 0 1 0 0 0
  // ---------------
  // 0 0 0 1 1 0 0 0
  //
  // which is 24
  //
  // then you have 24 & mask
  // and since this example we used 16 as a and 8 as b, we are left with a mask
  // that is 0x00000000
  //
  // 0 0 0 1 1 0 0 0
  // 0 0 0 0 0 0 0 0
  // ---------------
  // 0 0 0 0 0 0 0 0
  //
  // which is 0, then you have 8 XOR 0,
  //
  // 0 0 0 0 1 0 0 0
  // 0 0 0 0 0 0 0 0
  // ---------------
  // 0 0 0 0 1 0 0 0
  //
  // which returns 8
  //
  // this is apparently safer but then you dont get to play with your
  // bits(please laugh at this), there are ways to keep the shift mechanic if
  // you dont use this, like upcasting to a 64bit integer before applying the
  // shift, or the addition, but this is just meant for me to go deep on the
  // actual mechanics behind how this operates, and start to build a pattern
  // library and mental model of how the cpu actually does the math, a little
  // bit more on this, in C++, right shifting a negative signed int, is
  // technically implementation defined, thought for the compilers that I would
  // be using if I get hired at the place i want, they will use arithmatic shift
  // or so im told, ill probably learn more about this later, but the arithmatic
  // method preserves the sign bit, in many modern ISA's this apparently
  // compiles down to a setcc ( set on condition) or a conditional move(cmove),
  // which is also *sparkle emoji*B R A N C H L E S S*sparkle emoji*, there is a
  // trick where you can apparently use the absolute value without using the
  // std::abs() call, by doing something like int32_t abs_x = (x + mask) ^ mask,
  // its apparently the same logic where is x is negative the mask is
  // 0xFFFFFFFF, then you add -1, and it flips the bits, if x is positive the
  // mask becomes 0, and nothing changes
  //
  // ISA is the instruction set architecture, which is essentially the contract
  // between the hardware and the software, and it defines what the processor
  // can actually do, the registers it has, memory addressing modes, and the
  // binary format of the instructions, a few different ones would be x86_64
  // (CISC), which can do alot of instructions in one go, like lea(idk what this
  // is, another deep dive probably) + memory indexing, ARM/RISC-V (RISC)
  // simpler fixed length instructions, apparently the code is just more like a
  // "suggestion", and the compiler translates it to the ISA-specific machine
  // code of your specific machine, understanding this helps you predict if the
  // compiler will actually generate branchless code or if it will just use a
  // jump
  //
  // the LEA is the load effective address, apparently its like the swiss army
  // knife of the x86 instruction set, it was originally developed for
  // calculating memory addresses, (base + index * scale + displacement), but
  // because it doesnt use the same arithmatic unit, you can use it to do fast
  // math witout effecting the EFLAGS register, for example
  //
  // lea eax, [ebx + ebx * 2] is a fancy zero latency way to do eax = ebx * 3
  // this is asm, im not very familiar with it but will probably do a deeper
  // dive on that at some point, it seems interesting, because its just directly
  // managing and shifting the values within memory around
  //
  // Arithmatic shift(asr vs lsl), so in this function, there is apparently a
  // difference between a logical shift, and arithmatic shift, a logical shift
  // shifts the bits and fills the gaps with 0's which is apparently great for
  // unsigned math, and arithmatic shift, which shifts the bits, but fills the
  // bits with the SIGN bit, which is exactly the mechanism that enables
  // branchless programming like this, it smears the sign bit across the
  // register n shifts, i went over this in a different file in the deep_dives
  // folder, but im reiterating it here
  //
  // The setcc instructions like setl for set if less, or setz for set if zero,
  // are used to capture the state of the EFLAGS register(status flags) without
  // branching, i have no idea what this is and will probably study it more in
  // detail later, but ill give a brief overview here. You do a cmp a, b , the
  // CPU subtracts them and sets flags (Sign, Zero, Overflow), setl %al stores a
  // 1 bit in the 8-bit register, al if the "less than" flag is set, and a 0
  // otherwise, this is branchless as well, since your just moving a bit from
  // the status register to a general purpose register
  //
  // cmove is apparently the columbiab pure of the HFT world, it was introduced
  // in the pentium Pro era, and it allows the CPU to pick a value based on the
  // condition without a jump
  // an example
  // cmovl %eax, %ebx (move eax to ebx if the last comparison resulted in "less
  // than"), the cpu then actually executes the instructions to calculate both
  // possible values, and then the cmov instruction simply decides which one to
  // keep at the very last stage of the pipeline, no branch prediction, no
  // pipeline flushes, so its G O O D, we like this, we love this, we want some
  // more of this, in cases with highly predictable data, if/else is good
  // for 99.9% of cases, but for what im learning about cmov is THE B E S T,
  // because stocks are not inherently predictable.

  int32_t mask = diff >> 31;
  // so the way this works, is that mask becomes 0xFFFFFFFF if a < b, and
  // 0x00000000 is a >= b, in an example, lets use 8 and 16, where a is 8, and b
  // is 16, the diff value becomes -8, which in binary is 11111001, when you
  // shift everything right 31 places in the mask, you get 0xFFFFFFFF, then when
  // you compare the diff & mask statement it does the following below
  //
  // 1 1 1 1 1 0 0 1
  // 1 1 1 1 1 1 1 1
  // ---------------
  // 1 1 1 1 1 0 0 1
  //
  // then you take 16, which is b, and add -8 to it, because that is what the &
  // operator output after doing the bit matching operation, and your left with
  // 8, which is a
  //
  // suppose a is 16, and b becomes 8, you first apply the diff, which becomes
  // 8, which is 00001000 shift to the right 31 places, and your left with
  // 0x00000000
  //
  // and the diff & mask operator return this value
  //
  // 0 0 0 0 1 0 0 0
  // 0 0 0 0 0 0 0 0
  // ---------------
  // 0 0 0 0 0 0 0 0
  //
  // which is 0, and b + 0 is 8 + 0, so b is the lower value
  //
  // i freaking love this stuff, it is 10:52pm on a saturday, and im losing my
  // mind over bitshifting techniques and branchless programming, when i should
  // probably be doing my homework assignments or leetcode

  return b + (diff & mask);
  // if a < b, diff is negative so the mask is all 1's, you get b + (a - b), and
  // if a >= b, the diff is positive, so the mask becomes all 0's and your left
  // with b + 0, which is b
  //
  // this is FREAKING awsome, because its another way to stop using if/else
  // statements to decide what to do, when you can just use pure bit based
  // operations, while if/else statements may be faster in some cases, pure bit
  // shiting and masking approaches like this are generally fast enough, and
  // have a FAR smaller variance, like if/else can go from like 2ns to 200ns
  // spikes, which is so bad its not even funny lol, but the bit masking
  // approach may only go from like 3ns to like 10ns which is a far more
  // acceptable tolerance, part of the reason that approaches like this are
  // branchless is because the decision is build into the calculation, by virtue
  // of how bit manipulation works, which is still blowing my mind you can even
  // do this just by messing around with 1/0 states, there is an optimization
  // where if you HAVE to use an if statement, you can do something like
  // if(__builtin_expect(order_is_valid, 1)) and this basically tells the
  // compiler to make the code tell the cpu like "hey, this is true 99.9% of the
  // time", and to flag it as the hot path
  //
  // EDIT(22-02-2026) so the reason that the "branchless is faster than
  // prediction branches" is a bad arguement is that it doesnt actually address
  // the reasons that its used, i kinda touched on why, but didnt really go into
  // detail here, as you can see from the above, the branch prediction CAN
  // actually be faster in alot of cases, assuming your data and decision
  // branches are somewhat predictable, im sure theres some tolerance, or ratio
  // where the pipeline flush to faster execution is better, but for stocks, its
  // almost never completely predictable, like yes, stocks could in theory be
  // perfectly predicted if you could watch every person, and every event on
  // earth at the same time, since stock prices are basically just reflections
  // of the actions society takes measured against environmental events, to put
  // it simply, but thats not what this paragprah is about. SO, when your input
  // data is PREDICTABLE within a certain fault tolerance, its actually faster
  // to use if/else statements because of the way the compiler writes that to
  // the machine code, because the compiler is smarter than you in 99.9% of
  // cases lol, BUT, HUGE BUT here, stocks arnt predictable, so you need
  // operations that never use the branch prediction functionality of the cpu,
  // because while faster is good, the pipeline flush is like slamming into a
  // wall at 200mph, and the variance is absoultely HUGE, which is bad and you
  // cant really plan around that, because to predict what the branch predictor
  // is going to do, is just like why?? like why waste compute on that lol,
  // which brings us to the variance of the branchless programming, which in
  // theory should have almost the same execution time, nearly every single
  // time, and only varying by like cpu heat, clock cycles being used by
  // smething else, so the variance is WAY SMALLER, and this means that you have
  // a C O N S I S T E N T  M E A S U R A B L E  F A U L T  T O L E R A N C E,
  // that you can plan around, because random is BAD, we dont like that, we dont
  // want that, but consistency? OMG YES PLS GIMME MORE, because if you know
  // that it will ALWAYS take 10ns to execute an order cycle, then you can plan
  // and structure your execution harness and parameters around that, similar to
  // how in video games, 30ping 80% of the time and 300ping the other 20% is SO
  // ANNOYING, but a consistent 100ms is useable, because you get used to the
  // delay and its C O N S I S T E N T, and since from the way it was described
  // to me, CPU's are essentially state prediction machines, they L O V E
  // consistency
  //
  // I did some more research, and apparently the variance isnt 2ns to 200ns,
  // since a pipeline flush is like 15-20 cycles, so its more like 4-5ns on
  // modern cpu's but the point is still conceptually right, even if my numbers
  // were off, and the buy/sell execution logic isnt exactly right, its more
  // like, with the branch prediciton its basically a coin toss every single
  // time, where you have to pipeline flush on every single miss, which if that
  // happens half the time, youre essentially 50% slower than the branchless
  // way, and not only slower, but you cant really predict when the branch
  // prediction will flush or be wrong, so its not a consistent slowdown, its
  // just random whenever the CPU is wrong, assuming branchless is a consistent
  // 5ns execution time
  //
  // I ALMOST FORGOT, java is bad because it has the GC, which basically just
  // does what it wants to do whenever, which is somehow WORSE than the branch
  // prediction in this context, ok im fine now
}

#if 0 // main from return_operations.cpp
int main() {
  int32_t a;
  int32_t b;
  std::cout << "Please select value a: ";
  std::cin >> a;
  std::cout << "Please select value b: ";
  std::cin >> b;

  std::cout << smallest_of_two(a, b) << "\n";

  return 0;
}
#endif
