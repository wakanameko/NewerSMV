#include "koopatlas/starcoin.h"
#include <game.h>
#include <gameLanguage.h>

dWMStarCoin_c *dWMStarCoin_c::instance = 0;

dWMStarCoin_c *dWMStarCoin_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dWMStarCoin_c));
	dWMStarCoin_c *c = new(buffer) dWMStarCoin_c;

	instance = c;
	return c;
}

dWMStarCoin_c::dWMStarCoin_c() : state(this) {
	layoutLoaded = false;
	visible = false;
	state.setState(&StateID_Hidden);
}

CREATE_STATE(dWMStarCoin_c, Hidden);
CREATE_STATE(dWMStarCoin_c, ShowWait);
CREATE_STATE(dWMStarCoin_c, ShowSectionWait);
CREATE_STATE(dWMStarCoin_c, Wait);
CREATE_STATE(dWMStarCoin_c, HideSectionWait);
CREATE_STATE(dWMStarCoin_c, HideWait);

int dWMStarCoin_c::onCreate() {

	if (!layoutLoaded) {
		bool gotFile = layout.loadArc("StarCoins.arc", false);
		if (!gotFile)
			return false;

		bool output = layout.build("StarCoins.brlyt");

		layout.layout.rootPane->trans.x = -112.0f;
		if (IsWideScreen()) {
			layout.layout.rootPane->scale.x = 0.735f;
		} else {
			layout.clippingEnabled = true;
			layout.clipX = 0;
			layout.clipY = 52;
			layout.clipWidth = 640;
			layout.clipHeight = 352;
			layout.layout.rootPane->scale.x = 0.731f;
			layout.layout.rootPane->scale.y = 0.7711f;
		}

		static const char *brlanNames[] = {
			"StarCoins_Show.brlan",
			"StarCoins_ShowSection.brlan",
			"StarCoins_HideSection.brlan",
			"StarCoins_ShowArrow.brlan",
			"StarCoins_HideArrow.brlan",
		};
		static const char *groupNames[] = {
			"base", "section", "section", "leftArrow", "leftArrow", "rightArrow", "rightArrow"
		};

		layout.loadAnimations(brlanNames, 5);
		layout.loadGroups(groupNames, (int[7]){0, 1, 2, 3, 4, 3, 4}, 7);
		layout.disableAllAnimations();

		layout.drawOrder = 1;

		for (int col = 0; col < COLUMN_COUNT; col++) {
			for (int shine = 0; shine < SHINE_COUNT; shine++) {
				char name[8];
				sprintf(name, "Shine%d%d", col, shine);
				Shine[col][shine] = layout.findPictureByName(name);
			}

			for (int row = 0; row < ROW_COUNT; row++) {
				char lname[12];
				sprintf(lname, "LevelName%d%d", col, row);
				LevelName[col][row] = layout.findTextBoxByName(lname);

				char coname[16], cname[8];
				for (int i = 0; i < 3; i++) {
					sprintf(coname, "CoinOutline%d%d%d", col, row, i);
					CoinOutline[col][row][i] = layout.findPictureByName(coname);

					sprintf(cname, "Coin%d%d%d", col, row, i);
					Coin[col][row][i] = layout.findPictureByName(cname);
				}
			}
		}

		static const char *tbNames[] = {
			"LeftTitle", "RightTitle", "TotalCoinCount", "UnspentCoinCount",
			"EarnedCoinCount", "EarnedCoinMax", "BtnBackText",
			"TotalCoinsTitle", "UnspentTitle", // two containts added by wakanameko
		};
		layout.getTextBoxes(tbNames, &LeftTitle, 9);

		static const char *picNames[] = {
			"DPadLeft", "DPadRight",
		};
		layout.getPictures(picNames, &DPadLeft, 2);

		DPadLeft->SetVisible(false);
		DPadRight->SetVisible(false);

		layoutLoaded = true;
	}

	return true;
}


int dWMStarCoin_c::onDelete() {
	return layout.free();
}


void dWMStarCoin_c::show() {
	if (state.getCurrentState() == &StateID_Hidden)
		state.setState(&StateID_ShowWait);
}


int dWMStarCoin_c::onExecute() {
	state.execute();

	if (visible) {
		layout.execAnimations();
		layout.update();
	}

	return true;

}

int dWMStarCoin_c::onDraw() {
	if (visible)
		layout.scheduleForDrawing();

	return true;
}


void dWMStarCoin_c::showLeftArrow() {
	if (!isLeftArrowVisible) {
		isLeftArrowVisible = true;
		layout.enableNonLoopAnim(SHOW_LEFT_ARROW);
		DPadLeft->SetVisible(true);
	}
}

