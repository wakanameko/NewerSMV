#include <asm_setup.S>

.text
.set sp, 1
.align 4

.extern daTorchiLight_c__onCreate_this318	# PALv1: 0x808b0db0, JPNv1: 0x808b0b40

.global placeBehindOtherSprites	# 0x808b1008
placeBehindOtherSprites:
	# r28: thisptr, 

	# load settings to r3
	lwz		r3, 0x4(r28)
	rlwinm	r3, r3, 20, 28, 31
	cmpwi   r3, 0x0
	beq		L_restore
    lis     r3, placeBehindPosZ@ha
    lwz     r3, placeBehindPosZ@l(r3)
    stw     r3, 0x4b(r28)

L_restore:
	# restore instruction
	li  	r3, 0x1
	blr

data:
.align 4
placeBehindPosZ:
    .float  -3300.0