#include <iostream>
#include <random>
#include <vector>

std::vector<uint64_t>
build_order_book(const std::vector<uint8_t> &potential_trades,
                 uint32_t queued_orders) {
  std::vector<uint64_t> order_book_packed(queued_orders / 8);

  uint64_t order_bit_packed = 0;

  for (int i = 0; i < (int)order_book_packed.size(); i++) {
    order_bit_packed = 0;

    for (int j = 0; j < 8; j++) {
      order_bit_packed |= potential_trades[i * 8 + j] << (j * 8);
      // so, aparently theres a better way to do this by using BMI1/2, or PDEP,
      // which is parallel bits deposit, it was brought to my attention that
      // using a signed int can cause bit smearing, and stuff, but were using
      // uint so thats not a huge issue here, this is more so to turn this into
      // a single for loop i think, so basically what PDEP does, is that it
      // takes a source value and a mask,and it takes the low-order bits, from
      // the the source, and deposits them into the positions where the mask has
      // 1's, so apparently you can deposit like 4 - 8 different values
      // depending on the mask, im not too sure how this would build just using
      // individual 8bit orders, but i guess i ll find out, so for this trick,
      // you apparently need
      // #include <immintrin.h> which is for _pdep_u64, and _pext_u64
      // #include <cstdint>
      //
      // the actual code looks something like
      //
      // const uint64_t mask = 0xFFFFFFFFFFFFFFFF;
      // return _pdep_u64(source_data, mask);
      //
      // damn, well thats like super simple wtf lol, like why am i doing all
      // this when i can just do that lmao, always taking the harder route lmao,
      // FML, like wtf why lmao, and apparently this is like 1-2 instructions
      // instead of 10 XD, jesus christ, why do i make things so hard, ALWAYS
      // OVER THINKING, im sure we can all relate to that lol, so apparently
      // these have a constant latency on modern intel core(idk about amd), and
      // its usually like 3 cycles apparently so about the same as an imul
      // instruction, and this lets you avoid loops, but im pretty sure if the
      // loop is simple enough it doesnt really matter, because the compiler
      // will just unroll the loop anyways, still good to know though
      //
      // so if _pdep_u64 acts as the depositer, then _pext_u64 works to unpack
      // the orders, so for this you can just define a mask ONCE AGAIN, like so:
      //
      // const uint64_t mask = 0xFFFFFFFF00000000;
      // return (uint32_t)_pext_u32(packed_order, mask);
      //
      // like, i feel like im getting a history lesson as well when learning
      // about this, learn the hard way, spend hours breaking it apart, writing
      // a fucking thesis, and then gemini is just like, HEY GIRL, DID YOU KNOW
      // ABOUT PDEP AND PEXT, and im like nah whats that, and hes like lmao, so
      // your gonna love this, so now im just sitting here questioning life
      //
      // so using this apparently removes the chance a loop just doesnt get
      // unrolled, so it makes the compiler deterministic, and apparently i just
      // just unpack the entire 64bit integer at once using this lol, FML lmao,
      // brb gonna go jump out my window real quick(i kid), like seriously this
      // turns what was like a 4-6 line function, into a function declaration
      // and a return statement lol, like i wanna see how this compiles down now
      // lol, because this just blew my mind, also im apparently required by law
      // to have a vendetta against signed integers now since i wanna work in
      // HFT, because sign bits are I C K Y, ill probably try to make a
      // branchless select next, but i think i kinda already did that using
      // __builtin_popcount(), but it uses a for loop which is gross, so ill
      // probably look into ways to optimize that further just using my new
      // favorite thing which is B O O L E A N  A L G E B R A, beacuse seeing
      // those binary tables just makes me feel all warm and fuzzy inside,
      //
      //=========================================================================
      //[NOTE] branchless risk gate using mycroft to avoid branches, turn bad
      // trades into 0x00, note to continue that later
      //=========================================================================
    }
    order_book_packed[i] = order_bit_packed;
  }
  /*
          .p2align 4
  .globl	_Z16build_order_bookRKSt6vectorIhSaIhEEj
  .type	_Z16build_order_bookRKSt6vectorIhSaIhEEj, @function
_Z16build_order_bookRKSt6vectorIhSaIhEEj:
.LFB3722:
  .cfi_startproc
  pushq	%r13
  .cfi_def_cfa_offset 16 | if i remember right this stuff is all the stack
managment for the registers right? using the DWARF unwinders, yeah this is the
CPU tracking the stack frame pointer, but omitting it and not using the rbp
register, because this is apparently better and frees up a register

  .cfi_offset 13, -16
  pushq	%r12
  .cfi_def_cfa_offset 24
  .cfi_offset 12, -24
  pushq	%rbp
  .cfi_def_cfa_offset 32
  .cfi_offset 6, -32
  movq	%rdi, %rbp
  pushq	%rbx
  .cfi_def_cfa_offset 40
  .cfi_offset 3, -40
  subq	$24, %rsp
  .cfi_def_cfa_offset 64
  shrl	$3, %edx
  je	.L13 | AND WE HAVE OUR FIRST JUMP, but its ok beacuse this is just
simulated order generation

  leal	0(,%rdx,8), %ecx | either fast math or memory managment, not using imul
because this is calcualted seperatly from the main cpu logic, because lea has
its own math unit, but its highly constrained, so you can only get things like
1,3,5,9, etc, i was wrong about these numbers you only get like 1 + base2,
4,6,8, etc, so 3, 5, 9

  movl	%edx, 12(%rsp) | its kinda neat being able to look at this and realize
where its using 32 bit registers, and 64 bit registers

  movq	%rsi, %r12
  movq	%rcx, %rdi
  movq	%rcx, (%rsp)
  call	_Znwm@PLT
  movq	(%rsp), %rcx
  movq	%rax, 0(%rbp)
  movq	%rax, %rbx
  leaq	8(%rax), %rdi
  leaq	(%rax,%rcx), %r13
  movq	$0, (%rax)
  movq	%r13, 16(%rbp)
  cmpl	$1, 12(%rsp) | this is a compare, but i wonder why its 12? thats weird,
considering that the values i used in the for loop are mulitiples of 4 or 8 | so
this means that the 12 is likely the stack offset where the variable got stored,
because 12(%rsp) just means 12 bytes from the stack pointer, which is the 3rd
4-byte local variable slot, [0,4,8,12], this isnt related to a loop, its just
where the compiler stored a variable

  je	.L14
  leaq	-8(%rcx), %rdx | i think this is the for loops being counted up into
counting down because you skip and extra clock cycle when doing subtraction
loops because it skips the DIV call i think? | well not skipping a div call, but
subtraction skips an extra operation because when it hits 0, the Z flag is set
implicitly, so basically a free comparison

  xorl	%esi, %esi
  call	memset@PLT | i should probably add a section in the mem_stuff file about
memset calls

  leaq	-8(%r13,%rax), %rdx
  subq	%rbx, %rdx
  movq	%rdx, %rax
  movq	%rdx, 8(%rbp)
  subq	%rbx, %rax
  sarq	$3, %rax
  testl	%eax, %eax
  jle	.L1
.L5: | yeah the 2nd ret function returning here after it, must be the actual
integer being loaded into the vector, because why else would it jump back to
where a loop starts?

  subl	$1, %eax
  movq	(%r12), %r9
  xorl	%edi, %edi
  leaq	8(,%rax,8), %r8
  .p2align 4
  .p2align 3
.L8:
  leaq	(%r9,%rdi), %rdx
  xorl	%ecx, %ecx
  xorl	%esi, %esi
  .p2align 5
  .p2align 4
  .p2align 3
.L7:
  movzbl	(%rdx), %eax
  addq	$1, %rdx | so i guess this is where the for loops start, because you can
see it add one every time it jumps

  sall	%cl, %eax
  addl	$8, %ecx
  cltq
  orq	%rax, %rsi
  cmpl	$64, %ecx
  jne	.L7
  movq	%rsi, (%rbx,%rdi)
  addq	$8, %rdi | and then you have an add quad here adding 8 to the rdi
register, which is the 64bit data register, because were shifting and |= values
into a uint64_t integer
        order_bit_packed |= potential_trades[i * 8 + j] << (j * 8);
its associated with this line of code

  cmpq	%rdi, %r8
  jne	.L8
.L1:
  addq	$24, %rsp
  .cfi_remember_state
  .cfi_def_cfa_offset 40
  movq	%rbp, %rax
  popq	%rbx
  .cfi_def_cfa_offset 32 | then weve got snowwhite and the 7 dwarves here
tracking our stack location for us

  popq	%rbp
  .cfi_def_cfa_offset 24
  popq	%r12
  .cfi_def_cfa_offset 16
  popq	%r13
  .cfi_def_cfa_offset 8
  ret | im not too sure why there is a ret instruction here, necause this isnt
the end of the function, maybe this returns the stack, and loads another
variable since its pulling from a vector? | OH MAYBE THE STUFF BELOW THIS IS
WHAT ACTUALLY GETS RETURNED BY THE FUNCTION, AND THIS IS THE VECTOR BEING BUILT

.L13:
  .cfi_restore_state
  pxor	%xmm0, %xmm0 | and this stuff is the compiler initializing the 3
pointers that sets up a vector which are the begin, end, capacity

  movq	$0, 16(%rdi)
  movq	%rbp, %rax
  movups	%xmm0, (%rdi)
  addq	$24, %rsp
  .cfi_remember_state | and like i wrote about yesterday, youve got the DWARF
unwinder here storing the state of the stack, and using the restore and remember
functions to swap qiuckly between states, this shits so cool lol

  .cfi_def_cfa_offset 40
  popq	%rbx
  .cfi_def_cfa_offset 32
  popq	%rbp
  .cfi_def_cfa_offset 24
  popq	%r12
  .cfi_def_cfa_offset 16
  popq	%r13
  .cfi_def_cfa_offset 8
  ret | this is weird there are two ret instructions within this? i guess one of
them is what outputs into the vector, then once the vector is initialized, this
one actually returns the vector from the function?

.L14:
  .cfi_restore_state
  movq	%rdi, 8(%rbp) | and i guess this is move a quad from the rdi register to
8 bytes from base pointer, because the registers with b in them are the base
pointer i thnk, it uses these when it doesnt use the .cfi_def_chk fla DWARF
unwinders

  movl	$1, %eax
  jmp	.L5 | that may be what this does, because this jumps back to a for loop,
so maybe one of the ret instructions is the actual integer being loaded into the
vector, then once all the loop conditions are satisfied, then it actually
returns the vector from the function?
    order_book_packed[i] = order_bit_packed;
im guessing this is where this ret function ends? because thats when the loop
restarts

  .cfi_endproc
.LFE3722:
  .size	_Z16build_order_bookRKSt6vectorIhSaIhEEj,
.-_Z16build_order_bookRKSt6vectorIhSaIhEEj
  */

  return order_book_packed;
}
// for this order packing im not realy using a different method from the last
// time, im just packing a random distribution of numbers between 0-255 in each
// byte, and shoving them all in an unsigned 64 bit integer, because as we
// remember from last time, when you use signed ints, you lose the 32nd bit
// because you have to specify if its a positive or negative number, which for
// this stuff, basically means you lose an ENTIRE 8 BITS TO NOTHINGNESS, how
// dumb is that, can i get an ICKY

