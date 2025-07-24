#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>

const char* SMWTogezoArcNameList [] = {
	"togezo",
	NULL
};

class daSMWTogezo_c : public dEn_c {
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;

	m3d::mdl_c model;

	m3d::anmChr_c chrAnimation;

	mEf::es2 effect;

	float XSpeed;
	u32 cmgr_returnValue;
	bool isBouncing;
	bool stillFalling;

	bool enableGravity;
	bool liquid;

	//int nowPressed = Remocon_GetPressed(GetActiveRemocon());

	// StandOnTopCollider giantRider;

	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void updateModelMatrices();
	bool calculateTileCollisions();
	bool isInLiquid();

	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	// bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);
	// bool collisionCatB_SpinJump(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther);

	void _vf148();
	void _vf14C();
	bool CreateIceActors();

	bool willWalkOntoSuitableGround();

	USING_STATES(daSMWTogezo_c);
	DECLARE_STATE(Walk);
	DECLARE_STATE(Turn);
	DECLARE_STATE(DieStomp);

	//public: static dActor_c *build();
	static daSMWTogezo_c *build();
};

//dActor_c *daSMWTogezo_c::build() {
daSMWTogezo_c *daSMWTogezo_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daSMWTogezo_c));
	return new(buffer) daSMWTogezo_c;
}

//const SpriteData SMWTogezoSpriteData = {ProfileId::SMWTogezo, 8, -8 , 0 , 0, 0x100, 0x100, 0, 0, 0, 0, 0};
//Profile SMWTogezoProfile(&daSMWTogezo_c::build, SpriteId::SMWTogezo, &SMWTogezoSpriteData, ProfileId::SMWTogezo, ProfileId::SMWTogezo, "SMWTogezo", SMWTogezoArcNameList, 0);

///////////////////////
// Externs and States
///////////////////////
extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);

//FIXME make this dEn_c->used...
extern "C" char usedForDeterminingStatePress_or_playerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther, int unk1);
extern "C" int SmoothRotation(short* rot, u16 amt, int unk2);


CREATE_STATE(daSMWTogezo_c, Walk);
CREATE_STATE(daSMWTogezo_c, Turn);
CREATE_STATE(daSMWTogezo_c, DieStomp);

////////////////////////
// Collision Functions
////////////////////////
void daSMWTogezo_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	u16 name = ((dEn_c*)apOther->owner)->name;

	if (name == EN_COIN || name == EN_EATCOIN || name == AC_BLOCK_COIN || name == EN_COIN_JUGEM || name == EN_COIN_ANGLE
		|| name == EN_COIN_JUMP || name == EN_COIN_FLOOR || name == EN_COIN_VOLT || name == EN_COIN_WIND
		|| name == EN_BLUE_COIN || name == EN_COIN_WATER || name == EN_REDCOIN || name == EN_GREENCOIN
		|| /*name == EN_JUMPDAI ||*/ name == EN_ITEM)
		{ return; }

	//switch ((dEn_c*)apOther->owner) {
		if (name == EN_JUMPDAI && stillFalling == true){
			//if (!this->enableGravity) break;

			u8* jumpdaiTimer = (u8*)((u32)(this) + 0x632);

			if(*jumpdaiTimer == 0 && this->speed.y < 0.0f) {
				*jumpdaiTimer = 0x11;
				PlaySound(this, SE_PLY_JUMPDAI);
			}
			else if(*jumpdaiTimer == 8) {
				if (!this->isInLiquid()) this->speed.y = 6.0f;
				else this->speed.y = 5.0f;
			}
			//break;
		}
	//}

	if (acState.getCurrentState() == &StateID_Walk) {

		pos.x = ((pos.x - ((dEn_c*)apOther->owner)->pos.x) > 0) ? pos.x + 1.5 : pos.x - 1.5;
		// pos.x = direction ? pos.x + 1.5 : pos.x - 1.5;
		doStateChange(&StateID_Turn);
	}

	//dEn_c::spriteCollision(apThis, apOther);
}

