#ifndef __SINGALONG_H
#define __SINGALONG_H

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <playerAnim.h>
#include <stage.h>
#include <profile.h>
#include "minigame.h"

// Controls the tempo for the songs.
// Lower numbers are faster, by a ratio factor.

int Tempo[16] = {13,11,12,12,
				 8,8,7,15,
				 15,8,10,16,
				 12,10,8,8};

int Songs[16][4][16][3] = {

	// 15

 	// First number is the block, Second is the note/sfx, Third is timing: 30 is one quarter note at 120 bpm, 0,0 ends the sequence

	// Some possibles
	// SMW Donut Plains
	// SMW Forest of Illusion
	// Paper Mario Battle Theme
	// Thwomp Volcano: PiT
	// SMRPG Forest?
	// SMRPG Nimbus Land?
	// Teehee Valley: Superstar Saga
	// SML Theme
	// Mario Sunshine Main Theme
	// Mario Kart DS Theme

	{ // Song 1 - Super Mario Bros Melody |***  |
		{{3,17,0},{3,17,2},{3,17,6},{1,13,10},{3,17,12},{5,20,16},{0,0,0}},
		{{1,13,0},{5,8,3},{3,5,6},{6,10,9},{7,12,11},{6,11,13},{6,10,14},{0,0,0}},
		{{1,13,0},{1,13,1},{1,13,3},{1,13,5},{2,15,6},{3,17,8},{1,13,9},{7,10,11},{6,8,12},{0,0,0}},
		{{1,13,0},{1,13,1},{1,13,3},{1,13,5},{2,15,6},{3,17,7},{0,0,0}}
	},

	{ // Song 2 - Super Mario Bros. Underground |**** |
		{{7,12,0},{7,24,2},{5,9,4},{5,21,6},{6,10,8},{6,22,10},{0,0,0}},
		{{3,5,0},{3,17,2},{1,2,4},{1,14,6},{2,3,8},{2,15,10},{0,0,0}},
		{{2,15,0},{1,14,1},{1,13,2},{7,12,3},{2,15,5},{1,14,7},{5,8,9},{4,7,11},{1,13,13},{0,0,0}},
		{{7,12,0},{4,18,1},{3,17,2},{2,16,3},{6,22,4},{5,21,5},{4,20,6},{2,15,10},{6,11,12},{6,10,14},{5,9,16},{5,8,18},{0,0,0}}
	},

	{ // Song 3 - SMB Starman |**   |
		{{4,6,0},{4,6,2},{4,6,4},{2,3,6},{4,6,7},{4,6,9},{2,3,11},{4,6,12},{2,3,13},{4,6,14},{0,0,0}},
		{{3,5,0},{3,5,2},{3,5,4},{1,1,6},{3,5,7},{3,5,9},{1,1,11},{3,5,12},{1,1,13},{3,5,14},{0,0,0}},
		{{4,18,0},{4,18,2},{4,18,4},{2,15,6},{4,18,7},{4,18,9},{2,15,11},{4,18,12},{2,15,13},{4,18,14},{0,0,0}},
		{{3,17,0},{3,17,2},{3,17,4},{1,13,6},{3,17,7},{3,17,9},{1,13,11},{3,17,12},{1,13,13},{3,17,14},{0,0,0}}
	},

	{ // Song 4 - SMB3 Clouds |*    |
		{{1,8,0},{6,20,4},{1,8,12},{2,9,16},{7,21,20},{0,0,0}},
		{{6,23,0},{5,20,4},{1,8,8},{3,17,16},{4,18,20},{1,14,24},{6,11,32},{0,0,0}},
		{{1,8,0},{6,20,4},{1,8,12},{2,9,16},{7,21,20},{0,0,0}},
		{{6,23,0},{5,20,4},{1,8,8},{3,17,16},{4,18,20},{1,14,24},{6,11,32},{0,0,0}}
	},

	{ // Song 5 - SMB3 Hammer Bros |*****|
		{{2,16,0},{3,17,2},{2,16,4},{3,17,6},{2,15,8},{1,13,10},{6,10,12},{1,13,14},{0,0,0}},
		{{2,16,0},{3,17,2},{2,16,4},{3,17,6},{2,15,8},{1,13,10},{6,10,12},{1,13,14},{1,13,18},{3,16,20},{2,15,21},{1,13,22},{6,10,26},{1,13,28},{0,0,0}},
		{{4,17,0},{4,17,2},{5,19,4},{6,20,6},{7,21,8},{4,17,10},{4,17,14},{3,15,16},{3,15,18},{2,14,20},{3,15,22},{0,0,0}},
		{{2,16,0},{3,17,2},{2,16,4},{3,17,6},{2,15,8},{1,13,10},{6,10,12},{1,13,14},{5,17,18},{5,17,20},{5,17,22},{5,17,24},{5,17,26},{5,17,28},{5,17,30},{0,0,0}}
	},

	{ // Song 6 - SMB3 Underwater |***  |
		{{1,8,0},{3,13,2},{6,20,4},{5,19,6},{7,22,10},{0,0,0}},
		{{5,19,14},{6,20,16},{4,17,18},{3,13,20},{1,8,22},{2,10,26},{0,0,0}},
		{{1,7,0},{2,8,2},{4,13,4},{6,20,6},{5,19,8},{7,22,12},{0,0,0}},
		{{5,19,16},{6,20,18},{0,0,0}}

		// Fuckin' too hard
		// {{1,8,0},{3,13,2},{6,20,4},{5,19,6},{7,22,10},{5,19,14},{6,20,16},{4,17,18},{3,13,20},{1,8,22},{2,10,26},{0,0,0}},
		// {{1,7,0},{2,8,2},{4,13,4},{6,20,6},{5,19,8},{7,22,12},{5,19,16},{6,20,18},{0,0,0}},
		// {{2,10,0},{3,13,2},{6,22,4},{5,21,6},{7,24,10},{6,22,14},{4,20,16},{1,8,18},{6,22,20},{4,20,22},{0,0,0}},
		// {{1,8,0},{2,11,2},{5,20,4},{4,18,6},{5,20,10},{3,16,14},{6,22,16},{5,20,24},{0,0,0}}
	},

	{ // Song 7 - SMW Castle |*    |
		{{5,9,0},{4,6,8},{1,1,12},{2,3,16},{4,6,20},{0,0,0}},
		{{1,1,0},{4,6,4},{7,13,8},{5,9,12},{4,8,20},{0,0,0}},
		{{3,16,0},{2,15,4},{1,13,8},{2,15,12},{1,10,20},{0,0,0}},
		{{2,15,0},{1,13,4},{2,15,8},{3,16,12},{1,15,20},{0,0,0}}
	},

	{ // Song 8 - SMW Theme |**   |
		{{6,10,0},{4,6,4},{1,1,7},{2,3,8},{4,6,9},{4,6,11},{0,0,0}},
		{{2,3,0},{1,1,1},{4,6,3},{4,6,5},{7,13,7},{6,10,9},{5,8,12},{0,0,0}},
		{{6,10,0},{4,6,4},{1,1,7},{2,3,8},{4,6,9},{4,6,11},{5,8,15},{6,10,16},{4,6,17},{1,1,18},{2,3,20},{4,6,23},{0,0,0}},
		{{1,13,0},{2,15,2},{1,13,4},{2,15,6},{1,13,8},{1,1,11},{7,11,12},{6,10,13},{5,8,14},{4,6,16},{0,0,0}}
	},

	{ // Song 9 - Yoshi Story Theme |*    |
		{{5,17,0},{6,19,4},{5,17,5},{6,19,6},{5,17,8},{0,0,0}},
		{{3,14,0},{4,15,4},{3,14,5},{4,15,6},{3,14,8},{0,0,0}},
		{{5,17,0},{6,19,4},{5,17,5},{6,19,6},{5,17,8},{0,0,0}},
		{{4,15,0},{3,14,1},{4,15,2},{3,14,4},{2,12,8},{1,10,9},{2,12,10},{1,10,12},{0,0,0}}
	},

	{ // Song 10 - SM64 Peaches Castle |***  |
		{{4,8,0},{5,10,6},{4,8,10},{3,7,14},{4,8,16},{6,13,18},{7,17,20},{0,0,0}},
		{{3,6,0},{4,8,6},{3,6,10},{2,5,14},{3,6,16},{5,12,18},{6,15,20},{0,0,0}},
		{{2,5,0},{3,6,6},{2,5,10},{1,4,14},{2,5,16},{4,10,18},{5,13,20},{0,0,0}},
		{{4,10,2},{5,12,4},{6,13,6},{7,15,8},{6,13,10},{7,15,12},{4,10,14},{5,12,18},{4,10,20},{5,12,22},{6,13,24},{6,13,28},{6,13,34},{6,13,36},{6,13,38},{0,0,0}}
	},

	{ // Song 11 - SM64 Koopa Road |***  |
		{{3,6,0},{1,1,10},{3,6,12},{7,16,14},{6,15,16},{5,11,18},{3,6,20},{0,0,0}},
		{{3,6,0},{2,4,6},{3,6,10},{5,11,12},{4,9,14},{5,11,16},{0,0,0}},
		{{3,6,0},{1,1,10},{3,6,12},{7,16,14},{6,15,16},{5,11,18},{3,6,20},{0,0,0}},
		{{1,1,0},{3,6,2},{7,18,4},{6,16,6},{5,15,10},{4,13,20},{5,15,22},{0,0,0}}
	},

	{ // Song 12 - Frappe Snowland |***  |
		{{1,14,2},{2,15,3},{3,17,4},{6,22,5},{3,17,7},{0,0,0}},
		{{2,15,0},{1,14,2},{2,15,4},{1,14,5},{6,10,7},{3,5,9},{4,7,10},{3,5,11},{0,0,0}},
		{{1,14,2},{2,15,3},{3,17,4},{6,22,5},{3,17,7},{0,0,0}},
		{{2,15,0},{1,14,2},{2,15,4},{1,14,5},{6,10,7},{6,10,9},{7,12,10},{6,10,11},{0,0,0}}
	},

	{ // Song 13 - Ghost Luigi Mansion theme |**** |
		{{7,17,2},{7,17,4},{7,17,6},{7,17,8},{4,13,12},{7,17,14},{6,16,16},{3,12,20},{0,0,0}},
		{{5,15,2},{5,15,4},{5,15,6},{5,15,8},{3,12,12},{5,15,14},{4,13,16},{2,11,22},{3,12,24},{1,5,28},{0,0,0}},
		{{7,17,2},{7,17,4},{7,17,6},{7,17,8},{4,13,12},{7,17,14},{6,16,16},{3,12,20},{0,0,0}},
		{{5,15,2},{5,15,4},{5,15,6},{5,15,8},{3,12,12},{5,15,14},{7,17,16},{5,15,18},{4,13,20},{3,12,22},{1,10,24},{0,0,0}}
	},

	{ // Song 14 - Desert |**   |
		{{6,9,0},{4,6,4},{3,5,12},{3,5,16},{4,6,18},{0,0,0}},
		{{6,9,0},{4,6,4},{3,5,12},{3,5,16},{4,6,18},{0,0,0}},
		{{4,13,0},{3,12,20},{5,14,22},{6,17,24},{5,14,26},{4,13,32},{0,0,0}},
		{{1,9,0},{2,11,2},{3,12,4},{5,14,8},{4,13,12},{0,0,0}}
	},

	{ // Song 15 - Gusty Garden Galaxy |*****|
		{{5,14,0},{4,13,10},{6,16,12},{5,14,14},{3,9,16},{1,6,26},{2,7,28},{3,9,30},{3,9,32},{2,7,38},{0,0,0}},
		{{6,16,0},{5,15,10},{7,18,12},{6,16,14},{4,14,16},{3,13,22},{2,11,26},{3,13,30},{2,11,32},{1,9,38},{0,0,0}},
		{{6,21,0},{5,20,10},{7,23,12},{6,21,14},{4,19,16},{3,18,22},{4,19,32},{3,18,42},{6,21,44},{4,19,46},{3,18,48},{1,16,54},{0,0,0}},
		{{5,18,0},{4,17,10},{6,19,12},{5,18,14},{4,16,16},{3,15,22},{5,18,26},{4,16,30},{2,14,32},{1,13,38},{2,14,44},{4,16,48},{0,0,0}}
	},

	{ // Song 16 - Overworld or Cave
		{{0,0,0}},
		{{0,0,0}},
		{{0,0,0}},
		{{0,0,0}}
	}
};

