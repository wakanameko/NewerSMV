#include <gameLanguage.h>
#include "languageSelect.h"

dScLanguageSelect_c *dScLanguageSelect_c::instance = 0;

dScLanguageSelect_c *dScLanguageSelect_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dScLanguageSelect_c));
	dScLanguageSelect_c *c = new(buffer) dScLanguageSelect_c;

	instance = c;
	return c;
}

int dScLanguageSelect_c::onCreate() {
	*CurrentDrawFunc = languageSelectDrawFunc;

	if(!layoutLoaded) {
		if (!layout.loadArc("soundtest.arc", false))
			return false;

		static const char *brlanNames[4] = {
			"soundtest_asuAppear.brlan",
			"soundtest_upText.brlan",
			"soundtest_idleText.brlan",
			"soundtest_downText.brlan",
		};

		static const char *groupNames[4] = {
			"A00_asu", 
			"B00_textBox", "B00_textBox", "B00_textBox",
		};

		static const int groupIDs[4] = {
			0,
			1, 2, 3,
		};

		layout.build("soundtest.brlyt");

		layout.loadAnimations(brlanNames, 4);
		layout.loadGroups(groupNames, groupIDs, 4);
		layout.disableAllAnimations();

		// layout.drawOrder = 140;

		layoutLoaded = true;

		T_SFXTxt_00 = layout.findTextBoxByName("T_SFXTxt_00");
		T_SFXSha_00 = layout.findTextBoxByName("T_SFXSha_00");
		T_SFXNumTxt_00 = layout.findTextBoxByName("T_SFXNumTxt_00");
		T_SFXNumSha_00 = layout.findTextBoxByName("T_SFXNumSha_00");

		WriteBMGToTextBox(layout.findTextBoxByName("T_WelcomeTxt_00"), GetBMG(), 0x3EA, 19, 0);
		WriteBMGToTextBox(layout.findTextBoxByName("T_WelcomeSha_00"), GetBMG(), 0x3EA, 19, 0);
		WriteBMGToTextBox(layout.findTextBoxByName("T_InfosTxt_00"), GetBMG(), 0x3EA, 20, 0);
		WriteBMGToTextBox(layout.findTextBoxByName("T_InfosSha_00"), GetBMG(), 0x3EA, 20, 0);
	}

	isVisible = true;

	return true;
}

int dScLanguageSelect_c::onDelete() {
	return layout.free();
}

extern void callSoundTestState(dScLanguageSelect_c* handler, u32 state);

int dScLanguageSelect_c::onExecute() {
}

int dScLanguageSelect_c::onDraw() {
	if (isVisible) {
		layout.scheduleForDrawing();
	}

	return true;
}

void languageSelectDrawFunc() {
	Reset3DState();
	
	LinkScene(0);
	DrawOpa();
	DrawXlu();
	UnlinkScene(0);

	SetupLYTDrawing();
	DrawAllLayoutsBeforeX(0x81);
	RenderEffects(0, 3);
	RenderEffects(0, 2);
	GXDrawDone();
	RemoveAllFromScnRoot();
	Reset3DState();
	SetCurrentCameraID(1);
	DoSpecialDrawing1();

	LinkScene(1);
	DrawOpa();
	DrawXlu();
	UnlinkScene(1);

	SetCurrentCameraID(0);
	for (int i = 0; i < 4; i++)
		RenderEffects(0, 0xB+i);
	for (int i = 0; i < 4; i++)
		RenderEffects(0, 7+i);
	GXDrawDone();
	// Leaving out some stuff here
	DrawAllLayoutsAfterX(0x80);
	ClearLayoutDrawList();
	SetCurrentCameraID(0);
}

void dScLanguageSelect_c::exit() {
	ActivateWipe(WIPE_FADE);
	DoSceneChange(WORLD_9_DEMO, 0, 0);
}

