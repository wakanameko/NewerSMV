#include <asm_setup.S>

.text
.set sp, 1
.align 4

.extern _Z31isEventTriggeredGiantLightBlocki  # Clang mangled symbol

###############################################################
# Notes:
#   this ptr: 0x815309a8(PALv1)
#   3e2(this)に状態を保存。 OFF: 00010100, ON: 01010100
#   PALv1:800619f4 にて上記の動作を実行
#   OFFの値をぶち込めばいつでも状態を変更できることを確認済み
# Variables:
#   0x3e4: Movement Type
#   0x3e6: Activation Mode
###############################################################

.global turnOFFtheLight         # PALv1:0x807c07cc JPNv1:0x807c055c
turnOFFtheLight:
    # r28: this ptr
    # r31にリンクレジスタの値を保存
    mflr    r31

    # r3にsettingを読み込む
    lwz     r3, 0x4(r28)
    # トリガーID (00"00" 0000) を取得
    rlwinm  r3, r3, 16, 24, 31
    # トリガーIDが無かったら何もしない
    cmpwi   r3, 0x0
    beq     L_restore

    # 特定のイベントIDが実行されたか確認する
    #lwz	    r4, -0x5628(r13)    # =>dSwitchFlagMng_c::m_instance
    #lwz     r0, 0x358(r28)
    #lwz     r5, 0x0(r4)
    #lwz     r6, 0x4(r4)
    #lwz     r4, 0x35c(r28)
    #and     r0, r5, r0
    #and     r4, r6, r4
    #or.     r0, r4, r0
    # ASM意味わからんもうC++で書くわ
    bl      _Z31isEventTriggeredGiantLightBlocki
    cmpwi   r3, 0x1
    bne     L_restore
    li      r3, 0x0
    stb     r3, 0x3e2(r28)

L_restore:
    lwz     r31, 0x1c(sp)
    mr      r3, r31
    lwz     r0, 0x24(sp)
    addi    sp, sp, 0x20
    
    # return
    mtlr    r0
    blr