const char* Prizes[10][4] = {
	{ "I_kinoko", 		"g3d/I_kinoko.brres", 			"I_kinoko", 			"wait2" },
	{ "I_fireflower", 	"g3d/I_fireflower.brres", 		"I_fireflower", 		"wait2" },
	{ "I_propeller", 	"g3d/I_propeller.brres",	 	"I_propeller_model", 	"wait2" },
	{ "I_iceflower", 	"g3d/I_iceflower.brres", 		"I_iceflower", 			"wait2" },
	{ "I_penguin", 		"g3d/I_penguin.brres", 			"I_penguin", 			"wait2" },
	{ "I_kinoko_bundle","g3d/I_mini_kinoko.brres", 		"I_mini_kinoko", 		"wait2" },
	{ "I_star", 		"g3d/I_star.brres", 			"I_star", 				"wait2" },
	{ "I_hammer", 		"g3d/I_fireflower.brres", 		"I_fireflower", 			"wait2" },
	{ "I_kinoko_bundle","g3d/I_life_kinoko.brres", 		"I_life_kinoko", 		"wait2" },
	{ "obj_coin", 		"g3d/obj_coin.brres", 			"obj_coin", 			"wait2" }
};

const char* Notes[24] = {
	"sfx/3C",
	"sfx/3C#",
	"sfx/3D",
	"sfx/3D#",
	"sfx/3E",
	"sfx/3F",
	"sfx/3F#",
	"sfx/3G",
	"sfx/3G#",
	"sfx/3A",
	"sfx/3A#",
	"sfx/3B",
	"sfx/4C",
	"sfx/4C#",
	"sfx/4D",
	"sfx/4D#",
	"sfx/4E",
	"sfx/4F",
	"sfx/4F#",
	"sfx/4G",
	"sfx/4G#",
	"sfx/4A",
	"sfx/4A#",
	"sfx/4B"
};

