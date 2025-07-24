// Based arrowSignboard of NSMBW-TPC by Asu-chan.

// unused

#include <common.h>
#include <game.h>
#include <profile.h>
#include "talkBalloon.h"

const char* TalkFlowerFileList[] = {"oshaberiFlower", 0};

class daOshaberiFlower_c : public dActorState_c {
public:
	int onCreate();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	m3d::mdl_c model;
	m3d::anmChr_c animationChr;

	float animFrame;
	u32 unk; // Unused?

	bool showingTB;

	static dActor_c* build();

	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

	void updateModelMatrices();
	void playerCollision(ActivePhysics* apThis, ActivePhysics* apOther);
	void yoshiCollision(ActivePhysics* apThis, ActivePhysics* apOther);

	bool collisionCat7_GroundPound(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCatD_Drill(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCatA_PenguinMario(ActivePhysics* apThis, ActivePhysics* apOther);

	bool collisionCat1_Fireball_E_Explosion(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat9_RollingObject(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat13_Hammer(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat14_YoshiFire(ActivePhysics* apThis, ActivePhysics* apOther);
	bool collisionCat3_StarPower(ActivePhysics* apThis, ActivePhysics* apOther);

	void executeState_Wait();
};


const SpriteData OshaberiFlowerpriteData = { ProfileId::OshaberiFlower, 0, 0 , 0 , 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile OshaberiFlowerProfile(&daOshaberiFlower_c::build, SpriteId::OshaberiFlower, &OshaberiFlowerSpriteData, ProfileId::OshaberiFlower, ProfileId::OshaberiFlower, "OshaberiFlower", TalkFlowerFileList, 0);

// Coligion settings
void daOshaberiFlower::playerCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
}

void daOshaberiFlower::yoshiCollision(ActivePhysics* apThis, ActivePhysics* apOther) {
}

bool daOshaberiFlower::collisionCat7_GroundPound(ActivePhysics* apThis, ActivePhysics* apOther) {
	return true;
}
bool daOshaberiFlower::collisionCat7_GroundPoundYoshi(ActivePhysics* apThis, ActivePhysics* apOther) {
	return true;
}
bool daOshaberiFlower::collisionCatD_Drill(ActivePhysics* apThis, ActivePhysics* apOther) {
	return true;
}
bool daOshaberiFlower::collisionCatA_PenguinMario(ActivePhysics* apThis, ActivePhysics* apOther) {
	return true;
}
bool daOshaberiFlower::collisionCat1_Fireball_E_Explosion(ActivePhysics* apThis, ActivePhysics* apOther) {
	return false;
}
bool daOshaberiFlower::collisionCat2_IceBall_15_YoshiIce(ActivePhysics* apThis, ActivePhysics* apOther) {
	return false;
}
bool daOshaberiFlower::collisionCat9_RollingObject(ActivePhysics* apThis, ActivePhysics* apOther) {
	collect(true);
	return true;
}
bool daOshaberiFlower::collisionCat13_Hammer(ActivePhysics* apThis, ActivePhysics* apOther) {
	return false;
}
bool daOshaberiFlower::collisionCat14_YoshiFire(ActivePhysics* apThis, ActivePhysics* apOther) {
	return false;
}
bool daOshaberiFlower::collisionCat3_StarPower(ActivePhysics* apThis, ActivePhysics* apOther) {
	collect(false);
	return true;
}

dActor_c* daOshaberiFlower_c::build() {
	void* buffer = AllocFromGameHeap1(sizeof(daOshaberiFlower_c));
	return new(buffer) daOshaberiFlower_c;

	return c;
}



extern int getNybbleValue(u32 settings, int fromNybble, int toNybble);

void daOshaberiFlower_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->chrAnimation.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->chrAnimation, unk2);
	this->chrAnimation.setUpdateRate(rate);
}

int daOshaberiFlower_c::onCreate() {
	//this->type = this->settings >> 28 & 0xF;

	this->deleteForever = false;


	this->counter = 0;


	// Model creation
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("oshaberiFlower", "g3d/oshaberiFlower.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("oshaberiFlower");
	bodyModel.setup(mdl, &allocator, 0x128, 1, 0);
	SetupTextures_Item(&bodyModel, 0);

	anmClr = this->resFile.GetResAnmClr("oshaberiFlower");
	this->clrAnimation.setup(mdl, anmClr, &this->allocator, 0, 1);
	this->clrAnimation.bind(&this->bodyModel, anmClr, 0, 0);
	this->bodyModel.bindAnim(&this->clrAnimation);
	this->clrAnimation.setFrameForEntry(79.0f, 0);
	this->clrAnimation.setUpdateRateForEntry(1.0f, 0);

	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("wait");
	this->chrAnimation.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink();
/*
	ActivePhysics::Info HitMeBaby;

	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 0.0;

	HitMeBaby.xDistToEdge = 7.5;
	HitMeBaby.yDistToEdge = 7.5;

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0x8028E;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;
*/
	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

	this->disableEatIn();

	// Stuff I do understand
	this->scale = (Vec){2.0f, 2.0f, 2.0f};

	this->rot.x = 0;
	this->rot.y = 0;
	this->rot.z = 0;

	//this->pos.y -= 4;
	this->pos.z = 0;

	doStateChange(&StateID_Wait);

	bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 0.6);

	this->onExecute();
	return true;
}

int daOshaberiFlower_c::onDraw() {
	bodyModel.scheduleForDrawing();
	return true;
}


void daOshaberiFlower_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y - 1, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

int daOshaberiFlower_c::onExecute() {
}


void daOshaberiFlower_c::executeState_Wait() {
	u8 textID = (this->settings >> 28) & 0xF; // Nybble 5
	if(textID) {
		bool doIt = false;
		for(int i = 0; i < 4; i++) {
			if(daPlBase_c* player = GetPlayerOrYoshi(i)) {
				if(player->pos.x > this->pos.x - 48.0f && player->pos.x < this->pos.x + 48.0f
				&& player->pos.y > this->pos.y - 48.0f && player->pos.y < this->pos.y + 48.0f) {
					doIt = true;
				}
			}
		}

		if(doIt && !showingTB) {
			dTalkBalloon_c::instance->showTB((int[]){0xD, 0xE, 0xF, 0x10, 0x11, 0x12}[textID-1], this->pos);
			showingTB = true;
		}
		else if(!doIt && showingTB) {
			dTalkBalloon_c::instance->hideTB();
			showingTB = false;
		}
	}
}

