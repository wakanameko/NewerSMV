.text
#include <asm_setup.S>
.align 4
.extern ContinueFromClownCarDetectAdjacent
.extern calculateAdjacentCollision__14collisionMgr_cFPf
.global ClownCarDetectAdjacents
ClownCarDetectAdjacents:
	addi r3, r30, 0x1EC
	li r4, 0
	bl calculateAdjacentCollision__14collisionMgr_cFPf
	cmpwi r3, 0
	beq notAdjacent

	# what direction are we in?
	lfs f1, 0xE8(r30) # x speed
	lis r4, zeroFloat@h
	ori r4, r4, zeroFloat@l
	lfs f2, 0(r4)

	lbz r4, 0x348(r30)

	cmpwi r4, 1
	beq isLeft

	# RIGHT
	fcmpo cr0, f1, f2 # xspeed > 0
	blt notAdjacent
	stfs f2, 0xE8(r30)
	b notAdjacent

isLeft:
	# LEFT
	fcmpo cr0, f1, f2 # xspeed < 0
	bgt notAdjacent
	stfs f2, 0xE8(r30)

notAdjacent:
	# END HERE

	mr r3, r30
	b ContinueFromClownCarDetectAdjacent

.macro pushStack
    stwu sp, -0x80 (sp)#124 + パディング
    mflr r0
    stw r0, 0x84 (sp)
    stmw r3, 0x8(sp)
.endm

.macro popStack
    lmw r3, 0x8(sp)
    lwz r0, 0x84 (sp)
    mtlr r0
    addi sp, sp, 0x80
.endm

.extern daJrClownForPlayer_c__usePlayer	# 0x80810d80
#.global ClownCarPhysicsOverride
#ClownCarPhysicsOverride:
#	# r4, r28: thisptr
#	# replace f3(diameter)
#	lis r4, fakePhyDiameter@h
#	ori r4, r4, fakePhyDiameter@l
#	lfs f3, 0(r4)
#	# restore
#	mr  r4, r28
#	li r8, 0x1							# orig instruction
#	blr
.extern dBg_ctr_c__set_circle 			# PALv1: 0x8007fb80, void(r3(dBg_ctr_c *param_1), r4(dActor_c *self), f1(float left_offset), f2(float top_offset), f3(float diameter), r5(undefined *belowCheck), r6(undefined *aboveCheck), r7(undefined *adjCheck), r8(byte chainlinkMode), r9(byte layer))
.global ClownCarPhysicsOverride
ClownCarPhysicsOverride:
	# r3: isPlayerRiding, r12: belowCheck, r28: apOther, r30: apThis
	cmpwi r3, 0x1
	bne L_restoreCCPhyOverride
	pushStack

	# prepare arguments (float)	
	lis r4, zeroFloat@h
	ori r4, r4, zeroFloat@l
	lfs f1, 0(r4)						# left offs
	lis r4, zeroFloat@h
	ori r4, r4, zeroFloat@l
	lfs f2, 0(r4)						# top offs
	lis r4, zeroFloat@h
	ori r4, r4, zeroFloat@l
	lfs f3, 0(r4)						# Diameter

	# prepare arguments (general)
	addi r3, r30, 0xd78					# Thisptr + offset? maybe
	mr r4, r30
	mr r5, r12
	# 引数がよく分かんないので、初期化の時と全く同じ値をハードコードして渡す
	li r6, 0x0
	li r7, 0x0
	li r8, 0x1
	li r9, 0x0
	bl dBg_ctr_c__set_circle

	popStack
L_restoreCCPhyOverride:
	# restore
	mr r29, r3
	blr

.data
.align 4
zeroFloat: .float 0.0

.data
.align 4
fakePhyDiameter: .float 5.0				# orig: 11.9


.extern _Z23spawnIceEffectAndDeleteP5dEn_cS0_	# Clang mangled
.global BrosIceballDontFreeze				# JPNv1: 0x807d4fa8, PALv1: 0x807d5218
BrosIceballDontFreeze:
	# Hook to PlayerCollision, and if settings has 0x80 damage player and delete this iceball
	# r29: thisptr, r30: playerptr
	# load settings
	lwz r3, 0x4(r29)
	rlwinm r3, r3, 28, 28, 31				# 0000 00X0
	cmpwi r3, 0x8
	bne L_restoreBroIce

	# ここでダメージ与えた方が良いかも
	pushStack
	mr r3, r29								# 1st arg: this pointer 
	mr r4, r30								# 2nd arg: player pointer
	#bl _vf268__5dEn_cFPv					# なんかツタ生えて面白かった
	bl _vf220__5dEn_cFPv					# damage player
	popStack

	mr r3, r29								# 1st arg: this pointer
	mr r4, r30								# 2nd arg: player pointer
	b _Z23spawnIceEffectAndDeleteP5dEn_cS0_

L_restoreBroIce:
	lwz r12, 0x60(r30)						# restore instruction
	blr