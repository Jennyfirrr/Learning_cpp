#include <array> // this was included just to make you question my competence
#include <iostream>
#include <random>
#include <vector>

// LOL HERE WE GO NOW IM COOKING LOL
//
// gonna annotate this later, and probably do an inline asm analysis as well
// this is just playing around trying to see how packing different order bits
// into a single 32 bit int works

std::vector<int32_t>
build_order_book(const std::vector<int8_t> &potential_trades,
                 int32_t queued_orders) {
  int32_t order_bit_packed = 0;
  std::vector<int32_t> order_book_packed((queued_orders + 3) / 4);

  for (int j = 0; j < (int)order_book_packed.size(); j++) {
    order_bit_packed = 0;

    for (int i = 0; i < 4; i++) {
      order_bit_packed |= potential_trades[j * 4 + i] << (i * 8);
    }
    order_book_packed[j] = order_bit_packed;
  }

  return order_book_packed;
  // the packing for this works as expected i THInk, but the reading part and
  // applying the kill mask could be better, right now if one trade fails, they
  // all fail, this would probably need to be split at execution time, but im
  // experimenting with the bitwise operators and bit packing as well as
  // encoding information, plus this is just so fun lol, like, this shit is W A
  // C K Y, one way around this would be instead of completely killing the
  // order, shift to a cpu core dedicated for failure checks, and possibly
  // unpack and run each one through a single 8bit gate, but by that time the
  // trades may be bad? idk, or grouping similar orders together? This is a head
  // scratcher lol
  //
  // OH BOY LOL THIS IS QUITE A FUNCTION IN ASM LOL, H E R E  W E  G O
  /*
  .LHOTB1:
          .p2align 4
          .globl	_Z16build_order_bookRKSt6vectorIaSaIaEEi
          .type	_Z16build_order_bookRKSt6vectorIaSaIaEEi, @function
  _Z16build_order_bookRKSt6vectorIaSaIaEEi:
  .LFB3762:
          .cfi_startproc
          pushq	%r13
          .cfi_def_cfa_offset 16
          .cfi_offset 13, -16
          leal	6(%rdx), %eax
          pushq	%r12
          .cfi_def_cfa_offset 24
          .cfi_offset 12, -24
          pushq	%rbp
          .cfi_def_cfa_offset 32
          .cfi_offset 6, -32
          pushq	%rbx
          .cfi_def_cfa_offset 40
          .cfi_offset 3, -40
          subq	$24, %rsp
          .cfi_def_cfa_offset 64
          addl	$3, %edx
          cmovns	%edx, %eax
          sarl	$2, %eax
          movslq	%eax, %rcx
          movq	%rcx, %rax
          shrq	$61, %rax
          jne	.L12
          movq	%rdi, %rbp
          testq	%rcx, %rcx
          je	.L15
          leaq	0(,%rcx,4), %rdx | its really neat seeing this stuff
          movq	%rcx, 8(%rsp)
          movq	%rsi, %r12
          movq	%rdx, %rdi
          movq	%rdx, (%rsp)
          call	_Znwm@PLT
          movq	(%rsp), %rdx
          movq	%rax, 0(%rbp)
          movq	%rax, %rbx
          leaq	4(%rax), %rdi
          leaq	(%rax,%rdx), %r13
          movl	$0, (%rax)
          movq	%r13, 16(%rbp)
          cmpq	$1, 8(%rsp)
          je	.L16 | probably the first for loop, thats weird why does this
  jump to .L16 from  the begining?
          subq	$4, %rdx
          xorl	%esi, %esi
          call	memset@PLT
          leaq	-4(%r13,%rax), %rdx | F R E E  M A T H
          subq	%rbx, %rdx
          movq	%rdx, %rax
          movq	%rdx, 8(%rbp)
          subq	%rbx, %rax
          sarq	$2, %rax
          testl	%eax, %eax
          jle	.L1 | the second one?
  .L6:
          subl	$1, %eax
          movq	(%r12), %r9
          xorl	%edi, %edi
          leaq	4(,%rax,4), %r8
          .p2align 4
          .p2align 3
  .L9:
          leaq	(%r9,%rdi), %rdx
          xorl	%ecx, %ecx
          xorl	%esi, %esi
  .L8:
          movsbl	(%rdx), %eax
          addq	$1, %rdx
          sall	%cl, %eax
          addl	$8, %ecx
          orl	%eax, %esi
          cmpl	$32, %ecx
          jne	.L8
          movl	%esi, (%rbx,%rdi)
          addq	$4, %rdi
          cmpq	%rdi, %r8
          jne	.L9 | maybe these are the 2 for loops? theyre right next to
  eachother, so thats why i would think at first, and you can see the addl where
  the multiplication is with i, and the 32 bit cap on the inner loop, or maybe
  the .L8 jump is the inner loop, then it compares to the 32, and moves the .L9
  jump when its done?, resetting to the outer loop?, idk ill probably annotate
  this more when im in my next java lecture, its not like java is gonna be
  useful to me anyways XD
  .L1:
          addq	$24, %rsp
          .cfi_remember_state
          .cfi_def_cfa_offset 40
          movq	%rbp, %rax
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
  .L15:
          .cfi_restore_state
          pxor	%xmm0, %xmm0
          movq	$0, 16(%rdi)
          movq	%rbp, %rax
          movups	%xmm0, (%rdi)
          addq	$24, %rsp
          .cfi_remember_state
          .cfi_def_cfa_offset 40
          popq	%rbx
          .cfi_def_cfa_offset 32
          popq	%rbp
          .cfi_def_cfa_offset 24
          popq	%r12
          .cfi_def_cfa_offset 16
          popq	%r13
          .cfi_def_cfa_offset 8
          ret
  .L16:
          .cfi_restore_state
          movq	%rdi, 8(%rbp)
          movl	$1, %eax | and the i++ is right here
          jmp	.L6 | this is probably the outer for loop, because you can see
  how it resets the %rdi register, back to 0, which is what happens whenever the
  outer for loop resets .cfi_endproc
  */
  // i mean technically for the build_order_book function, it doesnt REALLY
  // matter, because its just initialization, but i guess in a hot path to keep
  // an order stream saturated, it would matter, but im new to this lol, and bad
  // at coding i guess, too bad im not some MIT wizard who can do leetcode while
  // blindfolded and being waterboarded
}

