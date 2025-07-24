#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include <profile.h>


const char* PParcNameList [] = {
	"pakkun",
	// "pakkun_projectile",
	"env_mist",
	NULL
};

const char* PRarcNameList [] = {
	// "pakkun_projectile",
	NULL
};

enum PakkunProjectileType {
	PakkunNormal = 0,
	PakkunIce = 1,
	PakkunBony = 2,
};

class dPakkunProjectile_c : public dEn_c {
	public:
		int onCreate();
		int onDelete();
		int onExecute();
		int onDraw();

		mHeapAllocator_c allocator;
		nw4r::g3d::ResFile resFile;
		m3d::mdl_c bodyModel;
		m3d::anmChr_c animationChr;
		m3d::anmClr_c clrAnimation;
		nw4r::g3d::ResAnmTexPat anmPat;
		m3d::anmTexPat_c patAnimation;

		
		PakkunProjectileType projectile;
		bool hasPresence;
		float Baseline;

		lineSensor_s below; // Right position (from center), Left distance (from center), Bottom position (from center)
		lineSensor_s above; // Right position (from center), Left distance (from center), Top position (from center)
		lineSensor_s adjacent; // Bottom position (from center), Top distance (from center), Horizontal distance (/2) between the two lines (from center)

		static dActor_c *build();

		void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
		void setupBodyModel();
		void updateModelMatrices();

		void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
		void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

		bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
		bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
		bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
		bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
		bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
		bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
		bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
		bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);

		bool generateActorID(Vec speedDir);
		char* getModelName();
		Vec2 collisionDistToEdge();
		void setSensors();
		void deathParticle();
		bool shouldBeDeleted();

		USING_STATES(dPakkunProjectile_c);
		DECLARE_STATE(Wait);

		u16 timer;
		bool initialState;
		dStageActor_c *child;
};


dActor_c *dPakkunProjectile_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dPakkunProjectile_c));
	dPakkunProjectile_c *c = new(buffer) dPakkunProjectile_c;

	return c;
}


// const SpriteData PakkunProjectileSpriteData = { ProfileId::PakkunProjectile, 0, 0, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile PakkunProjectileProfile(&dPakkunProjectile_c::build, ProfileId::PakkunProjectile, NULL, ProfileId::PakkunProjectile, ProfileId::PakkunProjectile, "Projectiles", PRarcNameList);



CREATE_STATE(dPakkunProjectile_c, Wait);

extern "C" void dAcPy_vf3F4(void* mario, void* other, int t);
extern "C" bool BigHanaFireball(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);
extern "C" void *dAcPy_c__ChangePowerupWithAnimation(void * Player, int powerup);
extern "C" int CheckExistingPowerup(void * Player);
extern "C" void *EN_LandbarrelPlayerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);
extern "C" char usedForDeterminingStatePress_or_playerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther, int unk1);

void dPakkunProjectile_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) { 
	this->_vf220(apOther->owner);
}
void dPakkunProjectile_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) { this->playerCollision(apThis, apOther); }
bool dPakkunProjectile_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool dPakkunProjectile_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->_vf220(apOther->owner);
	return true;
}
bool dPakkunProjectile_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->_vf220(apOther->owner);
	return true;
}
bool dPakkunProjectile_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool dPakkunProjectile_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->Delete(1);
	return true;
}

bool dPakkunProjectile_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool dPakkunProjectile_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool dPakkunProjectile_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->_vf220(apOther->owner);
	return true;
}




void dPakkunProjectile_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}



