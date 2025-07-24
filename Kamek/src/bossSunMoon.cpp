#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"

// Welcome to wakanameko's poor and dirty code!
// this sprite is using these trigger ids:
// id32: start walking
// id33: stop walking
// But, you can enter free trigger id of death

// Externs
extern "C" int posIsInZone(Vec,float*,float*,u8 zone);
extern "C" void* ScreenPositionClass;
extern "C" int SpawnThwompEffects(daBoss *);

extern "C" Vec ConvertStagePositionIntoScreenPosition__Maybe(Vec);
extern "C" void AnotherSoundRelatedFunction(void*,SFX,Vec,int);
extern "C" void *EN_LandbarrelPlayerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);
extern "C" bool SpawnEffect(const char* effectBale, int unk, Vec* effectPot, S16Vec* effectRot, Vec* effectScale); 

extern void triggerEvent(int eventID);
extern bool isEventTriggered(int eventID);
extern bool enableDebugMode;

bool isTriggered;

const char* bSMArcNameList [] = {
	"mrsun",
	"keronpa",
	NULL	
};

class daBossSM_c : public daBoss {
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
	float SpiralLoop;
	float yThreshold;
	float yAccel;
	u32 timer;
	float xSpiralOffset;
	float ySpiralOffset;
	float glowPos;
	short spinReduceZ;
	short spinReduceY;
	float spinStateOn;
	int dying;
	char sunDying;
	char killFlag;

	u64 eventFlag;

	int randomSpeedcreate;
	int firstRandomUpDown;
	int updowntimer;
	int iceballXPos;
	int saveIceYPos;
	int stackLatestAttack;

	// Reggie! LevelEditorで設定した値を取得
	int killEvent;

	//int fxxkinGravityTimer;
	/*	unused
	int randomfallfireKAKUDOminus;
	double timer2xjudge;
	int t2XJ;
	double totalXPosition;
	*/

	static daBossSM_c *build();

	void bindAnimChr_and_setUpdateRates(const char* name, m3d::anmChr_c &animationChr, m3d::mdl_c &model, float rate);
	void updateModelMatrices();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);					//player touches
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);						//yoshi touches
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther); //hit fire ball
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);	//hit ice ball
	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);			
	bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther);				//Mario is sliding on collision
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);				//hit Hammer
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther); 

	USING_STATES(daBossSM_c);
	DECLARE_STATE(Follow);
	DECLARE_STATE(Spit);
	DECLARE_STATE(Spin);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Grow);	
	DECLARE_STATE(Spit4);	
	//DECLARE_STATE(fallingfire);		//unused
	DECLARE_STATE(SpawnKeronpa);
	DECLARE_STATE(Outro);	
	DECLARE_STATE(Shower);
};

daBossSM_c *daBossSM_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daBossSM_c));
	return new(buffer) daBossSM_c;
}


CREATE_STATE(daBossSM_c, Follow);
CREATE_STATE(daBossSM_c, Spit);
CREATE_STATE(daBossSM_c, Spin);
CREATE_STATE(daBossSM_c, Wait);
CREATE_STATE(daBossSM_c, Grow); 
CREATE_STATE(daBossSM_c, Spit4);
//CREATE_STATE(daBossSM_c, fallingfire);
CREATE_STATE(daBossSM_c, SpawnKeronpa);
CREATE_STATE(daBossSM_c, Outro); 
CREATE_STATE(daBossSM_c, Shower); 

#define ACTIVATE	1
#define DEACTIVATE	0




