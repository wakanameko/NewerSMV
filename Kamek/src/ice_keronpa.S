#include <asm_setup.S>

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


.text

.set sp, 1

.align 4
.extern _Z19keronpaShotFireHookP5dEn_cS0_
.extern aWm_mr_iceball_ #0x80311e20, 名称は Wm_mr_iceball_b
.extern aWm_mr_icebal_0 #0x80311e30, 名称は Wm_mr_iceball_a

#無理やりdaEnkeronpa_c::executeState_Attackの中間にフックを付け、cppで書いたkeronpaShotFireHookを呼び出す
.global keronpaShotFireHookAsm
keronpaShotFireHookAsm:
	pushStack
	mr r3, r29
    mr r4, r31
	bl _Z19keronpaShotFireHookP5dEn_cS0_
	popStack
	mr r3, r30
	blr

#ファイヤーを吐く時の音を無理やり変更。無音の効果音と置換して、効果音自体はcppファイルから鳴らしています。
.global keronpaChangeShotFireSound
keronpaChangeShotFireSound:
    li r4, 0
    lwz r11, 0x4 (r29)
    li r12, 1
    and r12, r11, r12
    cmpwi r12, 1
    beq sound_is_not_default
    li r4, 457
sound_is_not_default:
    blr

.global keronpaShotFireDeleteNoPosCopy
keronpaShotFireDeleteNoPosCopy:
    lwz r11, 0x4 (r29)
    li r12, 1
    and r12, r11, r12
    cmpwi r12, 1
    beq skip_pos_copy
    lfs f0, 0x10 (sp)
    stfs f0, 0xAC (r31)
    lfs f0, 0x14 (sp)
    stfs f0, 0xB0 (r31)
    lfs f0, 0x18 (sp)
    stfs f0, 0xB4 (r31)
skip_pos_copy:
    blr


#settingsの一番下のbitが1なら氷で死なないように変更
.global keronpaCollisionCat2_IceBall_15_YoshiIce_Hook
keronpaCollisionCat2_IceBall_15_YoshiIce_Hook:
    lwz r11, 0x4 (r3)
    li r12, 1
    and r12, r11, r12
    cmpwi r12, 1
    bne skip_return
    mtlr r0
    blr
skip_return:
    stwu sp, -0x10 (sp)
    blr

#r3のアクターIDを差し替え、r3はThisポインタ
.global keronpaChangeShippo
keronpaChangeShippo:
	# r3, r28: thisptr
    lwz r3, 0x4(r3)
    rlwinm r3, r3, 0, 28, 31
    cmpwi r3, 0x1
    beq L_continueChangeShippo
    b L_remainShippo
L_continueChangeShippo:
    lwz r3, 0x4(r28)
    rlwinm r3, r3, 28, 20, 31
    cmpwi r3, 0x76 #BROS_ICEBALL
    bne L_shippoCompareCannonBall
    li r4, 0x1 #daKeronpaFireの見た目を変更させるためにsettingの値を変えとく
    b L_remainShippo
L_shippoCompareCannonBall:
    cmpwi r3, 0x15b #cannon ball
    bne L_remainShippo
    b L_returnShippo
L_remainShippo:
	li r3, 0xe1
L_returnShippo:
	blr

#爆発エフェクトを寒そうなやつに変える
.global keronpaChangeExplodeEffect
keronpaChangeExplodeEffect:
    lwz r5, 0x4(r29)
    rlwinm r5, r5, 0, 28, 31
    cmpwi r5, 0x1
    bne L_returnkeronpaExplode
    lwz r5, 0x4(r29)
    rlwinm r5, r5, 28, 20, 31
    cmpwi r5, 0x76 #BROS_ICEBALL
    bne L_returnkeronpaExplode
    
	lis r4, aWm_mr_iceball_@h
	ori r4, r4, aWm_mr_iceball_@l
L_returnkeronpaExplode:
    li r5, 0x0
    blr


#しっぽの火(Wm_en_keronpafire)を氷(Wm_mr_iceball_a)に変更
.global keronpaFireChangeBodyEffect
keronpaFireChangeBodyEffect:
    #r31:thisptr, r3:this+0x3dc
    #Restore instruction
    addi r8, r31, 0xdc
    #settingの下1桁が1だったら変更
    lwz r3, 0x4(r31)
    cmpwi r3, 0x1
    bne L_restoreKeronpaFireBE

	lis r4, aWm_mr_icebal_0@h
	ori r4, r4, aWm_mr_icebal_0@l
L_restoreKeronpaFireBE:
    #restore
    addi r3, r31, 0x3dc
    #return
    blr
