#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"
#include <profile.h>

const char* ChildSMArcNameList [] = {
	"mrsun",
	NULL
};

class daBossChildSM_c : public daBoss {
public:
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::mdl_c glowModel;

	nw4r::g3d::ResFile resFile;

	bool hasGlow;

	float Baseline;
	float gapEdge2Center;
	float yThreshold;
	float yAccel;
	u32 timer;
	float deathTimer;
	float xSpiralOffset;
	float ySpiralOffset;
	float glowPos;
	short spinReduceZ;
	short spinReduceY;
	float spinStateOn;
	int dying;
	char sunDying;
	char killFlag;

	int randomSpeedcreate;
	int firstRandomUpDown;
	int updowntimer;
	int iceballXPos;
	int saveIceYPos;
	int stackLatestAttack;
	bool isInvulnerable;
	int damage;
	int maxDamage;
	int showerRandInt;
	bool isIntro;

	// related movement
	float sinTimerY;
	bool sinTimerYRunning, stopMoving;

	int debugAttackType;
	enum debugAttackType{
		DISABLE = 0,
		SPIT = 1,
		SPIN = 2,
		SHOWER = 3,
	};

	bool isdying;

	static dActor_c* build();

	void bindAnimChr_and_setUpdateRates(const char* name, m3d::anmChr_c &animationChr, m3d::mdl_c &model, float rate);
	void updateModelMatrices();

	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	
	void addScoreWhenHit(void *other);	// スコアカウントをオーバーライドして削除

	USING_STATES(daBossChildSM_c);
	DECLARE_STATE(Intro);				// 常駐
	DECLARE_STATE(Spit);				// Attack
	DECLARE_STATE(Spin);
	DECLARE_STATE(Shower);
	//DECLARE_STATE(Rain);
	DECLARE_STATE(DieFall);				// Others
	DECLARE_STATE(Damage);
	DECLARE_STATE(Wait);
};

dActor_c *daBossChildSM_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daBossChildSM_c));
	return new(buffer) daBossChildSM_c;
}

/////////////////////////////
// more sprites config
const SpriteData BossChildSMSpriteData = { ProfileId::BossChildSM, 0, 0, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
// {Sprite's profileID, x pos offset, y pos offset, x spawn offset, y spawn offset, x size offset, y size offset, unk, unk, unk, unk, spawnflag 0}
Profile BossChildSMProfile(&daBossChildSM_c::build, SpriteId::BossChildSM, &BossChildSMSpriteData, ProfileId::BossChildSM, ProfileId::BossChildSM, "BossChildSM", ChildSMArcNameList, 0x20);

// State and extern
CREATE_STATE(daBossChildSM_c, Intro);	// 常駐
CREATE_STATE(daBossChildSM_c, Spit);	// Attack
CREATE_STATE(daBossChildSM_c, Spin);
CREATE_STATE(daBossChildSM_c, Shower);
//CREATE_STATE(daBossChildSM_c, Rain);
CREATE_STATE(daBossChildSM_c, DieFall);	// Others
CREATE_STATE(daBossChildSM_c, Damage);
CREATE_STATE(daBossChildSM_c, Wait);

extern bool isChildDead;
extern bool enableDebugMode;


// Collision
void daBossChildSM_c::addScoreWhenHit(void *other){};

bool daBossChildSM_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	apOther->owner->kill();
	S16Vec nullRot = {0,0,0};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
	SpawnEffect("Wm_en_burst_m", 0, &apOther->owner->pos, &nullRot, &oneVec);
	apThis->someFlagByte |= 2;
	return true;
}
void daBossChildSM_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	if (apOther->owner->name == WM_PAKKUN) {
		if (this->isInvulnerable) {	// 無敵時間
			return;
		}
		this->damage += 1;

		spawnHitEffectAtPosition((Vec2){apOther->owner->pos.x, apOther->owner->pos.y});

		S16Vec nullRot = {0,0,0};
		Vec oneVec = {1.0f, 1.0f, 1.0f};
		SpawnEffect("Wm_en_burst_m", 0, &apOther->owner->pos, &nullRot, &oneVec);
		PlaySound(apOther->owner, SE_BOSS_CMN_STOMPED);
		apOther->owner->Delete(1);

		if (this->damage == this->maxDamage){ doStateChange(&StateID_DieFall); }
		else 								{ doStateChange(&StateID_Damage); }
	}
}
void daBossChildSM_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayer(this, apThis, apOther);
}

