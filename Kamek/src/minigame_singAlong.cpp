#include "minigame_singAlong.h"

/*****************************************************************************/
// The Prize Model
int dSongPrize::onCreate() {

	// Settings
	queue = this->settings & 0xF;
	int prize = this->settings >> 16;
	scale = (Vec){ 3.0, 3.0, 3.0 };

	p = prize;


	// Model creation
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	resFile.data = getResource(Prizes[p][0], Prizes[p][1]);
	nw4r::g3d::ResMdl mdl = resFile.GetResMdl(Prizes[p][2]);
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Item(&bodyModel, 0); // 800B42B0


	// Animation Assignment
	nw4r::g3d::ResAnmChr anmChr = resFile.GetResAnmChr("wait2");
	aw.setup(mdl, anmChr, &allocator, 0);
	aw.bind(&bodyModel, anmChr, 1);
	bodyModel.bindAnim(&aw, 0.0);
	aw.setUpdateRate(1.0);
	allocator.unlink();


	// Change State
	doStateChange(&dSongPrize::StateID_Wait);

	return true;
}

int dSongPrize::onDelete() {
	return true;
}

int dSongPrize::onExecute() {
	acState.execute();
	return true;
}

int dSongPrize::beforeDraw() { return 1; }
int dSongPrize::onDraw() {

	if (p == 9) {
		matrix.translation(pos.x, pos.y + (8.0 * scale.y), pos.z);
	}
	else {
		matrix.translation(pos.x, pos.y, pos.z);
	}

	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);

	bodyModel.calcWorld(false);


	bodyModel.scheduleForDrawing();
	bodyModel._vf1C();

	if(this->aw.isAnimationDone())
		this->aw.setCurrentFrame(0.0);

	return true;
}

void dSongPrize::beginState_Wait() {}
void dSongPrize::executeState_Wait() {}
void dSongPrize::endState_Wait() {}

void dSongPrize::beginState_Shrink() {
	this->timer = 0;

	Xkey_count = 2;
	Ykey_count = 2;
	Skey_count = 2;

	// /* keysX[i] = { frame, value, slope }; */
	keysX[0] = (HermiteKey){ 0.0f, pos.x, 0.8f };
	keysY[0] = (HermiteKey){ 0.0f, pos.y, 0.8f };
	keysS[0] = (HermiteKey){ 0.0f, 3.0f, -0.8f };

	keysX[1] = (HermiteKey){ 60.0f, pos.x + (30.0f * queue) - 172.0f, 1.0f };
	keysY[1] = (HermiteKey){ 60.0f, pos.y + 64.0f, 1.0f };
	keysS[1] = (HermiteKey){ 60.0f, 1.0f, 0.0f };
}
void dSongPrize::executeState_Shrink() {
	float modX = GetHermiteCurveValue(timer, keysX, Xkey_count);
	float modY = GetHermiteCurveValue(timer, keysY, Ykey_count);
	//float modS = GetHermiteCurveValue(timer, keysS, Skey_count);
	float modS = 3.0f - (timer * (2.0f / 64.0f));	// orig: 60.0f

	pos = (Vec){ modX, modY, pos.z };
	scale = (Vec){ modS, modS, modS };

	if (timer == 60) { doStateChange(&StateID_Wait); }

	timer += 1;
}
void dSongPrize::endState_Shrink() {}

void dSongPrize::beginState_Spin() {
	this->timer = 0;

	Xkey_count = 2;
	keysX[0] = (HermiteKey){ 0.0f, 0.0f, 0.0f };
	keysX[1] = (HermiteKey){ 20.0f, 65535.0f, 0.0f };
}
void dSongPrize::executeState_Spin() {
	float modX = GetHermiteCurveValue(timer, keysX, Xkey_count);
	rot.y = (int)modX;

	Vec efPos = {pos.x-10.0f, pos.y-2.0f, pos.z-100.0f};
	S16Vec nullRot = {0,0,0};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
	if (timer == 20) { SpawnEffect("Wm_ob_flagget", 0, &efPos, &nullRot, &oneVec); }
	if (timer == 30) { doStateChange(&StateID_Wait); }

	timer += 1;
}
void dSongPrize::endState_Spin() {}


/*****************************************************************************/
// Events
CREATE_STATE(dSingAlong, Init);
CREATE_STATE(dSingAlong, ShowTitle);
CREATE_STATE(dSingAlong, HideTitle);
CREATE_STATE(dSingAlong, ShowRules);
CREATE_STATE(dSingAlong, HideRules);
CREATE_STATE(dSingAlong, Intro);
CREATE_STATE(dSingAlong, Prize);
CREATE_STATE(dSingAlong, Call);
CREATE_STATE(dSingAlong, Response);
CREATE_STATE(dSingAlong, Mistake);
CREATE_STATE(dSingAlong, Failure);
CREATE_STATE(dSingAlong, ShowFail);
CREATE_STATE(dSingAlong, Win);
CREATE_STATE(dSingAlong, ShowWin);
CREATE_STATE(dSingAlong, End);