bool dPakkunProjectile_c::generateActorID(Vec speedDir) {
	dStageActor_c *proj;

	switch(this->projectile) {
		case PakkunNormal:
			proj = CreateActor(PAKKUN_FIREBALL, this->settings, this->pos, 0, currentLayerID);
			proj->speed.x = speedDir.x;
			proj->speed.y = speedDir.y;
			proj->x_speed_inc = 0.0f;
			proj->y_speed_inc = 0.0f;
			return true;

		case PakkunIce:
			proj = CreateActor(BROS_ICEBALL, 0, this->pos, 0, currentLayerID);
			proj->aPhysics.removeFromList();
			proj->speed.x = speedDir.x;
			proj->speed.y = speedDir.y;
			this->timer = 30;
			proj->x_speed_inc = 0.0f;
			proj->y_speed_inc = 0.0f;
			this->child = proj;
			return true;

		case PakkunBony:
			proj = CreateActor(BROS_ICEBALL, 0, this->pos, 0, currentLayerID);
			proj->aPhysics.removeFromList();
			proj->speed.x = speedDir.x;
			proj->speed.y = speedDir.y;
			this->timer = 30;
			proj->x_speed_inc = 0.0f;
			proj->y_speed_inc = 0.0f;
			this->child = proj;
			return true;
	}
	return true;
}


char* dPakkunProjectile_c::getModelName() {
	switch(this->projectile) {
		default:
			break;
	}
	return "INT";
}

Vec2 dPakkunProjectile_c::collisionDistToEdge() {
	switch(this->projectile) {
		default:
			break;
	}
	return (Vec2){8.0, 8.0};
}

void dPakkunProjectile_c::setSensors() {
	u32 flags;
	Vec2 d;
	s32 dx, dy;

	bool initCollMgr = false;

	switch (this->projectile) {
		default:
			break;
	}

	if (initCollMgr) collMgr.init(this, &this->below, &this->above, &this->adjacent);
}


void dPakkunProjectile_c::deathParticle() {
	S16Vec nullRot = {0,0,0};
	Vec2 d = collisionDistToEdge();
	float size = max(d.x, d.y) / 16.0;
	Vec oneVec = {size, size, size};

	const char* effectName = "Wm_mr_hardhit";
	SpawnEffect(effectName, 0, &pos, &nullRot, &oneVec);
}


void dPakkunProjectile_c::setupBodyModel() {
	// allocator.link(-1, GameHeaps[0], 0, 0x20);

	// this->resFile.data = getResource("pakkun_projectile", "g3d/pakkun_projectile.brres");
	// nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl(getModelName());
	// bodyModel.setup(mdl, &allocator, 0x32F, 1, 0);
	// SetupTextures_Enemy(&bodyModel, 0);

	// bool ret;
	// nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("pakkun_projectile");
	// ret = this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	// allocator.unlink();
}


