	.file	"11_implementation_attemp1.cpp"
	.text
#APP
	.globl _ZSt21ios_base_library_initv
#NO_APP
	.section	.text._ZNKSt5ctypeIcE8do_widenEc,"axG",@progbits,_ZNKSt5ctypeIcE8do_widenEc,comdat
	.align 2
	.p2align 4
	.weak	_ZNKSt5ctypeIcE8do_widenEc
	.type	_ZNKSt5ctypeIcE8do_widenEc, @function
_ZNKSt5ctypeIcE8do_widenEc:
.LFB9022:
	.cfi_startproc
	movl	%esi, %eax
	ret
	.cfi_endproc
.LFE9022:
	.size	_ZNKSt5ctypeIcE8do_widenEc, .-_ZNKSt5ctypeIcE8do_widenEc
	.section	.text.unlikely,"ax",@progbits
.LCOLDB0:
	.text
.LHOTB0:
	.p2align 4
	.type	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_.isra.0, @function
_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_.isra.0:
.LFB11922:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdi, %rbx
	subq	$16, %rsp
	.cfi_def_cfa_offset 32
	movq	(%rdi), %rax
	movq	-24(%rax), %rax
	movq	240(%rdi,%rax), %rdi
	testq	%rdi, %rdi
	je	.L8
	cmpb	$0, 56(%rdi)
	je	.L5
	movsbl	67(%rdi), %esi
.L6:
	movq	%rbx, %rdi
	call	_ZNSo3putEc@PLT
	addq	$16, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 16
	movq	%rax, %rdi
	popq	%rbx
	.cfi_def_cfa_offset 8
	jmp	_ZNSo5flushEv@PLT
.L5:
	.cfi_restore_state
	movq	%rdi, 8(%rsp)
	call	_ZNKSt5ctypeIcE13_M_widen_initEv@PLT
	movq	8(%rsp), %rdi
	movl	$10, %esi
	leaq	_ZNKSt5ctypeIcE8do_widenEc(%rip), %rdx
	movq	(%rdi), %rax
	movq	48(%rax), %rax
	cmpq	%rdx, %rax
	je	.L6
	movl	$10, %esi
	call	*%rax
	movsbl	%al, %esi
	jmp	.L6
	.cfi_endproc
	.section	.text.unlikely
	.cfi_startproc
	.type	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_.isra.0.cold, @function
_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_.isra.0.cold:
.LFSB11922:
.L8:
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -16
	call	_ZSt16__throw_bad_castv@PLT
	.cfi_endproc
.LFE11922:
	.text
	.size	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_.isra.0, .-_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_.isra.0
	.section	.text.unlikely
	.size	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_.isra.0.cold, .-_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_.isra.0.cold
.LCOLDE0:
	.text
.LHOTE0:
	.p2align 4
	.globl	_Z14OrderPool_initP9OrderPoolj
	.type	_Z14OrderPool_initP9OrderPoolj, @function
_Z14OrderPool_initP9OrderPoolj:
.LFB11174:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movq	%rdi, %rbx
	movl	%esi, %edi
	movl	$4, %esi
	movq	%rdi, %rbp
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	call	calloc@PLT
	movq	$0, (%rbx)
	movq	%rax, 16(%rbx)
	movl	%ebp, 8(%rbx)
	addq	$8, %rsp
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE11174:
	.size	_Z14OrderPool_initP9OrderPoolj, .-_Z14OrderPool_initP9OrderPoolj
	.p2align 4
	.globl	_Z18OrderPool_AllocateP9OrderPool
	.type	_Z18OrderPool_AllocateP9OrderPool, @function
_Z18OrderPool_AllocateP9OrderPool:
.LFB11175:
	.cfi_startproc
	movq	(%rdi), %rdx
	movq	%rdx, %rax
	notq	%rax
	rep bsfq	%rax, %rax
	btsq	%rax, %rdx
	cltq
	movq	%rdx, (%rdi)
	movq	16(%rdi), %rdx
	leaq	(%rdx,%rax,4), %rax
	ret
	.cfi_endproc
.LFE11175:
	.size	_Z18OrderPool_AllocateP9OrderPool, .-_Z18OrderPool_AllocateP9OrderPool
	.p2align 4
	.globl	_Z14OrderPool_FreeP9OrderPoolP16OrderInformation
	.type	_Z14OrderPool_FreeP9OrderPoolP16OrderInformation, @function
_Z14OrderPool_FreeP9OrderPoolP16OrderInformation:
.LFB11176:
	.cfi_startproc
	subq	16(%rdi), %rsi
	movq	$-2, %rax
	movq	%rsi, %rcx
	sarq	$2, %rcx
	rolq	%cl, %rax
	andq	%rax, (%rdi)
	ret
	.cfi_endproc
.LFE11176:
	.size	_Z14OrderPool_FreeP9OrderPoolP16OrderInformation, .-_Z14OrderPool_FreeP9OrderPoolP16OrderInformation
	.p2align 4
	.globl	_Z21OrderPool_CountActivePK9OrderPool
	.type	_Z21OrderPool_CountActivePK9OrderPool, @function
_Z21OrderPool_CountActivePK9OrderPool:
.LFB11177:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movq	(%rdi), %rdi
	call	__popcountdi2@PLT
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE11177:
	.size	_Z21OrderPool_CountActivePK9OrderPool, .-_Z21OrderPool_CountActivePK9OrderPool
	.p2align 4
	.globl	_Z20build_buy_conditionsP13BuyConditions
	.type	_Z20build_buy_conditionsP13BuyConditions, @function
