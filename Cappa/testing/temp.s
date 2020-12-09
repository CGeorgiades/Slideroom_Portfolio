	.text
	.file	"Module"
	.p2align	4, 0x90         # -- Begin function =
	.type	".L=",@function
".L=":                                  # @"="
	.cfi_startproc
# %bb.0:                                # %entry
	movsd	(%rsi), %xmm0           # xmm0 = mem[0],zero
	movq	(%rdi), %rax
	movsd	%xmm0, (%rax)
	movsd	(%rsi), %xmm0           # xmm0 = mem[0],zero
	retq
.Lfunc_end0:
	.size	".L=", .Lfunc_end0-".L="
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function =.1
	.type	".L=.1",@function
".L=.1":                                # @"=.1"
	.cfi_startproc
# %bb.0:                                # %entry
	movl	(%rsi), %eax
	movq	(%rdi), %rcx
	movl	%eax, (%rcx)
	movl	(%rsi), %eax
	retq
.Lfunc_end1:
	.size	".L=.1", .Lfunc_end1-".L=.1"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function =.2
	.type	".L=.2",@function
".L=.2":                                # @"=.2"
	.cfi_startproc
# %bb.0:                                # %entry
	movb	(%rsi), %al
	movq	(%rdi), %rcx
	movb	%al, (%rcx)
	movb	(%rsi), %al
	retq
.Lfunc_end2:
	.size	".L=.2", .Lfunc_end2-".L=.2"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function =.3
	.type	".L=.3",@function
".L=.3":                                # @"=.3"
	.cfi_startproc
# %bb.0:                                # %entry
	movb	(%rsi), %al
	movq	(%rdi), %rcx
	movb	%al, (%rcx)
	movb	(%rsi), %al
	retq
.Lfunc_end3:
	.size	".L=.3", .Lfunc_end3-".L=.3"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function +
	.type	".L+",@function
".L+":                                  # @"+"
	.cfi_startproc
# %bb.0:                                # %entry
	movsd	(%rdi), %xmm0           # xmm0 = mem[0],zero
	addsd	(%rsi), %xmm0
	retq
.Lfunc_end4:
	.size	".L+", .Lfunc_end4-".L+"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function +.4
	.type	".L+.4",@function
".L+.4":                                # @"+.4"
	.cfi_startproc
# %bb.0:                                # %entry
	movl	(%rdi), %eax
	addl	(%rsi), %eax
	retq
.Lfunc_end5:
	.size	".L+.4", .Lfunc_end5-".L+.4"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function -
	.type	".L-",@function
".L-":                                  # @-
	.cfi_startproc
# %bb.0:                                # %entry
	movsd	(%rdi), %xmm0           # xmm0 = mem[0],zero
	subsd	(%rsi), %xmm0
	retq
.Lfunc_end6:
	.size	".L-", .Lfunc_end6-".L-"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function -.5
	.type	".L-.5",@function
".L-.5":                                # @-.5
	.cfi_startproc
# %bb.0:                                # %entry
	movl	(%rdi), %eax
	subl	(%rsi), %eax
	retq
.Lfunc_end7:
	.size	".L-.5", .Lfunc_end7-".L-.5"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function -.6
	.type	".L-.6",@function
".L-.6":                                # @-.6
	.cfi_startproc
# %bb.0:                                # %entry
	xorl	%eax, %eax
	subl	(%rdi), %eax
	retq
.Lfunc_end8:
	.size	".L-.6", .Lfunc_end8-".L-.6"
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4               # -- Begin function -.7
.LCPI9_0:
	.quad	-9223372036854775808    # double -0
	.quad	-9223372036854775808    # double -0
	.text
	.p2align	4, 0x90
	.type	".L-.7",@function
".L-.7":                                # @-.7
	.cfi_startproc
# %bb.0:                                # %entry
	movsd	(%rdi), %xmm0           # xmm0 = mem[0],zero
	xorps	.LCPI9_0(%rip), %xmm0
	retq
.Lfunc_end9:
	.size	".L-.7", .Lfunc_end9-".L-.7"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function *
	.type	".L*",@function
".L*":                                  # @"*"
	.cfi_startproc
# %bb.0:                                # %entry
	movsd	(%rdi), %xmm0           # xmm0 = mem[0],zero
	mulsd	(%rsi), %xmm0
	retq
.Lfunc_end10:
	.size	".L*", .Lfunc_end10-".L*"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function *.8
	.type	".L*.8",@function
".L*.8":                                # @"*.8"
	.cfi_startproc
# %bb.0:                                # %entry
	movl	(%rdi), %eax
	imull	(%rsi), %eax
	retq
.Lfunc_end11:
	.size	".L*.8", .Lfunc_end11-".L*.8"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function /
	.type	".L/",@function
".L/":                                  # @"/"
	.cfi_startproc
# %bb.0:                                # %entry
	movsd	(%rdi), %xmm0           # xmm0 = mem[0],zero
	divsd	(%rsi), %xmm0
	retq
.Lfunc_end12:
	.size	".L/", .Lfunc_end12-".L/"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function /.9
	.type	".L/.9",@function