void daBossSM_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->_vf220(apOther->owner);
} 
void daBossSM_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) { 
	this->playerCollision(apThis, apOther);
} 
bool daBossSM_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	PlaySoundAsync(this, SE_OBJ_FIREBALL_DISAPP);
	S16Vec nullRot = {0,0,0};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
	SpawnEffect("Wm_mr_fireball_hit", 0, &apOther->owner->pos, &nullRot, &oneVec);
	return true;
}
bool daBossSM_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) { 
	return false;
}
bool daBossSM_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) { 
	return true;
}
bool daBossSM_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayer(this, apThis, apOther);
	return true;
} 
bool daBossSM_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayer(this, apThis, apOther);
	return true;
}
bool daBossSM_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat7_GroundPound(apThis, apOther);
}
bool daBossSM_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) { 
	dActor_c *block = apOther->owner;
	dEn_c *blah = (dEn_c*)block;
	PlaySound(this, SE_EMY_BIG_PAKKUN_DAMAGE_1);
	S16Vec nullRot = {0,0,0};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
	SpawnEffect("Wm_mr_kick_glow", 0, &apOther->owner->pos, &nullRot, &oneVec);
	return true;
}
bool daBossSM_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	EN_LandbarrelPlayerCollision(this, apThis, apOther);
	DamagePlayer(this, apThis, apOther);
	return true;
}
bool daBossSM_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayer(this, apThis, apOther);
	return true;
}
bool daBossSM_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {
	DamagePlayer(this, apThis, apOther);
	return true;
}
bool daBossSM_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	dActor_c *block = apOther->owner;
	dEn_c *blah = (dEn_c*)block;

	PlaySound(this, SE_EMY_BIG_PAKKUN_DAMAGE_1);

	S16Vec nullRot = {0,0,0};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
	SpawnEffect("Wm_mr_kick_glow", 0, &apOther->owner->pos, &nullRot, &oneVec);
	return true;
}
bool daBossSM_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	PlaySoundAsync(this, SE_OBJ_FIREBALL_DISAPP);
	S16Vec nullRot = {0,0,0};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
	SpawnEffect("Wm_mr_fireball_hit", 0, &apOther->owner->pos, &nullRot, &oneVec);
	
	dActor_c *iter = 0;
	while (iter = (dActor_c*)dActor_c::searchByBaseType(2, iter)) {	// Delete Yoshi's fire
		dStageActor_c *sa = (dStageActor_c*)iter;
		if (sa->name == YOSHI_FIRE) {
			sa->Delete(1);
		}
	}
	return true;
}



void daBossSM_c::bindAnimChr_and_setUpdateRates(const char* name, m3d::anmChr_c &animationChr, m3d::mdl_c &model, float rate) {
	nw4r::g3d::ResAnmChr anmChr = resFile.GetResAnmChr(name);
	animationChr.bind(&model, anmChr, 1);
	model.bindAnim(&animationChr, 0.0);
	animationChr.setUpdateRate(rate);
}

bool isEventTriggered(int eventID) {	//from SLLW by RedStoneMatt
	u64 eventFlag = ((u64)1 << (eventID - 1));
	if(dFlagMgr_c::instance->flags & eventFlag) {
		return true;
	}
	return false;
}

void triggerEvent(int eventID) {		//from SLLW by RedStoneMatt
	dFlagMgr_c::instance->flags |= ((u64)1 << (eventID - 1));		// Why he did add "- 1"? I very hate it
}

int daBossSM_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	if ((this->settings & 0xF) == 0) { // It's a sun
		hasGlow = true;

		nw4r::g3d::ResFile rf(getResource("mrsun", "g3d/sun.brres"));
		bodyModel.setup(rf.GetResMdl("Sun"), &allocator, 0x224, 1, 0);
		SetupTextures_Map(&bodyModel, 0);

		glowModel.setup(rf.GetResMdl("SunGlow"), &allocator, 0x224, 1, 0);
		SetupTextures_Boss(&glowModel, 0);
	}
	
	else { // It's a moon
		hasGlow = false;

		nw4r::g3d::ResFile rf(getResource("mrsun", "g3d/moon.brres"));
		bodyModel.setup(rf.GetResMdl("Moon"), &allocator, 0x224, 1, 0);
		SetupTextures_Map(&bodyModel, 0);

		glowModel.setup(rf.GetResMdl("MoonGlow"), &allocator, 0x224, 1, 0);
		SetupTextures_Boss(&glowModel, 0);
	}
	
	allocator.unlink();

	this->scale = (Vec){0.5, 0.5, 0.5};


	ActivePhysics::Info HitMeBaby;
	HitMeBaby.xDistToCenter = 0.0;		//スプライトの中心からどれだけ離すか(+で右)
	HitMeBaby.yDistToCenter = 0.0; 		//スプライトの中心からどれだけ離すか(+で上)
	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	//HitMeBaby.bitfield1 = 0x6F;

	//当たり判定
	if ((this->settings & 0xF) == 0) { // sun
		HitMeBaby.bitfield1 = 0x4F;
		HitMeBaby.bitfield2 = 0b11111111100110100011110011000110; // 右のバイトから、0unk,1ファイア,2アイス,3スター,4unk,5滑り(坂),6unk,7ヒップドロップ,8フェンス,9甲羅,10滑り(ペンギン),11スピン,12unk,13SpinFall(謎),14Fire(No FireBall, but like an explosion),15ヨッシー可食,16ヨッシーが勝手に食うかどうか,17大砲,18持ち上げ,19YoshiBullet,20ファイア(ヨッシー),21アイス(ヨッシー),残りは0
		HitMeBaby.xDistToEdge = 96.0; //base value is 24.0
		HitMeBaby.yDistToEdge = 96.0; //same as over
	}	
	else { // moon
		HitMeBaby.bitfield1 = 0x4F;	//0x4F
		HitMeBaby.bitfield2 = 0b00000000000110100111111111110010;	// base is 0xffbafffe
		HitMeBaby.xDistToEdge = 83.0;
		HitMeBaby.yDistToEdge = 83.0;
	}

	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;


	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

	this->Baseline = this->pos.y;
	this->SpiralLoop = 0;
	this->yThreshold = 15.0;
	this->yAccel = 0.2;
	this->timer = 0;
	this->xSpiralOffset = 0.0;
	this->ySpiralOffset = 0.0;
	this->dying = -5;
	this->sunDying = 0;
	this->killFlag = 0;
	this->stackLatestAttack = -1;
	
	if (this->settings == 1)
		this->pos.z = 6000.0f; // moon
	else
		this->pos.z = 5750.0f; // sun


	char eventNum	= (this->settings >> 16) & 0xFF;

	this->eventFlag = (u64)1 << (eventNum - 1);
	
	// Reggie!
	this->killEvent = (this->settings >> 16) & 0xFF;
	if (enableDebugMode){
		OSReport("Event Kill : %02d\n", this->killEvent);
	}

	// moving up or down
	firstRandomUpDown = GenerateRandomNumber(2);
	this -> updowntimer = 0;

	doStateChange(&StateID_Grow); //have to change to grow
	// this->onExecute();
	return true;
}