int dPakkunProjectile_c::onCreate() {
	this->projectile = PakkunProjectileType(this->settings >> 24 & 0xFF);	// 0000 0"0"00 0000 0000	// nybble 6

	float speed = 1.0f;
	this->timer = 0;

	int players = GetActivePlayerCount();
	Vec target;
	float dist;
	float distSquared;

	for (int i = 0; i < players; i++) {
		dAcPy_c *player = (dAcPy_c*)GetSpecificPlayerActor(i);
		if (i == 0) {
			target = player->pos;
			// dist = sqrtf((this->pos.x - player->pos.x) * (this->pos.x - player->pos.x) + (this->pos.y - player->pos.y) * (this->pos.y - player->pos.y));
			distSquared = (this->pos.x - player->pos.x) * (this->pos.x - player->pos.x) +
			        	    (this->pos.y - player->pos.y) * (this->pos.y - player->pos.y);	// This project cannot use sqrtf

		}
		else {
			// float distance = sqrtf((player->pos.x - this->pos.x) * (player->pos.x - this->pos.x) + (player->pos.y - this->pos.y) * (player->pos.y - this->pos.y));
			float distanceSquared = (player->pos.x - this->pos.x) * (player->pos.x - this->pos.x) +
			                	    (player->pos.y - this->pos.y) * (player->pos.y - this->pos.y);	// This project cannot use sqrtf
			if (distanceSquared < dist) {
				target = player->pos;
				dist = distanceSquared;
			}
		}
	}

	Vec speedDir = (Vec){target.x - this->pos.x, target.y - this->pos.y, 0.0f};
	float magnitude = sqrtf(speedDir.x * speedDir.x + speedDir.y * speedDir.y);

	speedDir.x /= magnitude;
	speedDir.y /= magnitude;


	if (speedDir.y > 0.687500) { // is in top deadzone
		speedDir.y = 0.687500;
		if (speedDir.x < 0) speedDir.x = -0.687500;
		else speedDir.x = 0.687500;
	}
	else if (speedDir.y < -0.687500) { // is in bottom deadzone
		speedDir.y = -0.687500;
		if (speedDir.x < 0) speedDir.x = -0.687500;
		else speedDir.x = 0.687500;
	}

	speedDir.x *= speed;
	speedDir.y *= speed;


	this->hasPresence = true;

	char* s = getModelName();

	if (s != "INT") {
		setupBodyModel();
	}
	else {
		if (generateActorID(speedDir)) {
			if (this->timer == 0) {
				this->Delete(1);
				return false;
			}
		}
		this->hasPresence = false;
	}


	this->scale = (Vec){1.0, 1.0, 1.0};
	this->rot.x = 0; // X is vertical axis
	this->rot.y = 0xD800; // Y is horizontal axis
	this->rot.z = 0; // Z is ... an axis >.>
	this->direction = 1; // Heading left.

	if (this->hasPresence) {
		this->speed.x = speedDir.x;
		this->speed.y = speedDir.y;
	}
	else {
		this->speed.x = 0.0;
		this->speed.y = 0.0;
	}
	this->x_speed_inc = 0.0f;
	this->y_speed_inc = 0.0f;
	this->Baseline = this->pos.y;

	ActivePhysics::Info HitMeBaby;
	
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 0.0;

	Vec2 collSize = collisionDistToEdge();

	HitMeBaby.xDistToEdge = collSize.x;
	HitMeBaby.yDistToEdge = collSize.y;

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0x8028C;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	if (this->hasPresence) {
		switch (this->projectile) {
			default:
				this->aPhysics.addToList();
		}

		setSensors();
		// bindAnimChr_and_setUpdateRate("pakkun_projectile", 1, 0.0, 1.0);
	}

	doStateChange(&StateID_Wait);

	this->onExecute();
	return true;
}

int dPakkunProjectile_c::onDelete() {
	return true;
}

int dPakkunProjectile_c::onExecute() {
	acState.execute();
	updateModelMatrices();

	HandleXSpeed();
	HandleYSpeed();
	doSpriteMovement();

	if (this->hasPresence) {
		collMgr.calculateBelowCollisionWithSmokeEffect();
		collMgr.calculateAboveCollision(0);
		collMgr.calculateAdjacentCollision();

		// if(this->animationChr.isAnimationDone()) {
		// 	this->animationChr.setCurrentFrame(0.0);
		// }
	}

	return true;
}


int dPakkunProjectile_c::onDraw() {
	if (!this->hasPresence) return true;
	bodyModel.scheduleForDrawing();
	bodyModel._vf1C();
	return true;
}


void dPakkunProjectile_c::updateModelMatrices() {
	if (!this->hasPresence) return;
	// This won't work with wrap because I'm lazy.
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}



// Wait State

void dPakkunProjectile_c::beginState_Wait() { }

void dPakkunProjectile_c::executeState_Wait() {
	// if (this->hasPresence) {
	// 	if (this->animationChr.isAnimationDone()) {
	// 		this->animationChr.setCurrentFrame(0.0);
	// 	}
	// }

	if (this->timer > 0 && (this->projectile == PakkunIce || this->projectile == PakkunBony)) {
		this->timer--;
		if (this->timer == 0) {
			if (this->child != 0) this->child->aPhysics.addToList();
			this->Delete(1);
		}
	}
}

void dPakkunProjectile_c::endState_Wait() { }
