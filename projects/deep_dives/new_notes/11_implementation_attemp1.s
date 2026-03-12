	.file	"11_implementation_attemp1.cpp"
	.text
#APP
	.globl _ZSt21ios_base_library_initv
#NO_APP
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
	movl	$8, %esi
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
	leaq	(%rdx,%rax,8), %rax
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
	sarq	$3, %rcx
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
	.globl	_Z20build_buy_conditionsP21BuySideGateConditions
	.type	_Z20build_buy_conditionsP21BuySideGateConditions, @function
_Z20build_buy_conditionsP21BuySideGateConditions:
.LFB11178:
	.cfi_startproc
	movq	(%rdi), %rax
	ret
	.cfi_endproc
.LFE11178:
	.size	_Z20build_buy_conditionsP21BuySideGateConditions, .-_Z20build_buy_conditionsP21BuySideGateConditions
	.p2align 4
	.globl	_Z21build_sell_conditionsP22SellSideGateConditions
	.type	_Z21build_sell_conditionsP22SellSideGateConditions, @function
_Z21build_sell_conditionsP22SellSideGateConditions:
.LFB11179:
	.cfi_startproc
	movq	(%rdi), %rax
	ret
	.cfi_endproc
.LFE11179:
	.size	_Z21build_sell_conditionsP22SellSideGateConditions, .-_Z21build_sell_conditionsP22SellSideGateConditions
	.p2align 4
	.globl	_Z15check_buy_lane0PK12BuyGateBuiltPK10DataStreamP9OrderPool
	.type	_Z15check_buy_lane0PK12BuyGateBuiltPK10DataStreamP9OrderPool, @function
_Z15check_buy_lane0PK12BuyGateBuiltPK10DataStreamP9OrderPool:
.LFB11180:
	.cfi_startproc
	movq	(%rdi), %rax
	movq	(%rdx), %rdi
	movq	(%rsi), %xmm0
	movq	%rdi, %rcx
	movd	%xmm0, %esi
	notq	%rcx
	pshufd	$0xe5, %xmm0, %xmm1
	rep bsfq	%rcx, %rcx
	cmpl	%eax, %esi
	movd	%xmm1, %esi
	setbe	%r8b
	shrq	$32, %rax
	cmpq	%rax, %rsi
	setnb	%al
	movzbl	%al, %eax
	andq	%r8, %rax
	salq	%cl, %rax
	movslq	%ecx, %rcx
	orq	%rdi, %rax
	movq	%rax, (%rdx)
	movq	16(%rdx), %rax
	movq	%xmm0, (%rax,%rcx,8)
	ret
	.cfi_endproc
.LFE11180:
	.size	_Z15check_buy_lane0PK12BuyGateBuiltPK10DataStreamP9OrderPool, .-_Z15check_buy_lane0PK12BuyGateBuiltPK10DataStreamP9OrderPool
	.p2align 4
	.globl	_Z16check_sell_lane0PK13SellGateBuiltP10DataStreamP9OrderPoolPK12ProfitTarget
	.type	_Z16check_sell_lane0PK13SellGateBuiltP10DataStreamP9OrderPoolPK12ProfitTarget, @function
_Z16check_sell_lane0PK13SellGateBuiltP10DataStreamP9OrderPoolPK12ProfitTarget:
.LFB11181:
	.cfi_startproc
	movq	%rsi, %rax
	movq	(%rdx), %rsi
	movq	%rdx, %r10
	testq	%rsi, %rsi
	je	.L11
	movl	(%rax), %r9d
	movl	(%rcx), %edi
	movq	16(%rdx), %r8
	movq	%rsi, %rdx
	.p2align 6
	.p2align 4
	.p2align 3
.L13:
	xorl	%ecx, %ecx
	rep bsfq	%rdx, %rcx
	movslq	%ecx, %rax
	movl	(%r8,%rax,8), %r11d
	xorl	%eax, %eax
	addl	%edi, %r11d
	cmpl	%r11d, %r9d
	setnb	%al
	salq	%cl, %rax
	notq	%rax
	andq	%rax, %rsi
	leaq	-1(%rdx), %rax
	andq	%rax, %rdx
	jne	.L13
	movq	%rsi, (%r10)
.L11:
	ret
	.cfi_endproc
.LFE11181:
	.size	_Z16check_sell_lane0PK13SellGateBuiltP10DataStreamP9OrderPoolPK12ProfitTarget, .-_Z16check_sell_lane0PK13SellGateBuiltP10DataStreamP9OrderPoolPK12ProfitTarget
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB11182:
	.cfi_startproc
	xorl	%eax, %eax
	ret
	.cfi_endproc
.LFE11182:
	.size	main, .-main
	.globl	__popcountdi2
	.ident	"GCC: (GNU) 15.2.1 20260209"
	.section	.note.GNU-stack,"",@progbits