void daSMWTogezo_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	char hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 2);

	if (hitType == 1) {	// regular jump
		this->_vf220(apOther->owner);
	}
	else if (hitType == 3) { // spin jump
		PlaySound(this, SE_EMY_YOSHI_STEP);
		//if ((GetActiveRemocon()->heldButtons == WPAD_TWO) || (GetActiveRemocon()->heldButtons == WPAD_A)){
		//	bouncePlayer(this, 3.0);
		//}
		//else{
			bouncePlayer(this, 1.0);
		//}
	}
	else if (hitType == 0) { // sides
		//this->daSMWTogezo_c::playerCollision(apThis, apOther);
		this->_vf220(apOther->owner);
	}
	else if (hitType == 2) { // mini mario
		this->_vf220(apOther->owner);
	}
}

void daSMWTogezo_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	char hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 2);

	if (hitType == 1 || hitType == 2 || hitType == 3) {	// regular jump or mini mario
		PlaySound(this, SE_EMY_YOSHI_STEP);
		bouncePlayer(this, 3.0);
	}
	else if (hitType == 0) { // sides
		this->daSMWTogezo_c::playerCollision(apThis, apOther);
		this->_vf220(apOther->owner);
	}
}
bool daSMWTogezo_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->_vf220(apOther->owner);
	return true;
}
bool daSMWTogezo_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->_vf220(apOther->owner);
	return true;
}
bool daSMWTogezo_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCat7_GroundPound(apThis, apOther);
}
bool daSMWTogezo_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	PlaySound(this, SE_EMY_DOWN);
	S16Vec nullRot = {0,0,0};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
	SpawnEffect("Wm_mr_hardhit", 0, &pos, &nullRot, &oneVec);
	dEn_c::_vf148();
	return true;
}
bool daSMWTogezo_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->_vf220(apOther->owner);
	return true;
}
//bool collisionCatB_SpinJump(ActivePhysics *apThis, ActivePhysics *apOther) {return false;}
bool daSMWTogezo_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->_vf220(apOther->owner);
	return true;
}
bool daSMWTogezo_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {
	PlaySound(this, SE_EMY_DOWN);
	S16Vec nullRot = {0,0,0};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
	SpawnEffect("Wm_mr_hardhit", 0, &pos, &nullRot, &oneVec);
	dEn_c::_vf148();
	return true;
}
bool daSMWTogezo_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	StageE4::instance->spawnCoinJump(pos, 0, 1, 0);
	PlaySound(this, SE_EMY_DOWN);
	S16Vec nullRot = {0,0,0};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
	SpawnEffect("Wm_mr_hardhit", 0, &pos, &nullRot, &oneVec);
	dEn_c::_vf148();
	return true;
}

bool daSMWTogezo_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	bool wut = dEn_c::collisionCat3_StarPower(apThis, apOther);
	return wut;
}

bool daSMWTogezo_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCat1_Fireball_E_Explosion(apThis, apOther);
}

bool daSMWTogezo_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	StageE4::instance->spawnCoinJump(pos, 0, 1, 0);

	PlaySound(this, SE_EMY_DOWN);
	S16Vec nullRot = {0,0,0};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
	SpawnEffect("Wm_mr_hardhit", 0, &pos, &nullRot, &oneVec);
	dEn_c::_vf148();
	return true;
}

// void daSMWTogezo_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	
// 	dEn_C::collisionCat2_IceBall_15_YoshiIce(apThis, apOther);
// }

// These handle the ice crap
void daSMWTogezo_c::_vf148() {
	dEn_c::_vf148();
}
void daSMWTogezo_c::_vf14C() {
	dEn_c::_vf14C();
}

extern "C" void sub_80024C20(void);
extern "C" void __destroy_arr(void*, void(*)(void), int, int);
//extern "C" __destroy_arr(struct DoSomethingCool, void(*)(void), int cnt, int bar);

