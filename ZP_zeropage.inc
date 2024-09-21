#importonce

/*
 * ZP - Zeropage handling
 * These functions provide an easy way to allocate and keep track of used zeropage addresses
 * in your code. Wherever your code needs any number of zeropage addresses, use the appropriate
 * alloc*() function to obtain them. They will be marked as used and not given out again.
 */

#import "scripting/List.inc"

/*
 * .add() ROM-/hardware-used addresses to this list so ZP allocate will not use them.
 * Remember to do this at the beginning of your code, before the first ZP alloc!
 */ 
.const ZP_ReservedAddresses = List().add($00, $01)

/*
 * .add() statically used addresses in your code to this list
 * Remember to do this at the beginning of your code, before the first ZP alloc!
 */
.const ZP_UsedAddresses = List()

// reserves one word (two consecutive zeropage addresses) anywhere in the zeropage
.function ZP_allocWord() {
	.return ZP_allocWords(1, 0)
}
// reserves one word (two consecutive zeropage addresses) at the given address, if possible
.function ZP_allocWord(startAddress) {
	.return ZP_allocWords(1, startAddress)
}
// reserves the given number of words (consecutively) anywhere in the zeropage
.function ZP_allocWords(amount) {
	.return ZP_allocWords(amount, 0)
}
// reserves the given number of words (consecutively) at the given address, if possible
.function ZP_allocWords(amount, startAddress) {
	.return ZP_allocBytes(amount*2, startAddress)
}

// reserves one byte anywhere in the zeropage
.function ZP_allocByte() {
	.return ZP_allocBytes(1, 0)
}
// reserves one byte at the given address, if possible
.function ZP_allocByte(startAddress) {
	.return ZP_allocBytes(1, startAddress)
}
// reserves the given number of bytes (consecutively) anywhere in the zeropage
.function ZP_allocBytes(amount) {
	.return ZP_allocBytes(amount, 0)
}
// reserves the given number of bytes (consecutively) at the given address, if possible
.function ZP_allocBytes(amount, startAddress) {
	.errorif startAddress >= $100, "ZP: cannot allocate outside the zeropage at "+toHexString(startAddress,4)
	.var endAddress = $ff
	.if (startAddress > 0) {
		.eval endAddress = startAddress + amount - 1
		.errorif endAddress >= $100, "ZP: allocating "+amount+" ($"+toHexString(amount,2)+") bytes from "+toHexString(startAddress,2)+" would exceed the zeropage"
	}
	.print "ZP: allocating "+amount+" ($"+toHexString(amount,2)+") bytes between $"+toHexString(startAddress,2)+" and $"+toHexString(endAddress,2)
	// find gaps in zeropage to hold the desired amount of bytes, consecutively
	.const foundGaps = Hashtable()
	.var foundGapStart = -1
	.var foundPerfectGap = false
	.for (var address = startAddress; address <= endAddress+1 && !foundPerfectGap; address++) {
		.if (address > endAddress || List_includes(ZP_ReservedAddresses, address) || List_includes(ZP_UsedAddresses, address)) {
			.if (foundGapStart >= 0) {
				.const addresses = address - foundGapStart
				.if (addresses >= amount) {
					.const extraBytes = addresses - amount
					.if (!foundGaps.containsKey(extraBytes)) {
						.eval foundGaps.put(extraBytes, foundGapStart)
					}
					.if (extraBytes == 0) {
						.eval foundPerfectGap = true
					}
				}
				.eval foundGapStart = -1
			}
		} else {
			.if (foundGapStart < 0) .eval foundGapStart = address
		}
	}
	.errorif foundGaps.keys().size() == 0, "ZP: could not allocate enough zeropage addresses, wanted: "+amount
	
	// foundGaps hashtable has extra bytes as keys, so take the smallest one (should ideally be 0 - fill gap completely)
	.const smallestGap = foundGaps.keys().sort().get(0)
	.const zp = foundGaps.get(smallestGap)
	// mark zeropage addresses as used
	.for (var i = 0; i < amount; i++) {
		.eval ZP_UsedAddresses.add(zp+i)
	}
	// cleanup list
	.eval List_unique(ZP_UsedAddresses)
	.eval ZP_UsedAddresses.sort()

	.if (amount == 1) {
		.print "ZP: allocated 1 zeropage address at $"+toHexString(zp,2)
	} else {
		.print "ZP: allocated "+amount+" zeropage addresses from $"+toHexString(zp,2)+" to $"+toHexString(zp+amount-1,2)
	}

	.return zp
}
/*
 * Prints the current state of the zeropage, for debugging/statistic purposes.
 */
.function ZP_printUsage() {
	.print "/ZP 0123456789ABCDEF"
	.var freeAddresses = 0
	.for (var i = 0; i < 16; i++) {
		.var line = "$"+toHexString(i*16,2)+" "
		.for (var j = 0; j < 16; j++) {
			.const index = i*16+j
			.var currentByte = "."
			.if (List_includes(ZP_ReservedAddresses, index)) {
				.eval currentByte = "R"
			} else .if (List_includes(ZP_UsedAddresses, index)) {
				.eval currentByte = "U"
			} else {
				.eval freeAddresses += 1
			}
			.eval line += currentByte
		}
		.print line
	}
	.print "used: $"+toHexString(256-freeAddresses,2)+", free: $"+toHexString(freeAddresses,2)
}