uint64_t risk_gate(uint8_t risk_gate_sell, uint8_t risk_gate_buy) {
  uint64_t risk_gate_built = 0;

  // i wanted to try packing 4 buy and 4 sell ID's here
  // gonna write more on this later, E X P E C T  A N  E S S A Y  L O L,
  // so this is basicaly implicit endoding, where you have a buy AND sell side,
  // of the integer, instead of wasting a single bit to specify that, you can
  // just implcilty encode which half of the integer a byte falls on to
  // communicate that information, how cool is that?? LIke java would be way
  // more fun to learn if you could do crazy stuff like this, but its all stiff
  // and boring and corporate, like when i think of java, its just like
  // everything is grey, and black and white, but coding in cpp feels like the
  // world has ACTUAL saturation and colors and vibrance, anyways, java bad,
  // this could also be considered schema based development, because you use a
  // schema or data format, that basically sets the imlpication for how data is
  // structured and flows throughout the entire program, its pretty NEAT, id
  // reccomend reading about it, its one of the main tenets that i tried
  // developing the trading and research pipeline with, but that was mostly done
  // using ai to code it because, it was python, and while i respect python as a
  // language, because it is VERY USEFUL, i personally feel slight offense when
  // having to write it myself, but nowhere close to how offended wirting
  // java(icky) makes me feel
  //
  // the functions below are pretty self explanatory, but thats basically how
  // the buy and sell sides of the integer are defined, where the sell is
  // defined as the values of i*8 where i is less than 4, for the begining bit
  // index of the integer, and the buy side is defined as i*8 + 32, where i is
  // less than 4, for the sell side, because this will start the 0 index bit at
  // bit position 32, because the last sell order position ends at bit 31, isnt
  // that neat?
  //
  // but basically when you selection the buy/sell side risk gate ID's, its
  // between 0 -> 255, because thats the maximum for an 8 but number, and then
  // we just take that binary code, and shove it to the desired bit index, god i
  // swear its like i was made for this

  for (int i = 0; i < 4; i++) {
    risk_gate_built |= risk_gate_sell << (i * 8);
  }
  // 0 -> 31 are sell bytes

  for (int i = 0; i < 4; i++) {
    risk_gate_built |= risk_gate_buy << ((i * 8) + 32);
  }

  // 32 -> 63 are buy bytes

  return risk_gate_built;
  /*
          .p2align 4
  .globl	_Z9risk_gatehh
  .type	_Z9risk_gatehh, @function
_Z9risk_gatehh:
.LFB3732:
  .cfi_startproc
  movzbl	%dil, %edx | im guessing this is where the function inputs are
grabbed and moved into the appropriate registers? | so this is move zero-extend
byte to long, which takes the lowest byte of %rdr, which is the first function
argument like i guessed, and zero extends it into the full 32 bit register, so
since were passing a uint8_t into this function, its passing in a 0xAB, and then
extending it to 0x000000AB

so mov = move, z = zero-extend, fill the rest of the integer with zeros
extending from the right to left b = source is a byte, and l means destination
is a long

  movl	%edi, %eax
  sall	$24, %eax | so this is weird, this is obviously the first for loop, im
not sure what the second is, imul adds that large number from rdi to rdx, for
some reason, im guessing thats the limit of like 16bit integers? so i guess its
doing something weird where for the second loop, its doing some weird thing
where its maxing out a 16bit integer per loop, or adding it to the above?
im pretty sure that sall means shift all?, so the sell side gets handled by
this, so its the first for loop

  imulq	$65792, %rdx, %rdi | ah so i was wrong, that number is 0x10100, which is
apparently 1 + (1 << 8) + (1 << 16), so its basically using this to cast the
risk_gate_buy/sell into multiple bytes at the same time | OH COOL, it took both
for loops, and converted it into de-bruijn style multiplication, i should
probably do a more indepth reference for that under bool algebra, wow, so i
guess its basically doing both for loops in like a single 3 clock cycle
instruction? because im pretty sure that imul uses only like 3 cycles or
something on most cpu architectures, god i love this shit more than drugs, and
the buy side is handled by this neat little trick, i should probably trace this
out in a binary table, because seeing how this actually works would be kinda
interesting

alrighty here we go lmao

so lets assume that a buy order is like, 0x000000AB right, and were multiplying
that by the magic number which happens to be 0x00010100, and apparently when you
trace this out, you end up with something like, 0x00AB00AB00, or something close
to this from what im being told, im definitely gonna add this to the bool
algebra file, this is SO worth taking a deeper look at

  for (int i = 0; i < 4; i++) {
    risk_gate_built |= risk_gate_sell << (i * 8);
  }
  // 0 -> 31 are sell bytes

  for (int i = 0; i < 4; i++) {
    risk_gate_built |= risk_gate_buy << ((i * 8) + 32);
  }

  // 32 -> 63 are buy bytes
 this is so cool lol, it converts these lines of code into whats essentially a 2
line instruction lol wtf, i wish i was as smart as the compiler

  cltq
  orq	%rdx, %rax | im guessing this is where the risk_gate_buy and sell
actually get |= into the main integer, so yeah i was right about these parts, im
not sure how the sell side gets converted to this, but if you look at the lower
one, the registers tend to match, and %rax is the return register, so yeah, the
first one is the sell side, and then the imul instruction outputs to the one
below

  orq	%rdi, %rax
  ret
  .cfi_endproc
.LFE3732:
  .size	_Z9risk_gatehh, .-_Z9risk_gatehh
*/
}