const char* SAarcNameList [] = {
	"obj_coin",
	"I_hammer",
	"I_star",
	"block_light",
	"block_light_color",
	"I_kinoko_bundle",
	NULL
};



/*****************************************************************************/
// Playing frickin' sounds
static nw4r::snd::StrmSoundHandle handle;

u8 hijackMusicWithSongName(const char *songName, int themeID, bool hasFast, int channelCount, int trackCount, int *wantRealStreamID);

void BonusMusicPlayer(int id) {
	if (handle.Exists())
		handle.Stop(0);

	int sID;
	hijackMusicWithSongName(Notes[id], -1, false, 2, 1, &sID);
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, sID, 1);
}



/*****************************************************************************/
// The Prize Model
class dSongPrize: public dEn_c {
public:
	int onCreate();
	int onDelete();
	int onExecute();
	int beforeDraw();
	int onDraw();

	void doSpin();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c aw;

	int queue;
	int p;
	int timer;

	HermiteKey keysX[0x10];
	unsigned int Xkey_count;
	HermiteKey keysY[0x10];
	unsigned int Ykey_count;
	HermiteKey keysS[0x10];
	unsigned int Skey_count;

	USING_STATES(dSongPrize);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Shrink);
	DECLARE_STATE(Spin);

	static dSongPrize *build();
};