int dSingAlong::onCreate() {
	if(!layoutLoaded) {
		OSReport("loading miniGameSingAlong\n");
		if (!layout.loadArc("miniGameSingAlong.arc", false))
			return false;

		OSReport("loaded miniGameSingAlong\n");

		static const char *brlanNames[7] = {
			"miniGameSingAlong_inWindow.brlan",
			"miniGameSingAlong_loopWindow.brlan",
			"miniGameSingAlong_inItem.brlan",
			"miniGameSingAlong_loopGetNum.brlan",
			"miniGameSingAlong_inTitle.brlan",
			"miniGameSingAlong_outTitle.brlan",
			"miniGameSingAlong_outWindow.brlan",
		};

		static const char *groupNames[21] = {
			"A00_Window",
			"A00_Window",
			"C00_Skino_00", "C01_fire_00", "C02_pro_00", "C03_pen_00", "C04_mame_00", "C05_ice_00", "C06_star_00", "C07_hammer_00",
			"C00_Skino_00", "C01_fire_00", "C02_pro_00", "C03_pen_00", "C04_mame_00", "C05_ice_00", "C06_star_00", "C07_hammer_00",
			"D00_title",
			"D00_title",
			"A00_Window",
		};

		static const int groupIDs[21] = {
			0,
			1,
			2, 2, 2, 2, 2, 2, 2, 2,
			3, 3, 3, 3, 3, 3, 3, 3,
			4,
			5,
			6,
		};

		layout.build("miniGameSingAlong.brlyt");

		layout.loadAnimations(brlanNames, 7);
		layout.loadGroups(groupNames, groupIDs, 21);
		layout.disableAllAnimations();

		layoutLoaded = true;

		N_info_00 = layout.findPaneByName("N_info_00");
		N_result_00 = layout.findPaneByName("N_result_00");
		N_result_01 = layout.findPaneByName("N_result_01");

		WinItemPanes[0] = layout.findPaneByName("N_Skino_00");
		WinItemPanes[1] = layout.findPaneByName("N_fire_00");
		WinItemPanes[2] = layout.findPaneByName("N_pro_00");
		WinItemPanes[3] = layout.findPaneByName("N_pen_00");
		WinItemPanes[4] = layout.findPaneByName("N_mame_00");
		WinItemPanes[5] = layout.findPaneByName("N_ice_00");
		WinItemPanes[6] = layout.findPaneByName("N_star_00");
		WinItemPanes[7] = layout.findPaneByName("N_hammer_00");

		WinItemTextBoxes[0] = layout.findTextBoxByName("T_xNum_00");
		WinItemTextBoxes[1] = layout.findTextBoxByName("T_xNum_01");
		WinItemTextBoxes[2] = layout.findTextBoxByName("T_xNum_02");
		WinItemTextBoxes[3] = layout.findTextBoxByName("T_xNum_03");
		WinItemTextBoxes[4] = layout.findTextBoxByName("T_xNum_04");
		WinItemTextBoxes[5] = layout.findTextBoxByName("T_xNum_05");
		WinItemTextBoxes[6] = layout.findTextBoxByName("T_xNum_06");
		WinItemTextBoxes[7] = layout.findTextBoxByName("T_xNum_07");

		for(int i = 0; i < 8; i++) 
			WinItemPanes[i]->SetVisible(false);


		static const char *bmgTextboxesNames[8] = {
			"T_itemInfo_00",	// "Press 1 on the map" Text
			"T_infoS_01",		// "Press 1 on the map" Shadow
			"T_info_02",		// "There will be item balloons on the way"
			"T_info_00",		// "Throw a shell" Text
			"T_infoS_00",		// "Throw a shell" Shadow
			"T_info_03",		// "You win"
			"T_info_04",		// "Too bad"
			"T_title_00",		// "Item Bowling"
		};

		static const int bmgTextboxesIDs[8] = {
			9,		// 常駐 "Press 1 on the map!"
			9,
			15,		// Game Overview (ex: Match a pair to win! ~)
			13,		// Game Super (ex: Press 1 to punch a panel and flip it.)
			13,
			2,		// 常駐 "You win!"
			7,		// 常駐 "You lose"
			14,		// Game Title (ex: Power-up Panels)
		};

		layout.setLangStrings(bmgTextboxesNames, bmgTextboxesIDs, 0x12D, 8);

		this->disableFlagMask = 0;

		dStageActor_c *vPlatform = 0;
		while((vPlatform = (dEn_c*)fBase_c::search(AC_LIFT_RIDE_VMOVE, vPlatform)))
			if((vPlatform->settings >> 4) & 0x1)
				vPlatform->disableFlagMask |= 0x30;
	}
	NoMichaelBuble = true;
	StageC4::instance->_1D = 1; // enable no-pause

	// Load in the settings
	this->song = this->settings & 0xF;
	this->prize[0] = (this->settings >> 28) & 0xF;
	this->prize[1] = (this->settings >> 24) & 0xF;
	this->prize[2] = (this->settings >> 20) & 0xF;
	this->prize[3] = (this->settings >> 16) & 0xF;
	this->chorus = -1;
	this->currentNote = 0;
	this->success = 0;

	this->Powerups[0] = 0; // Mushroom
	this->Powerups[1] = 0; // Fireflower
	this->Powerups[2] = 0; // Propeller
	this->Powerups[3] = 0; // Iceflower
	this->Powerups[4] = 0; // Penguin
	this->Powerups[5] = 0; // MiniShroom
	this->Powerups[6] = 0; // Starman
	this->Powerups[7] = 0; // Hammer
	this->Powerups[8] = 0; // 1-ups
	this->Powerups[9] = 0; // Coins

	// Create and prepare the blocks
	S16Vec rot = (S16Vec){0,0,0};
	float x = pos.x;
	float y = pos.y - 40.0;
	float z = pos.z;

	Vec blockPos = {x-96.0f, y, z};
	SBa = (dSongBlock*)create(WM_KILLER, 1, &blockPos, &rot, 0);
	blockPos.x += 32.0f;
	SBb = (dSongBlock*)create(WM_KILLER, 2, &blockPos, &rot, 0);
	blockPos.x += 32.0f;
	SBc = (dSongBlock*)create(WM_KILLER, 3, &blockPos, &rot, 0);
	blockPos.x += 32.0f;
	SBd = (dSongBlock*)create(WM_KILLER, 4, &blockPos, &rot, 0);
	blockPos.x += 32.0f;
	SBe = (dSongBlock*)create(WM_KILLER, 5, &blockPos, &rot, 0);
	blockPos.x += 32.0f;
	SBf = (dSongBlock*)create(WM_KILLER, 6, &blockPos, &rot, 0);
	blockPos.x += 32.0f;
	SBg = (dSongBlock*)create(WM_KILLER, 7, &blockPos, &rot, 0);

	// // Trigger the intro state
	// state.setState(&StateID_Intro);
	isResponding = 0;

	isVisible = true;
	hideUIMode = true;	// to hide HUD
	return true;
}

