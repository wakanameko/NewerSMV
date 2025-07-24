#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include "Profile.h"


const char* NSLULSArcNameList [] = { "NSLULogoSign", NULL };

class daNSLULogoSign_c : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void updateModelMatrices();

	//void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	//void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	//void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	//bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther); 
	
	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c animationChr;
	nw4r::g3d::ResFile resFile;

	// Reggie configs
	int style;
	int layer;

	static dActor_c* build();
};


void daNSLULogoSign_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

dActor_c* daNSLULogoSign_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daNSLULogoSign_c));
	return new(buffer) daNSLULogoSign_c;
}

/////////////////////////////
// more sprites config
const SpriteData NSLULSSpriteData = { ProfileId::nsluLogoSign, 0, 0, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
// {Sprite's profileID, x pos offset, y pos offset, x spawn offset, y spawn offset, x size offset, y size offset, unk, unk, unk, unk, spawnflag 0}
Profile NSLULSProfile(&daNSLULogoSign_c::build, SpriteId::nsluLogoSign, &NSLULSSpriteData, ProfileId::nsluLogoSign, ProfileId::nsluLogoSign, "nsluLogoSign", NSLULSArcNameList, 0);

int daNSLULogoSign_c::onCreate() {
	// Reggie settings
	this->style = this->settings >> 28 & 0xF;	// 0000 "0"000 0000 0000	// nybble 5
	this->layer = this->settings >> 24 & 0xF;	// 0000 0"0"00 0000 0000	// nybble 6

	// model
	allocator.link(-1, GameHeaps[0], 0, 0x20);
	resFile.data = getResource("NSLULogoSign", "g3d/t00.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("NSLULogoSign");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Player(&bodyModel, 0);
	allocator.unlink(); 

	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;

	if (this->layer == 0){ this->pos.z = 3300.0; }
	else if (this->layer == 1){ this->pos.z = 0.0; }
	else if (this->layer == 2){ this->pos.z = -3300.0; }

	this->onExecute();

	return true;
}

int daNSLULogoSign_c::onExecute() {
	updateModelMatrices();
	bodyModel._vf1C();

	return true;
}

int daNSLULogoSign_c::onDelete() {
	return true;
}

int daNSLULogoSign_c::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}