// 常駐
int daBossChildSM_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	if ((this->settings & 0xF) == 0) { // 0000 000X
		hasGlow = true;

		nw4r::g3d::ResFile rf(getResource("mrsun", "g3d/sun.brres"));
		bodyModel.setup(rf.GetResMdl("Sun"), &allocator, 0x224, 1, 0);
		SetupTextures_Map(&bodyModel, 0);

		glowModel.setup(rf.GetResMdl("SunGlow"), &allocator, 0x224, 1, 0);
		SetupTextures_Boss(&glowModel, 0);
	}
	else { // moon
		hasGlow = false;

		nw4r::g3d::ResFile rf(getResource("mrsun", "g3d/moon.brres"));
		bodyModel.setup(rf.GetResMdl("Moon"), &allocator, 0x224, 1, 0);
		SetupTextures_Map(&bodyModel, 0);
	}
	
	allocator.unlink();

	this->scale = (Vec){0.5, 0.5, 0.5};


	ActivePhysics::Info HitMeBaby;
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 0.0;
	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x6F;

	if ((this->settings & 0xF) == 0) { // It's a sun
		HitMeBaby.bitfield2 = 0xffbafffc; 
		HitMeBaby.xDistToEdge = 24.0;
		HitMeBaby.yDistToEdge = 24.0;
	}	
	else { // It's a moon
		HitMeBaby.bitfield2 = 0xffbafffe; 
		HitMeBaby.xDistToEdge = 12.0;
		HitMeBaby.yDistToEdge = 12.0;
	}

	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;


	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

	this->Baseline = this->pos.x;
	this->yThreshold = 15.0;
	this->yAccel = 0.2;
	this->timer = 0;
	this->xSpiralOffset = 0.0;
	this->ySpiralOffset = 0.0;
	this->dying = -5;
	this->sunDying = 0;
	this->killFlag = 0;
	this->damage = 0;
	if (!enableDebugMode){this->maxDamage = 12; }
	else				{ this->maxDamage = 1;}
	this->isInvulnerable = false;
	this->isIntro = false;
	isChildDead = false;
	this->sinTimerYRunning = true;
	this->isdying = false;
	
	if (this->settings == 1)
		this->pos.z = 6000.0f; // moon
	else
		this->pos.z = 5750.0f; // sun
		
	this->rot.x = 18000;
	this->rot.y = 0;
	this->rot.z = 0;

	// Reggie
	if(enableDebugMode){
		this->debugAttackType = this->settings >> 4 & 0xF;		// 0000 0000 00"0"0 0000	// nybble 11
	}
	else{ this->debugAttackType = DISABLE; }
	
	doStateChange(&StateID_Intro);

	return true;
}

int daBossChildSM_c::onDelete() {
	return true;
}

int daBossChildSM_c::onExecute() {
	acState.execute();
	updateModelMatrices();

	// Movement
	ClassWithCameraInfo *cwci = ClassWithCameraInfo::instance;
	if (!this->isdying){
		// x
		if (!this->isIntro){	// xは画面端で固定 (余裕があれば少し揺らしたい)
			this->pos.x = cwci->screenCentreX + this->gapEdge2Center;
		}
		// y
		if (!this->isIntro){
			float ymod = sin(this->sinTimerY * 3.14 / 130.0) * (sinTimerYRunning ? 84.0 : 10.0);	// 計算はBossCaptainBowserの受け売りです
			this->pos.y = ClassWithCameraInfo::instance->screenCentreY + ymod;
			sinTimerY++;
			if (sinTimerY >= 260) {
				sinTimerY = 0;
				if (stopMoving)
					sinTimerYRunning = false;
			}
		}
	}

	return true;
}

int daBossChildSM_c::onDraw() {
	bodyModel.scheduleForDrawing();
	if (hasGlow)
		glowModel.scheduleForDrawing();

	return true;
}

void daBossChildSM_c::updateModelMatrices() {
	// This won't work with wrap because I'm lazy.
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);

	if (hasGlow) {
		mMtx glowMatrix;
		short rotY;
		
		glowPos += 0.01666666666666;
		if (glowPos > 1) { glowPos = 0; }
		
		rotY = (1000 * sin(glowPos * 3.14)) + 500;


		glowMatrix.translation(pos.x, pos.y, pos.z);
		glowMatrix.applyRotationX(&rot.x);
		glowMatrix.applyRotationY(&rotY);

		glowModel.setDrawMatrix(glowMatrix);
		glowModel.setScale(&scale);
		glowModel.calcWorld(false);
	}
}