int32_t build_kill_mask(int8_t kill_mask_bits) {
  int32_t kill_mask_built = 0;

  // this should probably be renamed the risk gate
  // its not a true kill mask or kill switch
  // a kill mask would probably only be engaged if something went
  // catastrophically wrong, and would probably be engaged by a self monitoring
  // system for system health or something

  for (int i = 0; i < 4; i++) {
    kill_mask_built |= kill_mask_bits << (i * 8);
  }
  // use packed_orders & kill_mask == kill_mask
  //
  // SO I JUST FOUND OUT ABOUT de Brujin-style multiplication, where you can
  // apparently multiply your int8_t, or byte mask by 0x01010101, and the math
  // naturally places the byte into all four slots
  // ID0: 0x01
  // ID1: 0x100
  // ID2: 0x10000
  // ID3: 0x1000000
  //
  //============================================================================
  // EDIT [24-02-26 : 11:58am]: So one thing about the de Brujin style, thing is
  // that while this does fire off with a single imul instruction, you would
  // still need a way to build each individual block based of each set of 4 in
  // the order vector, but im already thinking about that, and im gonna try
  // implementing that in the next thing i do, also remember kids, everything
  // you see is performed by a TRAINED PROFESSIONAL, do not attempt this at home
  // without parental supervision,
  //
  // **Fox ML infrastructure LLC is not legally
  // responsible for bodily harm, mental health concerns, or anything else that
  // may impact life, liberty, or happiness from performing unlicensed bit
  // manipulation without proper supervision or safety measures**
  //============================================================================
  //
  // the cpu does this in a single imul instruction instead of shifting the same
  // byte and using |= to place the byte ID in the correct place
  //
  // Toggling the mask is just using XOR tricks, inverting it is 0xFFFFFFFF
  // or just use the ~ operator lol
  //
  // You can use XOR for something called delta encoding, which is using the
  // old_order_state, and new_order_state, which when XOR'd tells you exactly
  // which bits changed, if the result is 0, nothing changed and you can just
  // send the next order through, this lets you bypass the entire logic gate, so
  // I guess to take advantage of this you would basically check the state of
  // the current order_book, and store it in a register pre kill_mask, and post
  // kill_mask, then if the next order book was the same, and nothing changed,
  // you check it against the post kill_mask order state, which is probably
  // faster because its just a bool check which when compiled converts down to a
  // bit check i believe(?)
  //
  // the delta encoding mentioned above is basically this pattern:
  // delta = old_state ^ new_state;
  // if(!delta) skip_validation();
  //
  // and the reason this is good, is because this basically compiles down to
  // essentially 2 instructions, which become, xor + jz
  //
  // This is called loop unrolling, which is manually doing what the compiler
  // TRIES to do, because while the compiler is very VERY smart, it still has a
  // deterministic output, its not ai, and I was right about it being faster
  // because this goes from like 8+ instructions, down to 1-2 which is GOOD, i
  // love this, you love this, everyone loves this
  //
  // SO because i havent made notes about imul yet, and ive referenced it a few
  // times, H E R E  W E  G O, imul is the x86 instruction set for signed
  // integer multiplication, this comes in a few flavors(just like crayons, red
  // is my favorite flavor personally), also these are gonna be added to the asm
  // ref file, as well as here:
  //
  // One operand:
  // imul r/m : multiples the EAX register(or RAX), by the operand, storing the
  // result in EDX:EAX(or RDX:RAX) for the full double width result
  //
  // Two operand:
  // imul dest, src : multiplies dest by src, and stores the truncated result in
  // dest
  //
  // Three operand:
  // imul dest, src, imm : multiplies src by the immedediate value, stores in
  // dest
  //
  // WHY ITS IMPORTANT, in low latency systems, its relevant because when you do
  // fixed point arithmatic nstead of FP to maintain deterministic execution,
  // integer multiplay by imul has P R E D I C T A B L E, C O N S I S T A N T
  // latency, (on INTEL cpu's typically around 3 cycles on modern cores), no
  // rouding mode surprises, no denormal stalls, as a note, for signed ints, the
  // compiler like imul, for unsigned it will choose mul, For signed quantities
  // like P&L, position deltas, or anything that can go N E G A T I V E(like my
  // bank account), the compiler picks imul, The two operand, and three operand
  // forms discard the upper half of the result(no EDX:EAX split), which is fine
  // when you know the result fits in the 32/64bit range, but if you need
  // overflow detection, the one operand form gives you the full width result to
  // check against, im kinda surprised im still going with this lol, like these
  // are WHOLE ASS PAPERS IN THE COMMENTS, and im having T H E  T I M E  O F  M
  // Y  L I F E writing these, like this is so freaking cool, imagine using java
  // and not having to know any of this lol, couldnt be me lmao, Also heres
  // another Z I N G E R
  //
  // What does the JIT stand for?
  //
  // Java Is Trash H A H A H A H A H A(your supposed to laugh here)
  // ok back to being serious *wink*
  //
  // also, to address some errors within the code here, im using int8_t, which
  // is signed, so that means the values from 128-255 are getting interpreted as
  // negative numbers(i wonder if this caused the issue earlier where the cin
  // error occured with it interpreting 128 as 1), when you shift a N E G T I V
  // E, int8_t left, you get sign extension into the upper bits, which will
  // corrupt adjacent packed slots, i probably should use uint8_t for the trade
  // ID's, but thats why WE ARE LEARNING, and also the kill mask 128 is probably
  // killing all trades because it converts to -128, and when you shift this
  // left, it smears 1's across the entire packed register, which is probably
  // why results looked weird,

  return kill_mask_built;
  /*
  .LCOLDE1:
  .text
.LHOTE1:
  .p2align 4
  .globl	_Z15build_kill_maska
  .type	_Z15build_kill_maska, @function
_Z15build_kill_maska:
.LFB3772:
  .cfi_startproc
  movsbl	%dil, %eax
  movl	%eax, %edx
  movl	%eax, %ecx
  sall	$8, %edx
  sall	$16, %ecx
  orl	%ecx, %edx
  orl	%eax, %edx
  sall	$24, %eax
  orl	%edx, %eax
  ret
  .cfi_endproc
.LFE3772:
  .size	_Z15build_kill_maska, .-_Z15build_kill_maska
*/
  // well how about that, i guess the for loop was simple enough it could just
  // accomplis this with just move operations
}