int dSingAlong::onExecute() {
	state.execute();

	layout.execAnimations();
	layout.update();
	
	return true;
}

int dSingAlong::onDraw() {
	if (isVisible) {
		layout.scheduleForDrawing();
	}
	return true;
}

int dSingAlong::onDelete() {
	hideUIMode = false;	// to show HUD
	return layout.free();
	instance = 0;
	return 1;
}

/*****************************************************************************/
// Register a Note being played by the players
void dSingAlong::RegisterNote(int note) {
	nw4r::snd::SoundHandle handle;

	if (isResponding == 1) {

		if (note == Songs[song][chorus][currentNote][0]) {
			BonusMusicPlayer(Songs[song][chorus][currentNote][1]-1);
			// MapSoundPlayer(SoundRelatedClass, Notes[Songs[song][chorus][currentNote][1]-1], 1);
			currentNote += 1;
		}
		else {
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_MG_CMN_WIN_CLOSE, 1);
			isResponding = 0;
			state.setState(&StateID_Mistake);
		}
	}
}

/*****************************************************************************/
// Init State
void dSingAlong::beginState_Init() {
	this->hideAll();
	layout.resetAnim(SHOW_ALL);
	layout.resetAnim(SHOW_TITLE);
	MakeMarioEnterDemoMode();

	OSReport("Initializing\n");
	StageC4::instance->_1D = 1;

	this->timer = 0;
}

void dSingAlong::executeState_Init() {
	if(this->timer > 60) {
		if(ClassWithCameraInfo::instance->xOffset > 0.0f) {
			ClassWithCameraInfo::instance->xOffset -= 4.0f;
		}
		else {
			// StageC4::instance->_1D = 0;
			ClassWithCameraInfo::instance->xOffset = 0.0f;

			layout.enableNonLoopAnim(SHOW_TITLE);
			state.setState(&StateID_ShowTitle);
		}
	}

	this->timer++;
}

void dSingAlong::endState_Init() {
	
}

/*****************************************************************************/
// Related Layout States
// ShowTitle State
void dSingAlong::beginState_ShowTitle() {
	OSReport("ShowTitle Item Bowling.\n");
	this->timer = 0;
}

void dSingAlong::executeState_ShowTitle() {
	if(!layout.isAnimOn(SHOW_TITLE)) {
		if(this->timer > 120) {
			layout.enableNonLoopAnim(HIDE_TITLE);
			state.setState(&StateID_HideTitle);
		}

		this->timer++;
	}
}

void dSingAlong::endState_ShowTitle() {
	
}

// HideTitle State

void dSingAlong::beginState_HideTitle() {
	OSReport("HideTitle Item Bowling.\n");
	this->timer = 0;
}

void dSingAlong::executeState_HideTitle() {
	if(!layout.isAnimOn(HIDE_TITLE)) {
		layout.enableNonLoopAnim(SHOW_ALL);
		doBGMStuff(_8042A788, 8);
		if(this->timer > 60) {
			MapSoundPlayer(SoundRelatedClass, SE_SYS_DIALOGUE_IN, 1);

			this->showRules();
			state.setState(&StateID_ShowRules);
		}

		this->timer++;
	}
}

