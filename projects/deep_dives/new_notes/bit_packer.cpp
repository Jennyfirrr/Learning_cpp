#include <array>
#include <iostream>
#include <random>
#include <vector>

// asm generated using the compile command
// [g++ -S -O2 bit_packer.cpp -o bit_packer.s]
//
// Compiler: GCC 15.2.1 20260103
// ISA: x86_64
//
// these outputs can change depending on compiler version

// probably gonna look at how this converts to asm when i havent been up for
// like 24 hours, toodles
//
// UDPATE: ASM IS COOL AF WTF
// its so cool seeing what the actual code compiles down to
// Im still not sure how to completely read all this, but holy shit this rabbit
// hole was so freaking worth lol
//
// imagine if java like actually let you control things lmao, but NOOO garbage
// collector go brrrrrr, god i hate java so much

int32_t kill_switch(int32_t order_book_state, int32_t kill_mask) {
  return (order_book_state & kill_mask) == kill_mask;
  // this compiles to asm with no jumps, only cmovne
  // EDIT:nvm was wrong about that, its just a sete, ill probably update the
  // asm_ref.cpp file to explain what more of these do but right now the
  // priority is learning how things compile down for branchless programming, i
  // love how weird this code looks too lol, like idk, its SO FREAKING COOL TO
  // SEE THIS STUFF
  //
  /*_Z11kill_switchii:
  .LFB3762:
          .cfi_startproc
          notl	%edi
          xorl	%eax, %eax
          testl	%esi, %edi
          sete	%al
          ret
          .cfi_endproc
                  */
}

int32_t build_kill_mask(const std::array<int8_t, 32> &kill_mask_bits) {
  int32_t kill_mask_built = 0;

  for (int i = 0; i < 32; i++) {
    // this if statement compiles to a cmovne
    // EDIT: i was wrong, its apparently just a movl, which is move if less
    // the jne at the end is because of the for loop, i think this is fine
    // thought, because its not branch prediction for non-predictable data
    // considering its just a counter
    //
    // EDIT2: I was wrong about the movl being move if less, it just means mov
    // with an l suffix, which means long(32-bit), because of the int32_t tags
    if (kill_mask_bits[i]) {
      kill_mask_built |= (1 << i);
    }
  }
  /*
.LFE3762: | I guess this is like defining the function name and inputs?
  .size	_Z11kill_switchii, .-_Z11kill_switchii
  .p2align 4
  .globl	_Z15build_kill_maskRKSt5arrayIaLm32EE
  .type	_Z15build_kill_maskRKSt5arrayIaLm32EE, @function
_Z15build_kill_maskRKSt5arrayIaLm32EE:
.LFB3763: | and this is the actual function logic?
  .cfi_startproc
  xorl	%ecx, %ecx
  xorl	%eax, %eax
  .p2align 5
  .p2align 4
  .p2align 3
.L5:
  cmpb	$0, (%rdi,%rcx)
  je	.L4
  movl	$1, %edx | gonna need to read about what the $1 means, but it may be
core? that would be my initial guess, it just means "put a 1 in the edx register
lol"
  sall	%cl, %edx
  orl	%edx, %eax
.L4:
  addq	$1, %rcx
  cmpq	$32, %rcx
  jne	.L5
  ret
  .cfi_endproc
  */

  return kill_mask_built;
}

