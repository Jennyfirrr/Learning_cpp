	.file	"allocator_practice.cpp"
	.text
#APP
	.globl _ZSt21ios_base_library_initv
#NO_APP
	.p2align 4
	.type	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0, @function
_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0:
.LFB11752:
	.cfi_startproc
	testq	%rsi, %rsi
	je	.L6
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdi, %rbx
	movq	%rsi, %rdi
	subq	$16, %rsp
	.cfi_def_cfa_offset 32
	movq	%rsi, 8(%rsp)
	call	strlen@PLT
	movq	8(%rsp), %rsi
	addq	$16, %rsp
	.cfi_def_cfa_offset 16
	movq	%rbx, %rdi
	movq	%rax, %rdx
	popq	%rbx
	.cfi_restore 3
	.cfi_def_cfa_offset 8
	jmp	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
.L6:
	movq	(%rdi), %rax
	addq	-24(%rax), %rdi
	movl	32(%rdi), %esi
	orl	$1, %esi
	jmp	_ZNSt9basic_iosIcSt11char_traitsIcEE5clearESt12_Ios_Iostate@PLT
	.cfi_endproc
.LFE11752:
	.size	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0, .-_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0
	.p2align 4
	.globl	_Z19order_packing_8byte9OrderPair
	.type	_Z19order_packing_8byte9OrderPair, @function
_Z19order_packing_8byte9OrderPair:
.LFB10995:
	.cfi_startproc
	movq	%rdi, %rax
	ret
	.cfi_endproc
.LFE10995:
	.size	_Z19order_packing_8byte9OrderPair, .-_Z19order_packing_8byte9OrderPair
	.p2align 4
	.globl	_Z14OrderPool_initP9OrderPoolj
	.type	_Z14OrderPool_initP9OrderPoolj, @function
_Z14OrderPool_initP9OrderPoolj:
.LFB10996:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movq	%rdi, %rbx
	movl	%esi, %edi
	movl	$8, %esi
	movq	%rdi, %rbp
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	call	calloc@PLT
	movq	$0, 8(%rbx)
	movq	%rax, (%rbx)
	movl	%ebp, 16(%rbx)
	addq	$8, %rsp
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE10996:
	.size	_Z14OrderPool_initP9OrderPoolj, .-_Z14OrderPool_initP9OrderPoolj
	.p2align 4
	.globl	_Z18OrderPool_AllocateP9OrderPool
	.type	_Z18OrderPool_AllocateP9OrderPool, @function
_Z18OrderPool_AllocateP9OrderPool:
.LFB10997:
	.cfi_startproc
	movq	8(%rdi), %rdx
	movq	%rdx, %rax
	notq	%rax
	rep bsfq	%rax, %rax
	btsq	%rax, %rdx
	cltq
	movq	%rdx, 8(%rdi)
	movq	(%rdi), %rdx
	leaq	(%rdx,%rax,8), %rax
	ret
	.cfi_endproc
.LFE10997:
	.size	_Z18OrderPool_AllocateP9OrderPool, .-_Z18OrderPool_AllocateP9OrderPool
	.p2align 4
	.globl	_Z14OrderPool_FreeP9OrderPoolP9OrderPair
	.type	_Z14OrderPool_FreeP9OrderPoolP9OrderPair, @function
_Z14OrderPool_FreeP9OrderPoolP9OrderPair:
.LFB10998:
	.cfi_startproc
	subq	(%rdi), %rsi
	movq	$-2, %rax
	movq	%rsi, %rcx
	sarq	$3, %rcx
	rolq	%cl, %rax
	andq	%rax, 8(%rdi)
	ret
	.cfi_endproc
.LFE10998:
	.size	_Z14OrderPool_FreeP9OrderPoolP9OrderPair, .-_Z14OrderPool_FreeP9OrderPoolP9OrderPair
	.p2align 4
	.globl	_Z21OrderPool_CountActivePK9OrderPool
	.type	_Z21OrderPool_CountActivePK9OrderPool, @function
_Z21OrderPool_CountActivePK9OrderPool:
.LFB10999:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movq	8(%rdi), %rdi
	call	__popcountdi2@PLT
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE10999:
	.size	_Z21OrderPool_CountActivePK9OrderPool, .-_Z21OrderPool_CountActivePK9OrderPool
	.p2align 4
	.globl	_Z15build_risk_gate9risk_gate
	.type	_Z15build_risk_gate9risk_gate, @function
_Z15build_risk_gate9risk_gate:
.LFB11000:
	.cfi_startproc
	movq	%rdi, %rax
	ret
	.cfi_endproc
