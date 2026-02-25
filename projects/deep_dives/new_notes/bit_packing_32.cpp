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
  // **Fox ML infrastructure LLC, and its employees or subsidiaries is not
  // legally responsible for bodily harm, mental health concerns, or anything
  // else that may impact life, liberty, or happiness from performing unlicensed
  // bit manipulation without proper supervision or safety measures**
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
  //============================================================================
  // EDIT[24-02-26 21:58pm]:
  // when i did this, i forgot that it returns a bool, i would probably need to
  // change this to allow, or packaed a yes/no bit within each individual order,
  // but this just checks all 4 at once as more like a risk gate than kill
  // switch, i think i handled this in the testing harness in main using the
  // branchless mechanism for adding up 2 different values without if/else
  // statements, but in a production system this part would need to be WAY more
  // complex, but am baby HFT engineer(please notice me) or whatever else i said
  // lol
  //============================================================================
  //
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
  // this is a branchless way to handle adding up 2 values by simply using the
  // output of a bool, if its 1, then a valid trade doesnt get added, BECAUSE
  // valid trades are 0, and 1 - 0 is *shocker* 0

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
//==========================================================================
// EDIT [24-02-26 01:04pm]:
// I BET YOU THOUGHT I WASNT GONNA ANNOTATE THE MAIN FUNCTION DIDNT YOU,
// im gonna annotate and dig into this after i finish making up my sociology
// assignments, and getting told how bad I did on my discrete structures exam
// lol
//==========================================================================
/*
_ZNSt12_Vector_baseIiSaIiEED2Ev: | so im guessing this is where the kill mask
gets build, because it has what loops like a simple loop? either that or this is
where the faux trades vector gets built, or the for loop right below those, but
this stuff is all above the string outputs, so its also possible i broke this up
wrong, im still LEARNING to read assembly
.LFB4141:
        .cfi_startproc
        movq	%rdi, %rax
        movq	(%rdi), %rdi
        testq	%rdi, %rdi
        je	.L19
        movq	16(%rax), %rsi
        subq	%rdi, %rsi
        jmp	_ZdlPvm@PLT
        .p2align 4,,10
        .p2align 3
.L19:
        ret
        .cfi_endproc
.LFE4141:
        .size	_ZNSt12_Vector_baseIiSaIiEED2Ev,
.-_ZNSt12_Vector_baseIiSaIiEED2Ev .weak	_ZNSt12_Vector_baseIiSaIiEED1Ev .set
_ZNSt12_Vector_baseIiSaIiEED1Ev,_ZNSt12_Vector_baseIiSaIiEED2Ev .section
.text._ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv,"axG",@progbits,_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv,comdat
        .align 2
        .p2align 4
        .weak
_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv
        .type
_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv,
@function
_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv:
| yeah because you can see where the random engine is used for the
potential_trades function, and with the way asm works, its going to have this
worked out here, im not sure if this is the random generator being intialized,
or if the ones below this are THOUGHT, no i was wrong here, this is just the
assembly for that function call, or like the internal state refresh of the
function, it processes 624 64-bit state words in two loops, with 0-226 being
handled by .L22, and .L23 handling 227-623, its safe to disregard the comments
here

.LFB4613:
        .cfi_startproc
        movq	(%rdi), %r8
        movq	%rdi, %rdx
        leaq	1816(%rdi), %r9 | im guessing this is actually the lea being
used in the way intended
        movq	%rdi, %rcx
        .p2align 4
        .p2align 3
.L22:
        movq	%r8, %rsi
        movq	8(%rcx), %r8
        addq	$8, %rcx
        andq	$-2147483648, %rsi
        movq	%r8, %rax
        andl	$2147483647, %eax | you can see where the comment below
references this, and its the - to positive range because its a signed integer
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
        jne	.L22 | no this is definitely where the for loop builds the
trades vector beacuse ti caps out at the INT_MAX or something close to that, i
was wrong here, this is just the Mersenne Twister assembly

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
        jne	.L23 | this is probably a for loop from a function i think?
probably where the kill mask or the actual orders gets built, because it has
multiples of 8 being added every iteration, which is what i was doing to bit
pack
        movq	4984(%rdi), %rax
        movq	(%rdi), %rdx
        movq	$0, 4992(%rdi)
        andl	$2147483647, %edx
        andq	$-2147483648, %rax
        orq	%rdx, %rax
        movq	%rax, %rdx
        andl	$1, %eax
        negq	%rax
        shrq	%rdx | im guessing this means share quad? so like its maybe a
const or the & reference within the function inputs, i over thought this, its
just shift right lmao

        xorq	3168(%rdi), %rdx
        andl	$2567483615, %eax | im not sure what this number is for, its not
quite the int max, but its close?
        xorq	%rdx, %rax
        movq	%rax, 4984(%rdi)
        ret
        .cfi_endproc | it also has a function end point right here, so this is
probably where a function gets called, and the actual data gets passed, which is
way the INT_MAX and INT_MIN are referenced here in the edx and rax registers
.LFE4613:
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
| haha you can see the random function implemented here, im guessing this is
probably where the actual function uses it, because above this is where the
random distribution bounds are set?

.LFB4525:
        .cfi_startproc
        subq	$24, %rsp
        .cfi_def_cfa_offset 32
        movq	4992(%rdi), %rax
        cmpq	$623, %rax
        ja	.L29
.L27:
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
        andl	$4022730752, %edx |this is still technically a 32bit, when it
doesnt have the sign at the begining, it basically doubles the actual range,
because you cant display negatives

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
.LFE4525:
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
.LFB4685:
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
.L40:
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
        jne	.L40 | i wonder why this wasnt turned into a simple cmov
instruction or something else, probably because it iterates like 32 times? you
can see it lowers by 1 every single function loop, at the movl $-1, or atleast i
think thats what that means, because a normal for loop has $1 instead, well this
is probably just where the trades get packed into a single integer, because it
has the addq $8 below, its just actually being used after being declared in the
above, so apparently this wasnt converted to a cmov because the branch target
being called here is a call, and cmov can only conditionally move between
registers/memory, which makes sense because if branches only get converted down
to this when theyre simple, which excludes function calls

        addq	$8, %rsp | so i was wrong about this just being used after being
called, apparently this is just deallocating 8 bytes of local stack space, it
was allocated earlier with a subq $8, %rsp, its just the function cleaning up
before the epilogue sequence of popq instructions

        .cfi_remember_state
        .cfi_def_cfa_offset 40
        leal	(%rbx,%rcx), %eax
        popq	%rbx
        .cfi_def_cfa_offset 32 | im not sure what these offsets mean, so they
are apparently called DWARF Call Frame Information directives, they tell the
debugger/unwinder how to find the previous stack frame for each point in the
code, from what im reading its like an offset from the current stack pointer to
the canonical frame address (CFA - lol chik fla), which is the stack pointer
value right before the call that entered the function, so in the example below,
i guess when you use a stack memory layout in higher level code, push and pop
just manipulate these registers directly

EDIT: these are a push pop mechanism for the unwinders state, the compiler saves
the CFI state before an alternate code path so that are the paths rejoin, it can
restore without respecifying every registers location, like a checkpoint

        popq	%rbp
        .cfi_def_cfa_offset 24
        popq	%r12
        .cfi_def_cfa_offset 16
        popq	%r13
        .cfi_def_cfa_offset 8
        ret
        .p2align 4,,10
        .p2align 3 | these are called directives, and theyre basically padding
with NOPs to aligh loop entry points to 16-byte or 8-byte boundaries, this
apparently matters because instruction fetch on x86 pulls 16-byte aligned
blocks, this is a compiler optimization i guess because i didnt explcitily say
to do this, but it does it because if a hot loop header straddles a boundary,
you waste a fetch cycle E V E R Y  S I N G L E  I T E R A T I O N, free
performance, we like this, we like this ALOT, the second parameter is a max
padding limit, it means align to 16 bytes, but O N L Y if it takes 10 or fewer
bytes of NOP padding to get there,

.L42:
        .cfi_restore_state
        call
_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv
        leaq	1(%r13), %r12
        imulq	%r12, %rax | IMUL INSTRUCTIONS HAHA, i wrote about these last
night, instead of DIV, this reduces compute by 20-90 cycles, which I think i
wrote about how its more effecient somewhere else, but this actually lets me see
it in the code, and as stated before, IMUL is like 3 cycles

        movq	%rax, %rcx
        cmpl	%r12d, %eax
        jnb	.L32 | this is jump if not below, ill probably try to map this
to which function is actually ebing called here, but its probably the
kill_mask_built function, no, its something being used in the actual main
function, im guessing the compiler turned this into a count down instead because
a max size is known, based on the code that i wrote? so apparently GCC/Clang
loves doing this, because counting towards zero lets them flag results from the
DEC/SUB directly, which probably reduces cycle counts further because less
overhead

        movl	%r12d, %eax
        xorl	%edx, %edx
        negl	%eax
        divl	%r12d
        movl	%edx, %r13d
        cmpl	%edx, %ecx | this is a modular arithmatic block, apparently this
computes something like (-n) % n or something equivalent, i only had 1 direct
division call that i personally wrote, so im guessing this is part of something
else that got turned into a top -> down format instead of counting up, because
of being able to reference the DEC/SUB directly like mentioned above

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
.LFE4685:
        .size
_ZNSt24uniform_int_distributionIiEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEiRT_RKNS0_10param_typeE.isra.0,
.-_ZNSt24uniform_int_distributionIiEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEiRT_RKNS0_10param_typeE.isra.0
        .section	.rodata.str1.1,"aMS",@progbits,1
.LC2:
        .string	"Generate orders amount: "
        .section	.rodata.str1.8
        .align 8
.LC3:
        .string	"Select a kill mask id #[0-255]: "
        .section	.rodata.str1.1
.LC4:
        .string	"Successful trades: "
.LC5:
        .string	"\n"
.LC6:
        .string	"Failed trades: "
.LC7:
        .string	"Success rate: "
.LC9:
        .string	"%"
.LC10:
        .string	"Java bad\n"
        .section	.text.unlikely
.LCOLDB11:
        .section	.text.startup,"ax",@progbits
.LHOTB11:
        .p2align 4
        .globl	main
        .type	main, @function
main:
.LFB3774:
        .cfi_startproc
        .cfi_personality 0x9b,DW.ref.__gxx_personality_v0
        .cfi_lsda 0x1b,.LLSDA3774
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
        leaq	.LC2(%rip), %rsi
.LEHB0:
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
        leaq	24(%rsp), %rsi
        leaq	_ZSt3cin(%rip), %rdi
        call	_ZNSirsERi@PLT
        leaq	.LC3(%rip), %rsi
        sall	$2, 24(%rsp)
        leaq	_ZSt4cout(%rip), %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
        leaq	28(%rsp), %rsi
        leaq	_ZSt3cin(%rip), %rdi
        call	_ZNSirsERi@PLT
        movl	28(%rsp), %ebx
        leaq	104(%rsp), %rsi
        movq	$21, 96(%rsp)
        movl	$21, %ecx
        movl	$1, %edx
        .p2align 6
        .p2align 4
        .p2align 3
.L44: | this is the mersenne twister seeding loop, not the actual trade
generation

        movq	%rcx, %rax
        addq	$8, %rsi
        shrq	$30, %rax
        xorq	%rcx, %rax
        imulq	$1812433253, %rax, %rax | this is apparently the MT seed
initialization constant, im not sure if it will A L W A Y S be this number, or
if its deoendent on the seed that is set within the function parameters

        leal	(%rax,%rdx), %ecx
        addq	$1, %rdx
        movq	%rcx, -8(%rsi)
        cmpq	$624, %rdx
        jne	.L44
        movslq	24(%rsp), %r15
        movq	$624, 5088(%rsp)
        movq	%r15, %r14
        testl	%r15d, %r15d
        js	.L75
        testq	%r15, %r15
        je	.L79
        movq	%r15, %rdi
        call	_Znwm@PLT
.LEHE0:
        leaq	(%rax,%r15), %r12
        movq	%rax, 32(%rsp)
        movq	%rax, %rbp
        leaq	1(%rax), %rdi
        movq	%r12, 8(%rsp)
        movq	%r12, 48(%rsp)
        movb	$0, (%rax)
        cmpq	$1, %r15
        jne	.L80
.L49:
        movq	%rdi, 40(%rsp)
        movq	%rbp, %r13
        .p2align 4
        .p2align 3
.L51: | THIS IS WHERE TRADES GET GENERATED

        movl	$255, %edx
        xorl	%esi, %esi
        leaq	96(%rsp), %rdi
        addq	$1, %r13
        call
_ZNSt24uniform_int_distributionIiEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEiRT_RKNS0_10param_typeE.isra.0
        movb	%al, -1(%r13)
        cmpq	%r13, %r12
        jne	.L51
.L48:
        leaq	64(%rsp), %r12
        leaq	32(%rsp), %rsi
        movl	%r14d, %edx
        movq	%r12, %rdi
.LEHB1:
        call	_Z16build_order_bookRKSt6vectorIaSaIaEEi
.LEHE1:
        movq	64(%rsp), %rax
        movq	72(%rsp), %rsi
        cmpq	%rax, %rsi
        je	.L60
        movsbl	%bl, %ebx
        xorl	%r14d, %r14d
        xorl	%r15d, %r15d
        movl	%ebx, %edx
        movl	%ebx, %ecx
        sall	$8, %edx
        sall	$16, %ecx
        orl	%ecx, %edx
        orl	%ebx, %edx
        sall	$24, %ebx
        orl	%edx, %ebx
        .p2align 6
        .p2align 4
        .p2align 3
.L53:
        movl	(%rax), %edx
        xorl	%ecx, %ecx
        andl	%ebx, %edx
        cmpl	%ebx, %edx
        sete	%cl
        addl	%ecx, %r14d
        cmpl	%ebx, %edx
        setne	%dl
        addq	$4, %rax
        movzbl	%dl, %edx
        addl	%edx, %r15d
        cmpq	%rax, %rsi
        jne	.L53
.L52:
        leaq	.LC4(%rip), %rsi
        leaq	_ZSt4cout(%rip), %rdi
.LEHB2:
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
        movl	%r15d, %esi
        movq	%rax, %rdi
        call	_ZNSolsEi@PLT
        leaq	.LC5(%rip), %rsi
        movq	%rax, %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
        leaq	.LC6(%rip), %rsi
        leaq	_ZSt4cout(%rip), %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
        movl	%r14d, %esi
        movq	%rax, %rdi
        call	_ZNSolsEi@PLT
        leaq	.LC5(%rip), %rsi
        movq	%rax, %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
        leaq	.LC7(%rip), %rsi
        leaq	_ZSt4cout(%rip), %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
        pxor	%xmm1, %xmm1
        pxor	%xmm0, %xmm0
        movq	%rax, %rdi
        cvtsi2ssl	%r14d, %xmm1
        addl	%r15d, %r14d
        cvtsi2ssl	%r14d, %xmm0
        divss	%xmm0, %xmm1
        movss	.LC8(%rip), %xmm0
        mulss	%xmm0, %xmm1
        subss	%xmm1, %xmm0
        cvtss2sd	%xmm0, %xmm0
        call	_ZNSo9_M_insertIdEERSoT_@PLT
        leaq	.LC9(%rip), %rsi
        movq	%rax, %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
        leaq	.LC5(%rip), %rsi
        movq	%rax, %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
        leaq	.LC10(%rip), %rsi
        leaq	_ZSt4cout(%rip), %rdi
        call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
.LEHE2:
        movq	%r12, %rdi
        call	_ZNSt12_Vector_baseIiSaIiEED2Ev
        testq	%rbp, %rbp
        je	.L54
        movq	8(%rsp), %rsi
        movq	%rbp, %rdi
        subq	%rbp, %rsi
        call	_ZdlPvm@PLT
.L54:
        movq	5096(%rsp), %rax
        subq	%fs:40, %rax
        jne	.L81
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
.L80:
        .cfi_restore_state
        leaq	-1(%r15), %rdx
        xorl	%esi, %esi
        call	memset@PLT
        movq	%r12, %rdi
        jmp	.L49
.L79:
        xorl	%eax, %eax
        xorl	%edx, %edx
        xorl	%ebp, %ebp
        movq	%rax, 32(%rsp)
        movq	%rax, 48(%rsp)
        movq	%rax, 40(%rsp)
        movq	%rdx, 8(%rsp)
        jmp	.L48
.L60:
        xorl	%r14d, %r14d
        xorl	%r15d, %r15d
        jmp	.L52
.L81:
        call	__stack_chk_fail@PLT
.L61:
        movq	%rax, %rbx
        jmp	.L56
.L62:
        movq	%rax, %rbx
        jmp	.L55
        .section	.gcc_except_table,"a",@progbits
.LLSDA3774:
        .byte	0xff
        .byte	0xff
        .byte	0x1
        .uleb128 .LLSDACSE3774-.LLSDACSB3774
.LLSDACSB3774:
        .uleb128 .LEHB0-.LFB3774
        .uleb128 .LEHE0-.LEHB0
        .uleb128 0
        .uleb128 0
        .uleb128 .LEHB1-.LFB3774
        .uleb128 .LEHE1-.LEHB1
        .uleb128 .L61-.LFB3774
        .uleb128 0
        .uleb128 .LEHB2-.LFB3774
        .uleb128 .LEHE2-.LEHB2
        .uleb128 .L62-.LFB3774
        .uleb128 0
.LLSDACSE3774:
        .section	.text.startup
        .cfi_endproc
        .section	.text.unlikely
        .cfi_startproc
        .cfi_personality 0x9b,DW.ref.__gxx_personality_v0
        .cfi_lsda 0x1b,.LLSDAC3774
        .type	main.cold, @function
main.cold:
.LFSB3774:
.L75:
        .cfi_def_cfa_offset 5168
        .cfi_offset 3, -56
        .cfi_offset 6, -48
        .cfi_offset 12, -40
        .cfi_offset 13, -32
        .cfi_offset 14, -24
        .cfi_offset 15, -16
        movq	5096(%rsp), %rax
        subq	%fs:40, %rax
        jne	.L82
        leaq	.LC0(%rip), %rdi
.LEHB3:
        call	_ZSt20__throw_length_errorPKc@PLT
.L55:
        movq	%r12, %rdi
        call	_ZNSt12_Vector_baseIiSaIiEED2Ev
.L56:
        testq	%rbp, %rbp
        je	.L57
        movq	8(%rsp), %rsi
        movq	%rbp, %rdi
        subq	%rbp, %rsi
        call	_ZdlPvm@PLT
.L57:
        movq	5096(%rsp), %rax
        subq	%fs:40, %rax
        jne	.L83
        movq	%rbx, %rdi
        call	_Unwind_Resume@PLT
.LEHE3:
.L83:
        call	__stack_chk_fail@PLT
.L82:
        call	__stack_chk_fail@PLT
        .cfi_endproc
.LFE3774:
        .section	.gcc_except_table
.LLSDAC3774:
        .byte	0xff
        .byte	0xff
        .byte	0x1
        .uleb128 .LLSDACSEC3774-.LLSDACSBC3774
.LLSDACSBC3774:
        .uleb128 .LEHB3-.LCOLDB11
        .uleb128 .LEHE3-.LEHB3
        .uleb128 0
        .uleb128 0
.LLSDACSEC3774:
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
.LC8:
        .long	1120403456
        .hidden	DW.ref.__gxx_personality_v0
        .weak	DW.ref.__gxx_personality_v0
        .section
.data.rel.local.DW.ref.__gxx_personality_v0,"awG",@progbits,DW.ref.__gxx_personality_v0,comdat
        .align 8
        .type	DW.ref.__gxx_personality_v0, @object
        .size	DW.ref.__gxx_personality_v0, 8
*/
