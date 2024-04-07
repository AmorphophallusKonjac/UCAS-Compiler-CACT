	.text
	.file	"26_true_void_func_stmt.cact"
	.globl	_Z4funci                        # -- Begin function _Z4funci
	.p2align	4, 0x90
	.type	_Z4funci,@function
_Z4funci:                               # @_Z4funci
	.cfi_startproc
# %bb.0:
	retq
.Lfunc_end0:
	.size	_Z4funci, .Lfunc_end0-_Z4funci
	.cfi_endproc
                                        # -- End function
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	movl	$4, %eax
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc
                                        # -- End function
	.ident	"Ubuntu clang version 14.0.0-1ubuntu1.1"
	.section	".note.GNU-stack","",@progbits
	.addrsig
