	.file	"mycroft_implementation.cpp"
	.text
#APP
	.globl _ZSt21ios_base_library_initv
#NO_APP
	.p2align 4
	.globl	_Z16build_order_bookRKSt6vectorIhSaIhEEj
	.type	_Z16build_order_bookRKSt6vectorIhSaIhEEj, @function
_Z16build_order_bookRKSt6vectorIhSaIhEEj:
.LFB3722:
	.cfi_startproc
	pushq	%r13
	.cfi_def_cfa_offset 16
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
	je	.L13
	leal	0(,%rdx,8), %ecx
	movl	%edx, 12(%rsp)
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
	cmpl	$1, 12(%rsp)
	je	.L14
	leaq	-8(%rcx), %rdx
	xorl	%esi, %esi
	call	memset@PLT
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
	addq	$1, %rdx
	sall	%cl, %eax
	addl	$8, %ecx
	cltq
	orq	%rax, %rsi
	cmpl	$64, %ecx
	jne	.L7
	movq	%rsi, (%rbx,%rdi)
	addq	$8, %rdi
	cmpq	%rdi, %r8
	jne	.L8
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
.L13:
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
.L14:
	.cfi_restore_state
	movq	%rdi, 8(%rbp)
	movl	$1, %eax
	jmp	.L5
	.cfi_endproc
.LFE3722:
	.size	_Z16build_order_bookRKSt6vectorIhSaIhEEj, .-_Z16build_order_bookRKSt6vectorIhSaIhEEj
	.p2align 4
	.globl	_Z9risk_gatehh
	.type	_Z9risk_gatehh, @function
_Z9risk_gatehh:
.LFB3732:
	.cfi_startproc
	movzbl	%dil, %edx
	movl	%edi, %eax
	sall	$24, %eax
	imulq	$65792, %rdx, %rdi
	cltq
	orq	%rdx, %rax
	orq	%rdi, %rax
	ret
	.cfi_endproc
.LFE3732:
	.size	_Z9risk_gatehh, .-_Z9risk_gatehh
	.p2align 4
	.globl	_Z21calc_laneMatchCount64mm
	.type	_Z21calc_laneMatchCount64mm, @function
_Z21calc_laneMatchCount64mm:
.LFB3733:
	.cfi_startproc
	xorq	%rsi, %rdi
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	.p2align 5
	.p2align 4
	.p2align 3
.L17:
	movq	%rdi, %rdx
	shrq	%cl, %rdx
	cmpb	$1, %dl
	adcl	$0, %eax
	addl	$8, %ecx
	cmpl	$64, %ecx
	jne	.L17
	ret
	.cfi_endproc
.LFE3733:
	.size	_Z21calc_laneMatchCount64mm, .-_Z21calc_laneMatchCount64mm
	.section	.text._ZNSt12_Vector_baseImSaImEED2Ev,"axG",@progbits,_ZNSt12_Vector_baseImSaImEED5Ev,comdat
	.align 2
	.p2align 4
	.weak	_ZNSt12_Vector_baseImSaImEED2Ev
	.type	_ZNSt12_Vector_baseImSaImEED2Ev, @function
_ZNSt12_Vector_baseImSaImEED2Ev:
.LFB4101:
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
.LFE4101:
	.size	_ZNSt12_Vector_baseImSaImEED2Ev, .-_ZNSt12_Vector_baseImSaImEED2Ev
	.weak	_ZNSt12_Vector_baseImSaImEED1Ev
	.set	_ZNSt12_Vector_baseImSaImEED1Ev,_ZNSt12_Vector_baseImSaImEED2Ev
	.section	.text._ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv,"axG",@progbits,_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv,comdat
	.align 2
	.p2align 4
	.weak	_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv
	.type	_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv, @function
_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv:
.LFB4576:
	.cfi_startproc
	movq	(%rdi), %r8
	movq	%rdi, %rdx
	leaq	1816(%rdi), %r9
	movq	%rdi, %rcx
	.p2align 4
	.p2align 3
.L22:
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
	.size	_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv, .-_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv
	.section	.text._ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv,"axG",@progbits,_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv,comdat
	.align 2
	.p2align 4
	.weak	_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv
	.type	_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv, @function
_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv:
.LFB4488:
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
	call	_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv
	movq	8(%rsp), %rdi
	movq	4992(%rdi), %rax
	jmp	.L27
	.cfi_endproc
.LFE4488:
	.size	_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv, .-_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv
	.text
	.align 2
	.p2align 4
	.type	_ZNSt24uniform_int_distributionIiEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEiRT_RKNS0_10param_typeE.isra.0, @function
_ZNSt24uniform_int_distributionIiEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEiRT_RKNS0_10param_typeE.isra.0:
.LFB4648:
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
	call	_ZNSt24uniform_int_distributionIiEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEiRT_RKNS0_10param_typeE.isra.0
	movq	%rbp, %rdi
	movl	%eax, %r12d
	call	_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv
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
	call	_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv
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
	call	_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv
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
	call	_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv
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
	.size	_ZNSt24uniform_int_distributionIiEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEiRT_RKNS0_10param_typeE.isra.0, .-_ZNSt24uniform_int_distributionIiEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEiRT_RKNS0_10param_typeE.isra.0
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
	call	_ZNSt24uniform_int_distributionIiEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEiRT_RKNS0_10param_typeE.isra.0
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
	.section	.data.rel.local.DW.ref.__gxx_personality_v0,"awG",@progbits,DW.ref.__gxx_personality_v0,comdat
	.align 8
	.type	DW.ref.__gxx_personality_v0, @object
	.size	DW.ref.__gxx_personality_v0, 8
DW.ref.__gxx_personality_v0:
	.quad	__gxx_personality_v0
	.globl	__gxx_personality_v0
	.ident	"GCC: (GNU) 15.2.1 20260103"
	.section	.note.GNU-stack,"",@progbits