int32_t kill_switch(int32_t packed_order_int, const int32_t &kill_mask_built) {
  // so theres apparently a better way to do this, apparently I could have just
  // checked each 8 bit lane seperatly using something like the following:
  //
  // bool lane0_killed = (packed & 0xFF) == (kill_mask & 0xFF);
  // bool lane1_killed = ((packed >> 8) & 0xFF) == ((kill_mask >> 8) & 0xFF);
  //
  // and so one, etc, this gives per lane granularity, without having to unpack
  // back to individual 8bit values, in a real system, this is where you would
  // wanna use SIMD(_mm_cmpeq_epi8), but im just a baby engineer, so thats kinda
  // out of my expertise, but ILL LEARN, toodles
  //
  // also please clone my theme repo *pleading eyes emoji* it would make me SO
  // HAPPY
  //
  //
  return (packed_order_int & kill_mask_built) == kill_mask_built;
  /*
  .p2align 4
  .globl	_Z11kill_switchiRKi
  .type	_Z11kill_switchiRKi, @function
_Z11kill_switchiRKi:
.LFB3773:
  .cfi_startproc
  movl	(%rsi), %eax
  notl	%edi
  testl	%eax, %edi
  sete	%al
  movzbl	%al, %eax
  ret
  .cfi_endproc
.LFE3773:
  */
  // yeah not too surprised about this one, its just a simple compare, well, a
  // not + test + set, which is the thing i pointed out in the first file where
  // i dug into the assembly code, it converts it to (~A & B) == B, or something
  // like this, idk its like 2am lol, G I M M E  A  B R E A K, im like actually
  // studying(shocker i know right????)
}

