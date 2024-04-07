	.text
	.file	"22_true_no_arg_func.cact"
	.globl	_Z4funcv                        # -- Begin function _Z4funcv
	.p2align	4, 0x90
	.type	_Z4funcv,@function
_Z4funcv:                               # @_Z4funcv
	.cfi_startproc
# %bb.0:
	movl	$5, %eax
	retq
.Lfunc_end0:
	.size	_Z4funcv, .Lfunc_end0-_Z4funcv
	.cfi_endproc
                                        # -- End function
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	movl	$5, %eax
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc
                                        # -- End function
	.ident	"Ubuntu clang version 14.0.0-1ubuntu1.1"
	.section	".note.GNU-stack","",@progbits
	.addrsig
