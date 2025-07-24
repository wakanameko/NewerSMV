#include "messageWindow.h"

CREATE_STATE(dMessageWindow_c, Init);
CREATE_STATE(dMessageWindow_c, Wait);
CREATE_STATE(dMessageWindow_c, End);

dMessageWindow_c *dMessageWindow_c::instance = 0;
dActor_c *dMessageWindow_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dMessageWindow_c));
	dMessageWindow_c *c = new(buffer) dMessageWindow_c;

	instance = c;
	return c;
}

const SpriteData messageWindowData = {ProfileId::MESSAGEWINDOW, 0, 0, 0, 0, 0x1000, 0x1000, 0, 0, 0, 0, 2}; 
Profile messageWindowProfile(&dMessageWindow_c::build, SpriteId::MESSAGEWINDOW, &messageWindowData, ProfileId::MESSAGEWINDOW, ProfileId::MESSAGEWINDOW, "MESSAGEWINDOW", NULL, 0);


int dMessageWindow_c::onCreate() {
	// Related layout
	if(!layoutLoaded) {
		if (!layout.loadArc("messageWindow/messageWindow.arc", false))	// Load original layout file
			return false;

		static const char *brlanNames[3] = {
			"messageWindow_02_inWindow.brlan",
			"messageWindow_02_loopWindow.brlan",
			"messageWindow_02_outWindow.brlan",
		};

		static const char *groupNames[1] = {
			"A00_inWindow",
		};

		static const int groupIDs[1] = {
			0,
		};

		layout.build("messageWindow_02.brlyt");

		layout.loadAnimations(brlanNames, 3);
		layout.loadGroups(groupNames, groupIDs, 1);
		layout.disableAllAnimations();
		
		layout.drawOrder = 0x13;

		layoutLoaded = true;

		rootPane = layout.getRootPane();

		// Adjust scale
		rootPane->scale.x += 0.18;
		rootPane->scale.y += 0.18;

		T_text[0] = layout.findTextBoxByName("T_message_00");
		T_text[1] = layout.findTextBoxByName("T_messageS_00");
	}
	isVisible = true;
    OSReport("Loaded messageWindow.arc correctly!\n");

	// Sprite settings
	this->messageType = this->settings >> 28 & 0xFF;// 0000 "0"000 0000 0000	// nybble 5
	int nybble6 = this->settings >> 24 & 0xF;		// 0000 000"0" 0000 0000	// nybble 6
	this->isdebug = nybble6 & 0b1;					// 0001						// nybble 6.4
	this->textID = ((this->settings >> 20 & 0xFF) << 4) | (this->settings >> 16 & 0xFF);	// 0000 00"00" 0000 0000	// Nybble 7-8
	this->category = ((this->settings >> 8 & 0xFF) << 8) | ((this->settings >> 4 & 0xFF) << 4) | (this->settings >> 0 & 0xFF);	// 0000 0000 0"000" 0000	// Nybble 10-12
    OSReport("isDebug: %d\n", this->isdebug);
    OSReport("TextID: %d\n", this->textID);
    OSReport("Category: %d\n", this->category);

	return true;
}


int dMessageWindow_c::onDelete() {
	return layout.free();
}

int dMessageWindow_c::onDraw() {
	if (isVisible) {
		layout.scheduleForDrawing();
	}

	return true;
}

int dMessageWindow_c::onExecute() {
	acState.execute();
	
	layout.execAnimations();
	layout.update();

	return true;
}


void dMessageWindow_c::showMessageWindow(int category, int textID, bool debug) {
	OSReport("Called dMessageWindow_c::showMessageWindow\ncategory: %d\n", category);
	OSReport("textID: %d\n", textID);
	
	this->category = category;
	this->textID = textID;
	this->isdebug = debug;
	
	acState.setState(&StateID_Init);
}

void dMessageWindow_c::hideMessageWindow() {
	if(isShown) {
		isShown = false;

		acState.setState(&StateID_End);
	}
}

/////////////////////
// Init State
void dMessageWindow_c::beginState_Init(){
	// Visual
	// Text
	if(!this->isdebug){
		OSReport("Type: NORMAL\n");
		if(messageType == 1){	// Peach Castle
			this->category = 0x0;
			this->textID = 0x49;
		}
		else if(messageType == 2){	// Toad House : Item Panel
			this->category = 0x0;
			this->textID = 0x47;
		}
		else if(messageType == 3){	// Toad House : 1UP Blast
			this->category = 0x0;
			this->textID = 0x46;
		}
		else if(messageType == 4){	// Toad House : 1UP Blast
			this->category = 0x0;
			this->textID = 0x48;
		}
		else if(messageType == 5){	// Toad House (Newer) : Collect Stars
			this->category = 0x3ec;
			this->textID = 0x0;
		}
		else if(messageType == 6){	// Toad House (Newer) : Music House
			this->category = 0x3ec;
			this->textID = 0x1;
		}
		else if(messageType == 7){	// Chimp House (NSMBW-TPC By Asu-chan) : Item Bowling
			this->category = 0x3ec;
			this->textID = 0x2;
		}
		WriteBMGToTextBox(T_text[0], GetBMG(), this->category, this->textID, 0);
		WriteBMGToTextBox(T_text[1], GetBMG(), this->category, this->textID, 0);
	}
	else{
		OSReport("Type: DEBUG\n");
		WriteBMGToTextBox(T_text[0], GetBMG(), this->category, this->textID, 0);
		WriteBMGToTextBox(T_text[1], GetBMG(), this->category, this->textID, 0);
	}
	// Layout/Animation
	layout.enableNonLoopAnim(APPEAR);
	OSReport("Switched amin to APPEAR(0)\n");
	isVisible = true;

	// Sound
	PlaySoundWithFunctionB4(SoundRelatedClass, &handleMSGWindow, SE_SYS_KO_DIALOGUE_IN, 1);
}
void dMessageWindow_c::executeState_Init(){
	if(!layout.isAnimOn(APPEAR)) {
		acState.setState(&StateID_Wait);
	}
}
void dMessageWindow_c::endState_Init(){
	OSReport("In fin_Init\n");
	// layout.enableLoopAnim(WAIT);
	OSReport("Switched amin to WAIT(1)\n");
}

/////////////////////
// Wait State
void dMessageWindow_c::beginState_Wait(){
	OSReport("In ini_Wait\n");
	this->baseline = layout.posY; 
	this->timer = 0;
}
void dMessageWindow_c::executeState_Wait(){
	//if(!layout.isAnimOn(WAIT)) {
	//	acState.setState(&StateID_End);
	//}

	// Omg what is this shit
	if(this->timer < 25 /*|| (this->baseline + 4.25) < layout.posY*/){
		layout.posY -= 0.25;
		layout.posY += this->timer / 100.0;
	}
	else if(this->timer < 50 /*|| (this->baseline + 4.25) > layout.posY*/){
		layout.posY += 0.25;
		layout.posY -= (this->timer - 25) / 100.0;
	}

	if(this->timer >= 50){
		this->timer = 0;
		layout.posY = this->baseline;
	}

	this->timer += 1;
}
void dMessageWindow_c::endState_Wait(){}

/////////////////////
// End State
void dMessageWindow_c::beginState_End(){
	layout.enableNonLoopAnim(END);
	PlaySoundWithFunctionB4(SoundRelatedClass, &handleMSGWindow, SE_SYS_DIALOGUE_OUT, 1);
}
void dMessageWindow_c::executeState_End(){}
void dMessageWindow_c::endState_End(){}