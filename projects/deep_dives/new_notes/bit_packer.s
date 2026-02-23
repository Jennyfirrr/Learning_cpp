	.file	"bit_packer.cpp"
	.text
#APP
	.globl _ZSt21ios_base_library_initv
#NO_APP
	.p2align 4
	.globl	_Z11kill_switchii
	.type	_Z11kill_switchii, @function
_Z11kill_switchii:
.LFB3762:
	.cfi_startproc
	notl	%edi
	xorl	%eax, %eax
	testl	%esi, %edi
	sete	%al
	ret
	.cfi_endproc
.LFE3762:
	.size	_Z11kill_switchii, .-_Z11kill_switchii
	.p2align 4
	.globl	_Z15build_kill_maskRKSt5arrayIaLm32EE
	.type	_Z15build_kill_maskRKSt5arrayIaLm32EE, @function
_Z15build_kill_maskRKSt5arrayIaLm32EE:
.LFB3763:
	.cfi_startproc
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 5
	.p2align 4
	.p2align 3
.L5:
	cmpb	$0, (%rdi,%rcx)
	je	.L4
	movl	$1, %edx
	sall	%cl, %edx
	orl	%edx, %eax
.L4:
	addq	$1, %rcx
	cmpq	$32, %rcx
	jne	.L5
	ret
	.cfi_endproc
.LFE3763:
	.size	_Z15build_kill_maskRKSt5arrayIaLm32EE, .-_Z15build_kill_maskRKSt5arrayIaLm32EE
	.p2align 4
	.globl	_Z22build_kill_switch_bitsRKi
	.type	_Z22build_kill_switch_bitsRKi, @function
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
	jne	.L8
	movq	%rdi, %rax
	ret
	.cfi_endproc
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
.L11:
	btl	%eax, %esi
	setc	(%rdx)
	addl	$1, %eax
	addq	$1, %rdx
	cmpl	$32, %eax
	jne	.L11
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
	jne	.L13
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
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	cmpq	%rbx, %r12
	jne	.L14
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
	jne	.L17
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
	jne	.L25
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
