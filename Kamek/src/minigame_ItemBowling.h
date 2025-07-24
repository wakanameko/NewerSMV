#ifndef __ITEMBOWLING_H
#define __ITEMBOWLING_H

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>
#include "minigame.h"


class dItemBowlingMgr_c : public dStageActor_c {
public:
	dItemBowlingMgr_c() : acState(this, &StateID_Init) { }

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	m2d::EmbedLayout_c layout;
	dStateWrapper_c<dItemBowlingMgr_c> acState;
	Physics physicsBarrierR;
	Physics::Info physicsBarrierInfoR;
	Physics physicsBarrierL;
	Physics::Info physicsBarrierInfoL;

	bool layoutLoaded, isVisible;
	u8 roomLength;
	// u8 wonItemTypesFlags;
	u8 wonItemsCount[8];
	u8 totalItemCount;

	u32 timer;
	float roomLimit;
	float baseScreenWall;

	nw4r::lyt::Pane *N_info_00;
	nw4r::lyt::Pane *N_result_00;
	nw4r::lyt::Pane *N_result_01;

	nw4r::lyt::Pane *WinItemPanes[8];
	nw4r::lyt::TextBox *WinItemTextBoxes[8];

	dAcPy_c *mario;
	Vec freezeMarioPos;
	dEn_c *shell;
	u32 shellID;

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

	// enum WonItemTypesFlags {
	// 	WIFLAG_MUSHROOM = 0b1,
	// 	WIFLAG_FIREFLOWER = 0b10,
	// 	WIFLAG_PROPELLER = 0b100,
	// 	WIFLAG_PENGUIN = 0b1000,
	// 	WIFLAG_MINI = 0b10000,
	// 	WIFLAG_ICEFLOWER = 0b100000,
	// 	WIFLAG_STAR = 0b1000000,
	// }

	void showRules();
	void showWin();
	void showLose();
	void hideAll();

	static dActor_c *build();
	static dItemBowlingMgr_c *instance;

	USING_STATES(dItemBowlingMgr_c);
	DECLARE_STATE(Init);
	DECLARE_STATE(ShowTitle);
	DECLARE_STATE(HideTitle);
	DECLARE_STATE(ShowRules);
	DECLARE_STATE(HideRules);
	DECLARE_STATE(WaitForThrow);
	DECLARE_STATE(WaitForShellDeath);
	DECLARE_STATE(FollowShell);
	DECLARE_STATE(GetResults);
	DECLARE_STATE(WaitForResults);
	DECLARE_STATE(End);
};


#endif