void dWMStarCoin_c::showRightArrow() {
	if (!isRightArrowVisible) {
		isRightArrowVisible = true;
		layout.enableNonLoopAnim(SHOW_RIGHT_ARROW);
		DPadRight->SetVisible(true);
	}
}

void dWMStarCoin_c::hideLeftArrow() {
	if (isLeftArrowVisible) {
		isLeftArrowVisible = false;
		layout.enableNonLoopAnim(HIDE_LEFT_ARROW);
		DPadLeft->SetVisible(false);
	}
}

void dWMStarCoin_c::hideRightArrow() {
	if (isRightArrowVisible) {
		isRightArrowVisible = false;
		layout.enableNonLoopAnim(HIDE_RIGHT_ARROW);
		DPadRight->SetVisible(false);
	}
}

void dWMStarCoin_c::setLeftArrowVisible(bool value) {
	if (value)
		showLeftArrow();
	else
		hideLeftArrow();
}

void dWMStarCoin_c::setRightArrowVisible(bool value) {
	if (value)
		showRightArrow();
	else
		hideRightArrow();
}


bool dWMStarCoin_c::canScrollLeft() const {
	return (currentSectionIndex > 0);
}
bool dWMStarCoin_c::canScrollRight() const {
	return (currentSectionIndex < (availableSectionCount - 1));
}

// HARD CODINGS IN HERE!!
void dWMStarCoin_c::loadInfo() {
	WriteBMGToTextBox(BtnBackText, GetBMG(), 3, 1, 0);

	if (SetGameLanguage == 0){	// EN messages by NewerTeam
		TotalCoinsTitle->SetString(L"StarCoins:");
		UnspentTitle->SetString(L"unspent");
	}
	if (SetGameLanguage == 1){	// JP messages by wakanameko 
		TotalCoinsTitle->SetString(L"スターコイン:");
		UnspentTitle->SetString(L"ぜん体のコイン");
	}
	if (SetGameLanguage == 2){	// DE messages by Vadenimo
		TotalCoinsTitle->SetString(L"Sternenmünzen:");
		UnspentTitle->SetString(L"unverbraucht");
	}
	if (SetGameLanguage == 3){	// IT messages by Jacopo Plays
		TotalCoinsTitle->SetString(L"Monete Stella:");
		UnspentTitle->SetString(L"Non Spese");
	}

	int unspentCoins = getUnspentStarCoinCount();
	int coins = getStarCoinCount();

	WriteNumberToTextBox(&unspentCoins, UnspentCoinCount, false);
	WriteNumberToTextBox(&coins, TotalCoinCount, false);

	currentSection = -1;
	currentSectionIndex = -1;
	availableSectionCount = 0;

	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	int wantedSection = save->newerWorldID;

	// figure out which sections should be available
	for (int i = 0; i < dLevelInfo_c::s_info.sectionCount(); i++) {
		dLevelInfo_c::section_s *section = dLevelInfo_c::s_info.getSectionByIndex(i);

		bool haveLevels = false;
		for (int j = 0; j < section->levelCount; j++) {
			dLevelInfo_c::entry_s *l = &section->levels[j];
			if (l->flags & 2) {
				if (save->GetLevelCondition(l->worldSlot, l->levelSlot) & COND_UNLOCKED) {
					haveLevels = true;
					break;
				}
			}
		}

		if (haveLevels) {
			if (i == wantedSection) {
				currentSection = wantedSection;
				currentSectionIndex = availableSectionCount;
			}
			sectionIndices[availableSectionCount++] = i;
		}
	}

	// if we didn't find the wanted one, use the first one available
	if (currentSectionIndex == -1) {
		currentSectionIndex = 0;
		currentSection = sectionIndices[0];
	}
}