int daBossSM_c::onDelete() {
	isTriggered = false;	//from SLLW by RedStoneMatt
	return true;
}

int daBossSM_c::onExecute() {
	acState.execute();
	updateModelMatrices();
	
	// moving up or down
	bool flagOnUD = isEventTriggered(32);
	if(flagOnUD && !isTriggered) {
		if (this -> firstRandomUpDown == 0){ // up
			if (this -> updowntimer <= 40 ){
				this->pos.y += 1;
			}
			else if (this -> updowntimer <= 60 ){
			}
			else if (this -> updowntimer <= 100 ){
				this->pos.y += -1;
			}
			else if (this -> updowntimer <= 140 ){
				this->pos.y += -1;
			}
			else if (this -> updowntimer <= 160 ){
			}
			else if (this -> updowntimer <= 200 ){
				this->pos.y += 1;
			}
			else if (this -> updowntimer >= 201 ){
				this -> updowntimer = 0;
			}
		}
		else { // down
			if (this -> updowntimer <= 40 ){
				this->pos.y += -1;
			}
			else if (this -> updowntimer <= 60 ){
			}
			else if (this -> updowntimer <= 100 ){
				this->pos.y += 1;
			}
			else if (this -> updowntimer <= 140 ){
				this->pos.y += 1;
			}
			else if (this -> updowntimer <= 160 ){
			}
			else if (this -> updowntimer <= 200 ){
				this->pos.y += -1;
			}
			else if (this -> updowntimer >= 201 ){
				this -> updowntimer = 0;
			}
		}
	}
	this -> updowntimer = this -> updowntimer + 1;

	// moving right side
	bool flagOnW = isEventTriggered(32);
	if(flagOnW && !isTriggered) {
		bool flagOnS = isEventTriggered(33);
		if(flagOnS && !isTriggered) {
			this->pos.x += -0.5;
		}
		//OSReport("=========[Walking Now!]=========\n");
		this->pos.x += 0.5;
		//this->totalXPosition += 0.5;
	}
	
	if (dFlagMgr_c::instance->flags & this->eventFlag) {
		if (this->killFlag == 0 && acState.getCurrentState()->isNotEqual(&StateID_Outro)) {
			doStateChange(&StateID_Outro);
		}
	}

	bool flagOn = isEventTriggered(this->killEvent);
	if(flagOn && !isTriggered) {
		OSReport("OWO TRIGGGGGEEEEERRRRRRRRED\n");
		updowntimer = 250;
		isTriggered = true;
	}
	return true;
}

int daBossSM_c::onDraw() {
	bodyModel.scheduleForDrawing();
	if (hasGlow)
		glowModel.scheduleForDrawing();

	return true;
}


