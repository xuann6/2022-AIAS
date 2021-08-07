	.file	"mini_sudoku.cpp"
	.text
	.globl	matrix
	.data
	.align 32
	.type	matrix, @object
	.size	matrix, 324
matrix:
	.long	1
	.long	0
	.long	3
	.long	4
	.long	0
	.long	0
	.long	7
	.long	0
	.long	9
	.long	0
	.long	5
	.long	6
	.long	0
	.long	8
	.long	9
	.long	0
	.long	2
	.long	3
	.long	0
	.long	8
	.long	9
	.long	1
	.long	0
	.long	3
	.long	4
	.long	0
	.long	6
	.long	2
	.long	1
	.long	4
	.long	0
	.long	6
	.long	5
	.long	0
	.long	9
	.long	7
	.long	3
	.long	0
	.long	0
	.long	8
	.long	0
	.long	7
	.long	0
	.long	1
	.long	4
	.long	8
	.long	0
	.long	7
	.long	0
	.long	1
	.long	4
	.long	0
	.long	6
	.long	5
	.long	0
	.long	3
	.long	1
	.long	0
	.long	4
	.long	0
	.long	9
	.long	7
	.long	8
	.long	6
	.long	4
	.long	0
	.long	9
	.long	7
	.long	0
	.long	5
	.long	3
	.long	1
	.long	0
	.long	7
	.long	8
	.long	0
	.long	0
	.long	1
	.long	0
	.long	4
	.long	2
	.section	.rodata
.LC0:
	.string	"%d\t"
.LC1:
	.string	"\n"
	.text
	.globl	_Z12print_sudokuv
	.type	_Z12print_sudokuv, @function
_Z12print_sudokuv:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	$0, -8(%rbp)
.L5:
	cmpl	$8, -8(%rbp)
	jg	.L6
	movl	$0, -4(%rbp)
