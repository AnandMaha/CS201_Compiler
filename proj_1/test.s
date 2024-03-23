	.text
	.file	"test.c"
	.globl	example_function                # -- Begin function example_function
	.p2align	4, 0x90
	.type	example_function,@function
example_function:                       # @example_function
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	cmpl	$0, -4(%rbp)
	jle	.LBB0_2
# %bb.1:
	movabsq	$.L.str, %rdi
	movb	$0, %al
	callq	printf@PLT
	jmp	.LBB0_6
.LBB0_2:
	cmpl	$0, -4(%rbp)
	jge	.LBB0_4
# %bb.3:
	movabsq	$.L.str.1, %rdi
	movb	$0, %al
	callq	printf@PLT
	jmp	.LBB0_5
.LBB0_4:
	movabsq	$.L.str.2, %rdi
	movb	$0, %al
	callq	printf@PLT
.LBB0_5:
	jmp	.LBB0_6
.LBB0_6:
	addq	$16, %rsp
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end0:
	.size	example_function, .Lfunc_end0-example_function
	.cfi_endproc
                                        # -- End function
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	$0, -12(%rbp)
	movl	$5, -8(%rbp)
	movl	$-2, -4(%rbp)
	movl	-8(%rbp), %eax
	cmpl	-4(%rbp), %eax
	jle	.LBB1_2
# %bb.1:
	movl	-8(%rbp), %edi
	callq	example_function
	jmp	.LBB1_3
.LBB1_2:
	movl	-4(%rbp), %edi
	callq	example_function
.LBB1_3:
	xorl	%eax, %eax
	addq	$16, %rsp
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc
                                        # -- End function
	.type	.L.str,@object                  # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"Positive\n"
	.size	.L.str, 10

	.type	.L.str.1,@object                # @.str.1
.L.str.1:
	.asciz	"Negative\n"
	.size	.L.str.1, 10

	.type	.L.str.2,@object                # @.str.2
.L.str.2:
	.asciz	"Zero\n"
	.size	.L.str.2, 6

	.ident	"Ubuntu clang version 17.0.6 (++20231209124227+6009708b4367-1~exp1~20231209124336.77)"
	.section	".note.GNU-stack","",@progbits