void daBossSM_c::updateModelMatrices() {
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


// Follow State

void daBossSM_c::beginState_Follow() { 
	this->timer = 0;
	this->rot.x = 18000;
	this->rot.y = 0;
	this->rot.z = 0;
}
void daBossSM_c::executeState_Follow() { 
	if (this->timer > 200) { this->doStateChange(&StateID_Wait); }
	this->timer = this->timer + 1;
}
void daBossSM_c::endState_Follow() { 
	this->speed.y = 0;
}



// Spit State

void daBossSM_c::beginState_Spit() { 
	this->rot.x = 18000;
	this->rot.y = 0;
	this->rot.z = 0;
	
	this->timer = 0;
	//this->fxxkinGravityTimer = 0;
	this->spinStateOn = 1;
}
void daBossSM_c::executeState_Spit() { 
	
	if (this->timer == 10) {
		PlaySound(this, 432);

		if ((this->settings & 0xF) == 0) { 
			dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
			spawner->speed.x = 3.5;
			spawner->speed.y = 6.0;
			spawner->pos.z = 5550.0;
			
			spawner = CreateActor(106, 0, this->pos, 0, 0);
			spawner->speed.x = 6.0;
			spawner->speed.y = 0.0;
			spawner->pos.z = 5550.0;
		
			spawner = CreateActor(106, 0, this->pos, 0, 0);
			spawner->speed.x = 3.5;
			spawner->speed.y = -6.0;
			spawner->pos.z = 5550.0;
		} // It's a sun
		else { 
			Vec facePos = {this->pos.x + 105.0f, this->pos.y, this->pos.z};
			dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0, facePos, 0, currentLayerID);
			spawner->speed.x = 3.5; //up right 
			spawner->speed.y = 6.0f;
			spawner->pos.z = 5550.0;
            spawner->x_speed_inc = 0.0f;
            spawner->y_speed_inc = 0.0f;
			
			spawner = CreateActor(BROS_ICEBALL, 0, facePos, 0, currentLayerID);
			spawner->speed.x = 6.0;
			spawner->speed.y = 0.0f;
			spawner->pos.z = 5550.0;
            spawner->x_speed_inc = 0.0f;
            spawner->y_speed_inc = 0.0f;
		
			spawner = CreateActor(BROS_ICEBALL, 0, facePos, 0, currentLayerID);
			spawner->speed.x = 3.5;
			spawner->speed.y = -6.0f;
			spawner->pos.z = 5550.0;
            spawner->x_speed_inc = 0.0f;
            spawner->y_speed_inc = 0.0f;
		} // It's a moon	
	}
	
	this->timer = this->timer + 1;

	if (this->timer > 30) { doStateChange(&StateID_Follow); }

}
void daBossSM_c::endState_Spit() { 
	this->spinStateOn = 0;
}

// Spit4 State

void daBossSM_c::beginState_Spit4() { 
	this->rot.x = 18000;
	this->rot.y = 0;
	this->rot.z = 0;

	this->timer = 0;
	this->spinStateOn = 1;
}
void daBossSM_c::executeState_Spit4() { 
	
	if (this->timer == 10) {

		PlaySound(this, 432);
	
		this->direction = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);
		
		float neg = -1.0;
		if (this->direction == 0) { neg = 1.0; }
		

		if ((this->settings & 0xF) == 0) { 
			dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
			spawner->speed.x = 3.5 * neg;
			spawner->speed.y = 5.0;
			spawner->pos.z = 5550.0;
			
			spawner = CreateActor(106, 0, this->pos, 0, 0);
			spawner->speed.x = 6.0 * neg;
			spawner->speed.y = 1.5;
			spawner->pos.z = 5550.0;
		
			spawner = CreateActor(106, 0, this->pos, 0, 0);
			spawner->speed.x = 6.0 * neg;
			spawner->speed.y = -1.5;
			spawner->pos.z = 5550.0;
		
			spawner = CreateActor(106, 0, this->pos, 0, 0);
			spawner->speed.x = 3.5 * neg;
			spawner->speed.y = -5.0;
			spawner->pos.z = 5550.0;
		} // It's a sun
		else { 
			Vec facePos = {this->pos.x + 105.0f, this->pos.y, this->pos.z};
			
			dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0, facePos, 0, currentLayerID);
			spawner->speed.x = 3.5;
			spawner->speed.y = 5.0f;
			spawner->pos.z = 5550.0;
            spawner->x_speed_inc = 0.0f;
            spawner->y_speed_inc = 0.0f;
			
			spawner = CreateActor(BROS_ICEBALL, 0, facePos, 0, currentLayerID);
			spawner->speed.x = 6.0;
			spawner->speed.y = 1.5f;
			spawner->pos.z = 5550.0;
            spawner->x_speed_inc = 0.0f;
            spawner->y_speed_inc = 0.0f;
		
			spawner = CreateActor(BROS_ICEBALL, 0, facePos, 0, currentLayerID);
			spawner->speed.x = 6.0;
			spawner->speed.y = -1.5f;
			spawner->pos.z = 5550.0;
            spawner->x_speed_inc = 0.0f;
            spawner->y_speed_inc = 0.0f;
		
			spawner = CreateActor(BROS_ICEBALL, 0, facePos, 0, currentLayerID);
			spawner->speed.x = 3.5;
			spawner->speed.y = -5.0f;
			spawner->pos.z = 5550.0;
            spawner->x_speed_inc = 0.0f;
            spawner->y_speed_inc = 0.0f;
		} // It's a moon	

	}
	
	this->timer = this->timer + 1;

	if (this->timer > 30) { doStateChange(&StateID_Follow); }

}
void daBossSM_c::endState_Spit4() { 
	this->spinStateOn = 0;
}