void dWMStarCoin_c::loadSectionInfo() {
	dLevelInfo_c::entry_s *visibleLevels[COLUMN_COUNT][ROW_COUNT];

	// reset everything... everything
	for (int i = 0; i < COLUMN_COUNT; i++) {
		for (int j = 0; j < SHINE_COUNT; j++)
			Shine[i][j]->SetVisible(false);

		for (int j = 0; j < ROW_COUNT; j++) {
			visibleLevels[i][j] = 0;

			LevelName[i][j]->SetVisible(false);

			for (int k = 0; k < 3; k++) {
				CoinOutline[i][j][k]->SetVisible(false);
				Coin[i][j][k]->SetVisible(false);
			}
		}
	}

	// get everything we'll need
	SaveBlock *save = GetSaveFile()->GetBlock(-1);
	dLevelInfo_c *linfo = &dLevelInfo_c::s_info;

	dLevelInfo_c::entry_s *names[COLUMN_COUNT];
	for (int i = 0; i < COLUMN_COUNT; i++)
		names[i] = linfo->searchByDisplayNum(currentSection, 100+i);

	bool useSubworlds = (COLUMN_COUNT > 1) && names[1];

	int currentPosition[COLUMN_COUNT];
	int currentColumn = 0; // only incremented in single-subworld mode

	for (int i = 0; i < COLUMN_COUNT; i++)
		currentPosition[i] = 0;

	dLevelInfo_c::section_s *section = linfo->getSectionByIndex(currentSection);

	int earnedCoins = 0, earnableCoins = 0;
	// earnedCoins is calculated later

	for (int i = 0; i < section->levelCount; i++) {
		dLevelInfo_c::entry_s *level = &section->levels[i];

		// only pay attention to real levels
		if (!(level->flags & 2))
			continue;

		earnableCoins += 3;

		// is this level unlocked?
		u32 conds = save->GetLevelCondition(level->worldSlot, level->levelSlot);

		if (!(conds & COND_UNLOCKED))
			continue;

		// well, let's give it a slot
		if (useSubworlds) {
			currentColumn = (level->flags & 0x400) ? 1 : 0;
		} else {
			if (currentPosition[currentColumn] >= ROW_COUNT)
				currentColumn++;
		}

		visibleLevels[currentColumn][currentPosition[currentColumn]++] = level;
	}

	// if the first column is empty, then move the second one over
	if (currentPosition[0] == 0 && useSubworlds) {
		for (int i = 0; i < currentPosition[1]; i++) {
			visibleLevels[0][i] = visibleLevels[1][i];
			visibleLevels[1][i] = 0;
		}

		names[0] = names[1];
		names[1] = 0;
	}

	// if the second column is empty, remove its name
	if (currentPosition[1] == 0 && useSubworlds)
		names[1] = 0;
	
	// four lines from SLLW by rsm(RedStoneMatt)
	WriteWorldNameToTextBox(LeftTitle, currentSection);
	if (names[1])
		WriteAsciiToTextBox(RightTitle, linfo->getNameForLevel(names[1]));
	RightTitle->SetVisible(names[1] != 0);

	// work out the names
	// from SLLW
	WriteAsciiToTextBox(LeftTitle, linfo->getNameForLevel(names[0]));
	// WriteAsciiToTextBox(LeftTitle, currentSection);
	if (names[1])
		WriteAsciiToTextBox(RightTitle, linfo->getNameForLevel(names[1]));
	RightTitle->SetVisible(names[1] != 0);

	// load all level info
	for (int col = 0; col < COLUMN_COUNT; col++) {
		for (int row = 0; row < ROW_COUNT; row++) {
			dLevelInfo_c::entry_s *level = visibleLevels[col][row];
			if (!level)
				continue;

			u32 conds = save->GetLevelCondition(level->worldSlot, level->levelSlot);

			if (!(row & 1)) {
				int shineID = row / 2;
				if (shineID < SHINE_COUNT)
					Shine[col][shineID]->SetVisible(true);
			}

			for (int coin = 0; coin < 3; coin++) {
				CoinOutline[col][row][coin]->SetVisible(true);

				if (conds & (COND_COIN1 << coin)) {
					Coin[col][row][coin]->SetVisible(true);
					earnedCoins++;
				}
			}

			LevelName[col][row]->SetVisible(true);
			WriteAsciiToTextBox(LevelName[col][row], linfo->getNameForLevel(level));
		}
	}

	// set up coin things
	WriteNumberToTextBox(&earnedCoins, EarnedCoinCount, false);
	WriteNumberToTextBox(&earnableCoins, EarnedCoinMax, false);
}


void dWMStarCoin_c::beginState_Hidden() { }
void dWMStarCoin_c::executeState_Hidden() { }
void dWMStarCoin_c::endState_Hidden() { }

static const int secretCode[] = {
	WPAD_UP,WPAD_UP,WPAD_DOWN,WPAD_DOWN,
	WPAD_LEFT,WPAD_RIGHT,WPAD_LEFT,WPAD_RIGHT,
	WPAD_ONE,WPAD_TWO,0
};
static const int secretCodeButtons = WPAD_UP|WPAD_DOWN|WPAD_LEFT|WPAD_RIGHT|WPAD_ONE|WPAD_TWO;
static int secretCodeIndex = 0;
static int minusCount = 0;
static int plusCount = 0;
extern bool enableHardMode;
extern bool enableDebugMode;
//extern bool enableCheatMode;
extern u8 isReplayEnabled;

