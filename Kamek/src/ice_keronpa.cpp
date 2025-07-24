#include <common.h>
#include <game.h>
#include <sfx.h>
#include "player.h"

#include "my_mathlib.h"

// by kazuki_4ys and wakanameko

dStageActor_c* nearestPlayerActor(dStageActor_c* actor){	// mess
	char nearest = NearestPlayer(actor);
	// if(nearest < 0)return (dStageActor_c )((void)0);
	if(nearest < 0)return NULL;
	return GetSpecificPlayerActor(nearest);
}

void keronpaShotFireHook(dEn_c *self, dEn_c *fire){
	void **vtable =  *((void ***) (((char *) self) + 0x60));//仮想関数テーブル
	void *keronpaCollisionCat2_IceBall_15_YoshiIce = vtable[0x4D];
	OSReport("keronpaCollisionCat2_IceBall_15_YoshiIce = 0x%08x\n", ((unsigned int)keronpaCollisionCat2_IceBall_15_YoshiIce));
	//selfはケロンパ本体
	//r3 = self
	//r4 = fire
	//reggieでの設定がデフォルトのままなら普通の挙動をさせる
	int nybble12 = self->settings >> 0 & 0xF;
	if((nybble12 & 0b1) == 0)return;				// nybble12.1 bool
	bool isShift = nybble12 & 0b10;					// nybble12.2 bool

	// 蛇足1 ReggieからスプライトのIDを指定
	u16 actorID1 = self->settings >> 4 & 0xF;			// 1桁目 nybble11
	u16 actorID2 = (self->settings >> 8 & 0xF)*16;
	u16 actorID3 = (self->settings >> 12 & 0xF)*256;	// 3 nybble9
	u32 actorID = actorID1 + actorID2 + actorID3;
	OSReport("ActorID : %02d\n", actorID);
	// 蛇足2 Reggieからスプライトのsettingを指定 // nybble 1 to 8
	u16 settingN9 = (self->settings >> 28 & 0xF)*4096;
	u16 settingN10 = (self->settings >> 24 & 0xF)*256;
	u16 settingN11 = (self->settings >> 20 & 0xF)*16;
	u16 settingN12 = self->settings >> 16 & 0xF;	// 0000 000"0" 0000 0000
	u32 settingDEC = settingN9 + settingN10 + settingN11 + settingN12;
	if(isShift){settingDEC = settingDEC * 0x10000;}
	OSReport("settingDEC : %02d\n", settingDEC);

	fire->Delete(1);//しっぽの炎を無理やり消す
    OSReport("keronpaShotFireHookCalled!!\n");
    dStageActor_c *targetPlayer = nearestPlayerActor(self);
	if(targetPlayer){
		Vec2 toTargetVec;
		Vec2 bulletSpeed;
		toTargetVec.x = targetPlayer->pos.x - self->pos.x;
		toTargetVec.y = targetPlayer->pos.y - self->pos.y;
		normalizeVector(&toTargetVec, &bulletSpeed);
		//CreateActorでアイスを作って投げる
		//詳しくはNewerのMrSunのコード参照
        dStageActor_c *spawner = CreateActor(actorID, settingDEC, self->pos, 0, 0);
		// 蛇足3 ActorIDがアイスボール系だった時だけ効果音変更
		nw4r::snd::SoundHandle handleShotSFX;
		if(actorID == BROS_ICEBALL){	// 118, 
			PlaySoundWithFunctionB4(SoundRelatedClass, &handleShotSFX, SE_EMY_ICE_BROS_ICE, 1);	// 491
		}
		else{
			PlaySoundWithFunctionB4(SoundRelatedClass, &handleShotSFX, SE_EMY_KERONPA_FIRE, 1);	// 457
		}
        
	    spawner->speed.x = 5.0f * bulletSpeed.x;
	    spawner->speed.y = 5.0f * bulletSpeed.y;
	    spawner->pos.z = 5550.0;
        *((u32 *) (((char *) spawner) + 0x3DC)) = self->id;
	}
}