std::array<int8_t, 32> build_kill_switch_bits(const int32_t &order_book_seed) {
  std::array<int8_t, 32> kill_switch_bits;

  for (int i = 0; i < 32; i++) {
    kill_switch_bits[i] = (order_book_seed >> i) & 1;
    // this basically checks each bit in the order_book_seed, and shifts it to
    // the right according to the index, so its always comparing the bit to the
    // 1 binary
    //
    // my bad, i keep defaulting to the positional storage instead of the
    // normalized 1/0, im learning :D
    //
    // same thing here, jne because of the counter in the for loop, which i
    // think should be fine, ill need to read about that, yeah, its fine because
    // after building the actualy kill switch, this would basically only run any
    // time it changed, so the jump here is like 1 misprediction chance out of
    // like millions+ of cycles so its largely negligiable, because its not on a
    // hot path, this would be more under initialization code, rather than the
    // actual execution logic, and even then, an overseer when changing it would
    // probably do that on another cpu or something, and it would be imported to
    // the hotpath to be used rather than actual calculated by the same cores
    // that are executing trades, but I guess this is where FPGA modules come
    // in, because those are specially designed to deal with things like this I
    // think, I only did light reading about those
  }

  /*
  _Z22build_kill_switch_bitsRKi:
.LFB3764:
  .cfi_startproc
  movl	(%rsi), %esi
  movq	%rdi, %rdx
  xorl	%eax, %eax
  .p2align 5
  .p2align 4
  .p2align 3
.L8:
  btl	%eax, %esi
  setc	(%rdx)
  addl	$1, %eax
  addq	$1, %rdx
  cmpl	$32, %eax
  jne	.L8 | for loop jump
  movq	%rdi, %rax
  ret
  .cfi_endproc
*/

  return kill_switch_bits;
}