_Z20build_buy_conditionsP13BuyConditions:
.LFB11178:
	.cfi_startproc
	movq	(%rdi), %rax
	ret
	.cfi_endproc
.LFE11178:
	.size	_Z20build_buy_conditionsP13BuyConditions, .-_Z20build_buy_conditionsP13BuyConditions
	.p2align 4
	.globl	_Z21build_sell_conditionsP14SellConditions
	.type	_Z21build_sell_conditionsP14SellConditions, @function
_Z21build_sell_conditionsP14SellConditions:
.LFB11179:
	.cfi_startproc
	movq	(%rdi), %rax
	ret
	.cfi_endproc
.LFE11179:
	.size	_Z21build_sell_conditionsP14SellConditions, .-_Z21build_sell_conditionsP14SellConditions
	.p2align 4
	.globl	_Z23check_buy_lane0_and_addPK12BuyGateBuiltmP9OrderPool
	.type	_Z23check_buy_lane0_and_addPK12BuyGateBuiltmP9OrderPool, @function
_Z23check_buy_lane0_and_addPK12BuyGateBuiltmP9OrderPool:
.LFB11180:
	.cfi_startproc
	movl	4(%rdi), %eax
	notq	%rsi
	testq	%rax, %rsi
	je	.L19
	xorl	%eax, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L19:
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movq	(%rdx), %rcx
	movq	%rdx, (%rsp)
	movq	%rcx, %rdi
	movq	%rcx, 8(%rsp)
	call	__popcountdi2@PLT
	movq	(%rsp), %rdx
	cmpl	8(%rdx), %eax
	jnb	.L21
	movq	8(%rsp), %rcx
	movq	%rcx, %rax
	notq	%rax
	rep bsfq	%rax, %rax
	btsq	%rax, %rcx
	cltq
	movq	%rcx, (%rdx)
	movq	16(%rdx), %rdx
	leaq	(%rdx,%rax,4), %rax
	testq	%rax, %rax
	je	.L21
	movl	$655460, (%rax)
	movl	$1, %eax
	jmp	.L18
	.p2align 4,,10
	.p2align 3
.L21:
	xorl	%eax, %eax
.L18:
	addq	$24, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE11180:
	.size	_Z23check_buy_lane0_and_addPK12BuyGateBuiltmP9OrderPool, .-_Z23check_buy_lane0_and_addPK12BuyGateBuiltmP9OrderPool
	.p2align 4
	.globl	_Z15check_buy_lane0PK12BuyGateBuiltm
	.type	_Z15check_buy_lane0PK12BuyGateBuiltm, @function
_Z15check_buy_lane0PK12BuyGateBuiltm:
.LFB11181:
	.cfi_startproc
	movl	4(%rdi), %ecx
	xorl	%eax, %eax
	xorl	%edx, %edx
	andq	%rcx, %rsi
	cmpq	%rcx, %rsi
	movl	$10, %ecx
	cmove	%ecx, %eax
	movl	$100, %ecx
	cmove	%ecx, %edx
	sall	$16, %eax
	movw	%dx, %ax
	ret
	.cfi_endproc
.LFE11181:
	.size	_Z15check_buy_lane0PK12BuyGateBuiltm, .-_Z15check_buy_lane0PK12BuyGateBuiltm
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC2:
	.string	"Order added! Active orders: "
.LC3:
	.string	"Failed to add order"
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB11182:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	movl	$64, %edi
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	subq	$48, %rsp
	.cfi_def_cfa_offset 80
	movq	%fs:40, %rsi
	movq	%rsi, 40(%rsp)
	movl	$4, %esi
	call	calloc@PLT
	leaq	16(%rsp), %rdx
	leaq	8(%rsp), %rdi
	movabsq	$281474724065520, %rsi
	movq	%rax, 32(%rsp)
	movq	%rax, %rbx
	movabsq	$-1085102596613406721, %rax
	movl	$64, 24(%rsp)
	movq	%rax, 8(%rsp)
	movq	$0, 16(%rsp)
	call	_Z23check_buy_lane0_and_addPK12BuyGateBuiltmP9OrderPool
	testb	%al, %al
	je	.L32
	movq	16(%rsp), %r14
	leaq	.LC2(%rip), %rsi
	leaq	_ZSt4cout(%rip), %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
	movq	%r14, %rdi
	movq	%rax, %r15
	call	__popcountdi2@PLT
	movq	%r15, %rdi
	movslq	%eax, %rsi
	call	_ZNSo9_M_insertImEERSoT_@PLT
	movq	%rax, %rdi
	call	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_.isra.0
.L33:
	movq	%rbx, %rdi
	call	free@PLT
	movq	40(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L36
	addq	$48, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 32
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	ret
.L32:
	.cfi_restore_state
	leaq	.LC3(%rip), %rsi
	leaq	_ZSt4cout(%rip), %rdi
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@PLT
	movq	%rax, %rdi
	call	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_.isra.0
	jmp	.L33
.L36:
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE11182:
	.size	main, .-main
	.globl	__popcountdi2
	.ident	"GCC: (GNU) 15.2.1 20260209"
	.section	.note.GNU-stack,"",@progbits
