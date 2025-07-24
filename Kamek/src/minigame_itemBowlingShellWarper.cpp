#include <common.h>
#include <game.h>
#include <profile.h>
#include <sfx.h>


class dShellWarper_c : public dEn_c {
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	dShellWarper_c* boundWarp;

	bool isReceiver;
	u8 ID;
	u8 warpType;

	enum warpType{
		PIPE = 1,
	};

	u8 timer;



	public: static dActor_c *build();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);

};


void dShellWarper_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {}

bool dShellWarper_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool dShellWarper_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool dShellWarper_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool dShellWarper_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool dShellWarper_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool dShellWarper_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool dShellWarper_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	if(!isReceiver && boundWarp) {
		apOther->owner->pos = boundWarp->pos;
		apOther->owner->pos.y -= 8;
		apOther->owner->pos.x += 8;

		if (this->warpType == PIPE){
			MapSoundPlayer(SoundRelatedClass, SE_PLY_DOKAN_IN_OUT, 1);
		}
	}

	return false;
}
bool dShellWarper_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool dShellWarper_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool dShellWarper_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}



dActor_c *dShellWarper_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dShellWarper_c));
	return new(buffer) dShellWarper_c;
}

const SpriteData minigameItemBowlingShellData = {ProfileId::SHELLWARPER, 0, 0, 0, 0, 0x1000, 0x1000, 0, 0, 0, 0, 2}; 
Profile minigameItemBowlingShellProfile(&dShellWarper_c::build, SpriteId::SHELLWARPER, &minigameItemBowlingShellData, ProfileId::SHELLWARPER, ProfileId::SHELLWARPER, "ITEMBALLOON", NULL, 0);



int dShellWarper_c::onCreate() {


	ActivePhysics::Info HitMeBaby;
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 0.0;
	HitMeBaby.xDistToEdge = 7.5;
	HitMeBaby.yDistToEdge = 7.5;
	HitMeBaby.category1 = 0x5;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0xFFFFFFFF;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

	this->scale.x = 1.0;
	this->scale.y = 1.0;
	this->scale.z = 1.0;

	this->isReceiver = (this->settings >> 4) & 0x1;
	this->ID = this->settings & 0xF;
	this->warpType = (this->settings >> 8) & 0xF;

	this->onExecute();
	return true;
}


int dShellWarper_c::onDelete() {
	return true;
}

int dShellWarper_c::onDraw() {
	return true;
}

int dShellWarper_c::onExecute() {
	if(timer < 60) {
		timer++;
	}
	else if(timer == 60) { // 1 second timer so we are sure every other Shell Warper got loaded
		if(!isReceiver) {
			dShellWarper_c *warper = 0;
			while((warper = (dShellWarper_c*)fBase_c::search(SHELLWARPER, warper))) {
				if(warper->isReceiver && warper->ID == this->ID) {
					boundWarp = warper;
					break;
				}
			}
		}
		timer++;
	}

	return true;
}

