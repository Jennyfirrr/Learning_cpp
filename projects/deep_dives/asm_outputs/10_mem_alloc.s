	.file	"10_memory_allocation_implementation.cpp"
	.text
#APP
	.globl _ZSt21ios_base_library_initv
#NO_APP
	.p2align 4
	.globl	_Z14OrderPool_initP9OrderPoolj
	.type	_Z14OrderPool_initP9OrderPoolj, @function
_Z14OrderPool_initP9OrderPoolj:
.LFB11181:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movq	%rdi, %rbx
	movl	%esi, %edi
	movl	$1, %esi
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
.LFE11181:
	.size	_Z14OrderPool_initP9OrderPoolj, .-_Z14OrderPool_initP9OrderPoolj
	.p2align 4
	.globl	_Z18OrderPool_AllocateP9OrderPool
	.type	_Z18OrderPool_AllocateP9OrderPool, @function
_Z18OrderPool_AllocateP9OrderPool:
.LFB11182:
	.cfi_startproc
	movq	(%rdi), %rdx
	movq	%rdx, %rax
	notq	%rax
	rep bsfq	%rax, %rax
	btsq	%rax, %rdx
	cltq
	addq	16(%rdi), %rax
	movq	%rdx, (%rdi)
	ret
	.cfi_endproc
.LFE11182:
	.size	_Z18OrderPool_AllocateP9OrderPool, .-_Z18OrderPool_AllocateP9OrderPool
	.p2align 4
	.globl	_Z14OrderPool_FreeP9OrderPoolP12CurrentOrder
	.type	_Z14OrderPool_FreeP9OrderPoolP12CurrentOrder, @function
_Z14OrderPool_FreeP9OrderPoolP12CurrentOrder:
.LFB11183:
	.cfi_startproc
	movq	%rsi, %rcx
	movq	$-2, %rax
	subq	16(%rdi), %rcx
	rolq	%cl, %rax
	andq	%rax, (%rdi)
	ret
	.cfi_endproc
.LFE11183:
	.size	_Z14OrderPool_FreeP9OrderPoolP12CurrentOrder, .-_Z14OrderPool_FreeP9OrderPoolP12CurrentOrder
	.p2align 4
	.globl	_Z21OrderPool_CountActivePK9OrderPool
	.type	_Z21OrderPool_CountActivePK9OrderPool, @function
_Z21OrderPool_CountActivePK9OrderPool:
.LFB11184:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movq	(%rdi), %rdi
	call	__popcountdi2@PLT
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE11184:
	.size	_Z21OrderPool_CountActivePK9OrderPool, .-_Z21OrderPool_CountActivePK9OrderPool
	.p2align 4
	.globl	_Z20order_packing_8_byte13TrackedOrders
	.type	_Z20order_packing_8_byte13TrackedOrders, @function
_Z20order_packing_8_byte13TrackedOrders:
.LFB11185:
	.cfi_startproc
	movq	%rdi, %rax
	ret
	.cfi_endproc
.LFE11185:
	.size	_Z20order_packing_8_byte13TrackedOrders, .-_Z20order_packing_8_byte13TrackedOrders
	.p2align 4
	.globl	_Z19build_risk_gate_buy16RiskGate_BuySide
	.type	_Z19build_risk_gate_buy16RiskGate_BuySide, @function
_Z19build_risk_gate_buy16RiskGate_BuySide:
.LFB11186:
	.cfi_startproc
	movq	%rdi, %rax
	ret
	.cfi_endproc
.LFE11186:
	.size	_Z19build_risk_gate_buy16RiskGate_BuySide, .-_Z19build_risk_gate_buy16RiskGate_BuySide
	.p2align 4
	.globl	_Z20build_risk_gate_sell17RiskGate_SellSide
	.type	_Z20build_risk_gate_sell17RiskGate_SellSide, @function
_Z20build_risk_gate_sell17RiskGate_SellSide:
.LFB11187:
	.cfi_startproc
	movq	%rdi, %rax
	ret
	.cfi_endproc
.LFE11187:
	.size	_Z20build_risk_gate_sell17RiskGate_SellSide, .-_Z20build_risk_gate_sell17RiskGate_SellSide
	.p2align 4
	.globl	_Z24risk_gate_check_buy_sidemm
	.type	_Z24risk_gate_check_buy_sidemm, @function
_Z24risk_gate_check_buy_sidemm:
.LFB11188:
	.cfi_startproc
	movabsq	$-9187201950435737472, %rdx
	movq	%rsi, %rax
	subq	%rdi, %rax
	andq	%rdx, %rax
	ret
	.cfi_endproc
.LFE11188:
	.size	_Z24risk_gate_check_buy_sidemm, .-_Z24risk_gate_check_buy_sidemm
	.p2align 4
	.globl	_Z25risk_gate_check_sell_sidemm
	.type	_Z25risk_gate_check_sell_sidemm, @function
_Z25risk_gate_check_sell_sidemm:
.LFB11189:
	.cfi_startproc
	movabsq	$8102099357864587376, %rdx
	movq	%rsi, %rax
	subq	%rdi, %rax
	andq	%rdx, %rax
	ret
	.cfi_endproc
.LFE11189:
	.size	_Z25risk_gate_check_sell_sidemm, .-_Z25risk_gate_check_sell_sidemm
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.type	_GLOBAL__sub_I__Z14OrderPool_initP9OrderPoolj, @function
_GLOBAL__sub_I__Z14OrderPool_initP9OrderPoolj:
.LFB11926:
	.cfi_startproc
	movq	$123124, _ZL3rng(%rip)
	movl	$123124, %ecx
	movl	$1, %edx
	leaq	_ZL3rng(%rip), %rdi
	movabsq	$6364136223846793005, %rsi
	.p2align 6
	.p2align 4
	.p2align 3
.L14:
	movq	%rcx, %rax
	shrq	$62, %rax
	xorq	%rcx, %rax
	imulq	%rsi, %rax
	leaq	(%rax,%rdx), %rcx
	movq	%rcx, (%rdi,%rdx,8)
	addq	$1, %rdx
	cmpq	$312, %rdx
	jne	.L14
	movq	$312, 2496+_ZL3rng(%rip)
	ret
	.cfi_endproc
.LFE11926:
	.size	_GLOBAL__sub_I__Z14OrderPool_initP9OrderPoolj, .-_GLOBAL__sub_I__Z14OrderPool_initP9OrderPoolj
	.section	.init_array,"aw"
	.align 8
	.quad	_GLOBAL__sub_I__Z14OrderPool_initP9OrderPoolj
	.local	_ZL3rng
	.comm	_ZL3rng,2504,32
	.globl	__popcountdi2
	.ident	"GCC: (GNU) 15.2.1 20260103"
	.section	.note.GNU-stack,"",@progbits