/* fallingfire State	//unused

void daBossSM_c::beginState_fallingfire() { 
	this->rot.x = 18000;
	this->rot.y = 0;
	this->rot.z = 0;
	
	this->timer = 0;
	this->spinStateOn = 1;

	this->t2XJ = 0;
}
void daBossSM_c::executeState_fallingfire() { 
	int randomfallfire;
	int randomfallfireKAKUDO;
	double timer2xjudge(timer / 2.0); 				//timerを2.0で割り算
	if (timer2xjudge - timer2xjudge + 0.5 == 0.0) {	//2で割った数 - 2で割った数 + 0.5 これで結果的に偶数だった場合は0.0となる 
		this->t2XJ == 0;							//上記から、元のtimerの数値が結果的に奇数だった場合、t2XJに1が代入される
	} 

	if (this->timer == 2 || this->timer == 4 || this->timer == 6 || this->timer == 8 || this->timer == 10 || this->timer == 12 || this->timer == 14 || this->timer == 16 || this->timer == 18 || this->timer == 20) {
		//if (t2XJ == 0){
			PlaySound(this, 432);
			if ((this->settings & 0xF) == 0) { 
				dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
				spawner->speed.x = 0.5;
				spawner->speed.y = 6.0;
				spawner->pos.z = 5550.0;
			} // It's a sun
			else { 
				dStageActor_c *spawner = CreateActor(118, 0, this->pos, 0, 0);
				spawner->speed.x = 0.5;
				spawner->speed.y = 6.0;
				spawner->pos.z = 5550.0;
				*((u32 *) (((char *) spawner) + 0x3DC)) = this->id;
			} // It's a moon	
		//}
	}
	if (this->timer >= 22) {
		//if (t2XJ == 0){
			randomfallfire = GenerateRandomNumber(500);				// 0~499
			randomfallfireKAKUDO = GenerateRandomNumber(2);			// 0 or 1
			randomfallfireKAKUDOminus = GenerateRandomNumber(2);	// 0 or 1

			PlaySound(this, 716);
			
			if (this->randomfallfireKAKUDOminus == 0){ //not minus values
				if ((this->settings & 0xF) == 0) { 
					dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
					spawner->speed.x = randomfallfireKAKUDO * 1.0;
					spawner->speed.y = -6.0;
					spawner->pos.x = this->pos.x + randomfallfire;
					spawner->pos.y = this->pos.y + 400.0;
					spawner->pos.z = 5550.0;
				} // It's a sun
				else { 
					dStageActor_c *spawner = CreateActor(118, 0, this->pos, 0, 0);
					spawner->speed.x = randomfallfireKAKUDO;
					spawner->speed.y = -6.0;
					spawner->pos.x = randomfallfire;
					spawner->pos.y = 200;
					spawner->pos.z = 5550.0;
					*((u32 *) (((char *) spawner) + 0x3DC)) = this->id;
				} // It's a moon	
			}
			else{ // minus values
				if ((this->settings & 0xF) == 0) { 
					dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
					spawner->speed.x = randomfallfireKAKUDO * -1.0;
					spawner->speed.y = -6.0;
					spawner->pos.x = this->pos.x + randomfallfire;
					spawner->pos.y = this->pos.y + 400.0;
					spawner->pos.z = 5550.0;
				} // It's a sun
				else { 
					dStageActor_c *spawner = CreateActor(118, 0, this->pos, 0, 0);
					spawner->speed.x = randomfallfireKAKUDO * -1.0;
					spawner->speed.y = -6.0;
					spawner->pos.x = randomfallfire;
					spawner->pos.y = 200;
					spawner->pos.z = 5550.0;
					*((u32 *) (((char *) spawner) + 0x3DC)) = this->id;
				} // It's a moon	
			}
		//}
	}
	
	this->timer = this->timer + 1;

	if (this->timer > 60) { doStateChange(&StateID_Follow); }
}
void daBossSM_c::endState_fallingfire() { 
	this->spinStateOn = 0;
}
*/


// Spin State