/*
uint32_t calc_laneMatchCount(uint64_t packed_order, uint64_t risk_gate) {
  uint64_t diff = packed_order ^ risk_gate;

  uint64_t matches =
      ((diff - 0x0101010101010101ULL) & ~diff & 0x8080808080808080);

  return __builtin_popcount(matches);
}
*/
// ALRIGHTY THEN, were starting halfway through the file this time lol, but
// anyways, this WOULD work, assuming that the int that im packing would be all
// the same type of order group, so like 8xsell orders or 8x buy orders, but i
// wanted to try packing 4 buy and 4 sell in the same integer because i wanna
// feel S P E C I A L, so i had to modify to something else, which is the thing
// below
//
// so, apparently this version below is the RIGHT version, so i get to be
// special AND RIGHT, i guess it cant get any better than that can it lol,
// anyways, assuming you use the top thing, its basically checking to see if
// there are EXACT matches, which for extremely strict order conditions would be
// ideal, but if you just need certain instances to check, other methods may
// work better, but for strict matches, the above and below functions are
// probably what you would wanna use i think, i think in the one where i was
// packing 4 orders into a 32 bit integer, i was using something like
// (packed_order & mask) == mask, which is basically a way more permissive
// version of this, thats only checking for a single bit, or a set of bits
// rather than all the bits, i was gonna take a day or two off of doing these
// because 15-20k words is, something lol, like lemme tell ya, that shit wore me
// out, but my brain just like wont let me stop, so here we are again, im
// probably gonna add the asm breakdown to this as well, because idk, that stuff
// is just super interesting to analyze and see how the compiler auto optimizes
// some things, i F E E L like it makes me a better programmer, but who knows,
// im bad at this stuff anyways