.LFE11000:
	.size	_Z15build_risk_gate9risk_gate, .-_Z15build_risk_gate9risk_gate
	.p2align 4
	.globl	_Z15risk_gate_checkmm
	.type	_Z15risk_gate_checkmm, @function
_Z15risk_gate_checkmm:
.LFB11001:
	.cfi_startproc
	movabsq	$-9187201950435737472, %rdx
	movq	%rsi, %rax
	subq	%rdi, %rax
	andq	%rdx, %rax
	ret
	.cfi_endproc
.LFE11001:
	.size	_Z15risk_gate_checkmm, .-_Z15risk_gate_checkmm
	.section	.text._ZNSt23mersenne_twister_engineImLm64ELm312ELm156ELm31ELm13043109905998158313ELm29ELm6148914691236517205ELm17ELm8202884508482404352ELm37ELm18444473444759240704ELm43ELm6364136223846793005EE11_M_gen_randEv,"axG",@progbits,_ZNSt23mersenne_twister_engineImLm64ELm312ELm156ELm31ELm13043109905998158313ELm29ELm6148914691236517205ELm17ELm8202884508482404352ELm37ELm18444473444759240704ELm43ELm6364136223846793005EE11_M_gen_randEv,comdat
	.align 2
	.p2align 4
	.weak	_ZNSt23mersenne_twister_engineImLm64ELm312ELm156ELm31ELm13043109905998158313ELm29ELm6148914691236517205ELm17ELm8202884508482404352ELm37ELm18444473444759240704ELm43ELm6364136223846793005EE11_M_gen_randEv
	.type	_ZNSt23mersenne_twister_engineImLm64ELm312ELm156ELm31ELm13043109905998158313ELm29ELm6148914691236517205ELm17ELm8202884508482404352ELm37ELm18444473444759240704ELm43ELm6364136223846793005EE11_M_gen_randEv, @function
_ZNSt23mersenne_twister_engineImLm64ELm312ELm156ELm31ELm13043109905998158313ELm29ELm6148914691236517205ELm17ELm8202884508482404352ELm37ELm18444473444759240704ELm43ELm6364136223846793005EE11_M_gen_randEv:
.LFB11531:
	.cfi_startproc
	movq	$-2147483648, %rcx
	movl	$2147483647, %esi
	movq	%rdi, %rdx
	movdqa	.LC3(%rip), %xmm4
	movq	%rcx, %xmm7
	movl	$1, %ecx
	movq	%rsi, %xmm6
	movq	%rcx, %xmm5
	movdqa	%xmm4, %xmm8
	leaq	1248(%rdi), %rax
	punpcklqdq	%xmm7, %xmm7
	punpcklqdq	%xmm6, %xmm6
	punpcklqdq	%xmm5, %xmm5
	psrlq	$32, %xmm8
	.p2align 4
	.p2align 3
.L17:
	movdqu	(%rdx), %xmm0
	movdqu	8(%rdx), %xmm1
	addq	$16, %rdx
	movdqu	1232(%rdx), %xmm2
	pand	%xmm6, %xmm1
	pand	%xmm7, %xmm0
	por	%xmm1, %xmm0
	movdqa	%xmm0, %xmm1
	pand	%xmm5, %xmm0
	psrlq	$1, %xmm1
	movdqa	%xmm0, %xmm3
	pxor	%xmm1, %xmm2
	pmuludq	%xmm4, %xmm3
	movdqa	%xmm0, %xmm1
	psrlq	$32, %xmm1
	pmuludq	%xmm8, %xmm0
	pmuludq	%xmm4, %xmm1
	paddq	%xmm0, %xmm1
	psllq	$32, %xmm1
	paddq	%xmm1, %xmm3
	movdqa	%xmm3, %xmm0
	pxor	%xmm2, %xmm0
	movups	%xmm0, -16(%rdx)
	cmpq	%rdx, %rax
	jne	.L17
	movl	$2147483647, %ecx
	movq	$-2147483648, %rsi
	leaq	2480(%rdi), %rdx
	movq	%rcx, %xmm6
	movq	%rsi, %xmm7
	movl	$1, %esi
	movabsq	$-5403634167711393303, %rcx
	movq	%rcx, %xmm4
	movq	%rsi, %xmm5
	punpcklqdq	%xmm7, %xmm7
	punpcklqdq	%xmm4, %xmm4
	punpcklqdq	%xmm6, %xmm6
	punpcklqdq	%xmm5, %xmm5
	movdqa	%xmm4, %xmm8
	psrlq	$32, %xmm8
	.p2align 4
	.p2align 3