".L/.9":                                # @"/.9"
	.cfi_startproc
# %bb.0:                                # %entry
	movl	(%rdi), %eax
	cltd
	idivl	(%rsi)
	retq
.Lfunc_end13:
	.size	".L/.9", .Lfunc_end13-".L/.9"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function %
	.type	".L%",@function
".L%":                                  # @"%"
	.cfi_startproc
# %bb.0:                                # %entry
	movl	(%rdi), %eax
	cltd
	idivl	(%rsi)
	movl	%edx, %eax
	retq
.Lfunc_end14:
	.size	".L%", .Lfunc_end14-".L%"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function <
	.type	".L<",@function
".L<":                                  # @"<"
	.cfi_startproc
# %bb.0:                                # %entry
	movl	(%rdi), %eax
	cmpl	(%rsi), %eax
	setl	%al
	retq
.Lfunc_end15:
	.size	".L<", .Lfunc_end15-".L<"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function <.10
	.type	".L<.10",@function
".L<.10":                               # @"<.10"
	.cfi_startproc
# %bb.0:                                # %entry
	movsd	(%rsi), %xmm0           # xmm0 = mem[0],zero
	ucomisd	(%rdi), %xmm0
	seta	%al
	retq
.Lfunc_end16:
	.size	".L<.10", .Lfunc_end16-".L<.10"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function >
	.type	".L>",@function
".L>":                                  # @">"
	.cfi_startproc
# %bb.0:                                # %entry
	movl	(%rdi), %eax
	cmpl	(%rsi), %eax
	setg	%al
	retq
.Lfunc_end17:
	.size	".L>", .Lfunc_end17-".L>"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function >.11
	.type	".L>.11",@function
".L>.11":                               # @">.11"
	.cfi_startproc
# %bb.0:                                # %entry
	movsd	(%rdi), %xmm0           # xmm0 = mem[0],zero
	ucomisd	(%rsi), %xmm0
	seta	%al
	retq
.Lfunc_end18:
	.size	".L>.11", .Lfunc_end18-".L>.11"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function <=
	.type	".L<=",@function
".L<=":                                 # @"<="
	.cfi_startproc
# %bb.0:                                # %entry
	movl	(%rdi), %eax
	cmpl	(%rsi), %eax
	setle	%al
	retq
.Lfunc_end19:
	.size	".L<=", .Lfunc_end19-".L<="
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function <=.12
	.type	".L<=.12",@function
".L<=.12":                              # @"<=.12"
	.cfi_startproc
# %bb.0:                                # %entry
	movsd	(%rsi), %xmm0           # xmm0 = mem[0],zero
	ucomisd	(%rdi), %xmm0
	setae	%al
	retq
.Lfunc_end20:
	.size	".L<=.12", .Lfunc_end20-".L<=.12"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function >=
	.type	".L>=",@function
".L>=":                                 # @">="
	.cfi_startproc
# %bb.0:                                # %entry
	movl	(%rdi), %eax
	cmpl	(%rsi), %eax
	setge	%al
	retq
.Lfunc_end21:
	.size	".L>=", .Lfunc_end21-".L>="
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function >=.13
	.type	".L>=.13",@function
".L>=.13":                              # @">=.13"
	.cfi_startproc
# %bb.0:                                # %entry
	movsd	(%rdi), %xmm0           # xmm0 = mem[0],zero
	ucomisd	(%rsi), %xmm0
	setae	%al
	retq
.Lfunc_end22:
	.size	".L>=.13", .Lfunc_end22-".L>=.13"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function ==
	.type	".L==",@function
".L==":                                 # @"=="
	.cfi_startproc
# %bb.0:                                # %entry
	movl	(%rdi), %eax
	cmpl	(%rsi), %eax
	sete	%al
	retq
.Lfunc_end23:
	.size	".L==", .Lfunc_end23-".L=="
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function ==.14
	.type	".L==.14",@function
".L==.14":                              # @"==.14"
	.cfi_startproc
# %bb.0:                                # %entry
	movb	(%rdi), %al
	xorb	(%rsi), %al
	xorb	$1, %al
	retq
.Lfunc_end24:
	.size	".L==.14", .Lfunc_end24-".L==.14"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function ==.15
	.type	".L==.15",@function
".L==.15":                              # @"==.15"
	.cfi_startproc
# %bb.0:                                # %entry
	movsd	(%rsi), %xmm0           # xmm0 = mem[0],zero
	cmpeqsd	(%rdi), %xmm0
	movq	%xmm0, %rax
	andl	$1, %eax
                                        # kill: def $al killed $al killed $rax
	retq
.Lfunc_end25:
	.size	".L==.15", .Lfunc_end25-".L==.15"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function !=
	.type	".L!=",@function
".L!=":                                 # @"!="
	.cfi_startproc
# %bb.0:                                # %entry
	movl	(%rdi), %eax
	cmpl	(%rsi), %eax
	setne	%al
	retq
.Lfunc_end26:
	.size	".L!=", .Lfunc_end26-".L!="
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function !=.16
	.type	".L!=.16",@function
".L!=.16":                              # @"!=.16"
	.cfi_startproc