int main() {
  // i dont think im gonna put the main function asm code here lol, its like
  // 500-1000 lines lmao, maybe though, its not like line count stopped me in
  // other files lmao, i should probably do an execution class or something
  // next, to actually see how the cmov and jump operators actually get placed
  // within a hot path, that sounds like a good idea, NOT THAT ANYONE ACTUALLY
  // CARES(please love me)
  //
  int32_t generate_x_orders;
  int32_t temp_kill_mask;

  std::cout << "Generate orders amount: ";
  std::cin >> generate_x_orders;
  generate_x_orders = generate_x_orders * 4;

  std::cout << "Select a kill mask id #[0-255]: ";
  std::cin >> temp_kill_mask;

  int8_t kill_mask_bits_id = static_cast<int8_t>(temp_kill_mask);
  // apparently when you use cin to grab an int8_t int, it reads it as a
  // character, and not as a number, so when you entered 1, or 128, it was
  // reading BOTH of them as 1 lol, thats so weird
  // using 128 as the kill mask id will block 100% of trades becaues its
  // basically checking every bit that is packed at the 7th bit, im sure theres
  // a bug in my kill switch logic, and i need to work it out, but thats HOW WE
  // LEARN RIGHT???, this is just a testing harness i slaped together in the
  // main function, ideally you would probabably just wanna use a class or
  // something that wraps everything else together in a big warm hug, but im
  // kinda bad at coding and citadel doesnt wanna reply to me becuase im BaD aT
  // lEeTcOdE </3, its ok i forgive you though
  //
  int32_t kill_mask_built = build_kill_mask(kill_mask_bits_id);

  std::mt19937 rng(21); // deterministic fixed seed
  std::uniform_int_distribution<int> dist(0, 255);

  std::vector<int8_t> potential_trades(generate_x_orders);

  for (int i = 0; i < generate_x_orders; i++) {
    potential_trades[i] = static_cast<int8_t>(dist(rng));
  }

  auto packed = build_order_book(potential_trades, generate_x_orders);

  int32_t valid_trades = 0;
  int32_t killed_trades = 0;

  for (auto &order : packed) {
    int32_t result = kill_switch(order, kill_mask_built);
    killed_trades += result;
    valid_trades += (1 - result);
  }

  std::cout << "Successful trades: " << valid_trades << "\n";
  std::cout << "Failed trades: " << killed_trades << "\n";
  std::cout << "Success rate: "
            << 100 - ((static_cast<float>(killed_trades) /
                       (killed_trades + valid_trades)) *
                      100)
            << "%" << "\n";
  // so glad i remembered what i forgot, this file felt like it was missing
  // something
  std::cout << "Java bad\n";

  return 0;
}