.L18:
	movdqu	(%rax), %xmm0
	movdqu	8(%rax), %xmm1
	addq	$16, %rax
	movdqu	-1264(%rax), %xmm2
	pand	%xmm6, %xmm1
	pand	%xmm7, %xmm0
	por	%xmm1, %xmm0
	movdqa	%xmm0, %xmm1
	pand	%xmm5, %xmm0
	psrlq	$1, %xmm1
	movdqa	%xmm0, %xmm3
	pxor	%xmm1, %xmm2
	pmuludq	%xmm4, %xmm3
	movdqa	%xmm0, %xmm1
	psrlq	$32, %xmm1
	pmuludq	%xmm8, %xmm0
	pmuludq	%xmm4, %xmm1
	paddq	%xmm0, %xmm1
	psllq	$32, %xmm1
	paddq	%xmm1, %xmm3
	movdqa	%xmm3, %xmm0
	pxor	%xmm2, %xmm0
	movups	%xmm0, -16(%rax)
	cmpq	%rax, %rdx
	jne	.L18
	movq	2488(%rdi), %rsi
	movq	2480(%rdi), %rax
	movq	$0, 2496(%rdi)
	movq	%rsi, %rdx
	andq	$-2147483648, %rax
	andq	$-2147483648, %rsi
	andl	$2147483647, %edx
	orq	%rdx, %rax
	movq	%rax, %rdx
	andl	$1, %eax
	negq	%rax
	shrq	%rdx
	xorq	1232(%rdi), %rdx
	andq	%rcx, %rax
	xorq	%rdx, %rax
	movq	%rax, 2480(%rdi)
	movq	(%rdi), %rax
	andl	$2147483647, %eax
	orq	%rsi, %rax
	movq	%rax, %rdx
	andl	$1, %eax
	negq	%rax
	shrq	%rdx
	xorq	1240(%rdi), %rdx
	andq	%rcx, %rax
	xorq	%rdx, %rax
	movq	%rax, 2488(%rdi)
	ret
	.cfi_endproc
.LFE11531:
	.size	_ZNSt23mersenne_twister_engineImLm64ELm312ELm156ELm31ELm13043109905998158313ELm29ELm6148914691236517205ELm17ELm8202884508482404352ELm37ELm18444473444759240704ELm43ELm6364136223846793005EE11_M_gen_randEv, .-_ZNSt23mersenne_twister_engineImLm64ELm312ELm156ELm31ELm13043109905998158313ELm29ELm6148914691236517205ELm17ELm8202884508482404352ELm37ELm18444473444759240704ELm43ELm6364136223846793005EE11_M_gen_randEv
	.text
	.p2align 4
	.globl	_Z16order_generationv
	.type	_Z16order_generationv, @function
_Z16order_generationv:
.LFB11002:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movq	2496+_ZL3rng(%rip), %rax
	leaq	_ZL3rng(%rip), %rdx
	cmpq	$311, %rax
	ja	.L24
.L22:
	leaq	1(%rax), %rcx
	movq	(%rdx,%rax,8), %rax
	movq	%rcx, 2496+_ZL3rng(%rip)
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	movabsq	$6148914691236517205, %rcx
	movq	%rax, %rdx
	shrq	$29, %rdx
	andq	%rcx, %rdx
	movabsq	$8202884508482404352, %rcx
	xorq	%rdx, %rax
	movq	%rax, %rdx
	salq	$17, %rdx
	andq	%rcx, %rdx
	movabsq	$-2270628950310912, %rcx
	xorq	%rdx, %rax
	movq	%rax, %rdx
	salq	$37, %rdx
	andq	%rcx, %rdx
	xorq	%rdx, %rax
	movq	%rax, %rdx
	shrq	$43, %rdx
	xorq	%rdx, %rax
	movabsq	$9187201950435737471, %rdx
	andq	%rdx, %rax
	ret
	.p2align 4,,10
	.p2align 3
.L24:
	.cfi_restore_state
	movq	%rdx, %rdi
	call	_ZNSt23mersenne_twister_engineImLm64ELm312ELm156ELm31ELm13043109905998158313ELm29ELm6148914691236517205ELm17ELm8202884508482404352ELm37ELm18444473444759240704ELm43ELm6364136223846793005EE11_M_gen_randEv
	movq	2496+_ZL3rng(%rip), %rax
	leaq	_ZL3rng(%rip), %rdx
	jmp	.L22
	.cfi_endproc