void dSingAlong::endState_HideTitle() {
	
}

// ShowRules State

void dSingAlong::beginState_ShowRules() {
	OSReport("ShowRules Item Bowling.\n");

}

void dSingAlong::executeState_ShowRules() {
	if(!layout.isAnimOn(SHOW_ALL)) {
		int nowPressed = Remocon_GetPressed(GetActiveRemocon());
		if(nowPressed & (WPAD_TWO | WPAD_A)) {
			NextStageActorDisableFlags &= ~0x10;
			MapSoundPlayer(SoundRelatedClass, 0x79, 1);

			layout.enableNonLoopAnim(HIDE_ALL);
			doBGMStuff2(_8042A788);
			state.setState(&StateID_HideRules);
		}
	}
}

void dSingAlong::endState_ShowRules() {
	
}

// HideRules State

void dSingAlong::beginState_HideRules() {
	OSReport("HideRules Item Bowling.\n");

}

void dSingAlong::executeState_HideRules() {
	if(!layout.isAnimOn(HIDE_ALL)) {
		MakeMarioExitDemoMode();
		state.setState(&StateID_Intro);
	}
}

void dSingAlong::endState_HideRules() {
	// layout.resetAnim(SHOW_ALL);
	// layout.resetAnim(SHOW_TITLE);
	// layout.disableAllAnimations();
}

/*****************************************************************************/
// Intro
void dSingAlong::beginState_Intro() {}
void dSingAlong::endState_Intro() {}
void dSingAlong::executeState_Intro() {
	MakeMarioEnterDemoMode();
	state.setState(&StateID_Prize);
}

//*****************************************************************************/
// Prize
void dSingAlong::beginState_Prize() {
	this->timer = 90;
}
void dSingAlong::executeState_Prize() {

	if ((timer == 90) && (chorus >= 0)) { // Play a nice success sound, and wait a second
		nw4r::snd::SoundHandle handle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_MG_IH_PAIR_OK, 1);
		//PlaySound(this, SE_MG_IH_PAIR_OK); // SE_MG_IH_NICE or SE_MG_UH_NICE

		int p;
		p = prize[chorus];
		this->Powerups[p] += 1;
		if(p != 3 || p != 4 || p != 5){	// Other than ice, peng, mini
			wonItemsCount[p] += 1;
		}
		else if (p == 3){	// ice
			wonItemsCount[5] += 1;
		}
		else if (p == 4){	// penguin
			wonItemsCount[3] += 1;
		}
		else if (p == 5){	// mini shroom
			wonItemsCount[4] += 1;
		}
	}

	if ((timer == 60) && (chorus >= 0)) {
		if (chorus == 0) { Pa = PrizeModel; }
		if (chorus == 1) { Pb = PrizeModel; }
		if (chorus == 2) { Pc = PrizeModel; }
		if (chorus == 3) { Pd = PrizeModel; }

		PrizeModel->doStateChange(&dSongPrize::StateID_Shrink);
	}

	if (timer == 0) {
		chorus += 1;

		if (chorus == 4) {
			if (success == 0)
				state.setState(&StateID_Failure);
			else
				state.setState(&StateID_Win);
			return;
		}

		Vec efPos = {pos.x, pos.y+32.0f, pos.z+500.0f};
		S16Vec nullRot = {0,0,0};
		Vec efScale = {1.5f, 1.5f, 1.5f};
		SpawnEffect("Wm_en_blockcloud", 0, &efPos, &nullRot, &efScale);
		nw4r::snd::SoundHandle handle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_OBJ_ITEM_APPEAR, 1);
		//PlaySound(this, SE_OBJ_ITEM_APPEAR); // SE_OBJ_GOOD_ITEM_APPEAR

		PrizeModel = (dSongPrize*)create(WM_SINKSHIP, chorus + (prize[chorus] << 16), &pos, &rot, 0);
	}

	if (timer == -90) {
		state.setState(&StateID_Call);
	}

	timer -= 1;
}
void dSingAlong::endState_Prize() {}


//*****************************************************************************/
// Call
void dSingAlong::beginState_Call() {
	timer = 0;
	currentNote = 0;
}
void dSingAlong::executeState_Call() {
	if (timer == (Songs[song][chorus][currentNote][2] * Tempo[song])) {

		BonusMusicPlayer(Songs[song][chorus][currentNote][1]-1);

		Vec effPos;

		if      (Songs[song][chorus][currentNote][0] == 1) { SBa->glow(); effPos = SBa->pos; }
		else if (Songs[song][chorus][currentNote][0] == 2) { SBb->glow(); effPos = SBb->pos; }
		else if (Songs[song][chorus][currentNote][0] == 3) { SBc->glow(); effPos = SBc->pos; }
		else if (Songs[song][chorus][currentNote][0] == 4) { SBd->glow(); effPos = SBd->pos; }
		else if (Songs[song][chorus][currentNote][0] == 5) { SBe->glow(); effPos = SBe->pos; }
		else if (Songs[song][chorus][currentNote][0] == 6) { SBf->glow(); effPos = SBf->pos; }
		else if (Songs[song][chorus][currentNote][0] == 7) { SBg->glow(); effPos = SBg->pos; }

		Vec ringPos = {effPos.x, effPos.y+8.0f, effPos.z-100.0f};
		S16Vec nullRot = {0,0,0};
		Vec oneVec = {1.0f, 1.0f, 1.0f};
		SpawnEffect("Wm_en_vshit_ring", 0, &ringPos, &nullRot, &oneVec);

		currentNote += 1;

		if (Songs[song][chorus][currentNote][0] == 0) {
			state.setState(&StateID_Response);
		}
	}

	timer += 1;
}
void dSingAlong::endState_Call() {
	MakeMarioExitDemoMode();
}


