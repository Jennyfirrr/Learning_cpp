#include <iostream>

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
}

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
