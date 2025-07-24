.text
#include <asm_setup.S>
.align 4

# Made by wakanameko
# tested in JPNv1(Console), PALv1(Emu)

# Stack
.macro pushStack
    stwu sp, -0x80 (sp)#124 + パディング
    mflr r0
    stw r0, 0x84 (sp)
    stmw r3, 8 (sp)
.endm
.macro popStack
    lmw r3, 8 (sp)
    lwz r0, 0x84 (sp)
    mtlr r0
    addi sp, sp, 0x80
.endm

.global doShotEverythingsASM        # P1: 0x808a1368
doShotEverythingsASM:
	# (r28: thisptr)
    # is hack enabled?
    lwz r3, 0x4(r28)
	rlwinm r3, r3, 20, 28, 31		# 0000 "0"000
	cmpwi r3, 0x3
	beq L_continue
    b L_restoreShot

L_continue:
	# get ActorID
    lwz r3, 0x4(r28)
	rlwinm r3, r3, 12, 20, 31		# "000"0 0000
    b L_returnShot

L_restoreShot:
    # restore instruction
    li r3, 0x85
L_returnShot:
    blr

.global shotEverythingsASMSettings  # P1: 0x808a137c
shotEverythingsASMSettings:
    # is it Bom-omb?
        cmpwi r3, 0x85
        bne L_continueCompare_Bill
        or r4, r8, r0
        b L_returnSettings

    L_continueCompare_Bill:
    # is it Bullet Bill?
        cmpwi r3, 127
        blt L_continueCompare_Other		# より小さい
        cmpwi r3, 130
        bgt L_continueCompare_Other		# より大きい

        # 一旦ActorIDにByeBye
        # 発射口pos.x - 本体pos.x の結果がMinusなら、LeftにFire
        lwz r4, 0x1c(sp)                    # firepos.x
        lwz r3, 0xac(r28)                   # cannonpos.x
        sub r3, r4, r3
        cmpwi r3, 0x0
        # popStack
        bgt L_shotBillRight
        # LeftにFire
            # Fix pos.x
            lwz r4, 0x1c(sp)
            rlwinm r4, r4, 16, 16, 31
            addi r4, r4, 0x4
            rlwinm r4, r4, 16, 0, 31
            stw r4, 0x1c(sp)
            # Update Settings
            li r4, 0x0000000A			    # Up Left
            b L_returnShotBill
        L_shotBillRight:
        # RightにFire
            # Fix pos.x
            lwz r4, 0x1c(sp)
            rlwinm r4, r4, 16, 16, 31
            subi r4, r4, 0x4
            rlwinm r4, r4, 16, 0, 31
            stw r4, 0x1c(sp)
            # Update Settings
            li r4, 0x00000009				# Up Right
            b L_returnShotBill

        L_returnShotBill:
            # get ActorID
            lwz r3, 0x4(r28)
            rlwinm r3, r3, 12, 20, 31		# "000"0 0000
            b L_returnSettings

    L_continueCompare_Other:
    # other
        li r4, 0x0
        b L_returnSettings

L_returnSettings:
    blr