/*****************************************************************************/
// Response
void dSingAlong::beginState_Response() {
	timer = 0;
	currentNote = 0;
	isResponding = 1;
}
void dSingAlong::executeState_Response() {
	if (Songs[song][chorus][currentNote][0] == 0) {
		isResponding = 0;
		MakeMarioEnterDemoMode();
		this->success = this->success + 1;
		state.setState(&StateID_Prize);
	}
}
void dSingAlong::endState_Response() {}


//*****************************************************************************/
// Prize
void dSingAlong::beginState_Mistake() {
	SBa->unglow();
	SBb->unglow();
	SBc->unglow();
	SBd->unglow();
	SBe->unglow();
	SBf->unglow();
	SBg->unglow();

	MakeMarioEnterDemoMode();
	this->timer = 120;
	chorus += 1;
}
void dSingAlong::executeState_Mistake() {

	if (timer == 60) {
		nw4r::snd::SoundHandle handle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_MG_IH_NOPAIR_NG, 1);

		Vec efPos = {pos.x, pos.y+32.0f, pos.z+500.0f};
		S16Vec nullRot = {0,0,0};
		Vec efScale = {1.5f, 1.5f, 1.5f};
		SpawnEffect("Wm_en_blockcloud", 0, &efPos, &nullRot, &efScale);
		PrizeModel->Delete(1);
	}

	if (chorus >= 4 && timer == 15) {
		if (success == 0){
			state.setState(&StateID_Failure);
		}
		else{
			state.setState(&StateID_Win);
		}
		return;
	}

	if (timer == 0) {
		Vec efPos = {pos.x, pos.y+32.0f, pos.z+500.0f};
		S16Vec nullRot = {0,0,0};
		Vec efScale = {1.5f, 1.5f, 1.5f};
		SpawnEffect("Wm_en_blockcloud", 0, &efPos, &nullRot, &efScale);
		nw4r::snd::SoundHandle handle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_OBJ_ITEM_APPEAR, 1);

		PrizeModel = (dSongPrize*)create(WM_SINKSHIP, chorus + (prize[chorus] << 16), &pos, &rot, 0);
	}

	if (timer == -90) {
		state.setState(&StateID_Call);
	}

	timer -= 1;
}
void dSingAlong::endState_Mistake() {

	if (chorus != 4) {
		SBa->isDead = 0;
		SBb->isDead = 0;
		SBc->isDead = 0;
		SBd->isDead = 0;
		SBe->isDead = 0;
		SBf->isDead = 0;
		SBg->isDead = 0;
	}
}

/*****************************************************************************/
// Failure
void dSingAlong::beginState_Failure() {
	this->timer = 0;
	MakeMarioEnterDemoMode();
}
void dSingAlong::executeState_Failure() {
	if (timer >= 5) {
		for (int i = 0; i < 4; i++){
			if (dAcPy_c *player = dAcPy_c::findByID(i)){
				if (timer == 5){
					player->setAnimePlayWithAnimID(dm_surprise);
				}
				player->rot.y = 0.0;
			}
		}
	}

	if (timer == 20) {
		state.setState(&StateID_ShowFail);
	}

	timer += 1;
}
void dSingAlong::endState_Failure() {}

/*****************************************************************************/
// Show Fail
void dSingAlong::beginState_ShowFail() {
	MapSoundPlayer(SoundRelatedClass, 0x5F3, 1);
	this->showLose();
	layout.enableNonLoopAnim(SHOW_ALL);
	doBGMStuff(_8042A788, 8);
}
void dSingAlong::executeState_ShowFail() {
	// Set Players rotation
	for (int i = 0; i < 4; i++){
		if (dAcPy_c *player = dAcPy_c::findByID(i)){
			player->rot.y = 0.0;
		}
	}

	if(!layout.isAnimOn(SHOW_ALL)) {
		int nowPressed = Remocon_GetPressed(GetActiveRemocon());

		if(nowPressed & (WPAD_TWO | WPAD_A)) {
			MapSoundPlayer(SoundRelatedClass, SE_SYS_DIALOGUE_OUT, 1);
			doBGMStuff2(_8042A788);
			layout.enableNonLoopAnim(HIDE_ALL);
			state.setState(&StateID_End);
		}
	}
}
void dSingAlong::endState_ShowFail() {}