int main() {
  int order_book_cycle;
  int kill_mask_seed;

  std::cout << "Cycles: ";
  std::cin >> order_book_cycle;

  std::cout << "Seed: ";
  std::cin >> kill_mask_seed;

  std::array<int8_t, 32> kill_mask_bits =
      build_kill_switch_bits(kill_mask_seed);

  int32_t kill_mask = build_kill_mask(kill_mask_bits);

  int32_t killed_trades = 0;

  for (int i = 0; i < 32; i++) {
    std::cout << static_cast<int>(kill_mask_bits[i]) << " ";
  }

  for (int i = 0; i < order_book_cycle; i++) {
    // the loop condition within this for loops compiles down to a jump using
    // cmpl %ecx, %eax+jne
    if (kill_switch(i, kill_mask)) {
      killed_trades++;
    }
  }

  std::cout << "\nCompleted trades: " << order_book_cycle - killed_trades
            << "\n";
  std::cout << "Killed trades: " << killed_trades << "\n";
  std::cout << "% killed: "
            << (static_cast<float>(killed_trades) / order_book_cycle) * 100
            << "%\n";

  /*
  .LFE3764:
  .size	_Z22build_kill_switch_bitsRKi, .-_Z22build_kill_switch_bitsRKi
  .section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
  .string	"Cycles: "
.LC1:
  .string	"Seed: "
.LC2:
  .string	" "
.LC3:
  .string	"\nCompleted trades: "
.LC4:
  .string	"\n"
.LC5:
  .string	"Killed trades: "
.LC6:
  .string	"% killed: "
.LC8:
  .string	"%\n"
  .section	.text.startup,"ax",@progbits
  .p2align 4
  .globl	main
  .type	main, @function
main:
.LFB3765:
  .cfi_startproc
  pushq	%r12
  .cfi_def_cfa_offset 16
  .cfi_offset 12, -16
  leaq	_ZSt4cout(%rip), %rdi
  pushq	%rbp
  .cfi_def_cfa_offset 24
  .cfi_offset 6, -24
  pushq	%rbx
  .cfi_def_cfa_offset 32
  .cfi_offset 3, -32
  subq	$64, %rsp
  .cfi_def_cfa_offset 96
  movq	%fs:40, %rsi
  movq	%rsi, 56(%rsp)
  leaq	.LC0(%rip), %rsi
  leaq	16(%rsp), %rbx
  call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
  leaq	8(%rsp), %rsi
  leaq	_ZSt3cin(%rip), %rdi
  call	_ZNSirsERi@PLT
  leaq	.LC1(%rip), %rsi
  leaq	_ZSt4cout(%rip), %rdi
  call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
  leaq	12(%rsp), %rsi
  leaq	_ZSt3cin(%rip), %rdi
  call	_ZNSirsERi@PLT
  movl	12(%rsp), %esi
  movq	%rbx, %rdx
  xorl	%eax, %eax
  .p2align 5
  .p2align 4
  .p2align 3
.L11: i guess these are line headers?
  btl	%eax, %esi
  setc	(%rdx)
  addl	$1, %eax
  addq	$1, %rdx
  cmpl	$32, %eax
  jne	.L11 | I think this may be one of the for loop counters
  movq	%rbx, %rax
  xorl	%ebp, %ebp
  xorl	%ecx, %ecx
  .p2align 5
  .p2align 4
  .p2align 3
.L13:
  movl	$1, %edx
  sall	%cl, %edx
  orl	%ebp, %edx
  cmpb	$0, (%rax)
  cmovne	%edx, %ebp
  addl	$1, %ecx
  addq	$1, %rax
  cmpl	$32, %ecx
  jne	.L13 | most likely the other foor loop counter
  leaq	32(%rbx), %r12
  .p2align 4
  .p2align 3
.L14:
  movsbl	(%rbx), %esi
  leaq	_ZSt4cout(%rip), %rdi
  addq	$1, %rbx
  call	_ZNSolsEi@PLT
  movl	$1, %edx
  leaq	.LC2(%rip), %rsi
  movq	%rax, %rdi
  call
_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
  cmpq	%rbx, %r12
  jne	.L14 | not sure about this one
  movl	8(%rsp), %ecx
  testl	%ecx, %ecx
  jle	.L19
  xorl	%eax, %eax
  xorl	%ebx, %ebx
  .p2align 5
  .p2align 4
  .p2align 3
.L17:
  movl	%eax, %edx
  notl	%edx
  testl	%ebp, %edx
  sete	%dl
  addl	$1, %eax
  movzbl	%dl, %edx
  addl	%edx, %ebx
  cmpl	%ecx, %eax
  jne	.L17 | or this one, the earlier jumps may be the cin/cout
.L15:
  leaq	.LC3(%rip), %rsi
  leaq	_ZSt4cout(%rip), %rdi
  call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
  movl	8(%rsp), %esi
  movq	%rax, %rdi
  subl	%ebx, %esi
  call	_ZNSolsEi@PLT
  leaq	.LC4(%rip), %rsi
  movq	%rax, %rdi
  call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
  leaq	.LC5(%rip), %rsi
  leaq	_ZSt4cout(%rip), %rdi
  call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
  movl	%ebx, %esi
  movq	%rax, %rdi
  call	_ZNSolsEi@PLT
  leaq	.LC4(%rip), %rsi
  movq	%rax, %rdi
  call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
  leaq	.LC6(%rip), %rsi
  leaq	_ZSt4cout(%rip), %rdi
  call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
  pxor	%xmm0, %xmm0
  pxor	%xmm1, %xmm1
  cvtsi2ssl	8(%rsp), %xmm1
  movq	%rax, %rdi
  cvtsi2ssl	%ebx, %xmm0
  divss	%xmm1, %xmm0
  mulss	.LC7(%rip), %xmm0
  cvtss2sd	%xmm0, %xmm0
  call	_ZNSo9_M_insertIdEERSoT_@PLT
  leaq	.LC8(%rip), %rsi
  movq	%rax, %rdi
  call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
  movq	56(%rsp), %rax
  subq	%fs:40, %rax
  jne	.L25 | this is more likely probably the other for loop? idk, like 5
jumps in main
  addq	$64, %rsp
  .cfi_remember_state
  .cfi_def_cfa_offset 32
  xorl	%eax, %eax
  popq	%rbx
  .cfi_def_cfa_offset 24
  popq	%rbp
  .cfi_def_cfa_offset 16
  popq	%r12
  .cfi_def_cfa_offset 8
  ret
.L19:
  .cfi_restore_state
  xorl	%ebx, %ebx
  jmp	.L15
.L25:
  call	__stack_chk_fail@PLT
  .cfi_endproc
.LFE3765:
  .size	main, .-main
  .section	.rodata.cst4,"aM",@progbits,4
  .align 4
.LC7:
  .long	1120403456
  .ident	"GCC: (GNU) 15.2.1 20260103"
  .section	.note.GNU-stack,"",@progbits
*/

  return 0;
}