.LFE11002:
	.size	_Z16order_generationv, .-_Z16order_generationv
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC5:
	.string	"Please select how long to run this: "
	.align 8
.LC6:
	.string	"Please select risk gate id [0-127]: "
	.align 8
.LC7:
	.string	"\n======================================================================\n"
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC8:
	.string	"Total Orders Generated: "
.LC9:
	.string	"\n"
	.section	.rodata.str1.8
	.align 8
.LC10:
	.string	"======================================================================\n"
	.section	.rodata.str1.1
.LC11:
	.string	"Passed: "
.LC12:
	.string	"Failed: "
.LC13:
	.string	"Passed %: "
.LC15:
	.string	"%\n"
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB11003:
	.cfi_startproc
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
	subq	$56, %rsp
	.cfi_def_cfa_offset 112
	movq	%fs:40, %rsi
	movq	%rsi, 40(%rsp)
	leaq	.LC5(%rip), %rsi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0
	leaq	32(%rsp), %rsi
	leaq	_ZSt3cin(%rip), %rdi
	call	_ZNSi10_M_extractImEERSiRT_@PLT
	leaq	.LC6(%rip), %rsi
	leaq	_ZSt4cout(%rip), %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0
	leaq	28(%rsp), %rsi
	leaq	_ZSt3cin(%rip), %rdi
	call	_ZNSi10_M_extractIjEERSiRT_@PLT
	movq	32(%rsp), %r14
	testq	%r14, %r14
	je	.L34
	movzbl	28(%rsp), %eax
	movq	2496+_ZL3rng(%rip), %r13
	xorl	%r12d, %r12d
	xorl	%ebx, %ebx
	movabsq	$4311810305, %rcx
	leaq	_ZL3rng(%rip), %rbp
	movabsq	$6148914691236517205, %r15
	imulq	%rax, %rcx
	movq	%rax, %rsi
	salq	$40, %rsi
	orq	%rcx, %rsi
	movq	%rax, %rcx
	salq	$56, %rax
	salq	$48, %rcx
	orq	%rsi, %rcx
	orq	%rcx, %rax
	movq	%rax, 8(%rsp)
	jmp	.L28
	.p2align 4,,10
	.p2align 3
.L27:
	leaq	1(%rax), %r13
	movq	0(%rbp,%rax,8), %rax
	movq	8(%rsp), %rdi
	movabsq	$8202884508482404352, %rdx
	movabsq	$-2270628950310912, %rcx
	movq	%r13, 2496(%rbp)
	movq	%rax, %rsi
	shrq	$29, %rsi
	andq	%r15, %rsi
	xorq	%rsi, %rax
	movq	%rax, %rsi
	salq	$17, %rsi
	andq	%rdx, %rsi
	movabsq	$9187201950435737471, %rdx
	xorq	%rsi, %rax
	movq	%rax, %rsi
	salq	$37, %rsi
	andq	%rcx, %rsi
	xorq	%rsi, %rax
	movq	%rax, %rsi
	shrq	$43, %rsi
	xorq	%rsi, %rax
	andq	%rdx, %rax
	subq	%rax, %rdi
	movabsq	$-9187201950435737472, %rax
	andq	%rax, %rdi
	call	__popcountdi2@PLT
	cltq
	subq	%rax, %rbx
	addq	%rax, %r12
	addq	$8, %rbx
	subq	$1, %r14
	movq	%r14, 32(%rsp)
	je	.L26
.L28:
	movq	%r13, %rax
	cmpq	$311, %r13
	jbe	.L27
	movq	%rbp, %rdi
	call	_ZNSt23mersenne_twister_engineImLm64ELm312ELm156ELm31ELm13043109905998158313ELm29ELm6148914691236517205ELm17ELm8202884508482404352ELm37ELm18444473444759240704ELm43ELm6364136223846793005EE11_M_gen_randEv
	movq	2496(%rbp), %rax
	jmp	.L27
.L34:
	xorl	%r12d, %r12d
	xorl	%ebx, %ebx
	.p2align 4
	.p2align 3