uint32_t calc_laneMatchCount64(uint64_t packed_order, uint64_t risk_gate) {
  uint64_t xor_result = packed_order ^ risk_gate;
  // this basically just XOR's the packed order with the risk gate i set up, and
  // then it comapres that result with the logic down there using 0xFF, pretty
  // simple stuff, im probably gonna add the binary algebra table because that
  // stuff is just kinda fun as weird as that sounds, it makes my brain tingle
  // in that way i like
  //
  // so the way this works, is you take the packed order, and the build risk
  // gate, and because both are unsigned 64bit integers, you can just xor it,
  // and then you take that result, and you keep shifting it to the right by a
  // multiple of 8, starting at 0, so you end up with something llike this
  // pattern
  //
  // 0xF0F0F0F0F0F0F0F0 & 0xFF, and you get something like 0xF0, because none of
  // the bits in the first 4 match, then you gotta continue down the chain, so
  // the next iteration would be something like this
  //
  // 0x00F0F0F0F0F0F0F0  & 0xFF, and because im using the same numbers its just
  // 0xF0 again, because thats what matches, but whats more important, is that
  // because it doesnt == 0, its not a match, so it doesnt count, i didnt really
  // put too much thought into this part though, so defining what counts as a
  // buy and what counts as a sell would need more implications defined on code
  // or out put or executions based on this information, but remember I M  J U S
  // T  A  G I R L, and a baby(hopeful) HFT engineer, i think in this specific
  // example, im counting matches of 0 as a confirmed trade, but a more
  // realistic simulation would probably be setting it equal to something else,
  // but thats not the main idea behind this file, because thats just setting a
  // definition for what YOU want to be considered a successful trade within the
  // implications defined by whatever schema you set

  uint32_t count = 0;
  for (int i = 0; i < 8; i++) {
    count += ((xor_result >> (i * 8)) & 0xFF) == 0;
  }
  return count;
  /*
                  .p2align 4
          .globl	_Z21calc_laneMatchCount64mm
          .type	_Z21calc_laneMatchCount64mm, @function
  _Z21calc_laneMatchCount64mm:
  .LFB3733:
          .cfi_startproc
          xorq	%rsi, %rdi | well these are obviously the XOR statments, idk why
  it has 3 though, im guessing each one represents each variable within the
  statement? where this is the actual comparison, and then the lower two are
  each of the variables being compared?

          xorl	%ecx, %ecx | so i was wrong about these, this is the count
  integer, and the loop counter being intialized, so the actual comparison is
  handled by the above, and then the 2 variables needed for the loop and count
  are set up here

          xorl	%eax, %eax
          .p2align 5 | i guess this has something to do with how the variables
  are passed into the function?

          .p2align 4
          .p2align 3
  .L17:
          movq	%rdi, %rdx | yeah, the xorq must be the actual result of the ^
  operation, because its moving the outfrom there into a data register, then
  using it in the part with the loop

          shrq	%cl, %rdx
          cmpb	$1, %dl | so apparently this is something neat, instead of
  checking if the result == 0, its using the carry flag from the compare to add
  either a one or a zero, which is branchless, i cant tell if i hate or love the
  compiler at this point

          adcl	$0, %eax
          addl	$8, %ecx | i guess this is the part where it basically takes the
  i * 8 part, and the for loop capping at 8, and basically turns it into the for
  loop ending once it hits 64, and adding 8 every loop to remove that
  mulitiplication from the count line, to save on instruction cycles because
  this reduces the amount of cycles it takes

          cmpl	$64, %ecx
          jne	.L17 | so it looks like this didnt get unrolled into a single
  set of instruction, its having to loop

          ret
          .cfi_endproc
  .LFE3733:
          .size	_Z21calc_laneMatchCount64mm, .-_Z21calc_laneMatchCount64mm
  */
}

// the logic in main is basically all the same from the 05 packing file, the
// only real difference is mainly in the lane count logic, and the way the risk
// gate is built, other than that this is mostly a copy and being done to build
// reps and deeper understanding behind how these patterns actually work,
// because i dont wanna just spend my life developing CRUD apps, i sleep in
// java, this stuff makes me feel alive, even if it is HARD