/*****************************************************************************/
// Win
void dSingAlong::beginState_Win() {
	this->timer = 0;
	MakeMarioEnterDemoMode();
}
void dSingAlong::executeState_Win() {
	if (timer == 5)
		for (int i = 0; i < 4; i++)
			if (dAcPy_c *player = dAcPy_c::findByID(i)) {
				player->setAnimePlayWithAnimID(dm_glad);
				player->setFlag(0x24);
			}

	// Play a success sound
	if (timer == 30) {
		nw4r::snd::SoundHandle handle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, STRM_BGM_MINIGAME_FANFARE_GOOD, 1);
		StopBGMMusic();
	}

	// Make the banked powerups do a little dance/effect
	// Play a sound for each powerup gained
	if (timer == 30*3 || timer == 30*4 || timer == 30*5 || timer == 30*6) {
		dSongPrize *dancer = 0;
		if (timer == 30*3)
			dancer = Pa;
		else if (timer == 30*4)
			dancer = Pb;
		else if (timer == 30*5)
			dancer = Pc;
		else if (timer == 30*6)
			dancer = Pd;

		if (dancer) {
			dancer->doStateChange(&dSongPrize::StateID_Spin);
			nw4r::snd::SoundHandle handle;
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_OBJ_ITEM_FROM_KINOPIO, 1);
		}
		else {
			timer += 29;
		}
	}

	// If victory, make mario do a little dance/sound
	if (timer == 30*8) {
		nw4r::snd::SoundHandle handle1, handle2, handle3, handle4;

		if (GetSpecificPlayerActor(0) != 0)
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle1, SE_VOC_MA_CLEAR_MULTI, 1);
		if (GetSpecificPlayerActor(1) != 0)
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle2, SE_VOC_LU_CLEAR_MULTI, 1);
		if (GetSpecificPlayerActor(2) != 0)
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle3, SE_VOC_KO_CLEAR_MULTI, 1);
		if (GetSpecificPlayerActor(3) != 0)
			PlaySoundWithFunctionB4(SoundRelatedClass, &handle4, SE_VOC_KO2_CLEAR_MULTI, 1);

		Vec efPos = {pos.x-80.0f, pos.y+20.0f, pos.z+500.0f};
		S16Vec nullRot = {0,0,0};
		Vec efScale = {0.75f, 0.75f, 0.75f};
		SpawnEffect("Wm_ob_fireworks_y", 0, &efPos, &nullRot, &efScale);
	}

	if (timer == (int)30*8.5) {
		Vec efPos = {pos.x+108.0f, pos.y+32.0f, pos.z+500.0f};
		S16Vec nullRot = {0,0,0};
		Vec efScale = {0.75f, 0.75f, 0.75f};
		SpawnEffect("Wm_ob_fireworks_b", 0, &efPos, &nullRot, &efScale);
	}

	if (timer == 30*9) {
		Vec efPos = {pos.x, pos.y+50.0f, pos.z+500.0f};
		S16Vec nullRot = {0,0,0};
		Vec efScale = {0.75f, 0.75f, 0.75f};
		SpawnEffect("Wm_ob_fireworks_g", 0, &efPos, &nullRot, &efScale);
	}


	// Add the powerups and exit the stage
	if (timer == 30*11) {
		this->addPowerups();
		state.setState(&StateID_ShowWin);
	}
	timer += 1;
}
void dSingAlong::endState_Win() {}

/*****************************************************************************/
// Show Win
void dSingAlong::beginState_ShowWin() {
	MapSoundPlayer(SoundRelatedClass, 0x5F2, 1);
	this->showWin();
	layout.enableNonLoopAnim(SHOW_ALL);
	doBGMStuff(_8042A788, 8);
}
void dSingAlong::executeState_ShowWin() {
	if(!layout.isAnimOn(SHOW_ALL)) {
		int nowPressed = Remocon_GetPressed(GetActiveRemocon());

		if(nowPressed & (WPAD_TWO | WPAD_A)) {
			MapSoundPlayer(SoundRelatedClass, SE_SYS_DIALOGUE_OUT, 1);
			doBGMStuff2(_8042A788);
			layout.enableNonLoopAnim(HIDE_ALL);
			state.setState(&StateID_End);
		}
	}
}
void dSingAlong::endState_ShowWin() {}

/*****************************************************************************/
// Exit level
void dSingAlong::beginState_End() {
	OSReport("End Item Bowling.\n");
	this->timer = 0;
}

void dSingAlong::executeState_End() {
	// Set Players rotation
	for (int i = 0; i < 4; i++){
		if (dAcPy_c *player = dAcPy_c::findByID(i)){
			player->rot.y = 0.0;
		}
	}

	if(!layout.isAnimOn(HIDE_ALL)) {
		if(this->timer == 60) {
			ExitStage(WORLD_MAP, 0, BEAT_LEVEL, MARIO_WIPE);
		}
		this->timer++;
	}
}