void daBossSM_c::beginState_Spin() { 
	this->timer = 0;
	this->rot.x = 18000;
	this->rot.y = 0;
	this->rot.z = 0;
	this->spinReduceZ = 0;
	this->spinReduceY = 0;
}
void daBossSM_c::executeState_Spin() { 
	PlaySound(this, 282);

	this->direction = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);
	
	this->timer = this->timer + 1;

	this->HandleXSpeed();

	short rotBonus;
	if (this->timer < 60) { rotBonus = this->timer; }
	else { rotBonus = 120 - this->timer; }

	this->rot.z = this->rot.z + (55.1 * rotBonus);
	this->rot.y = this->rot.y + (18.4 * rotBonus); //}


	float spitspeed;
	if ((this->settings & 0xF) == 0) { spitspeed = 3.0; } // It's a sun
	else { spitspeed = 4.0;  } // It's a moon	

	int randomBall;
	if ((this->settings & 0xF) == 0) { randomBall = GenerateRandomNumber(8); }
	else { randomBall = GenerateRandomNumber(4); }
	if (randomBall == 1) {
		int direction;
		direction = GenerateRandomNumber(8);
		
		float xlaunch;
		float ylaunch;
		
		if (direction == 0) { 
			xlaunch = spitspeed;
			ylaunch = 0.0; }
		else if (direction == 1) { // SE
			xlaunch = spitspeed;
			ylaunch = spitspeed; }
		else if (direction == 2) { // S
			xlaunch = 0.0;
			ylaunch = spitspeed; }
		else if (direction == 3) { // SW
			xlaunch = -spitspeed;
			ylaunch = spitspeed; }
		else if (direction == 4) {	// W
			xlaunch = -spitspeed;
			ylaunch = 0.0; }
		else if (direction == 5) {	// NW
			xlaunch = -spitspeed;
			ylaunch = -spitspeed; }
		else if (direction == 6) {	// N
			xlaunch = 0.0;
			ylaunch = -spitspeed; }
		else if (direction == 7) {	// NE
			xlaunch = spitspeed;
			ylaunch = -spitspeed; }
		
		PlaySound(this, 432);

		if ((this->settings & 0xF) == 0) { 
			dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
			spawner->speed.x = xlaunch;
			spawner->speed.y = ylaunch;
			spawner->pos.z = 5550.0;
		} // It's a sun
		else { 
			Vec posUpperSky = {this->pos.x + 180.0f + (direction * 60.0f), this->Baseline + 160.0f, this->pos.z};
			
			// 強制的に南西に発射
			xlaunch = -spitspeed / 2.0;
			ylaunch = -spitspeed; 

			dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0, posUpperSky, 0, currentLayerID);
			spawner->speed.x = xlaunch;
			spawner->speed.y = ylaunch;
			spawner->pos.z = 5550.0;		
            spawner->x_speed_inc = 0.0f;
            spawner->y_speed_inc = 0.0f;
		} // It's a moon	
	}

	if (this->timer > 120) { this->doStateChange(&StateID_Follow); }
	
}
void daBossSM_c::endState_Spin() { 
	this->rot.x = 18000;
	this->rot.y = 0;
	this->rot.z = 0;
}


// SpawnKeronpa State

void daBossSM_c::beginState_SpawnKeronpa() {
	this->timer = 0;
	this->rot.x = 18000;
	this->rot.y = 0;
	this->rot.z = 0;
}
void daBossSM_c::executeState_SpawnKeronpa() {
	float neg = -1.0;
	if (this->direction == 0) { neg = 1.0; }

	if (this->timer == 10){
		PlaySound(this, 158);

		// spawn effect
		Vec tempPos = (Vec){this->pos.x + 150, this->pos.y, 3564.0f};
		S16Vec nullRot = {0,0,0};
		Vec oneVec = {1.0f, 1.0f, 1.0f};
		SpawnEffect("Wm_en_landsmoke", 0, &tempPos, &nullRot, &oneVec);

		// spawn keronpa
		if((this->settings & 0xF) == 0){	// sun	
			dStageActor_c *spawner = CreateActor(224, 0, this->pos, 0, 0);			// normal
			spawner->pos.x = this->pos.x + 150;
			spawner->pos.z = 5550.0;
		}
		else{						// moon
			dStageActor_c *spawner = CreateActor(224, 0x00000761, this->pos, 0, 0);	// Required: Kazuki_4ys's ice keronpa patch
			spawner->pos.x = this->pos.x + 150;
			spawner->pos.z = 5550.0; 
		}
	}

	this->timer = this->timer + 1;
	if (this->timer > 30) { doStateChange(&StateID_Follow); }
}
void daBossSM_c::endState_SpawnKeronpa() { }


// Shower State