// States
CREATE_STATE(dSongPrize, Wait);
CREATE_STATE(dSongPrize, Shrink);
CREATE_STATE(dSongPrize, Spin);

dSongPrize *dSongPrize::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dSongPrize));
	return new(buffer) dSongPrize;
}


/*****************************************************************************/
// The SongBlock
class dSongBlock : public daEnBlockMain_c {
public:
	Physics::Info physicsInfo;

	int onCreate();
	int onDelete();
	int onExecute();
	int beforeDraw();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	m3d::mdl_c bodyModel;
	m3d::mdl_c glowModel;
	m3d::mdl_c deadModel;
	m3d::mdl_c deadglowModel;

	StageActorLight light;
	mHeapAllocator_c allocatorB;

	int note;
	int isGlowing;
	int glowTimer;
	int isDead;
	int standAlone;
	int item;

	void calledWhenUpMoveExecutes();
	void calledWhenDownMoveExecutes();
	void blockWasHit(bool isDown);
	void glow();
	void unglow();

	USING_STATES(dSongBlock);
	DECLARE_STATE(Wait);

	static dSongBlock *build();
};

/*****************************************************************************/
// Sing Along
class dSingAlong : public dStageActor_c {
	public:
		// dSingAlong() : state(this, &StateID_Intro) { }
		dSingAlong() : state(this, &StateID_Init) { }

