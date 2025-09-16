#importonce

#import "pseudocommands.inc"

.macro CG_initAt(targetAddress, codePointer) {
	mov16 targetAddress : codePointer
	ldy #0
}

.macro CG(codePointer) {
	// increases codepointer by 3 bytes and the y register
	// resets y to 0
	skipTriByte:
		.break "if y>=253"
		iny
	// increases codepointer by 2 bytes and the y register
	// resets y to 0
	skipWord:
		.break "if y>=254"
		iny
	// increases codepointer by 1 byte and the y register
	// resets y to 0
	skipByte:
		.break "if y=255"
		iny
	// updates codepointer by value of y register
	// resets y to 0
	updateCodePointer:                   
		clc
	// updates codepointer by value of y register and carry
	// resets y to 0
	adcCodePointer:
		tya
		adc codePointer
		sta codePointer
		bcc !+
		inc codePointer+1
	!:
		ldy #0
		rts
		
	// adds accumulator to word starting at (codepointer),y
	// leaves y unchanged
	addToWord:
		clc
	// adds accumulator and carry to word starting at (codepointer),y
	// leaves y unchanged
	adcToWord:
		.break "if y=255"
		adc (codePointer),y
		sta (codePointer),y
		iny
		lda (codePointer),y
		adc #0
		sta (codePointer),y
		dey
		rts

	// stores current codepointer (word) at the current (codepointer),y
	// leaves y unchanged
	staCodePointer:
		.break "if y=255"
		lda codePointer
		sta (codePointer),y
		iny
		lda codePointer+1
		sta (codePointer),y
		dey
		rts
}