# %bb.0:                                # %entry
	movb	(%rdi), %al
	xorb	(%rsi), %al
	retq
.Lfunc_end27:
	.size	".L!=.16", .Lfunc_end27-".L!=.16"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function !=.17
	.type	".L!=.17",@function
".L!=.17":                              # @"!=.17"
	.cfi_startproc
# %bb.0:                                # %entry
	movsd	(%rdi), %xmm0           # xmm0 = mem[0],zero
	ucomisd	(%rsi), %xmm0
	setne	%al
	retq
.Lfunc_end28:
	.size	".L!=.17", .Lfunc_end28-".L!=.17"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function ||
	.type	".L||",@function
".L||":                                 # @"||"
	.cfi_startproc
# %bb.0:                                # %entry
	movb	(%rdi), %al
	orb	(%rsi), %al
	retq
.Lfunc_end29:
	.size	".L||", .Lfunc_end29-".L||"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function &&
	.type	".L&&",@function
".L&&":                                 # @"&&"
	.cfi_startproc
# %bb.0:                                # %entry
	movb	(%rdi), %al
	andb	(%rsi), %al
	retq
.Lfunc_end30:
	.size	".L&&", .Lfunc_end30-".L&&"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function I()
	.type	".LI()",@function
".LI()":                                # @"I()"
	.cfi_startproc
# %bb.0:                                # %entry
	cvtsi2sdl	(%rdi), %xmm0
	retq
.Lfunc_end31:
	.size	".LI()", .Lfunc_end31-".LI()"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function ->
	.type	".L->",@function
".L->":                                 # @"->"
	.cfi_startproc
# %bb.0:                                # %entry
	cvtsi2sdl	(%rdi), %xmm0
	retq
.Lfunc_end32:
	.size	".L->", .Lfunc_end32-".L->"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function ->.18
	.type	".L->.18",@function
".L->.18":                              # @"->.18"
	.cfi_startproc
# %bb.0:                                # %entry
	cvttsd2si	(%rdi), %eax
	retq
.Lfunc_end33:
	.size	".L->.18", .Lfunc_end33-".L->.18"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function ->.19
	.type	".L->.19",@function
".L->.19":                              # @"->.19"
	.cfi_startproc
# %bb.0:                                # %entry
	movzbl	(%rdi), %eax
	retq
.Lfunc_end34:
	.size	".L->.19", .Lfunc_end34-".L->.19"
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function printch
	.type	.Lprintch,@function
.Lprintch:                              # @printch
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	movb	$0, 7(%rsp)
	movw	$25381, 5(%rsp)         # imm = 0x6325
	movzbl	(%rdi), %esi
	leaq	5(%rsp), %rdi
	xorl	%eax, %eax
	callq	printf
	popq	%rax
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end35:
	.size	.Lprintch, .Lfunc_end35-.Lprintch
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function println
	.type	.Lprintln,@function
.Lprintln:                              # @println
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	movw	$10, 6(%rsp)
	leaq	6(%rsp), %rdi
	xorl	%eax, %eax
	callq	printf
	popq	%rax
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end36:
	.size	.Lprintln, .Lfunc_end36-.Lprintln
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function printint
	.type	.Lprintint,@function
.Lprintint:                             # @printint
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	movb	$0, 7(%rsp)
	movw	$25637, 5(%rsp)         # imm = 0x6425
	movl	(%rdi), %esi
	leaq	5(%rsp), %rdi
	xorl	%eax, %eax
	callq	printf
	popq	%rax
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end37:
	.size	.Lprintint, .Lfunc_end37-.Lprintint
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function printdouble
	.type	.Lprintdouble,@function
.Lprintdouble:                          # @printdouble
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	movb	$0, 7(%rsp)
	movw	$26149, 5(%rsp)         # imm = 0x6625
	movsd	(%rdi), %xmm0           # xmm0 = mem[0],zero
	leaq	5(%rsp), %rdi
	movb	$1, %al
	callq	printf
	popq	%rax
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end38:
	.size	.Lprintdouble, .Lfunc_end38-.Lprintdouble
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90         # -- Begin function printstr
	.type	.Lprintstr,@function
.Lprintstr:                             # @printstr
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	movb	$0, 7(%rsp)
	movl	$1932144165, 3(%rsp)    # imm = 0x732A2E25
	movl	(%rdi), %esi
	movq	8(%rdi), %rdx
	leaq	3(%rsp), %rdi
	xorl	%eax, %eax
	callq	printf
	popq	%rax
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end39:
	.size	.Lprintstr, .Lfunc_end39-.Lprintstr
	.cfi_endproc
                                        # -- End function
	.globl	main                    # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	callq	clock
	movl	%eax, 8(%rsp)
	leaq	8(%rsp), %rdi
	callq	srand
	callq	rand
	movl	%eax, 4(%rsp)
	leaq	4(%rsp), %rdi
	movq	%rdi, 16(%rsp)
	callq	.Lprintint
	callq	.Lprintln
	movl	$0, 12(%rsp)
	xorl	%eax, %eax
	addq	$24, %rsp
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end40:
	.size	main, .Lfunc_end40-main
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