.L4:
	cmpl	$8, -4(%rbp)
	jg	.L3
	movl	-4(%rbp), %eax
	movslq	%eax, %rcx
	movl	-8(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	matrix(%rip), %rax
	movl	(%rdx,%rax), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	addl	$1, -4(%rbp)
	jmp	.L4
.L3:
	leaq	.LC1(%rip), %rdi
	call	puts@PLT
	addl	$1, -8(%rbp)
	jmp	.L5
.L6:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	_Z12print_sudokuv, .-_Z12print_sudokuv
	.globl	_Z17number_unassignedPiS_
	.type	_Z17number_unassignedPiS_, @function
_Z17number_unassignedPiS_:
.LFB1:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movl	$0, -4(%rbp)
	movl	$0, -12(%rbp)
.L13:
	cmpl	$8, -12(%rbp)
	jg	.L8
	movl	$0, -8(%rbp)
.L12:
	cmpl	$8, -8(%rbp)
	jg	.L9
	movl	-8(%rbp), %eax
	movslq	%eax, %rcx
	movl	-12(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	matrix(%rip), %rax
	movl	(%rdx,%rax), %eax
	testl	%eax, %eax
	jne	.L10
	movq	-24(%rbp), %rax
	movl	-12(%rbp), %edx
	movl	%edx, (%rax)
	movq	-32(%rbp), %rax
	movl	-8(%rbp), %edx
	movl	%edx, (%rax)
	movl	$1, -4(%rbp)
	movl	-4(%rbp), %eax
	jmp	.L11
.L10:
	addl	$1, -8(%rbp)
	jmp	.L12
.L9:
	addl	$1, -12(%rbp)
	jmp	.L13
.L8:
	movl	-4(%rbp), %eax
.L11:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	_Z17number_unassignedPiS_, .-_Z17number_unassignedPiS_
	.globl	_Z7is_safeiii
	.type	_Z7is_safeiii, @function
_Z7is_safeiii:
.LFB2:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movl	%edx, -28(%rbp)
	movl	$0, -16(%rbp)
.L18:
	cmpl	$8, -16(%rbp)
	jg	.L15
	movl	-16(%rbp), %eax
	movslq	%eax, %rcx
	movl	-24(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	matrix(%rip), %rax
	movl	(%rdx,%rax), %eax
	cmpl	%eax, -20(%rbp)
	jne	.L16
	movl	$0, %eax
	jmp	.L17
.L16:
	addl	$1, -16(%rbp)
	jmp	.L18
.L15:
	movl	$0, -16(%rbp)
.L21:
	cmpl	$8, -16(%rbp)
	jg	.L19
	movl	-28(%rbp), %eax
	movslq	%eax, %rcx
	movl	-16(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	matrix(%rip), %rax
	movl	(%rdx,%rax), %eax
	cmpl	%eax, -20(%rbp)
	jne	.L20
	movl	$0, %eax
	jmp	.L17
.L20:
	addl	$1, -16(%rbp)
	jmp	.L21
.L19:
	movl	-24(%rbp), %eax
	movslq	%eax, %rdx
	imulq	$1431655766, %rdx, %rdx
	shrq	$32, %rdx
	sarl	$31, %eax
	subl	%eax, %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	movl	%eax, -8(%rbp)
	movl	-28(%rbp), %eax
	movslq	%eax, %rdx
	imulq	$1431655766, %rdx, %rdx
	shrq	$32, %rdx
	sarl	$31, %eax
	subl	%eax, %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	movl	%eax, -4(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -16(%rbp)
.L26:
	movl	-8(%rbp), %eax
	addl	$2, %eax
	cmpl	%eax, -16(%rbp)
	jg	.L22
	movl	-4(%rbp), %eax
	movl	%eax, -12(%rbp)
.L25:
	movl	-4(%rbp), %eax
	addl	$2, %eax
	cmpl	%eax, -12(%rbp)
	jg	.L23
	movl	-12(%rbp), %eax
	movslq	%eax, %rcx
	movl	-16(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	matrix(%rip), %rax
	movl	(%rdx,%rax), %eax
	cmpl	%eax, -20(%rbp)
	jne	.L24
	movl	$0, %eax
	jmp	.L17
.L24:
	addl	$1, -12(%rbp)
	jmp	.L25
.L23:
	addl	$1, -16(%rbp)
	jmp	.L26
.L22:
	movl	$1, %eax
.L17:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	_Z7is_safeiii, .-_Z7is_safeiii
	.globl	_Z12solve_sudokuv
	.type	_Z12solve_sudokuv, @function
_Z12solve_sudokuv:
.LFB3:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	leaq	-16(%rbp), %rdx
	leaq	-20(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	_Z17number_unassignedPiS_
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	.L28
	movl	$1, %eax
	jmp	.L34
.L28:
	movl	$1, -12(%rbp)
.L33:
	cmpl	$9, -12(%rbp)
	jg	.L30
	movl	-16(%rbp), %edx
	movl	-20(%rbp), %ecx
	movl	-12(%rbp), %eax
	movl	%ecx, %esi
	movl	%eax, %edi
	call	_Z7is_safeiii
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	je	.L31
	movl	-20(%rbp), %eax
	movl	-16(%rbp), %edx
	movslq	%edx, %rcx
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rcx
	leaq	matrix(%rip), %rdx
	movl	-12(%rbp), %eax
	movl	%eax, (%rcx,%rdx)
	call	_Z12solve_sudokuv
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	je	.L32
	movl	$1, %eax
	jmp	.L34
.L32:
	movl	-20(%rbp), %eax
	movl	-16(%rbp), %edx
	movslq	%edx, %rcx
	movslq	%eax, %rdx
	movq	%rdx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rdx
	leaq	matrix(%rip), %rax
	movl	$0, (%rdx,%rax)
.L31:
	addl	$1, -12(%rbp)
	jmp	.L33
.L30:
	movl	$0, %eax
.L34:
	movq	-8(%rbp), %rsi
	xorq	%fs:40, %rsi
	je	.L35
	call	__stack_chk_fail@PLT
.L35:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	_Z12solve_sudokuv, .-_Z12solve_sudokuv
	.section	.rodata
.LC2:
	.string	"No solution"
	.text
	.globl	main
	.type	main, @function
main:
.LFB4:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	call	_Z12solve_sudokuv
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	je	.L37
	call	_Z12print_sudokuv
	jmp	.L38
.L37:
	leaq	.LC2(%rip), %rdi
	call	puts@PLT
.L38:
	movl	$0, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:
