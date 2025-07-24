.text
.align 4

#include <asm_setup.S>

# LightBlock upthrow patch for New Super Mario Lost Worlds.
# This file was ported by ReaZ0n23 from New Super Mario World.Wii mod.
# by ReaZ0n based on NewerGEM's
# Some a few edits by wakanameko
################################################################################

.extern getHeldUp__14dPlayerInput_c

################################################################################

.global GlowThrowUp # PALv1:0x808566b4, JPNv1:0x80856444
GlowThrowUp:
    # r30, r3:  This Pointer
    # r5:       Player Pointer
    # r23:      Player->input   # in clang, this value is missing.
    mflr    r24                 # maybe unused register
    
    # is held up
    lbz     r3, 0x38d(r30)      # playerID
    bl      findByID__7dAcPy_cFi
    addi    r3, r3, 0xEA4       # player->input
    bl      getHeldUp__14dPlayerInput_c # return 0 (not held) or 2 (held)

    cmpwi   r3, 0x0
    beq     GlowThrowUp_return

    # set speed.y
    lis     r3, GlowThowYSpeeds@ha
    lwz     r3, GlowThowYSpeeds@l(r3)
    stw     r3, 0xec(r30)
    lfs     f1, 0xec(r30)

    # set speed.x
    lfs     f2, 0xe8(r5)

GlowThrowUp_return:
    stfs    f2, 0xe8(r30)       # restore instruction
    mr      r3, r30             # restore r3

    # return
    mtlr    r24
    blr

################################################################################

data:
.align 4
GlowThowYSpeeds:
    .float  7.0