		void RegisterNote(int note);
		void addPowerups();

		int onCreate();
		int onDelete();
		int onExecute();
		int onDraw();

		m2d::EmbedLayout_c layout;
		dStateWrapper_c<dSingAlong> state;

		//int beforeExecute() { return true; }
		int afterExecute(int) { return true; }

		mHeapAllocator_c allocator;

		dSongPrize *PrizeModel;
		dSongPrize *Pa;
		dSongPrize *Pb;
		dSongPrize *Pc;
		dSongPrize *Pd;

		dSongBlock *SBa;
		dSongBlock *SBb;
		dSongBlock *SBc;
		dSongBlock *SBd;
		dSongBlock *SBe;
		dSongBlock *SBf;
		dSongBlock *SBg;

		int song;
		int prize[4];
 		int chorus;
		int currentNote;
		int endNote;
		int timer;
		int counter;
		int Powerups[10];
		int isResponding;
		int success;
		u8 wonItemsCount[8];	// manage items we got
		// related layout things
			bool layoutLoaded, isVisible;
			nw4r::lyt::Pane *N_info_00;
			nw4r::lyt::Pane *N_result_00;
			nw4r::lyt::Pane *N_result_01;

			nw4r::lyt::Pane *WinItemPanes[8];
			nw4r::lyt::TextBox *WinItemTextBoxes[8];
			
			enum Animation {
				SHOW_ALL = 0,
				LOOP_ALL = 1,
				IN_ITEM = 2,
				IN_ITEM_MUSHROOM = 2,
				IN_ITEM_FIREFLOWER = 3,
				IN_ITEM_PROPELLER = 4,
				IN_ITEM_PENGUIN = 5,
				IN_ITEM_MINI = 6,
				IN_ITEM_ICEFLOWER = 7,
				IN_ITEM_STAR = 8,
				IN_ITEM_HAMMER = 9,
				ITEM_NUM = 10,
				ITEM_NUM_MUSHROOM = 10,
				ITEM_NUM_FIREFLOWER = 11,
				ITEM_NUM_PROPELLER = 12,
				ITEM_NUM_PENGUIN = 13,
				ITEM_NUM_MINI = 14,
				ITEM_NUM_ICEFLOWER = 15,
				ITEM_NUM_STAR = 16,
				ITEM_NUM_HAMMER = 17,
				SHOW_TITLE = 18,
				HIDE_TITLE = 19,
				HIDE_ALL = 20,
			};
			
			void showRules();
			void showWin();
			void showLose();
			void hideAll();
		/**/

		static dSingAlong *instance;
		static dSingAlong *build();

		// Intro, Call, Response, Display Prize, Failure, Win, Collect Prizes
		USING_STATES(dSingAlong);
		DECLARE_STATE(Init);
		DECLARE_STATE(ShowTitle);
		DECLARE_STATE(HideTitle);
		DECLARE_STATE(ShowRules);
		DECLARE_STATE(HideRules);
		DECLARE_STATE(Intro);
		DECLARE_STATE(Call);
		DECLARE_STATE(Response);
		DECLARE_STATE(Prize);
		DECLARE_STATE(Failure);
		DECLARE_STATE(ShowFail);
		DECLARE_STATE(Win);
		DECLARE_STATE(ShowWin);
		DECLARE_STATE(Mistake);
		DECLARE_STATE(End);
};

dSingAlong *dSingAlong::instance = 0;
dSingAlong *dSingAlong::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dSingAlong));
	dSingAlong *c = new(buffer) dSingAlong;

	instance = c;
	return c;
}

#endif