void dWMStarCoin_c::beginState_ShowWait() {
	visible = true;
	loadInfo();
	layout.enableNonLoopAnim(SHOW_ALL);
	layout.resetAnim(SHOW_SECTION);
	layout.resetAnim(SHOW_LEFT_ARROW);
	layout.resetAnim(SHOW_RIGHT_ARROW);

	secretCodeIndex = 0;
	minusCount = 0;
	plusCount = 0;
}
void dWMStarCoin_c::executeState_ShowWait() {
	if (!layout.isAnimOn(SHOW_ALL))
		state.setState(&StateID_ShowSectionWait);
}
void dWMStarCoin_c::endState_ShowWait() { }

void dWMStarCoin_c::beginState_ShowSectionWait() {
	loadSectionInfo();
	layout.enableNonLoopAnim(SHOW_SECTION);

	if (canScrollLeft())
		showLeftArrow();
	if (canScrollRight())
		showRightArrow();
}
void dWMStarCoin_c::executeState_ShowSectionWait() {
	if (!layout.isAnimOn(SHOW_SECTION))
		state.setState(&StateID_Wait);
}
void dWMStarCoin_c::endState_ShowSectionWait() { }

void dWMStarCoin_c::showSecretMessage(const wchar_t *title, const wchar_t **body, int lineCount, const wchar_t **body2, int lineCount2) {
	LeftTitle->SetVisible(true);
	LeftTitle->SetString(title);
	RightTitle->SetVisible(false);

	for (int c = 0; c < COLUMN_COUNT; c++) {
		for (int i = 0; i < SHINE_COUNT; i++)
			Shine[c][i]->SetVisible(false);
		for (int r = 0; r < ROW_COUNT; r++) {
			LevelName[c][r]->SetVisible(false);
			for (int i = 0; i < 3; i++) {
				CoinOutline[c][r][i]->SetVisible(false);
				Coin[c][r][i]->SetVisible(false);
			}
		}
	}

	for (int i = 0; i < lineCount; i++) {
		LevelName[0][i]->SetVisible(true);
		LevelName[0][i]->SetString(body[i]);
	}

	if (body2) {
		for (int i = 0; i < lineCount2; i++) {
			LevelName[1][i]->SetVisible(true);
			LevelName[1][i]->SetString(body2[i]);
		}
	}
}