/* Intro State */
void daBossChildSM_c::beginState_Intro(){
	this->isIntro = true;
	this->isInvulnerable = true;
	this->timer = 0;
	this->speed.x = 0.0;
	ClassWithCameraInfo *cwci = ClassWithCameraInfo::instance;
	this->pos.y = cwci->screenCentreY;
	this->pos.x = cwci->screenCentreX + 10.0 + 427.0;	// screen width: 854 maybe
}
void daBossChildSM_c::executeState_Intro(){
	this->timer += 1;

	// 回転
	short rotBonus;
	if (this->timer < 60) { rotBonus = this->timer; }
	else { rotBonus = 120 - this->timer; }
	this->rot.z = this->rot.z + (55.1 * rotBonus);
	this->rot.y = this->rot.y + (18.4 * rotBonus);

	// 画面外から画面右端に移動
	if (this->timer >= 80){	// なめらかに停止させ隊
		if (this->speed.x >= 0.0){
			this->speed.x = 0.0;
		}
		else{
			this->speed.x += 0.2;
		}
	}
	else{	// 滑らかに開始させ隊
		if (this->speed.x > -1.5){
			this->speed.x -= 0.5;
		}
		else{
			this->speed.x = -1.5;
		}
	}
	this->HandleXSpeed();
	this->UpdateObjectPosBasedOnSpeedValuesReal();

	if (this->timer > 120) { doStateChange(&StateID_Wait); }
}
void daBossChildSM_c::endState_Intro(){
	ClassWithCameraInfo *cwci = ClassWithCameraInfo::instance;
	this->Baseline = this->pos.x;
	this->gapEdge2Center = (this->Baseline - cwci->screenCentreX);

	this->isIntro = false;
	this->isInvulnerable = false;
}
/* Spit State */
void daBossChildSM_c::beginState_Spit(){
	this->timer = 0;
}
void daBossChildSM_c::executeState_Spit(){
	int randIntSpit;
	randIntSpit = GenerateRandomNumber(2);

	if (this->timer == 10) {
		PlaySound(this, 431);

		if(randIntSpit == 0){	// 3 balls
			if ((this->settings & 0xF) == 0) {	// sun
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -3.5;
				spawner->speed.y = 6.0;
				spawner->pos.z = 5550.0;
				
				spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -6.0;
				spawner->speed.y = 0.0;
				spawner->pos.z = 5550.0;
			
				spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -3.5;
				spawner->speed.y = -6.0;
				spawner->pos.z = 5550.0;
			}
			else{								// moon
				Vec facePos = {this->pos.x - 25.0f, this->pos.y, this->pos.z};
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, currentLayerID);
				spawner->speed.x = -3.5; //up right 
				spawner->speed.y = 6.0f;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
				spawner->y_speed_inc = 0.0f;
				
				spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, currentLayerID);
				spawner->speed.x = -6.0;
				spawner->speed.y = 0.0f;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
				spawner->y_speed_inc = 0.0f;
			
				spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, currentLayerID);
				spawner->speed.x = -3.5;
				spawner->speed.y = -6.0f;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
				spawner->y_speed_inc = 0.0f;
			}
		}

		if(randIntSpit == 1){	// 4 balls
			if ((this->settings & 0xF) == 0) {	// sun
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -3.5;
				spawner->speed.y = 5.0;
				spawner->pos.z = 5550.0;
				
				spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -6.0;
				spawner->speed.y = 1.5;
				spawner->pos.z = 5550.0;
			
				spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -6.0;
				spawner->speed.y = -1.5;
				spawner->pos.z = 5550.0;
			
				spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -3.5;
				spawner->speed.y = -5.0;
				spawner->pos.z = 5550.0;
			}
			else{								// moon
				Vec facePos = {this->pos.x - 25.0f, this->pos.y, this->pos.z};
				
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, currentLayerID);
				spawner->speed.x = -3.5;
				spawner->speed.y = 5.0f;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
				spawner->y_speed_inc = 0.0f;
				
				spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, currentLayerID);
				spawner->speed.x = -6.0;
				spawner->speed.y = 1.5f;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
				spawner->y_speed_inc = 0.0f;
			
				spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, currentLayerID);
				spawner->speed.x = -6.0;
				spawner->speed.y = -1.5f;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
				spawner->y_speed_inc = 0.0f;
			
				spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, currentLayerID);
				spawner->speed.x = -3.5;
				spawner->speed.y = -5.0f;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
				spawner->y_speed_inc = 0.0f;
			}
		}

	}
	this->timer += 1;
	if (this->timer > 30) { doStateChange(&StateID_Wait); }
}
void daBossChildSM_c::endState_Spit(){}
/* Spin State */
void daBossChildSM_c::beginState_Spin(){
	this->timer = 0;
}
void daBossChildSM_c::executeState_Spin(){
	PlaySound(this, 282);
	this->timer += 1;
	
	this->HandleXSpeed();

	short rotBonus;
	if (this->timer < 60) { rotBonus = this->timer; }
	else { rotBonus = 120 - this->timer; }
	this->rot.z = this->rot.z + (55.1 * rotBonus);
	this->rot.y = this->rot.y + (18.4 * rotBonus);


	float spitspeed;
	if ((this->settings & 0xF) == 0) { spitspeed = 3.0; } // It's a sun
	else { spitspeed = 4.0;  } // It's a moon	

	int randomBall;
	randomBall = GenerateRandomNumber(12);
	if (randomBall == 1) {
		int direction;
		direction = GenerateRandomNumber(4);
		
		float xlaunch;
		float ylaunch;
		
		if (direction == 0) {
			xlaunch = -spitspeed;
			ylaunch = spitspeed / 2; }
		else if (direction == 1) {
			xlaunch = -spitspeed;
			ylaunch = spitspeed; }
		else if (direction == 2) {
			xlaunch = -spitspeed;
			ylaunch = -spitspeed / 2; }
		else if (direction == 3) {
			xlaunch = -spitspeed;
			ylaunch = -spitspeed; }
		
		PlaySound(this, 432);

		if ((this->settings & 0xF) == 0) { 
			dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
			spawner->speed.x = xlaunch;
			spawner->speed.y = ylaunch;
			spawner->pos.z = 5550.0;
		} // sun
		else {
			Vec facePos = {this->pos.x - 25.0f, this->pos.y, this->pos.z};
			dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
			spawner->speed.x = xlaunch;
			spawner->speed.y = ylaunch;
			spawner->pos.z = 5550.0;
			spawner->x_speed_inc = 0.0f;
			spawner->y_speed_inc = -0.1f;
		} // moon
	}

	if (this->timer > 120) { doStateChange(&StateID_Wait); }
}
void daBossChildSM_c::endState_Spin(){
	this->rot.x = 18000;
	this->rot.y = 0;
	this->rot.z = 0;
}
/* Shower State */
void daBossChildSM_c::beginState_Shower(){
	this->timer = 0;
	this->rot.y = 0;
	this->showerRandInt = GenerateRandomNumber(2);
}
void daBossChildSM_c::executeState_Shower(){
	Vec facePos = {this->pos.x - 25.0f, this->pos.y, this->pos.z};

	if (this->showerRandInt == 0){ //start from upside
		if ((this->settings & 0xF) == 0) { 
			if (this -> timer == 0){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -0.5;
				spawner->speed.y = 6.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 5){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -1.5;
				spawner->speed.y = 5.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 10){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -2.4;
				spawner->speed.y = 4.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 15){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -3.3;
				spawner->speed.y = 3.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 20){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -4.2;
				spawner->speed.y = 2.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 25){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -5.1;
				spawner->speed.y = 1.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 30){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -6.0;
				spawner->speed.y = 0.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 35){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -5.1;
				spawner->speed.y = -1.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 40){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -4.2;
				spawner->speed.y = -2.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 45){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -3.3;
				spawner->speed.y = -3.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 50){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -2.4;
				spawner->speed.y = -4.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 55){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -1.5;
				spawner->speed.y = -5.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 60){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -0.6;
				spawner->speed.y = -6.0;
				spawner->pos.z = 5550.0;
			}
		} // It's a sun
		else {
			if (this -> timer == 0){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -0.5;
				spawner->speed.y = 6.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 5){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -1.5;
				spawner->speed.y = 5.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 10){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -2.4;
				spawner->speed.y = 4.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 15){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -3.3;
				spawner->speed.y = 3.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 20){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -4.2;
				spawner->speed.y = 2.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 25){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -5.1;
				spawner->speed.y = 1.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 30){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -6.0;
				spawner->speed.y = 0.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 35){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -5.1;
				spawner->speed.y = -1.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 40){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -4.2;
				spawner->speed.y = -2.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 45){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -3.3;
				spawner->speed.y = -3.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 50){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -2.4;
				spawner->speed.y = -4.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 55){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -1.5;
				spawner->speed.y = -5.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 60){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -0.6;
				spawner->speed.y = -6.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
		} // It's a moon
	}
	else{ //start from down 
		if ((this->settings & 0xF) == 0) { 
			if (this -> timer == 0){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -0.5;
				spawner->speed.y = -6.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 5){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -1.5;
				spawner->speed.y = -5.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 10){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -2.4;
				spawner->speed.y = -4.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 15){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -3.3;
				spawner->speed.y = -3.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 20){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -4.2;
				spawner->speed.y = -2.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 25){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -5.1;
				spawner->speed.y = -1.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 30){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -6.0;
				spawner->speed.y = 0.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 35){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -5.1;
				spawner->speed.y = 1.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 40){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -4.2;
				spawner->speed.y = 2.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 45){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -3.3;
				spawner->speed.y = 3.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 50){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -2.4;
				spawner->speed.y = 4.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 55){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -1.5;
				spawner->speed.y = 5.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 60){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(PAKKUN_FIREBALL, 0, this->pos, 0, 0);
				spawner->speed.x = -0.5;
				spawner->speed.y = 6.0;
				spawner->pos.z = 5550.0;
			}
		} // It's a sun
		else { 
			if (this -> timer == 0){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -0.5;
				spawner->speed.y = -6.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 5){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -1.5;
				spawner->speed.y = -5.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 10){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -2.4;
				spawner->speed.y = -4.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 15){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -3.3;
				spawner->speed.y = -3.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 20){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -4.2;
				spawner->speed.y = -2.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 25){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -5.1;
				spawner->speed.y = -1.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 30){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -6.0;
				spawner->speed.y = 0.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 35){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -5.1;
				spawner->speed.y = 1.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 40){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -4.2;
				spawner->speed.y = 2.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 45){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -3.3;
				spawner->speed.y = 3.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 50){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -2.4;
				spawner->speed.y = 4.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 55){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -1.5;
				spawner->speed.y = 5.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 60){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0x80, facePos, 0, 0);
				spawner->speed.x = -0.5;
				spawner->speed.y = 6.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
		} // It's a moon
	}

	this->timer += 1;

	if (this->timer > 61) { doStateChange(&StateID_Wait); }
}
void daBossChildSM_c::endState_Shower(){}
/* Wait State */
void daBossChildSM_c::beginState_Wait(){
	this->timer = 0;
	this->rot.y = 0;
}
void daBossChildSM_c::executeState_Wait(){
	int Choice;
	int TimerMax;
	
	if ((this->settings & 0xF) == 0) { TimerMax = 60; } // sun
	else { TimerMax = 40; } // moon
	
	if (this->timer > TimerMax) {
		Choice = GenerateRandomNumber(3);
		if(this->debugAttackType){
			Choice = this->debugAttackType - 1;
		}

		if (Choice == 0) { doStateChange(&StateID_Spit); }
		else if (Choice == 1) { doStateChange(&StateID_Spin); }
		else { doStateChange(&StateID_Shower); }
	}

	this->timer += 1;
}
void daBossChildSM_c::endState_Wait(){}
/* Damage State */
void daBossChildSM_c::beginState_Damage(){
	this->isInvulnerable = true;
	this->timer = 0;
}
void daBossChildSM_c::executeState_Damage(){
	this->timer += 1;

	// 回転
	short rotBonus;
	rotBonus = 60 - this->timer;
	this->rot.y = this->rot.y + (36.8 * rotBonus);
	
	if(this->timer > 60) {
		doStateChange(&StateID_Wait);
	}
}
void daBossChildSM_c::endState_Damage(){
	this->isInvulnerable = false;
	this->timer = 0;
}
/* */
void daBossChildSM_c::beginState_DieFall(){
	this->isInvulnerable = true;
	this->timer = 0;
	this->deathTimer = -6.0;
	this->isdying = true;

	this->rot.x = 18000;
	this->rot.y = 0;
	this->rot.z = 0;
}
void daBossChildSM_c::executeState_DieFall(){
	ClassWithCameraInfo *cwci = ClassWithCameraInfo::instance;

	this->deathTimer += 0.25;

	this->pos.x += 1.0;
	if (this->deathTimer < 8.0){
		this->pos.y += -1.0 * this->deathTimer;
	}
	else{
		this->pos.y += -1.0 * 8.0;
	}

	this->rot.y += 55.1 * (100.0 - (this->deathTimer + 8.0));
	this->rot.z += 36.8 * (100.0 - (this->deathTimer + 8.0));

	if (this->pos.y < cwci->screenCentreY){ isChildDead = true; }
	if (this->pos.y < cwci->screenCentreY - 250){
		nw4r::snd::SoundHandle handle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_EMY_BIG_DOSSUN_DEAD, 1);

		S16Vec nullRot = {0,0,0};
		Vec twoVec = {1.0f, 1.0f, 1.0f};
		SpawnEffect("Wm_mr_stockitemuse_b", 0, &this->pos, &nullRot, &twoVec);
		SpawnEffect("Wm_mr_stockitemuse_c", 0, &this->pos, &nullRot, &twoVec);

		this->kill();
		this->Delete(this->deleteForever);
	}
}
void daBossChildSM_c::endState_DieFall(){}