void dSingAlong::endState_End() {}

/*****************************************************************************/
// Add Powerups at the End of the Stage
void dSingAlong::addPowerups() {
	SaveFile *file = GetSaveFile();
	SaveBlock *block = file->GetBlock(file->header.current_file);

	for (int i = 0; i < 8; i++) { // Change this to 8 to support hammers
		block->powerups_available[i] = block->powerups_available[i] + this->Powerups[i];

		if (block->powerups_available[i] > 99) { block->powerups_available[i] = 99; }
	}

	for (int i = 0; i < 4; i++) { // Make sure all players get the reward!
		block->player_coins[i] = (this->Powerups[9] * 50) + block->player_coins[i];

		for (;block->player_coins[i] < 100; block->player_coins[i] - 100) {
			block->player_coins[i] = 1 + block->player_coins[i];
		}

		block->player_lives[i] = this->Powerups[8] + block->player_lives[i];
		if (block->player_lives[i] > 99) { block->player_lives[i] = 99; }
	}

	return;
}

/*****************************************************************************/
// Layout Functions
void dSingAlong::hideAll() {
	N_info_00->SetVisible(false);
	N_result_00->SetVisible(false);
	N_result_01->SetVisible(false);
}

void dSingAlong::showRules() {
	N_info_00->SetVisible(true);
	N_result_00->SetVisible(false);
	N_result_01->SetVisible(false);
}

void dSingAlong::showLose() {
	N_info_00->SetVisible(false);
	N_result_00->SetVisible(false);
	N_result_01->SetVisible(true);
}

void dSingAlong::showWin() {
	N_info_00->SetVisible(false);
	N_result_00->SetVisible(true);
	N_result_01->SetVisible(false);

	s8 itemsGot[8] = {-1, -1, -1, -1, -1, -1, -1, -1};	// Added Hammer element
	int itemTypeCount = 0;
	for(int i = 0; i < 8; i++) {
		if(this->Powerups[i] > 0) {
			WinItemPanes[i]->SetVisible(true);

			wchar_t count[3] = {L'x', L'X', 0};
			count[1] = wonItemsCount[i] + '0'; // No need for a sprintf, it's not like you could get 10 times the same item anyway
			WinItemTextBoxes[i]->SetString(count);

			itemsGot[itemTypeCount++] = i;

			layout.enableNonLoopAnim(IN_ITEM+i);
		}
	}

	for(int i = 0; i < itemTypeCount; i++) {
		char posPaneName[32];
		// wchar_t wposPaneName_t[32];
		snprintf(posPaneName, 32, "N_%dP_Pos_%02d", itemTypeCount, i);
		OSReport("Positionning: %d %d\n", itemTypeCount, i);
		// mbstowcs(wposPaneName_t, posPaneName, 32);

		WinItemPanes[itemsGot[i]]->trans = layout.findPaneByName(posPaneName)->trans;
	}
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
// Replaces: Nothing yet



CREATE_STATE(dSongBlock, Wait);


dSongBlock *dSongBlock::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dSongBlock));
	return new(buffer) dSongBlock;
}


int dSongBlock::onCreate() {

	// Settings
	this->note = this->settings & 0xF;
	this->standAlone = (this->settings >> 4) & 0xF;
	this->item = (this->settings >> 16) & 0xFF;

	// Model creation
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	char modelName [24];
	sprintf(modelName, "g3d/block_light_%d.brres", this->note);

	this->resFile.data = getResource("block_light", modelName);
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("block_light");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&bodyModel, 0); // 800B42B0

	char glowName [25];
	sprintf(glowName, "g3d/block_light_g%d.brres", this->note);

	this->resFile.data = getResource("block_light", glowName);
	mdl = this->resFile.GetResMdl("block_light");
	glowModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&glowModel, 0); // 800B42B0



	// Dead Models
	this->resFile.data = getResource("block_light", "g3d/block_light_8.brres");
	mdl = this->resFile.GetResMdl("block_light");
	deadModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&deadModel, 0); // 800B42B0

	this->resFile.data = getResource("block_light", "g3d/block_light_g8.brres");
	mdl = this->resFile.GetResMdl("block_light");
	deadglowModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&deadglowModel, 0); // 800B42B0

	allocator.unlink();

	// Glow related
	isGlowing = 0;
	glowTimer = 0;
	isDead = 0;

	light.init(&allocatorB, 2);


	// Block Physics
	blockInit(pos.y);

	physicsInfo.x1 = -8;
	physicsInfo.y1 = 16;
	physicsInfo.x2 = 8;
	physicsInfo.y2 = 0;

	physicsInfo.otherCallback1 = &daEnBlockMain_c::OPhysicsCallback1;
	physicsInfo.otherCallback2 = &daEnBlockMain_c::OPhysicsCallback2;
	physicsInfo.otherCallback3 = &daEnBlockMain_c::OPhysicsCallback3;

	physics.setup(this, &physicsInfo, 3, currentLayerID);
	physics.flagsMaybe = 0x260;
	physics.callback1 = &daEnBlockMain_c::PhysicsCallback1;
	physics.callback2 = &daEnBlockMain_c::PhysicsCallback2;
	physics.callback3 = &daEnBlockMain_c::PhysicsCallback3;
	physics.addToList();

	// Change State
	doStateChange(&dSongBlock::StateID_Wait);

	return true;
}