bool daSMWTogezo_c::CreateIceActors() {
	struct DoSomethingCool my_struct = { 0, this->pos, {1.25, 1.25, 1.25}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	this->frzMgr.Create_ICEACTORs( (void*)&my_struct, 1 );
	__destroy_arr( (void*)&my_struct, sub_80024C20, 0x3C, 1 );
	this->chrAnimation.setUpdateRate(0.0f);
	return true;
}

bool daSMWTogezo_c::calculateTileCollisions() {
	// Returns true if sprite should turn, false if not.

	HandleXSpeed();
	HandleYSpeed();
	doSpriteMovement();

	cmgr_returnValue = collMgr.isOnTopOfTile();
	collMgr.calculateBelowCollisionWithSmokeEffect();

	if (isBouncing) {
		stuffRelatingToCollisions(0.1875f, 1.0f, 0.5f);
		if (speed.y != 0.0f)
			isBouncing = false;
	}

	float xDelta = pos.x - last_pos.x;
	if (xDelta >= 0.0f)
		direction = 0;
	else
		direction = 1;

	if (collMgr.isOnTopOfTile()) {
		// Walking into a tile branch

		if (cmgr_returnValue == 0)
			isBouncing = true;

		if (speed.x != 0.0f) {
			//playWmEnIronEffect();
		}

		speed.y = 0.0f;

		max_speed.x = (direction == 1) ? -XSpeed : XSpeed;
	} else {
		x_speed_inc = 0.0f;
	}

	// Bouncing checks
	if (_34A & 4) {
		Vec v = (Vec){0.0f, 1.0f, 0.0f};
		collMgr.pSpeed = &v;

		if (collMgr.calculateAboveCollision(collMgr.outputMaybe))
			speed.y = 0.0f;

		collMgr.pSpeed = &speed;

	} else {
		if (collMgr.calculateAboveCollision(collMgr.outputMaybe))
			speed.y = 0.0f;
	}

	collMgr.calculateAdjacentCollision(0);

	// Switch Direction
	if (collMgr.outputMaybe & (0x15 << direction)) {
		if (collMgr.isOnTopOfTile()) {
			isBouncing = true;
		}
		return true;
	}
	return false;
}

void daSMWTogezo_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->chrAnimation.bind(&this->model, anmChr, unk);
	this->model.bindAnim(&this->chrAnimation, unk2);
	this->chrAnimation.setUpdateRate(rate);
}

int daSMWTogezo_c::onCreate() {
	this->stillFalling = 0;
	int color = this->settings >> 24 & 0xF;

	allocator.link(-1, GameHeaps[0], 0, 0x20);

	char resName[16];
	sprintf(resName, "g3d/t%02d.brres", color);
	resName[strlen(resName)] = 0;
	this->resFile.data = getResource("togezo", resName);

	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("togezo");
	model.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&model, 0);


	// Animations start here
	bool ret;
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("walk");
	ret = this->chrAnimation.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink();

	// Stuff I do understand

	this->scale = (Vec){1.0, 1.0, 1.0};

	this->rot.x = 0; // X is vertical axis
	this->rot.y = 0xD800; // Y is horizontal axis
	this->rot.z = 0; // Z is ... an axis >.>
	this->direction = 1; // Heading left.
	
	this->speed.x = 0.0;
	this->speed.y = 0.0;
	this->max_speed.x = 0.5;
	this->x_speed_inc = 0.15;
	this->XSpeed = 0.5;

    this->liquid = 0;



	if (this->enableGravity) {
		this->max_speed.y = -4.0;
		this->speed.y = -4.0;
		this->y_speed_inc = -0.1875;
	} else {
		this->max_speed.y = 0.0;
		this->speed.y = 0.0;
		this->y_speed_inc = 0.0;
	}


	ActivePhysics::Info HitMeBaby;

	// Note: if this gets changed, also change the point where the default
	// values are assigned after de-ballooning
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 8.0;
	HitMeBaby.xDistToEdge = 8.0;
	HitMeBaby.yDistToEdge = 8.0;

	HitMeBaby.category1 = 0x5;			//tested values ->3,5
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;			//tested values ->6F,4F
	HitMeBaby.bitfield2 = 0xFFFFFFFF;	//tested values ->0xffbafffe,0xFFFFFFFF
	HitMeBaby.unkShort1C = 0;
	//HitMeBaby.callback = &dEn_c::collisionCallback;
	HitMeBaby.callback = daSMWTogezo_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();


	// Tile collider

	// These fucking rects do something for the tile rect
	spriteSomeRectX = 28.0f;
	spriteSomeRectY = 32.0f;
	_320 = 0.0f;
	_324 = 16.0f;

	// These structs tell stupid collider what to collide with - these are from koopa troopa
	static const lineSensor_s below(-5<<12, 5<<12, 0<<12);
	static const pointSensor_s above(0<<12, 12<<12);
	static const lineSensor_s adjacent(6<<12, 9<<12, 6<<12);

	collMgr.init(this, &below, &above, &adjacent);
	collMgr.calculateBelowCollisionWithSmokeEffect();

	cmgr_returnValue = collMgr.isOnTopOfTile();

	if (collMgr.isOnTopOfTile())
		isBouncing = false;
	else
		isBouncing = true;

	bindAnimChr_and_setUpdateRate("walk", 1, 0.0, 1.0);
	doStateChange(&StateID_Walk);

	this->onExecute();
	return true;
}

