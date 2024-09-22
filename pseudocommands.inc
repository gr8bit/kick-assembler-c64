#importonce

.function _16bitFirstArgument(arg) {
	.if (arg.getType()==AT_IMMEDIATE)
		.return CmdArgument(arg.getType(),<arg.getValue())
	.return CmdArgument(arg.getType(),arg.getValue())
}
.function _16bitNextArgument(arg) {
	.if (arg.getType()==AT_IMMEDIATE)
		.return CmdArgument(arg.getType(),>arg.getValue())
	.return CmdArgument(arg.getType(),arg.getValue()+1)
}

// shorcut for lda/sta
.pseudocommand mov source:target {
	lda source
	sta target
}
// shorcut for ldx/stx
.pseudocommand movx source:target {
	ldx source
	stx target
}
// shorcut for ldy/sty
.pseudocommand movy source:target {
	ldy source
	sty target
}
.pseudocommand mov16 source:target {
	lda source
	sta target     
	lda _16bitNextArgument(source)
	sta _16bitNextArgument(target)
}                                           
.pseudocommand movx16 source:target {
	ldx source
	stx target     
	ldx _16bitNextArgument(source)
	stx _16bitNextArgument(target)
}                                           
.pseudocommand movy16 source:target {
	ldy source
	sty target
	ldy _16bitNextArgument(source)
	sty _16bitNextArgument(target)
}                                           
.pseudocommand inc16 target {
	inc target
	bne !+
	inc _16bitNextArgument(target)
!:
}
.pseudocommand dec16 target {
	lda target
	bne !+
	dec _16bitNextArgument(target)
!:
	dec target
}
.pseudocommand adc16 arg1 : arg2 : target {
	.if (target.getType()==AT_NONE) .eval target=arg1
	lda arg1
	adc arg2
	sta target
	lda _16bitNextArgument(arg1)
	adc _16bitNextArgument(arg2)
	sta _16bitNextArgument(target)
}
.pseudocommand add16 arg1 : arg2 : target {
	clc
	adc16 arg1 : arg2 : target
}
.pseudocommand sbc16 arg1 : arg2 : target {
	.if (target.getType()==AT_NONE) .eval target=arg1
	lda arg1
	sbc arg2
	sta target
	lda _16bitNextArgument(arg1)
	sbc _16bitNextArgument(arg2)
	sta _16bitNextArgument(target)
}
.pseudocommand sub16 arg1 : arg2 : target {
	sec
	sbc16 arg1 : arg2 : target
}

.pseudocommand cmp16 arg1 : arg2 {
	lda _16bitNextArgument(arg1)
	cmp _16bitNextArgument(arg2)
	bne !+
	lda arg1
	cmp arg2
!:
}
.pseudocommand cpx16 arg1 : arg2 {
	ldx _16bitNextArgument(arg1)
	cpx _16bitNextArgument(arg2)
	bne !+
	ldx arg1
	cpx arg2
!:
}
.pseudocommand cpy16 arg1 : arg2 {
	ldy _16bitNextArgument(arg1)
	cpy _16bitNextArgument(arg2)
	bne !+
	ldy arg1
	cpy arg2
!:
}
// sets the overflow cpu flag only. leaves all registers and other processor flags as-is.
// warning, takes a lot of cycles: 34 under normal circumstances (=no stack overflow).
.pseudocommand sev {
	php
	pha
	txa
	pha
	tsx
	lda $100+3,x
	ora #%01000000
	sta $100+3,x
	pla
	tax
	pla
	plp
}

.function _samePage(addr1, addr2) {
	.return (>addr1) == (>addr2)
}
.macro _samePageBranch(target, branchOpcode) {
	.errorif !_samePage(target.getValue(), *), "opcode $".toHexString(branchOpcode)+" branch crosses page boundary"
	.const diff = target - (* + 2)
	.errorif diff < -128 || diff > 127, "opcode $".toHexString(branchOpcode)+" branch is too far"
	
	.byte branchOpcode, diff
}
// like bne but cannot cross a page boundary
.pseudocommand xbne target {
	_samePageBranch(target, BNE_REL)
}
// like beq but cannot cross a page boundary
.pseudocommand xbeq target {
	_samePageBranch(target, BEQ_REL)
}
// like bcc but cannot cross a page boundary
.pseudocommand xbcc target {
	_samePageBranch(target, BCC_REL)
}
// like bcs but cannot cross a page boundary
.pseudocommand xbcs target {
	_samePageBranch(target, BCS_REL)
}
// like bpl but cannot cross a page boundary
.pseudocommand xbpl target {
	_samePageBranch(target, BPL_REL)
}
// like bmi but cannot cross a page boundary
.pseudocommand xbmi target {
	_samePageBranch(target, BMI_REL)
}
// like bvc but cannot cross a page boundary
.pseudocommand xbvc target {
	_samePageBranch(target, BVC_REL)
}
// like bvs but cannot cross a page boundary
.pseudocommand xbvs target {
	_samePageBranch(target, BVS_REL)
}

.macro _farBranch(target, branchOpcode) {
	.const diff = target - (* + 2)
	.if (diff < -128 || diff > 127) {
		// target is too far, invert branch opcode condition, cross the jmp (3 bytes)
		.byte branchOpcode ^ $20, 3
		jmp target
	!:
	} else {
		// use ordinary branch
		.byte branchOpcode, diff
	}
}
// like bne but uses jmp if branch would be too far
.pseudocommand fbne target {
	_farBranch(target, BNE_REL)
}
// like beq but uses jmp if branch would be too far
.pseudocommand fbeq target {
	_farBranch(target, BEQ_REL)
}
// like bcc but uses jmp if branch would be too far
.pseudocommand fbcc target {
	_farBranch(target, BCC_REL)
}
// like bcs but uses jmp if branch would be too far
.pseudocommand fbcs target {
	_farBranch(target, BCS_REL)
}
// like bpl but uses jmp if branch would be too far
.pseudocommand fbpl target {
	_farBranch(target, BPL_REL)
}
// like bmi but uses jmp if branch would be too far
.pseudocommand fbmi target {
	_farBranch(target, BMI_REL)
}
// like bvc but uses jmp if branch would be too far
.pseudocommand fbvc target {
	_farBranch(target, BVC_REL)
}
// like bvs but uses jmp if branch would be too far
.pseudocommand fbvs target {
	_farBranch(target, BVS_REL)
}