int dSongBlock::onDelete() {
	physics.removeFromList();
	return true;
}

int dSongBlock::onExecute() {
	acState.execute();
	physics.update();
	blockUpdate();

	if (glowTimer > 0) { glowTimer--; }
	else { isGlowing = 0; }

	light.pos.x = pos.x;
	light.pos.y = pos.y+8.0;
	light.pos.z = pos.z;

	light.size = 226.0;
	light.update();

	// now check zone bounds based on state
	if (acState.getCurrentState()->isEqual(&StateID_Wait)) {
		checkZoneBoundaries(0);
	}

	return true;
}

int dSongBlock::beforeDraw() {
	light.draw();
	return dStageActor_c::beforeDraw();
}

int dSongBlock::onDraw() {
	matrix.translation(pos.x, pos.y+8.0, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	if (isDead == 0) {
		if (isGlowing == 0) {
			bodyModel.setDrawMatrix(matrix);
			bodyModel.setScale(&scale);
			bodyModel.calcWorld(false);

			bodyModel.scheduleForDrawing();
		}

		else {
			glowModel.setDrawMatrix(matrix);
			glowModel.setScale(&scale);
			glowModel.calcWorld(false);

			glowModel.scheduleForDrawing();
		}
	}
	else {
		if (isGlowing == 0) {
			deadModel.setDrawMatrix(matrix);
			deadModel.setScale(&scale);
			deadModel.calcWorld(false);

			deadModel.scheduleForDrawing();
		}

		else {
			deadglowModel.setDrawMatrix(matrix);
			deadglowModel.setScale(&scale);
			deadglowModel.calcWorld(false);

			deadglowModel.scheduleForDrawing();
		}
	}

	return true;
}

void dSongBlock::blockWasHit(bool isDown) {
	pos.y = initialY;

	Vec ringPos = {pos.x, pos.y+8.0f, pos.z-100.0f};
	S16Vec nullRot = {0,0,0};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
	SpawnEffect("Wm_en_vshit_ring", 0, &ringPos, &nullRot, &oneVec);
	Vec keygetPos = {pos.x, pos.y+8.0f, pos.z-100.0f};
	Vec keygetScale = {0.5f, 0.5f, 0.5f};
	SpawnEffect("Wm_ob_keyget02_lighit", 0, &keygetPos, &nullRot, &keygetScale);

	if (item > 0) {
		Vec itemPos = {pos.x, pos.y, pos.z};
		create(EN_ITEM, item, &itemPos, &rot, 0);
	}

	if (standAlone) {

		int play;
		// C, C#, D, D#, E, F, F#, G, G#, A, A#, B

		if 		(note == 0) { play = 12; }
		else if (note == 1) { play = 12; }
		else if (note == 2) { play = 14; }
		else if (note == 3) { play = 16; }
		else if (note == 4) { play = 17; }
		else if (note == 5) { play = 19; }
		else if (note == 6) { play = 21; }
		else if (note == 7) { play = 23; }
		else if (note == 8) { play = 0; }

		// nw4r::snd::SoundHandle handle;
		// PlaySoundWithFunctionB4(SoundRelatedClass, &handle, Notes[play], 1);
		BonusMusicPlayer(play);
	}
	else {
		dSingAlong::instance->RegisterNote(this->note);
	}

	physics.setup(this, &physicsInfo, 3, currentLayerID);
	physics.addToList();

	doStateChange(&StateID_Wait);
}

void dSongBlock::calledWhenUpMoveExecutes() {
	if (initialY >= pos.y)
		blockWasHit(false);
}

void dSongBlock::calledWhenDownMoveExecutes() {
	if (initialY <= pos.y)
		blockWasHit(true);
}

void dSongBlock::glow() {
	isGlowing = 1;
	isDead = 0;
	glowTimer = 15;
}

void dSongBlock::unglow() {
	isDead = 1;
	Vec efPos = {pos.x, pos.y+8.0f, pos.z-100.0f};
	S16Vec nullRot = {0,0,0};
	Vec efScale = {0.4f, 0.4f, 0.4f};
	SpawnEffect("Wm_en_sanbohit_smk", 0, &efPos, &nullRot, &efScale);
}

void dSongBlock::beginState_Wait() {}
void dSongBlock::endState_Wait() {}
void dSongBlock::executeState_Wait() {
	int result = blockResult();

	if (result == 0)
		return;

	if (result == 1) {
		glow();
		doStateChange(&daEnBlockMain_c::StateID_UpMove);
		anotherFlag = 2;
		isGroundPound = false;
	} else {
		glow();
		doStateChange(&daEnBlockMain_c::StateID_DownMove);
		anotherFlag = 1;
		isGroundPound = true;
	}
}