int daSMWTogezo_c::onDelete() {
	return true;
}

int daSMWTogezo_c::onExecute() {
	acState.execute();
	updateModelMatrices();
	model._vf1C();
	
	return true;
}

int daSMWTogezo_c::onDraw() {
	model.scheduleForDrawing();
	return true;
}

bool daSMWTogezo_c::isInLiquid() {
	return this->liquid != 0;
}

void daSMWTogezo_c::updateModelMatrices() {
	// This won't work with wrap because I'm lazy.

	if (this->frzMgr._mstate == 1)
		matrix.translation(pos.x, pos.y + 2.0, pos.z);
	else
		matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	model.setDrawMatrix(matrix);
	model.setScale(&scale);
	model.calcWorld(false);
}

///////////////
// Real Walk State
///////////////
bool daSMWTogezo_c::willWalkOntoSuitableGround() {
	static const float deltas[] = {1.5f, -1.5f};
	VEC3 checkWhere = {
			pos.x + deltas[direction],
			4.0f + pos.y,
			pos.z
		};

	u32 props = collMgr.getTileBehaviour2At(checkWhere.x, checkWhere.y, currentLayerID);

	//if (getSubType(props) == B_SUB_LEDGE)
	if (((props >> 16) & 0xFF) == 8)
		return false;

	float someFloat = 0.0f;
	if (collMgr.sub_800757B0(&checkWhere, &someFloat, currentLayerID, 1, -1)) {
		if (someFloat < checkWhere.y && someFloat > (pos.y - 5.0f))
			return true;
	}

	return false;
}


void daSMWTogezo_c::beginState_Walk() {
	this->max_speed.x = (this->direction) ? -this->XSpeed : this->XSpeed;
	this->speed.x = (direction) ? -0.5f : 0.5f;

	this->max_speed.y = -4.0;
	this->speed.y = -4.0;
	this->y_speed_inc = -0.1875;

	this->chrAnimation.setUpdateRate(2.0);
}
void daSMWTogezo_c::executeState_Walk() {
	if (collMgr.isOnTopOfTile()) {
		stillFalling = false;

		//if (!willWalkOntoSuitableGround()) {
		//	pos.x = direction ? pos.x + 1.5 : pos.x - 1.5;
		//	doStateChange(&StateID_Turn);
		//}
	}


	bool ret = calculateTileCollisions();
	if (ret) {
		doStateChange(&StateID_Turn);
	}

	if (this->chrAnimation.isAnimationDone()) {
		this->chrAnimation.setCurrentFrame(0.0);
	}
}
void daSMWTogezo_c::endState_Walk() { }


///////////////
// Turn State
///////////////
void daSMWTogezo_c::beginState_Turn() {

	this->direction ^= 1;
	this->speed.x = 0.0;
}
void daSMWTogezo_c::executeState_Turn() {

	if (this->chrAnimation.isAnimationDone()) {
		this->chrAnimation.setCurrentFrame(0.0);
	}

	u16 amt = (this->direction == 0) ? 0x2800 : 0xD800;
	int done = SmoothRotation(&this->rot.y, amt, 0x800);

	if(done) {
		this->doStateChange(&StateID_Walk);
	}
}
void daSMWTogezo_c::endState_Turn() { }

///////////////
// Die by Stomp State (No means)
///////////////
void daSMWTogezo_c::beginState_DieStomp() {
	this->removeMyActivePhysics();

	bindAnimChr_and_setUpdateRate("damage", 1, 0.0, 1.0);
	//this->rot.y = 0;
	//this->rot.x = 0;
}
void daSMWTogezo_c::executeState_DieStomp() {
	if(this->chrAnimation.isAnimationDone()) {
		this->Delete(true);
	}
}
void daSMWTogezo_c::endState_DieStomp() { }