void daBossSM_c::beginState_Shower() { 
	this->rot.x = 18000;
	this->rot.y = 0;
	this->rot.z = 0;
	speed.x = 0;
	speed.y = 0;

	randomSpeedcreate = GenerateRandomNumber(2);

	this->timer = 0;
}
void daBossSM_c::executeState_Shower() { 
	Vec facePos = {this->pos.x + 105.0f, this->pos.y, this->pos.z};

	if (randomSpeedcreate == 0){ //start from upside
		if ((this->settings & 0xF) == 0) { 
			if (this -> timer == 0){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
				spawner->speed.x = 0.5;
				spawner->speed.y = 6.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 5){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
				spawner->speed.x = 1.5;
				spawner->speed.y = 5.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 10){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
				spawner->speed.x = 2.4;
				spawner->speed.y = 4.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 15){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
				spawner->speed.x = 3.3;
				spawner->speed.y = 3.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 20){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
				spawner->speed.x = 4.2;
				spawner->speed.y = 2.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 25){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
				spawner->speed.x = 5.1;
				spawner->speed.y = 1.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 30){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
				spawner->speed.x = 6.0;
				spawner->speed.y = 0.0;
				spawner->pos.z = 5550.0;
			}
		} // It's a sun
		else { 
			if (this -> timer == 0){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0, facePos, 0, currentLayerID);
				spawner->speed.x = 0.5;
				spawner->speed.y = 6.0;
				spawner->pos.z = 5550.0;	
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 5){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0, facePos, 0, currentLayerID);
				spawner->speed.x = 1.5;
				spawner->speed.y = 5.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 10){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0, facePos, 0, currentLayerID);
				spawner->speed.x = 2.4;
				spawner->speed.y = 4.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 15){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0, facePos, 0, currentLayerID);
				spawner->speed.x = 3.3;
				spawner->speed.y = 3.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 20){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0, facePos, 0, currentLayerID);
				spawner->speed.x = 4.2;
				spawner->speed.y = 2.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 25){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0, facePos, 0, currentLayerID);
				spawner->speed.x = 5.1;
				spawner->speed.y = 1.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 30){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0, facePos, 0, currentLayerID);
				spawner->speed.x = 6.0;
				spawner->speed.y = 0.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
		} // It's a moon
	}
	else{ //start from down 
		if ((this->settings & 0xF) == 0) { 
			if (this -> timer == 0){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
				spawner->speed.x = 6.0;
				spawner->speed.y = 0.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 5){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
				spawner->speed.x = 5.1;
				spawner->speed.y = 1.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 10){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
				spawner->speed.x = 4.2;
				spawner->speed.y = 2.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 15){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
				spawner->speed.x = 3.3;
				spawner->speed.y = 3.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 20){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
				spawner->speed.x = 2.4;
				spawner->speed.y = 4.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 25){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
				spawner->speed.x = 1.5;
				spawner->speed.y = 5.0;
				spawner->pos.z = 5550.0;
			}
			if (this -> timer == 30){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
				spawner->speed.x = 0.5;
				spawner->speed.y = 6.0;
				spawner->pos.z = 5550.0;
			}
		} // It's a sun
		else { 
			if (this -> timer == 0){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0, facePos, 0, currentLayerID);
				spawner->speed.x = 6.0;
				spawner->speed.y = 0.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 5){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0, facePos, 0, currentLayerID);
				spawner->speed.x = 5.1;
				spawner->speed.y = 1.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 10){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0, facePos, 0, currentLayerID);
				spawner->speed.x = 4.2;
				spawner->speed.y = 2.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 15){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0, facePos, 0, currentLayerID);
				spawner->speed.x = 3.3;
				spawner->speed.y = 3.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 20){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0, facePos, 0, currentLayerID);
				spawner->speed.x = 2.4;
				spawner->speed.y = 4.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 25){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0, facePos, 0, currentLayerID);
				spawner->speed.x = 1.5;
				spawner->speed.y = 5.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
			if (this -> timer == 30){
				PlaySound(this, 432);
				dStageActor_c *spawner = CreateActor(BROS_ICEBALL, 0, facePos, 0, currentLayerID);
				spawner->speed.x = 0.5;
				spawner->speed.y = 6.0;
				spawner->pos.z = 5550.0;
				spawner->x_speed_inc = 0.0f;
			}
		} // It's a moon
	}

	this->timer = this->timer + 1;

	if (this->timer > 31) { doStateChange(&StateID_Follow); }
}
void daBossSM_c::endState_Shower() { }


// Wait State

