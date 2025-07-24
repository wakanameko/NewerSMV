#include <asm_setup.S>

.text
.set sp, 1
.align 4

.extern LAB_807c0878

###############################################################

.global turnOFFtheLight         # 0x800619f0 li r0, 0x1
turnOFFtheLight:
    # Check now light condition
    lbz r3, 0x3e2(r3)
    cmpwi r3, 0x0
    bne L_restore               # branch when turned OFF

    # Get and Store trigger ID
	lwz r3, 0x4(r31)            # get settings
	rlwinm r3, r3, 16, 24, 31   # Turn Off trigger ID(00"00" 0000)
    stw r3, 0x370(r31)          # Store the trigger ID
    # Restore r3
    mr r3, r31

    # Calc trigger flag
    lwz r4, -0x5628(r13)        # dSwitchFlagMng_c::m_instance
    lwz r0, 0x370(r31)          # Load trigger ID Low
    lwz r5, 0x0(r4)
    lwz r6, 0x4(r4)
    lwz r4, 0x374(r31)          # Load trigger ID High
    # is enabled?
    and r0, r5, r0
    and r4, r6, r4
    or. r0, r4, r0 

    cmpwi r0, 0x0               # True: Do turn OFF
    beq L_restore

    li r0, 0x0                  # Turn OFF!!🤓🤓🤓
    stb r0, 0x3e2(r3)
    b LAB_807c0878

L_restore:
    # Restore r3
    mr r3, r31
    # Original instructions
    li r0, 0x1                  # Turn ON😭😭😭
    stb r0, 0x3e2(r3)
    lwz r4, -0x5628(r13)
    lbz r0, 0x6c8(r4)
    stb r0, 0x3e7(r3)

    b LAB_807c0878
