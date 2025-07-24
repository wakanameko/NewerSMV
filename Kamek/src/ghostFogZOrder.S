.text
#include <asm_setup.S>

.align 4

.global changeZOrder                # PALv1: 0x807b5248, JPNv1: 0x807b4fd8
changeZOrder:
    # (r3: thisptr), (r6: thisptr + 0x20000 (offset of anything?))
    # restore instruction
    lis     r4, -0x7fbd

    # get setting
    lwz     r27, 0x4(r3)
    rlwinm  r27, r27, 16, 28, 31      # 000"0" 0000
    cmpwi   r27, 0x0
    bne     L_continueZOrder

    b       L_returnZOrder

L_continueZOrder:
    # overwrite pos.z
    lis     r27, poszOverLayer0@ha
    lwz     r27, poszOverLayer0@l(r27)
    stw     r27, 0xb4(r3)

L_returnZOrder:
    # restore

    blr

data:
.align 4
poszOverLayer0:
    .float  -5500.0