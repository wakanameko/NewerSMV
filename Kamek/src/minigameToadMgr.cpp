#include <game.h>
#include <common.h>
#include <profile.h>
#include "messageWindow.h"

// by wakanameko

const char* toadArcNameList [] = { NULL };

class daMinigameToadMgr_c : public dEn_c {
public:
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	static dActor_c *build();

	enum textPatterns {
		FOR_DEBUG = 0,
		REGULAR_CollectStars = 1,
		REGULAR_MUSICHOUSE = 2,
		REGULAR_ITEMBOWLING = 3,
		SETTINGS_COUNT,
	};

	int houseType;
};

dActor_c *daMinigameToadMgr_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daMinigameToadMgr_c));
	return new(buffer) daMinigameToadMgr_c;
}

/////////////////////////////
// more sprites config
const SpriteData minigameToadMgrSpriteData = { ProfileId::minigameToadMgr, 0, 0, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
// {Sprite's profileID, x pos offset, y pos offset, x spawn offset, y spawn offset, x size offset, y size offset, unk, unk, unk, unk, spawnflag 0}
Profile minigameToadMgrProfile(&daMinigameToadMgr_c::build, SpriteId::minigameToadMgr, &minigameToadMgrSpriteData, ProfileId::minigameToadMgr, ProfileId::minigameToadMgr, "minigameToadMgr", toadArcNameList, 0);

int daMinigameToadMgr_c::onCreate() {
	this->houseType = this->settings >> 28 & 0xFF;	// 0000 "0"000 0000 0000	// nybble 5

	// for debug things
	int isDebug = this->settings >> 24 & 0xFF;		// 0000 0"0"00 0000 0000	// Nybble 6

	int textID1 = this->settings >> 16 & 0xFF;		// 0000 000"0" 0000 0000	// Nybble 8
	int textID2 = this->settings >> 20 & 0xFF;		// 0000 00"0"0 0000 0000	// Nybble 7
	int textID = (textID1) + (textID2 * 0x10);

	int category1 = this->settings >> 0 & 0xFF;		// 0000 0000 000"0" 0000	// nybble 12
	int category2 = this->settings >> 4 & 0xFF;		// 0000 0000 00"0"0 0000	// nybble 11
	int category3 = this->settings >> 8 & 0xFF;		// 0000 0000 0"0"00 0000	// nybble 10
	int category = (category1) + (category2 * 0x10) + (category3 * 0x100);

	// Call the message window
	if (this->houseType == FOR_DEBUG){
		dMessageWindow_c::instance->showMessageWindow(category, textID, isDebug);
	}
	if (this->houseType == REGULAR_CollectStars){
		dMessageWindow_c::instance->showMessageWindow(0x3ec, 0x0, 0);
	}

	return true;
}

int daMinigameToadMgr_c::onDelete() {
	return true;
}

int daMinigameToadMgr_c::onDraw() {
	return true;
}

int daMinigameToadMgr_c::onExecute() {
	return true;
}
