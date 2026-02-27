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
.L5:
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