.L26:
	leaq	.LC7(%rip), %rsi
	leaq	_ZSt4cout(%rip), %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0
	leaq	(%r12,%rbx), %rbp
	movl	$24, %edx
	leaq	.LC8(%rip), %rsi
	leaq	_ZSt4cout(%rip), %rdi
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	movq	%rbp, %rsi
	leaq	_ZSt4cout(%rip), %rdi
	call	_ZNSo9_M_insertImEERSoT_@PLT
	leaq	.LC9(%rip), %rsi
	movq	%rax, %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0
	leaq	.LC10(%rip), %rsi
	leaq	_ZSt4cout(%rip), %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0
	movl	$8, %edx
	leaq	.LC11(%rip), %rsi
	leaq	_ZSt4cout(%rip), %rdi
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	movq	%rbx, %rsi
	leaq	_ZSt4cout(%rip), %rdi
	call	_ZNSo9_M_insertImEERSoT_@PLT
	leaq	.LC9(%rip), %rsi
	movq	%rax, %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0
	movl	$8, %edx
	leaq	.LC12(%rip), %rsi
	leaq	_ZSt4cout(%rip), %rdi
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	movq	%r12, %rsi
	leaq	_ZSt4cout(%rip), %rdi
	call	_ZNSo9_M_insertImEERSoT_@PLT
	leaq	.LC9(%rip), %rsi
	movq	%rax, %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0
	leaq	.LC10(%rip), %rsi
	leaq	_ZSt4cout(%rip), %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0
	movl	$10, %edx
	leaq	.LC13(%rip), %rsi
	leaq	_ZSt4cout(%rip), %rdi
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	testq	%rbx, %rbx
	js	.L29
	pxor	%xmm0, %xmm0
	cvtsi2ssq	%rbx, %xmm0
.L30:
	testq	%rbp, %rbp
	js	.L31
	pxor	%xmm1, %xmm1
	cvtsi2ssq	%rbp, %xmm1
.L32:
	divss	%xmm1, %xmm0
	leaq	_ZSt4cout(%rip), %rdi
	mulss	.LC14(%rip), %xmm0
	cvtss2sd	%xmm0, %xmm0
	call	_ZNSo9_M_insertIdEERSoT_@PLT
	leaq	.LC15(%rip), %rsi
	movq	%rax, %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0
	leaq	.LC10(%rip), %rsi
	leaq	_ZSt4cout(%rip), %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc.isra.0
	movq	40(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L38
	addq	$56, %rsp
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
.L29:
	.cfi_restore_state
	movq	%rbx, %rax
	andl	$1, %ebx
	pxor	%xmm0, %xmm0
	shrq	%rax
	orq	%rbx, %rax
	cvtsi2ssq	%rax, %xmm0
	addss	%xmm0, %xmm0
	jmp	.L30
.L31:
	movq	%rbp, %rax
	andl	$1, %ebp
	pxor	%xmm1, %xmm1
	shrq	%rax
	orq	%rbp, %rax
	cvtsi2ssq	%rax, %xmm1
	addss	%xmm1, %xmm1
	jmp	.L32
.L38:
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE11003:
	.size	main, .-main
	.p2align 4
	.type	_GLOBAL__sub_I__Z19order_packing_8byte9OrderPair, @function
_GLOBAL__sub_I__Z19order_packing_8byte9OrderPair:
.LFB11750:
	.cfi_startproc
	movq	$125124, _ZL3rng(%rip)
	movl	$125124, %ecx
	movl	$1, %edx
	leaq	_ZL3rng(%rip), %rdi
	movabsq	$6364136223846793005, %rsi
	.p2align 6
	.p2align 4
	.p2align 3
.L40:
	movq	%rcx, %rax
	shrq	$62, %rax
	xorq	%rcx, %rax
	imulq	%rsi, %rax
	leaq	(%rax,%rdx), %rcx
	movq	%rcx, (%rdi,%rdx,8)
	addq	$1, %rdx
	cmpq	$312, %rdx
	jne	.L40
	movq	$312, 2496+_ZL3rng(%rip)
	ret
	.cfi_endproc
.LFE11750:
	.size	_GLOBAL__sub_I__Z19order_packing_8byte9OrderPair, .-_GLOBAL__sub_I__Z19order_packing_8byte9OrderPair
	.section	.init_array,"aw"
	.align 8
	.quad	_GLOBAL__sub_I__Z19order_packing_8byte9OrderPair
	.local	_ZL3rng
	.comm	_ZL3rng,2504,32
	.section	.rodata.cst16,"aM",@progbits,16
	.align 16
.LC3:
	.quad	-5403634167711393303
	.quad	-5403634167711393303
	.section	.rodata.cst4,"aM",@progbits,4
	.align 4
.LC14:
	.long	1120403456
	.globl	__popcountdi2
	.ident	"GCC: (GNU) 15.2.1 20260103"
	.section	.note.GNU-stack,"",@progbits