int main() {
  uint32_t generate_x_orders;
  uint32_t temp_kill_mask_buy;
  uint32_t temp_kill_mask_sell;

  std::cout << "Generate orders amount: ";
  std::cin >> generate_x_orders;

  generate_x_orders = generate_x_orders * 8;

  std::cout << "Select a kill mask ID, buy side[0-255]: ";
  std::cin >> temp_kill_mask_buy;
  std::cout << "Select a kill mask ID, sell side[0-255]: ";
  std::cin >> temp_kill_mask_sell;

  uint8_t kill_mask_buy = static_cast<uint8_t>(temp_kill_mask_buy);
  uint8_t kill_mask_sell = static_cast<uint8_t>(temp_kill_mask_sell);

  uint64_t risk_gate_built = risk_gate(kill_mask_sell, kill_mask_buy);

  std::mt19937 rng(42);
  std::uniform_int_distribution<int> dist(0, 255);

  std::vector<uint8_t> potential_trades(generate_x_orders);

  for (int i = 0; i < (int)generate_x_orders; i++) {
    potential_trades[i] = static_cast<uint8_t>(dist(rng));
  }

  auto packed = build_order_book(potential_trades, generate_x_orders);

  uint64_t valid_trades = 0;

  for (auto &order : packed) {
    valid_trades += calc_laneMatchCount64(order, risk_gate_built);
  }

  std::cout << "Total trades: " << generate_x_orders << "\n";

  std::cout << "Successful trades: " << valid_trades << "\n";
  std::cout << "Success rate: "
            << (static_cast<float_t>(valid_trades) / generate_x_orders) * 100
            << "%\n";
  std::cout << "Failed trades: " << generate_x_orders - valid_trades << "\n";
  std::cout << "Fail rate: "
            << ((static_cast<float_t>(generate_x_orders) - valid_trades) /
                generate_x_orders) *
                   100
            << "%\n";

  return 0;
}
/*
        .align 2
        .p2align 4
        .weak	_ZNSt12_Vector_baseImSaImEED2Ev
        .type	_ZNSt12_Vector_baseImSaImEED2Ev, @function
_ZNSt12_Vector_baseImSaImEED2Ev: | i guess a vector gets initialized here at the
beginning of main, to accomodate one of the functions? | so i was wrong, this is
aparently the destructor, and the testq is checking to see if the vector pointer
is null before freeing it, and if it is null, then skip and jump to .L19, which
just returns without doing anyting, kind of like a break statement i guess | so
apparently this is known as an early return guard, if theres nothing to do, just
bail

.LFB4101:
        .cfi_startproc
        movq	%rdi, %rax
        movq	(%rdi), %rdi
        testq	%rdi, %rdi
        je	.L19 | i wonder why this jumps down to a lower point

        movq	16(%rax), %rsi
        subq	%rdi, %rsi
        jmp	_ZdlPvm@PLT
        .p2align 4,,10
        .p2align 3
.L19:
        ret
        .cfi_endproc
.LFE4101:
        .size	_ZNSt12_Vector_baseImSaImEED2Ev,
.-_ZNSt12_Vector_baseImSaImEED2Ev .weak	_ZNSt12_Vector_baseImSaImEED1Ev .set
_ZNSt12_Vector_baseImSaImEED1Ev,_ZNSt12_Vector_baseImSaImEED2Ev .section
.text._ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv,"axG",@progbits,_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv,comdat
        .align 2
        .p2align 4
        .weak
_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv
        .type
_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv,
@function
_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv:
.LFB4576:
        .cfi_startproc
        movq	(%rdi), %r8
        movq	%rdi, %rdx
        leaq	1816(%rdi), %r9
        movq	%rdi, %rcx
        .p2align 4
        .p2align 3
.L22: | this is all the twister setup lol, no need to go over this

        movq	%r8, %rsi
        movq	8(%rcx), %r8
        addq	$8, %rcx
        andq	$-2147483648, %rsi
        movq	%r8, %rax
        andl	$2147483647, %eax
        orq	%rsi, %rax
        movq	%rax, %rsi
        andl	$1, %eax
        negq	%rax
        shrq	%rsi
        xorq	3168(%rcx), %rsi
        andl	$2567483615, %eax
        xorq	%rsi, %rax
        movq	%rax, -8(%rcx)
        cmpq	%r9, %rcx
        jne	.L22
        movq	1816(%rdi), %rsi
        leaq	3168(%rdi), %r8
        .p2align 4
        .p2align 3
.L23:
        movq	%rsi, %rcx
        movq	1824(%rdx), %rsi
        addq	$8, %rdx
        andq	$-2147483648, %rcx
        movq	%rsi, %rax
        andl	$2147483647, %eax
        orq	%rcx, %rax
        movq	%rax, %rcx
        andl	$1, %eax
        negq	%rax
        shrq	%rcx
        xorq	-8(%rdx), %rcx
        andl	$2567483615, %eax
        xorq	%rcx, %rax
        movq	%rax, 1808(%rdx)
        cmpq	%rdx, %r8
        jne	.L23
        movq	4984(%rdi), %rax
        movq	(%rdi), %rdx
        movq	$0, 4992(%rdi)
        andl	$2147483647, %edx
        andq	$-2147483648, %rax
        orq	%rdx, %rax
        movq	%rax, %rdx
        andl	$1, %eax
        negq	%rax
        shrq	%rdx
        xorq	3168(%rdi), %rdx
        andl	$2567483615, %eax
        xorq	%rdx, %rax
        movq	%rax, 4984(%rdi)
        ret
        .cfi_endproc
.LFE4576:
        .size
_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv,
.-_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv
        .section
.text._ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv,"axG",@progbits,_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv,comdat
        .align 2
        .p2align 4
        .weak
_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv
        .type
_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv,
@function
_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv:
.LFB4488:
        .cfi_startproc
        subq	$24, %rsp
        .cfi_def_cfa_offset 32
        movq	4992(%rdi), %rax
        cmpq	$623, %rax
        ja	.L29
.L27: | still the twister internals, not worth much because im more interested
in how MY code compiles down, this is probably already pretty heavily optimized

        leaq	1(%rax), %rdx
        movq	(%rdi,%rax,8), %rax
        movq	%rdx, 4992(%rdi)
        addq	$24, %rsp
        .cfi_remember_state
        .cfi_def_cfa_offset 8
        movq	%rax, %rdx
        shrq	$11, %rdx
        movl	%edx, %edx
        xorq	%rax, %rdx
        movq	%rdx, %rax
        salq	$7, %rax
        andl	$2636928640, %eax
        xorq	%rdx, %rax
        movq	%rax, %rdx
        salq	$15, %rdx
        andl	$4022730752, %edx
        xorq	%rax, %rdx
        movq	%rdx, %rax
        shrq	$18, %rax
        xorq	%rdx, %rax
        ret
        .p2align 4,,10
        .p2align 3
.L29:
        .cfi_restore_state
        movq	%rdi, 8(%rsp)
        call
_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv
        movq	8(%rsp), %rdi
        movq	4992(%rdi), %rax
        jmp	.L27
        .cfi_endproc
.LFE4488:
        .size
_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv,
.-_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv
        .text
        .align 2
        .p2align 4
        .type
_ZNSt24uniform_int_distributionIiEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEiRT_RKNS0_10param_typeE.isra.0,
@function
_ZNSt24uniform_int_distributionIiEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEiRT_RKNS0_10param_typeE.isra.0:
.LFB4648: | so apparnetly this is the uniform_int_distribution for the twister,
where it actually gets called by the code i wrote, i should probably skip this
part next time, as its not really worth analyzing it

        .cfi_startproc
        pushq	%r13
        .cfi_def_cfa_offset 16
        .cfi_offset 13, -16
        movslq	%esi, %rax
        movslq	%edx, %r13
        pushq	%r12
        .cfi_def_cfa_offset 24
        .cfi_offset 12, -24
        subq	%rax, %r13
        pushq	%rbp
        .cfi_def_cfa_offset 32
        .cfi_offset 6, -32
        movq	%rdi, %rbp
        pushq	%rbx
        .cfi_def_cfa_offset 40
        .cfi_offset 3, -40
        movq	%rax, %rbx
        movl	$4294967294, %eax
        subq	$8, %rsp
        .cfi_def_cfa_offset 48
        cmpq	%r13, %rax
        jnb	.L42
        movl	$4294967295, %eax
        cmpq	%rax, %r13
        je	.L35
.L40:| wow instead of actually clamping the distribution, they just reject a
number if it does fall within the clamp bounds lmao

        xorl	%esi, %esi
        movl	$-1, %edx
        movq	%rbp, %rdi
        call
_ZNSt24uniform_int_distributionIiEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEiRT_RKNS0_10param_typeE.isra.0
        movq	%rbp, %rdi
        movl	%eax, %r12d
        call
_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv
        movq	%r12, %rcx
        salq	$32, %rcx
        addq	%rax, %rcx
        setc	%al
        movzbl	%al, %eax
        cmpq	%rcx, %r13
        jb	.L40
        testq	%rax, %rax
        jne	.L40
        addq	$8, %rsp
        .cfi_remember_state
        .cfi_def_cfa_offset 40
        leal	(%rbx,%rcx), %eax
        popq	%rbx
        .cfi_def_cfa_offset 32
        popq	%rbp
        .cfi_def_cfa_offset 24
        popq	%r12
        .cfi_def_cfa_offset 16
        popq	%r13
        .cfi_def_cfa_offset 8
        ret
        .p2align 4,,10
        .p2align 3
.L42:
        .cfi_restore_state
        call
_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv
        leaq	1(%r13), %r12
        imulq	%r12, %rax
        movq	%rax, %rcx
        cmpl	%r12d, %eax
        jnb	.L32
        movl	%r12d, %eax
        xorl	%edx, %edx
        negl	%eax
        divl	%r12d
        movl	%edx, %r13d
        cmpl	%edx, %ecx
        jnb	.L32
        .p2align 4
        .p2align 3
.L33:
        movq	%rbp, %rdi
        call
_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv
        imulq	%r12, %rax
        movq	%rax, %rcx
        cmpl	%r13d, %eax
        jb	.L33
.L32:
        addq	$8, %rsp
        .cfi_remember_state
        .cfi_def_cfa_offset 40
        shrq	$32, %rcx
        leal	(%rbx,%rcx), %eax
        popq	%rbx
        .cfi_def_cfa_offset 32
        popq	%rbp
        .cfi_def_cfa_offset 24
        popq	%r12
        .cfi_def_cfa_offset 16
        popq	%r13
        .cfi_def_cfa_offset 8
        ret
        .p2align 4,,10
        .p2align 3
.L35:
        .cfi_restore_state
        call
_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv
        addq	$8, %rsp
        .cfi_def_cfa_offset 40
        movq	%rax, %rcx
        leal	(%rbx,%rcx), %eax
        popq	%rbx
        .cfi_def_cfa_offset 32
        popq	%rbp
        .cfi_def_cfa_offset 24
        popq	%r12
        .cfi_def_cfa_offset 16
        popq	%r13
        .cfi_def_cfa_offset 8
        ret
        .cfi_endproc
.LFE4648:
        .size
_ZNSt24uniform_int_distributionIiEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEiRT_RKNS0_10param_typeE.isra.0,
.-_ZNSt24uniform_int_distributionIiEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEiRT_RKNS0_10param_typeE.isra.0
        .section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
        .string	"Generate orders amount: "
        .section	.rodata.str1.8,"aMS",@progbits,1
        .align 8
.LC1:
        .string	"Select a kill mask ID, buy side[0-255]: "
        .align 8
.LC2:
        .string	"Select a kill mask ID, sell side[0-255]: "
        .section	.rodata.str1.1
.LC3:
        .string	"Total trades: "
.LC4:
        .string	"\n"
.LC5:
        .string	"Successful trades: "
.LC6:
        .string	"Success rate: "
.LC8:
        .string	"%\n"
.LC9:
        .string	"Failed trades: "
.LC10:
        .string	"Fail rate: "
        .section	.text.unlikely,"ax",@progbits
.LCOLDB11:
        .section	.text.startup,"ax",@progbits
.LHOTB11:
        .p2align 4
        .globl	main
        .type	main, @function
main:
.LFB3734:
        .cfi_startproc
        .cfi_personality 0x9b,DW.ref.__gxx_personality_v0
        .cfi_lsda 0x1b,.LLSDA3734
        pushq	%r15
        .cfi_def_cfa_offset 16
        .cfi_offset 15, -16
        leaq	_ZSt4cout(%rip), %rdi
        pushq	%r14
        .cfi_def_cfa_offset 24
        .cfi_offset 14, -24
        pushq	%r13
        .cfi_def_cfa_offset 32
        .cfi_offset 13, -32
        pushq	%r12
        .cfi_def_cfa_offset 40
        .cfi_offset 12, -40
        pushq	%rbp
        .cfi_def_cfa_offset 48
        .cfi_offset 6, -48
        pushq	%rbx
        .cfi_def_cfa_offset 56
        .cfi_offset 3, -56
        subq	$5112, %rsp
        .cfi_def_cfa_offset 5168
        movq	%fs:40, %rsi
        movq	%rsi, 5096(%rsp)
        leaq	.LC0(%rip), %rsi
.LEHB0:
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
        leaq	20(%rsp), %rsi
        leaq	_ZSt3cin(%rip), %rdi
        call	_ZNSi10_M_extractIjEERSiRT_@PLT
        leaq	.LC1(%rip), %rsi
        sall	$3, 20(%rsp)
        leaq	_ZSt4cout(%rip), %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
        leaq	24(%rsp), %rsi
        leaq	_ZSt3cin(%rip), %rdi
        call	_ZNSi10_M_extractIjEERSiRT_@PLT
        leaq	.LC2(%rip), %rsi
        leaq	_ZSt4cout(%rip), %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
        leaq	28(%rsp), %rsi
        leaq	_ZSt3cin(%rip), %rdi
        call	_ZNSi10_M_extractIjEERSiRT_@PLT
        movl	28(%rsp), %eax
        leaq	104(%rsp), %rsi
        movq	$42, 96(%rsp)
        movl	$42, %ecx
        movl	$1, %edx
        movl	%eax, 12(%rsp)
        .p2align 6
        .p2align 4
        .p2align 3
.L44:
        movq	%rcx, %rax
        addq	$8, %rsi
        shrq	$30, %rax
        xorq	%rcx, %rax
        imulq	$1812433253, %rax, %rax
        leal	(%rax,%rdx), %ecx
        addq	$1, %rdx
        movq	%rcx, -8(%rsi)
        cmpq	$624, %rdx
        jne	.L44
        movl	20(%rsp), %r13d
        movq	$624, 5088(%rsp)
        movq	%r13, %r14
        testq	%r13, %r13
        je	.L83
        movq	%r13, %rdi
        call	_Znwm@PLT
.LEHE0:
        leaq	(%rax,%r13), %r12
        movq	%rax, 32(%rsp)
        movq	%rax, %rbp
        leaq	1(%rax), %rdi
        movq	%r12, 48(%rsp)
        movb	$0, (%rax)
        cmpq	$1, %r13
        jne	.L47
        movq	%rdi, 40(%rsp)
.L48:
        movq	%rbp, %rbx
        addq	%rbp, %r13
        .p2align 4
        .p2align 3
.L50:
        movl	$255, %edx
        xorl	%esi, %esi
        leaq	96(%rsp), %rdi
        addq	$1, %rbx
        call
_ZNSt24uniform_int_distributionIiEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEiRT_RKNS0_10param_typeE.isra.0
        movb	%al, -1(%rbx)
        cmpq	%r13, %rbx
        jne	.L50
.L46:
        leaq	64(%rsp), %r13
        leaq	32(%rsp), %rsi
        movl	%r14d, %edx
        movq	%r13, %rdi
.LEHB1:
        call	_Z16build_order_bookRKSt6vectorIhSaIhEEj
.LEHE1:
        movq	64(%rsp), %rdi
        movq	72(%rsp), %r9
        cmpq	%rdi, %r9
        je	.L67
        movzbl	12(%rsp), %ebx
        imull	$16842752, %ebx, %r8d
        movl	%ebx, %eax
        sall	$8, %eax
        addl	%ebx, %eax
        xorl	%ebx, %ebx
        movl	%eax, %eax
        movslq	%r8d, %r8
        orq	%rax, %r8
        .p2align 4
        .p2align 3
.L54:
        movq	(%rdi), %rsi
        xorl	%ecx, %ecx
        xorl	%eax, %eax
        xorq	%r8, %rsi
        .p2align 5
        .p2align 4
        .p2align 3
.L53:
        movq	%rsi, %rdx
        shrq	%cl, %rdx
        cmpb	$1, %dl
        adcl	$0, %eax
        addl	$8, %ecx
        cmpl	$64, %ecx
        jne	.L53
        addq	$8, %rdi
        addq	%rax, %rbx
        cmpq	%rdi, %r9
        jne	.L54
.L52:
        leaq	.LC3(%rip), %rsi
        leaq	_ZSt4cout(%rip), %rdi
.LEHB2:
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
        movl	20(%rsp), %esi
        movq	%rax, %rdi
        call	_ZNSo9_M_insertImEERSoT_@PLT
        leaq	.LC4(%rip), %rsi
        movq	%rax, %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
        leaq	.LC5(%rip), %rsi
        leaq	_ZSt4cout(%rip), %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
        movq	%rbx, %rsi
        movq	%rax, %rdi
        call	_ZNSo9_M_insertImEERSoT_@PLT
        leaq	.LC4(%rip), %rsi
        movq	%rax, %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
        leaq	.LC6(%rip), %rsi
        leaq	_ZSt4cout(%rip), %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
        testq	%rbx, %rbx
        js	.L55
        pxor	%xmm4, %xmm4
        cvtsi2ssq	%rbx, %xmm4
        movss	%xmm4, 12(%rsp)
.L56:
        movl	20(%rsp), %edx
        pxor	%xmm1, %xmm1
        movss	12(%rsp), %xmm0
        movq	%rax, %rdi
        cvtsi2ssq	%rdx, %xmm1
        divss	%xmm1, %xmm0
        mulss	.LC7(%rip), %xmm0
        cvtss2sd	%xmm0, %xmm0
        call	_ZNSo9_M_insertIdEERSoT_@PLT
        leaq	.LC8(%rip), %rsi
        movq	%rax, %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
        leaq	.LC9(%rip), %rsi
        leaq	_ZSt4cout(%rip), %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
        movl	20(%rsp), %esi
        movq	%rax, %rdi
        subq	%rbx, %rsi
        call	_ZNSo9_M_insertImEERSoT_@PLT
        leaq	.LC4(%rip), %rsi
        movq	%rax, %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
        leaq	.LC10(%rip), %rsi
        leaq	_ZSt4cout(%rip), %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
        movl	20(%rsp), %edx
        pxor	%xmm1, %xmm1
        movq	%rax, %rdi
        cvtsi2ssq	%rdx, %xmm1
        movaps	%xmm1, %xmm3
        subss	12(%rsp), %xmm3
        movaps	%xmm3, %xmm0
        divss	%xmm1, %xmm0
        mulss	.LC7(%rip), %xmm0
        cvtss2sd	%xmm0, %xmm0
        call	_ZNSo9_M_insertIdEERSoT_@PLT
        leaq	.LC8(%rip), %rsi
        movq	%rax, %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
.LEHE2:
        movq	%r13, %rdi
        call	_ZNSt12_Vector_baseImSaImEED2Ev
        testq	%rbp, %rbp
        je	.L61
        movq	%r12, %rsi
        movq	%rbp, %rdi
        subq	%rbp, %rsi
        call	_ZdlPvm@PLT
.L61:
        movq	5096(%rsp), %rax
        subq	%fs:40, %rax
        jne	.L84
        addq	$5112, %rsp
        .cfi_remember_state
        .cfi_def_cfa_offset 56
        xorl	%eax, %eax
        popq	%rbx
        .cfi_def_cfa_offset 48
        popq	%rbp
        .cfi_def_cfa_offset 40
        popq	%r12
        .cfi_def_cfa_offset 32
        popq	%r13
        .cfi_def_cfa_offset 24
        popq	%r14
        .cfi_def_cfa_offset 16
        popq	%r15
        .cfi_def_cfa_offset 8
        ret
.L47:
        .cfi_restore_state
        leaq	-1(%r13), %rdx
        xorl	%esi, %esi
        call	memset@PLT
        movq	%r12, 40(%rsp)
        testl	%r13d, %r13d
        jg	.L48
        jmp	.L46
.L55:
        movq	%rbx, %rdx
        movq	%rbx, %rcx
        pxor	%xmm0, %xmm0
        shrq	%rdx
        andl	$1, %ecx
        orq	%rcx, %rdx
        cvtsi2ssq	%rdx, %xmm0
        addss	%xmm0, %xmm0
        movss	%xmm0, 12(%rsp)
        jmp	.L56
.L83:
        xorl	%eax, %eax
        xorl	%r12d, %r12d
        xorl	%ebp, %ebp
        movq	%rax, 32(%rsp)
        movq	%rax, 48(%rsp)
        movq	%rax, 40(%rsp)
        jmp	.L46
.L67:
        xorl	%ebx, %ebx
        jmp	.L52
.L84:
        call	__stack_chk_fail@PLT
.L69:
        movq	%rax, %rbx
        jmp	.L62
.L68:
        movq	%rax, %rbx
        jmp	.L63
        .section	.gcc_except_table,"a",@progbits
.LLSDA3734:
        .byte	0xff
        .byte	0xff
        .byte	0x1
        .uleb128 .LLSDACSE3734-.LLSDACSB3734
.LLSDACSB3734:
        .uleb128 .LEHB0-.LFB3734
        .uleb128 .LEHE0-.LEHB0
        .uleb128 0
        .uleb128 0
        .uleb128 .LEHB1-.LFB3734
        .uleb128 .LEHE1-.LEHB1
        .uleb128 .L68-.LFB3734
        .uleb128 0
        .uleb128 .LEHB2-.LFB3734
        .uleb128 .LEHE2-.LEHB2
        .uleb128 .L69-.LFB3734
        .uleb128 0
.LLSDACSE3734:
        .section	.text.startup
        .cfi_endproc
        .section	.text.unlikely
        .cfi_startproc
        .cfi_personality 0x9b,DW.ref.__gxx_personality_v0
        .cfi_lsda 0x1b,.LLSDAC3734
        .type	main.cold, @function
main.cold:
.LFSB3734:
.L62:
        .cfi_def_cfa_offset 5168
        .cfi_offset 3, -56
        .cfi_offset 6, -48
        .cfi_offset 12, -40
        .cfi_offset 13, -32
        .cfi_offset 14, -24
        .cfi_offset 15, -16
        movq	%r13, %rdi
        call	_ZNSt12_Vector_baseImSaImEED2Ev
.L63:
        testq	%rbp, %rbp
        je	.L64
        movq	%r12, %rsi
        movq	%rbp, %rdi
        subq	%rbp, %rsi
        call	_ZdlPvm@PLT
.L64:
        movq	5096(%rsp), %rax
        subq	%fs:40, %rax
        jne	.L85
        movq	%rbx, %rdi
.LEHB3:
        call	_Unwind_Resume@PLT
.LEHE3:
.L85:
        call	__stack_chk_fail@PLT
        .cfi_endproc
.LFE3734:
        .section	.gcc_except_table
.LLSDAC3734:
        .byte	0xff
        .byte	0xff
        .byte	0x1
        .uleb128 .LLSDACSEC3734-.LLSDACSBC3734
.LLSDACSBC3734:
        .uleb128 .LEHB3-.LCOLDB11
        .uleb128 .LEHE3-.LEHB3
        .uleb128 0
        .uleb128 0
.LLSDACSEC3734:
        .section	.text.unlikely
        .section	.text.startup
        .size	main, .-main
        .section	.text.unlikely
        .size	main.cold, .-main.cold
.LCOLDE11:
        .section	.text.startup
.LHOTE11:
        .section	.rodata.cst4,"aM",@progbits,4
        .align 4
.LC7:
        .long	1120403456
        .hidden	DW.ref.__gxx_personality_v0
        .weak	DW.ref.__gxx_personality_v0
        .section
.data.rel.local.DW.ref.__gxx_personality_v0,"awG",@progbits,DW.ref.__gxx_personality_v0,comdat
        .align 8
        .type	DW.ref.__gxx_personality_v0, @object
        .size	DW.ref.__gxx_personality_v0, 8
DW.ref.__gxx_personality_v0:
        .quad	__gxx_personality_v0
        .globl	__gxx_personality_v0
        .ident	"GCC: (GNU) 15.2.1 20260103"
        .section	.note.GNU-stack,"",@progbits
*/
