	.file	"bit_packing_32.cpp"
	.text
#APP
	.globl _ZSt21ios_base_library_initv
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC0:
	.string	"cannot create std::vector larger than max_size()"
#NO_APP
	.section	.text.unlikely,"ax",@progbits
.LCOLDB1:
	.text
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
	leaq	0(,%rcx,4), %rdx
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
	je	.L16
	subq	$4, %rdx
	xorl	%esi, %esi
	call	memset@PLT
	leaq	-4(%r13,%rax), %rdx
	subq	%rbx, %rdx
	movq	%rdx, %rax
	movq	%rdx, 8(%rbp)
	subq	%rbx, %rax
	sarq	$2, %rax
	testl	%eax, %eax
	jle	.L1
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
	jne	.L9
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
	movl	$1, %eax
	jmp	.L6
	.cfi_endproc
	.section	.text.unlikely
	.cfi_startproc
	.type	_Z16build_order_bookRKSt6vectorIaSaIaEEi.cold, @function
_Z16build_order_bookRKSt6vectorIaSaIaEEi.cold:
.LFSB3762:
.L12:
	.cfi_def_cfa_offset 64
	.cfi_offset 3, -40
	.cfi_offset 6, -32
	.cfi_offset 12, -24
	.cfi_offset 13, -16
	leaq	.LC0(%rip), %rdi
	call	_ZSt20__throw_length_errorPKc@PLT
	.cfi_endproc
.LFE3762:
	.text
	.size	_Z16build_order_bookRKSt6vectorIaSaIaEEi, .-_Z16build_order_bookRKSt6vectorIaSaIaEEi
	.section	.text.unlikely
	.size	_Z16build_order_bookRKSt6vectorIaSaIaEEi.cold, .-_Z16build_order_bookRKSt6vectorIaSaIaEEi.cold
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
	.size	_Z11kill_switchiRKi, .-_Z11kill_switchiRKi
	.section	.text._ZNSt12_Vector_baseIiSaIiEED2Ev,"axG",@progbits,_ZNSt12_Vector_baseIiSaIiEED5Ev,comdat
	.align 2
	.p2align 4
	.weak	_ZNSt12_Vector_baseIiSaIiEED2Ev
	.type	_ZNSt12_Vector_baseIiSaIiEED2Ev, @function
_ZNSt12_Vector_baseIiSaIiEED2Ev:
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
	.size	_ZNSt12_Vector_baseIiSaIiEED2Ev, .-_ZNSt12_Vector_baseIiSaIiEED2Ev
	.weak	_ZNSt12_Vector_baseIiSaIiEED1Ev
	.set	_ZNSt12_Vector_baseIiSaIiEED1Ev,_ZNSt12_Vector_baseIiSaIiEED2Ev
	.section	.text._ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv,"axG",@progbits,_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv,comdat
	.align 2
	.p2align 4
	.weak	_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv
	.type	_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv, @function
_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv:
.LFB4613:
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
.LFE4613:
	.size	_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv, .-_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EE11_M_gen_randEv
	.section	.text._ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv,"axG",@progbits,_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv,comdat
	.align 2
	.p2align 4
	.weak	_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv
	.type	_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv, @function
_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv:
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
.LFE4525:
	.size	_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv, .-_ZNSt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEclEv
	.text
	.align 2
	.p2align 4
	.type	_ZNSt24uniform_int_distributionIiEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEiRT_RKNS0_10param_typeE.isra.0, @function
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
.LFE4685:
	.size	_ZNSt24uniform_int_distributionIiEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEiRT_RKNS0_10param_typeE.isra.0, .-_ZNSt24uniform_int_distributionIiEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEiRT_RKNS0_10param_typeE.isra.0
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
.L51:
	movl	$255, %edx
	xorl	%esi, %esi
	leaq	96(%rsp), %rdi
	addq	$1, %r13
	call	_ZNSt24uniform_int_distributionIiEclISt23mersenne_twister_engineImLm32ELm624ELm397ELm31ELm2567483615ELm11ELm4294967295ELm7ELm2636928640ELm15ELm4022730752ELm18ELm1812433253EEEEiRT_RKNS0_10param_typeE.isra.0
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
	.section	.data.rel.local.DW.ref.__gxx_personality_v0,"awG",@progbits,DW.ref.__gxx_personality_v0,comdat
	.align 8
	.type	DW.ref.__gxx_personality_v0, @object
	.size	DW.ref.__gxx_personality_v0, 8
DW.ref.__gxx_personality_v0:
	.quad	__gxx_personality_v0
	.globl	__gxx_personality_v0
	.ident	"GCC: (GNU) 15.2.1 20260103"
	.section	.note.GNU-stack,"",@progbits
