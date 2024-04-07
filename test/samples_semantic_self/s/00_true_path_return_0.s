	.text
	.file	"00_true_path_return_0.cact"
	.globl	_Z3fooii                        # -- Begin function _Z3fooii
	.p2align	4, 0x90
	.type	_Z3fooii,@function
_Z3fooii:                               # @_Z3fooii
	.cfi_startproc
# %bb.0:
	movl	$1, %eax
	retq
.Lfunc_end0:
	.size	_Z3fooii, .Lfunc_end0-_Z3fooii
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
