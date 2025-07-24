#include <common.h>
#include <game.h>
#include <sfx.h>
#include <profile.h>

class daGeneralBall_c : public dEn_c {
    public:
        int onCreate();
        int onDelete();
        int onExecute();
        int onDraw();
    
        mEf::es2 effect;
    
        ActivePhysics::Info HitMeBaby;
    
        void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
        void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
    
        int ballType;
        int timer;

        enum ballTypeEnum{
            FIREBALL = 0,
            ICEBALL = 1,
        };

        USING_STATES(daGeneralBall_c);
        
        static dActor_c* build();
    };
    
dActor_c *daGeneralBall_c::build() {
    void *buffer = AllocFromGameHeap1(sizeof(daGeneralBall_c));
    return new(buffer) daGeneralBall_c;
}

/////////////////////////////
// more sprites config
const SpriteData generalBallSpriteData = { ProfileId::iceBallFake, 0, 0, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile generalBallProfile(&daGeneralBall_c::build, SpriteId::iceBallFake, &generalBallSpriteData, ProfileId::iceBallFake, ProfileId::iceBallFake, "iceBallFake", NULL, 0x20);

// collision
void daGeneralBall_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	u16 name = ((dEn_c*)apOther->owner)->profileId;
	if (!name == WM_PAKKUN){ return; }

    if (this->ballType == FIREBALL){
        PlaySound(this, SE_BOSS_KOOPA_FIRE_DISAPP);
    
        S16Vec nullRot = {0,0,0};
        Vec oneVec = {1.0f, 1.0f, 1.0f};
        SpawnEffect("Wm_mr_fireball_hit", 0, &apOther->owner->pos, &nullRot, &oneVec);

		this->kill();
		this->Delete(this->deleteForever);
    }
    else if (this->ballType == ICEBALL){
        PlaySound(this, SE_PLY_BRAKE_ICE);

		this->kill();
		this->Delete(this->deleteForever);
    }
}
void daGeneralBall_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
    this->_vf220(apOther->owner);
}

int daGeneralBall_c::onCreate() {
	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;

	/////////////////////////////
	// Hit me baby
	HitMeBaby.xDistToCenter = 0.0;		// 当たり判定をスプライトの中心からどれだけ離すか(x軸)
	HitMeBaby.yDistToCenter = 0.0;
	HitMeBaby.xDistToEdge = 1.0;		// x軸方向の大きさ
	HitMeBaby.yDistToEdge = 1.0;

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0b1111111110110000101011001011110;	// 右のバイトから、0unk,1ファイア,2アイス,3スター,4unk,5滑り(坂),6unk,7ヒップドロップ,8フェンス,9甲羅,10滑り(ペンギン),11スピン,12unk,13SpinFall(謎),14Fire(No FireBall, but like an explosion),15ヨッシー可食,16ヨッシーが勝手に食うかどうか,17大砲,18持ち上げ,19YoshiBullet,20ファイア(ヨッシー),21アイス(ヨッシー),残りは0

	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

	// Reggie! configs
	this->ballType = this->settings >> 0xF;		// 0000 0000 000"0" 0000	// nybble 12

	// initialize local variables
	this->timer = 0;

	this->onExecute();
	return true;
}

int daGeneralBall_c::onExecute() {
	acState.execute();
    
	this->UpdateObjectPosBasedOnSpeedValuesReal();

    Vec positionGFX = this->pos;

    if (this->timer == 0){
        if (this->ballType == FIREBALL){
            S16Vec nullRot = {0,0,0};
            Vec oneVec = {1.0f, 1.0f, 1.0f};
            SpawnEffect("Wm_en_pakkunfire", 0, &positionGFX, &nullRot, &oneVec);
        }
        else if (this->ballType == ICEBALL){
            S16Vec nullRot = {0,0,0};
            Vec oneVec = {1.0f, 1.0f, 1.0f};
            SpawnEffect("Wm_mr_iceball_a", 0, &positionGFX, &nullRot, &oneVec);
        }
    }

    this->timer += 1;

	return true;
}

int daGeneralBall_c::onDelete() { return true; }
int daGeneralBall_c::onDraw() { return true; }