void dWMStarCoin_c::beginState_Wait() { }
void dWMStarCoin_c::executeState_Wait() {
	int nowPressed = Remocon_GetPressed(GetActiveRemocon());

	if ((GetActiveRemocon()->heldButtons == 0x810) && (nowPressed & 0x810)) { // A, B, and Plus
		const int lineCountOn = 9, lineCountOff = 2;

		//Turned on messages
		static const wchar_t *hardModeEN[9] = {	// EN messages by NewerTeam
			L"You've activated Hard Mode!",
			L" ",
			L"In Hard Mode, Mario will die",
			L"any time he takes damage, and",
			L"the timer will be more strict.",
			L" ",
			L"So treasure your Yoshi, and",
			L"hold on to your hat, you're",
			L"in for a wild ride!",
		};
		static const wchar_t *hardModeJP[9] = {	// JP messages by wakanameko
			L"ハードモードへようこそ!",
			L" ",
			L"ハードモードでは、マリオが",
			L"いちげきでやられます。また、",
			L"じかんせいげんがきびしく",
			L"なります。",
			L"こんなにむずかしいマリオは",
			L"ほかにないでしょう。",
			L"うでだめしをしてみましょう!",
		};
		static const wchar_t *hardModeDE[9] = {	// DE messages by Vadenimo
			L"Der Harte Modus ist aktiviert!",
			L" ",
			L"Im Harten Modus stirbt Mario,",
			L"wenn er Schaden zu sich nimmt.",
			L"Das Zeitlimit ist auch strenger!",
			L" ",
			L"Nimm Yoshi mit, halte deine",
			L"Mütze fest, es wird Zeit",
			L"für ein wildes Abenteuer!",
		};
		static const wchar_t *hardModeIT[9] = {	// IT messages by Jacopo Plays
			L"Hai attivato la modalità difficile!",
			L" ",
			L"Nella modalità difficile mario",
			L"morirà ogni volta che prende",
			L"danno, e il timer sarà più corto.",
			L" ",
			L"Quindi prendi il tuo Yoshi, e",
			L"tieni il cappello, sei",
			L"in una corsa scatenata!",
		};
		//Turned off messages
		static const wchar_t *hardModeOffEN[2] = {	// EN messages by NewerTeam
			L"Hard Mode has been",
			L"turned off.",
		};
		static const wchar_t *hardModeOffJP[2] = {	// JP messages by wakanameko
			L"ハードモードがオフになり",
			L"ました。",
		};
		static const wchar_t *hardModeOffDE[2] = {	// DE messages by Vadenimo
			L"Der Harte Modus",
			L"wurde deaktiviert.",
		};
		static const wchar_t *hardModeOffIT[2] = {	// IT messages by Jacopo Plays
			L"La modalità difficile è stata",
			L"disattivata.",
		};

		if (!enableHardMode) {
			enableHardMode = true;
			OSReport("Hard Mode enabled!\n");
			SFX courseHardSFX[] = {SE_VOC_MA_CS_COURSE_IN_HARD, SE_VOC_LU_CS_COURSE_IN_HARD, SE_VOC_KO_CS_COURSE_IN_HARD, SE_VOC_KO2_CS_COURSE_IN_HARD};
			MapSoundPlayer(SoundRelatedClass, courseHardSFX[Player_ID[0]], 1);
			if (SetGameLanguage == 0){	// EN messages by NewerTeam
				showSecretMessage(L"Hard Mode", hardModeEN, lineCountOn);
			}
			if (SetGameLanguage == 1){	// JP message by wakanameko
				showSecretMessage(L"ハードモード", hardModeJP, lineCountOn);
			}
			if (SetGameLanguage == 2){	// DE message by Vadenimo
				showSecretMessage(L"Harter Modus", hardModeDE, lineCountOn);
			}
			if (SetGameLanguage == 3){	// IT message by Jacopo Plays
				showSecretMessage(L"Modalità difficile", hardModeIT, lineCountOn);
			}
		} else {
			enableHardMode = false;
			OSReport("Hard Mode disabled!\n");
			if (SetGameLanguage == 0){	// EN message by NewerTeam
				showSecretMessage(L"Classic Mario", hardModeOffEN, lineCountOff);
			}
			if (SetGameLanguage == 1){	// JP message by wakanameko
				showSecretMessage(L"クラシックモード", hardModeOffJP, lineCountOff);
			}
			if (SetGameLanguage == 2){	// DE message by Vadenimo
				showSecretMessage(L"Klassischer Modus", hardModeOffDE, lineCountOff);
			}
			if (SetGameLanguage == 3){	// IT message by Jacopo Plays
				showSecretMessage(L"Mario Classico", hardModeOffIT, lineCountOff);
			}
		}
		return;
	}

	if (nowPressed & secretCodeButtons) {
		int nextKey = secretCode[secretCodeIndex];
		if (nowPressed & nextKey) {
			secretCodeIndex++;
			if (secretCode[secretCodeIndex] == 0) {
				secretCodeIndex = 0;
				MapSoundPlayer(SoundRelatedClass, SE_VOC_MA_THANK_YOU, 1);
				//enableDebugMode = !enableDebugMode;
				//OSReport("Debug mode toggled!\n");
				const int lineCountOn = 9, lineCountOff = 2;
				const int lineCountOn2 = 4;

				//Turned on messages
				static const wchar_t *replayRecordEN[9] = {	// EN messages by NewerTeam
					L"The experimental Replay",
					L"Recording feature has",
					L"been enabled. Enjoy!",
					L"You'll find your Replays",
					L"on your SD or USB, depending",
					L"on where Newer's files are.",
					L"It might not work, so",
					L"save your game before you",
					L"play a level!",
				};
				static const wchar_t *replayRecordJP[9] = {	// JP messages by wakanameko
					L"みかんせいのリプレイろくが",
					L"きのうが、オンになりました",
					L" ",
					L"あなたのリプレイは、SDカー",
					L"ドかUSBメモリにほぞんされ",
					L"ます。",
					L"ただし、ふあんていなため、",
					L"ステージをあそぶまえに、セ",
					L"ーブしておきましょう。",
				};
				static const wchar_t *replayRecordDE[lineCountOn] = {	// DE messages by Vadenimo
					L"Das experimentelle",
					L"Aufnehmen",
					L"des Replays wurde",
					L"aktiviert. Viel Spaß!",
					L"Die Aufnahmen kannst du",
					L"auf deinem USB-Stick",
					L"oder deiner SD-Karte",
					L"finden, wo das Spiel",
					L"lolakisiert ist. Es könnte",
				};
				static const wchar_t *replayRecordDE2[lineCountOn2] = {	// DE messages section2 by Vadenimo
					L"nicht gehen, also",
					L"ist es wichtig dein ",
					L"Spiel zu speichern,",
					L"bevor du ein Level spielst.",
				};
				static const wchar_t *replayRecordIT[9] = {	// IT messages by Jacopo Plays
					L"Il replay sperimentale",
					L"La funzione di registrazione è",
					L"stata abilitata. Goditela!",
					L"Troverai i tuoi replay",
					L"sulla tua SD o USB, a seconda",
					L"su dove sono i file di Newer.",
					L"Potrebbe non funzionare, quindi",
					L"salva il gioco prima di",
					L"giocare un livello!",
				};
				//Turned off messages
				static const wchar_t *replayRecordOffEN[2] = {	// EN messages by NewerTeam
					L"Replay Recording",
					L"turned off.",
				};
				static const wchar_t *replayRecordOffJP[2] = {	// JP messages by wakanameko
					L"リプレイろくがきのうがオフ",
					L"になりました。",
				};
				static const wchar_t *replayRecordOffDE[2] = {	// DE messages by Vadenimo
					L"Das Aufnehmen des Replays",
					L"wurde deaktiviert.",
				};
				static const wchar_t *replayRecordOffIT[2] = {	// IT messages by Jacopo Plays
					L"Registrazione del Replay",
					L"disattivata.",
				};

				if (isReplayEnabled != 100) {
					isReplayEnabled = 100;
					OSReport("Replay Recording enabled!\n");
					if (SetGameLanguage == 0){	// EN message by NewerTeam
						showSecretMessage(L"Nice!", replayRecordEN, lineCountOn);
					}
					if (SetGameLanguage == 1){	// JP message by wakanameko
						showSecretMessage(L"リプレイろくが", replayRecordJP, lineCountOn);
					}
					if (SetGameLanguage == 2){	// DE message by Vadenimo
						showSecretMessage(L"Toll!", replayRecordDE, lineCountOn, replayRecordDE2, lineCountOn2);
					}
					if (SetGameLanguage == 3){	// IT message by Jacopo Plays
						showSecretMessage(L"Bene!", replayRecordIT, lineCountOn);
					}
				} else {
					isReplayEnabled = 0;
					OSReport("Replay Recording disabled!\n");
					if (SetGameLanguage == 0){	// EN message by NewerTeam
						showSecretMessage(L"Nice!", replayRecordOffEN, lineCountOff);
					}
					if (SetGameLanguage == 1){	// JP message by wakanameko
						showSecretMessage(L"リプレイろくが", replayRecordOffJP, lineCountOff);
					}
					if (SetGameLanguage == 2){	// DE message by Vadenimo
						showSecretMessage(L"Toll!", replayRecordOffDE, lineCountOff);
					}
					if (SetGameLanguage == 3){	// IT message by Jacopo Plays
						showSecretMessage(L"Bene!", replayRecordOffIT, lineCountOff);
					}
				}
			}
			return;
		} else {
			secretCodeIndex = 0;
		}
	}

	if (nowPressed & WPAD_MINUS) {
		minusCount++;
		if (minusCount >= 16) {
			minusCount = 0;

			enableDebugMode = !enableDebugMode;

			if (enableDebugMode) {
				MapSoundPlayer(SoundRelatedClass, SE_VOC_MA_GET_PRIZE, 1);
				const int msgCount = 9;
				const int msgCount2 = 9;

				//turned on messages
				static const wchar_t *debugModeEN[9] = {	// EN messages by NewerTeam
					L"You've found the Totally",
					L"Secret Collision Debug Mode.",
					L"We used this to make the",
					L"hitboxes on our custom sprites",
					L"and bosses suck less. Awesome,",
					L"right?!",
					L"Actually, I did it just to waste",
					L"some time, but it ended up",
					L"being pretty useful!",
				};
				static const wchar_t *debugModeEN2[9] = {
					L"And yes, I know it doesn't show",
					L"a couple of things properly",
					L"like round objects and rolling",
					L"hills and so on.",
					L"Can't have it all, can you?",
					L"Wonder if Nintendo had",
					L"something like this...",
					L"",
					L"    Treeki, 9th February 2013",
				};

				static const wchar_t *debugModeJP[9] = {	// JP messages by wakanameko
					L"あなたは、ひみつのデバッグ",
					L"モードをはっけんしました!",
					L"これをつかうことで、てきや",
					L"アイテム、じぶんのあたりは",
					L"んていをひょうじできます!",
					L" ",
					L"きっとなにかのやくにたつで",
					L"しょう。",
					L" ",
				};
				static const wchar_t *debugModeJP2[9] = {
					L"あ、これまるいものや、アー",
					L"チじょうになっているものは",
					L"しっかりひょうじできないか",
					L"もしれません。",
					L" ",
					L"あと、wakanamekoはこのモー",
					L"ドのどこかにチートきのうを",
					L"ついかしたみたいですよ?",
					L"    wakanameko, 2023/6/16",
				};

				static const wchar_t *debugModeDE[9] = {	// DE messages by Vadenimo
					L"Du hast den total geheimen",
					L"Kollisionsdebugmodus aktiviert.",
					L"Wir nutzten ihn,um die Hitboxen",
					L"der Gegner und Bosse weniger",
					L"beschissen zu machen. Grandios,",
					L"oder?!",
					L"Nun, es brauchte die Zeit, aber",
					L"am Ende hat sich herausgestellt,",
					L"dass es sinnvoll ist!",
				};
				static const wchar_t *debugModeDE2[9] = {
					L"Und ja, mir ist bewusst, dass",
					L"ein paar Sachen nicht angezeigt",
					L"werden, wie runde Objekte,",
					L"rollende Hügel und mehr.",
					L"Ihr könnt nicht alles haben, oder?",
					L"Es wundert mich, ob Nintendo",
					L"sowas Ähnliches hatte...",
					L"",
					L"    Treeki, 9. Februar 2013",
				};

				static const wchar_t *debugModeIT[9] = {	// IT messages by Jacopo Plays
					L"Hai trovato Totalmente",
					L"Modalità di debug di collisione.",
					L"Abbiamo usato questo per fare",
					L"hitbox sui nostri sprite e i capi",
					L"fanno meno schifo. Eccezionale",
					L"Giusto?!",
					L"Lho fatto solo per sprecare",
					L"un po' di tempo, ma è finita",
					L"essendo abbastanza utile!",
				};
				static const wchar_t *debugModeIT2[9] = {
					L"E sì, lo so che non si vede",
					L"correttamente un paio di cose",
					L"come oggetti rotondi, colline",
					L"rotolanti e così via.",
					L"Non puoi avere tutto, vero?",
					L"Chissà se Nintendo l'aveva",
					L"qualcosa come questo...",
					L"",
					L"    Treeki, 9 Febbraio 2013",
				};

				if (SetGameLanguage == 0){	//EN message by NewerTeam
					showSecretMessage(L"Groovy!", debugModeEN, msgCount, debugModeEN2, msgCount2);
				}
				if (SetGameLanguage == 1){	//JP message by wakanameko
					showSecretMessage(L"デバッグ", debugModeJP, msgCount, debugModeJP2, msgCount2);	
				}
				if (SetGameLanguage == 2){	//DE message by Vadenimo
					showSecretMessage(L"Groovig!", debugModeDE, msgCount, debugModeDE2, msgCount2);
				}
				if (SetGameLanguage == 3){	//IT message by Jacopo Plays
					showSecretMessage(L"Fantastico!", debugModeIT, msgCount, debugModeIT2, msgCount2);
				}
				

			} else {
				const int msgCount = 6;
				const int msgCountDE = 7;
				const int msgCountIT = 8;
				//Turned off messages
				
				static const wchar_t *debugModeOffEN[msgCount] = {	// EN messages by NewerTeam
					L"You've turned off the Totally",
					L"Secret Collision Debug Mode.",
					L"",
					L"... and no, I'm not going to write",
					L"another ridiculously long",
					L"message to go here. Sorry!",
				};
				static const wchar_t *hiddenMsgEN[] = {
					L"If you found these messages by",
					L"looking through strings in the DLCode",
					L"file, then... that's kind of cheating.",
					L"Though I can't say I wouldn't do the",
					L"same!",
					L"You won't actually see this in game",
					L"btw :p So why am I bothering with linebreaks anyway? I dunno. Oh well.",
					L"Also, don't put this message on TCRF. Or do! Whatever. :(",
				};

				static const wchar_t *debugModeOffJP[msgCount] = {	// JP messages by wakanameko
					L"デバッグモードがオフになり",
					L"ました。",
					L" ",
					L"...あの、",
					L"バカみてえにながいメッセー",
					L"ジかいて、ごめんなさい!",
				};
				static const wchar_t *hiddenMsgJP[] = {
					L"チート機能に関してですが、",
					L"本来は使い方をスターコイン",
					L"画面に表示するはずだったん",
					L"です。しかし、",
					L"翻訳チームに翻訳を頼むのが",
					L"面倒になってしまい、実装さ",
					L"れることはありませんでした。",
					L"自分でコード読んで使い方を覚えてください。",
				};

				static const wchar_t *debugModeOffDE[msgCountDE] = {	// DE messages by Vadenimo
					L"Du hast den total geheimen",
					L"Kollisionsdebugmodus",
					L"deaktiviert.",
					L" ",
					L"...Und nein, ich werde keine",
					L"lange Nachricht verfassen,",
					L"um zu verschwinden. Sorry!",
				};
				static const wchar_t *hiddenMsgDE[] = {
					L"Wenn du diese Nachricht durchs Finden",
					L"der Strings im DLCode fandest, dann...",
					L"ist das eine Form des Cheatings.",
					L"Auch wenn ich dasselbe machen würde,",
					L"wenn ich ehrlich bin!",
					L"Du wirst diese Nachricht nicht im Spiel sehen,",
					L"btw. :p So, wieso nerve ich trotzdem",
					L"mit Satzbrüchen? Kein Plan. Nun denn.",
					L"Noch was, bitte bringt den Text nicht auf TCRF.",
					L"Oder macht es doch! Was auch immer. :(",
				};

				static const wchar_t *debugModeOffIT[msgCountIT] = {	// IT messages by Jacopo Plays
					L"Hai disattivato Totally",
					L"Modalità debug di",
					L"collisione segreta.",
					L"",
					L"... e no, non scriverò",
					L"un altro ridicolmente lungo",
					L"messaggio per andare qui.",
					L"Scusa!",
				};
				static const wchar_t *hiddenMsgIT[] = {
					L"Se hai trovato questi messaggi di",
					L"guardando attraverso le stringhe nel DLCode",
					L"file, allora... è una specie di imbroglio.",
					L"Anche se non posso dire che non lo farei",
					L"Stesso!",
					L"In realtà non lo vedrai nel gioco",
					L"btw: p Allora perché mi preoccupo comunque delle interruzioni di riga?",
					L"Non so. Vabbè.",
					L"Inoltre, non mettere questo messaggio su TCRF. Oppure fallo!",
					L"Qualunque cosa. :(",
				};

				if (SetGameLanguage == 0){	//EN message by NewerTeam
					showSecretMessage(L"Groovy!", debugModeOffEN, msgCount, hiddenMsgEN, 0);
				}
				if (SetGameLanguage == 1){	//JP message by wakanameko
					showSecretMessage(L"デバッグ", debugModeOffJP, msgCount, hiddenMsgJP, 0);
				}
				if (SetGameLanguage == 2){	//DE message by Vadenimo
					showSecretMessage(L"Groovig!", debugModeOffDE, msgCountDE, hiddenMsgDE, 0);
				}
				if (SetGameLanguage == 3){	//IT message by Jacopo Plays
					showSecretMessage(L"Fantastico!", debugModeOffIT, msgCountIT, hiddenMsgIT, 0);
				}
			}
		}
	}
	
	// Secret Command 
	if (nowPressed & WPAD_PLUS) {
		if (enableDebugMode) {
			plusCount++;
			if (plusCount >= 5) {
				plusCount = 0;
				DoSceneChange(WM_IBARA, 0, 0); //enter the Asu's sound test room. but this function is broken
			}
		}
	}
	//*/
	
	else if (nowPressed & WPAD_ONE) {
		MapSoundPlayer(SoundRelatedClass, SE_SYS_DIALOGUE_OUT_AUTO, 1);
		willExit = true;
		state.setState(&StateID_HideSectionWait);
	} else if ((nowPressed & WPAD_LEFT) && canScrollLeft()) {
		currentSection = sectionIndices[--currentSectionIndex];
		willExit = false;
		state.setState(&StateID_HideSectionWait);
	} else if ((nowPressed & WPAD_RIGHT) && canScrollRight()) {
		currentSection = sectionIndices[++currentSectionIndex];
		willExit = false;
		state.setState(&StateID_HideSectionWait);
	}

	
}


void dWMStarCoin_c::endState_Wait() { }

void dWMStarCoin_c::beginState_HideSectionWait() {
	layout.enableNonLoopAnim(HIDE_SECTION);
	if (willExit) {
		hideLeftArrow();
		hideRightArrow();
	} else {
		setLeftArrowVisible(canScrollLeft());
		setRightArrowVisible(canScrollRight());
	}
}
void dWMStarCoin_c::executeState_HideSectionWait() {
	if (!layout.isAnimOn(HIDE_SECTION)) {
		if (willExit)
			state.setState(&StateID_HideWait);
		else
			state.setState(&StateID_ShowSectionWait);
	}
}
void dWMStarCoin_c::endState_HideSectionWait() { }

void dWMStarCoin_c::beginState_HideWait() {
	layout.enableNonLoopAnim(SHOW_ALL, true);
	layout.grpHandlers[SHOW_ALL].frameCtrl.flags = 3; // NO_LOOP | REVERSE
}
void dWMStarCoin_c::executeState_HideWait() {
	if (!layout.isAnimOn(SHOW_ALL))
		state.setState(&StateID_Hidden);
}
void dWMStarCoin_c::endState_HideWait() {
	visible = false;
}

