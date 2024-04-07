	.text
	.file	"35_true_float_multidim_call.cact"
	.globl	_Z3fooPdS_                      # -- Begin function _Z3fooPdS_
	.p2align	4, 0x90
	.type	_Z3fooPdS_,@function
_Z3fooPdS_:                             # @_Z3fooPdS_
	.cfi_startproc
# %bb.0:
	movsd	(%rdi), %xmm0                   # xmm0 = mem[0],zero
	addsd	(%rsi), %xmm0
	retq
.Lfunc_end0:
	.size	_Z3fooPdS_, .Lfunc_end0-_Z3fooPdS_
	.cfi_endproc
                                        # -- End function
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	xorl	%eax, %eax
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc
                                        # -- End function
	.ident	"Ubuntu clang version 14.0.0-1ubuntu1.1"
	.section	".note.GNU-stack","",@progbits
	.addrsig