void daBossSM_c::beginState_Wait() {
	this->timer = 0;
	this->rot.x = 18000;
	this->rot.y = 0;
	this->rot.z = 0;

	//this->speed.x = 0.0;

	dStageActor_c *Player = GetSpecificPlayerActor(0);
	if (Player == 0) { Player = GetSpecificPlayerActor(1); }
	if (Player == 0) { Player = GetSpecificPlayerActor(2); }
	if (Player == 0) { Player = GetSpecificPlayerActor(3); }
	if (Player == 0) { doStateChange(&StateID_Follow); }
}
void daBossSM_c::executeState_Wait() { 
	int Choice;
	int TimerMax;
	
	this->HandleXSpeed();

	if ((this->settings & 0xF) == 0) { TimerMax = 60; } // It's a sun
	else { TimerMax = 30; } // It's a moon	
	
	if (this->timer > TimerMax) {

		Choice = GenerateRandomNumber(5);
		if (Choice != this->stackLatestAttack){
			if (Choice == 0) { doStateChange(&StateID_Spin); 
									this->stackLatestAttack = Choice;
									OSReport("=========[Spining begin!]=========\n");
									}
			else if (Choice == 1) { doStateChange(&StateID_Spit);
									this->stackLatestAttack = Choice;
									OSReport("=========[Spit begin!]=========\n");
									}
			else if (Choice == 2) { doStateChange(&StateID_Spit4);
									this->stackLatestAttack = Choice;
									OSReport("=========[Spit 4 begin!]=========\n");
									}
			else if (Choice == 3) { doStateChange(&StateID_SpawnKeronpa);
									this->stackLatestAttack = Choice;
									OSReport("=========[SpawnKeronpa begin!]=========\n");
			}
			else if (Choice == 4) { doStateChange(&StateID_Shower);
									this->stackLatestAttack = Choice;
									OSReport("=========[Shower begin!]=========\n");
									}
		}
	}

	this->timer = this->timer + 1;
}
void daBossSM_c::endState_Wait() {
	this->timer = 0;
}

// Grow State

void daBossSM_c::beginState_Grow() {
	this->timer = 0;
	this->rot.x = 18000;
	this->rot.y = 0;
	this->rot.z = 0;

	SetupKameckFromRightSide(this, Kameck);
	this->scale = (Vec){0.5, 0.5, 0.5};
}
void daBossSM_c::executeState_Grow() {
	this->timer += 1;
	PlaySound(this, SE_BOSS_IGGY_WANWAN_TO_L);
	bool ret;
	if ((this->settings & 0xF) == 0){
		ret = GrowBossForSMV(this, Kameck, 0.5, 2.0, 25, this->timer); //[base scale], [growed scale], [y pos]
	}
	else{
		ret = GrowBossForSMV(this, Kameck, 0.5, 2.0, 25, this->timer); //[base scale], [growed scale], [y pos]
		//ret = GrowBossForSMV(this, Kameck, 0.5, 0.0, 25, this->timer); //[base scale], [growed scale], [y pos]
	}

	if (ret) { doStateChange(&StateID_Follow);	}

}
void daBossSM_c::endState_Grow() {
	this->Baseline = this->pos.y;
	CleanupKameck(this, Kameck);
}

//Outro State

void daBossSM_c::beginState_Outro() {
	this->timer = 0;
	this->rot.x = 18000;
	this->rot.y = 0;
	this->rot.z = 0;
	
	// Delete other enemies
	dActor_c *iter = 0;
	while (iter = (dActor_c*)dActor_c::searchByBaseType(2, iter)) {
		dStageActor_c *sa = (dStageActor_c*)iter;

		// 削除するスプライト（Actor IDで指定）
		if (sa->name == KERONPA_FIRE ||
				sa->name == WAKI_KOKOOPA_FIRE ||
				sa->name == BROS_ICEBALL) {
			sa->Delete(1);
		}
		// スコアUP付き？
		if (sa->name == EN_KERONPA ||
				sa->name == EN_KARON ||
				sa->name == EN_KURIBO) {
			sa->killedByLevelClear();
			sa->Delete(1);
		}
	}

	OutroSetup(this);
}
void daBossSM_c::executeState_Outro(){
	// rotation
	short rotBonus;
	if (this->timer < 60) { rotBonus = this->timer; }
	else { rotBonus = 120 - this->timer; }

	this->rot.z = this->rot.z + (55.1 * rotBonus);
	this->rot.y = this->rot.y + (18.4 * rotBonus);
	
	// kill boss sun moon
	if (this->dying == 1) {
		this->Delete(1);
		PlaySound (this,1882); //STRM_BGM_SHIRO_BOSS_CLEAR
		if (this->timer == 60) { PlayerVictoryCries(this); }
	}

	bool ret;
	if ((this->settings & 0xF) == 0){
		ret = ShrinkBoss(this, &this->pos, 1.5, this->timer);
	}
	else {
		ret = ShrinkBoss(this, &this->pos, 2.0, this->timer);
	}

	if (ret == true) 	{
		BossExplode(this, &this->pos);
		this->dying = 1;
	}

	this->kill();
	this->timer += 1;
}
void daBossSM_c::endState_Outro() {	}