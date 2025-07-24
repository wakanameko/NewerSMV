#include <common.h>
#include <game.h>
#include <stage.h>
#include <g3dhax.h>
#include <sfx.h>
#include <gamescene.h> //It contains the dGameDisplay_c, which was RE'd by Grop (because i was a noob back when he did it), thanks to him !
#include <gameLanguage.h>
#include <wakanalib.h>
#include "boss.h"
//#include "msgbox.h"

/******************************************************************************/
/**ALL THE STUFF BELOW IS TESTS, DEBUG MODE AND IS UNRELATED TO ANYTHING ELSE**/
/******************************************************************************/


/***************************/
/**Nybble getting function**/
/***************************/

extern "C" float pow(float num, float power);

int getNybbleValue(u32 settings, int fromNybble, int toNybble, bool doOSReport) { //This function is to get a nybble from a sprite's settings. It's unused but i used it for some tests, and i don't want to remove it :c
	int numberOfNybble = (toNybble  - fromNybble) + 1;               //gets how many nybbles are used for the process (example: nybbles 4-6 -> there's nybbles 4, 5 and 6 used -> numberOfNybble = 3) 
	int valueToUse = 48 - (4 * toNybble);                            //gets the value to use with the bitshift at the end 
	int fShit = pow(16, numberOfNybble) - 1;                         //gets the value to use with the "&" operator at the end 
	if(doOSReport) {
		OSReport("-> (settings >> %d) & 0x%x) => ", valueToUse, fShit);  //debugging
	}
	return ((settings >> valueToUse) & fShit);                       //uses everything to make the nybble value 
}

/*********************************************************/
/**Global variables declaring, externs, and lots of shit**/
/*********************************************************/

extern bool enableDebugMode;																					//Is the Collision Viewer enabled ?
extern bool enableCollisionMode;
extern "C" void *dAcPy_c__ChangePowerupWithAnimation(void * Player, int powerup); 	//Powerup Changing Functin | Powerups - 0 = small; 1 = big; 2 = fire; 3 = mini; 4 = prop; 5 = peng; 6 = ice; 7 = hammer
extern "C" int CheckExistingPowerup(void * Player);																						//Check the powerup of a player
static nw4r::snd::StrmSoundHandle s_handle;																								//sound handling storing
extern "C" void PlaySoundWithFunctionB4(void *spc, nw4r::snd::SoundHandle *handle, int id, int unk);									//play sound function
u8 hijackMusicWithSongName(const char *songName, int themeID, bool hasFast, int channelCount, int trackCount, int *wantRealStreamID);	//hijacking
int minuscounter;						//Counts how much time the minus button was pressed (to enable the debug mode)
extern bool isTrailerMode;				//Is the Trailer Mode enabled ?
bool isLuigiActive;						//Is Luigi an active player ?
bool isKinoBActive;						//Is Blue Toad an active player ?
bool isKinoYActive;						//Is Yellow Toad an active player ?
bool isSC1Active;						//Is the Star Coin 1 collected ?
bool isSC2Active;						//Is the Star Coin 2 collected ?
bool isSC3Active;						//Is the Star Coin 3 collected ?
int pActivityDoOneTime;					//Related to a Trailer Mode removed feature
bool enablePlayerDebug;					//Fly Hack
Vec playerPos;							//Get Player pos
bool hideUIMode;					//Added by wakanameko! YAFIIIIIIIIIIIIIIIIIIIIIIII!!!!! 
bool hideDebugtxtMode;					//Added by wakanameko! YAFIIIIIIIIIIIIIIIIIIIIIIII!!!!! 
int counter = 0x15;							//Shitto


/***************************************************************/
/**gameScene's onExecute adds for the debug mode and some shit**/
/***************************************************************/

int dGameDisplay_c::doWaitCheck() { //So the reason i'm doing this in the gameScene's onExecute is because it's executed every frame in every level, wherever the player are, so it's nice to check for things wherever level you're in and wherever the players are
	int orig_val = this->onExecute_orig();						//Execute the original onExecute, store it in orig_val
	int nowPressed = Remocon_GetPressed(GetActiveRemocon());	//Get the current pressed buttons
	int nowHeld = Remocon_GetButtons(GetActiveRemocon());
	/* define layouts */
	// Picture panes
	nw4r::lyt::Picture *P_base_01 = layout.findPictureByName("P_base_01");					//player icon bar
	nw4r::lyt::Picture *P_baseS_01 = layout.findPictureByName("P_baseS_01");				//shadow of players icon bar
	nw4r::lyt::Picture *P_marioIcon_00 = layout.findPictureByName("P_marioIcon_00");		//mario
	nw4r::lyt::Picture *P_luijiIcon_00 = layout.findPictureByName("P_luijiIcon_00");		//Luigi
	nw4r::lyt::Picture *P_kinoB_00 = layout.findPictureByName("P_kinoB_00");				//Blue toad
	nw4r::lyt::Picture *P_kinoY_00 = layout.findPictureByName("P_kinoY_00");				//yellow toad
	nw4r::lyt::Picture *P_coin_00 = layout.findPictureByName("P_coin_00");					//coin icon
	nw4r::lyt::Picture *P_collectOff_00 = layout.findPictureByName("P_collectOff_00");		//Star coins
	nw4r::lyt::Picture *P_collection_00 = layout.findPictureByName("P_collection_00");
	nw4r::lyt::Picture *P_collectOff_01 = layout.findPictureByName("P_collectOff_01");
	nw4r::lyt::Picture *P_collection_01 = layout.findPictureByName("P_collection_01");
	nw4r::lyt::Picture *P_collectOff_02 = layout.findPictureByName("P_collectOff_02");
	nw4r::lyt::Picture *P_collection_02 = layout.findPictureByName("P_collection_02");
	nw4r::lyt::Picture *P_timer_00 = layout.findPictureByName("P_timer_00");				//Timer
	// Text panes
	nw4r::lyt::TextBox *T_score_00 = dGameDisplay_c::instance->layout.findTextBoxByName("T_score_00");
	nw4r::lyt::TextBox *T_time_00 = dGameDisplay_c::instance->layout.findTextBoxByName("T_time_00");
	nw4r::lyt::TextBox *T_level_hint = dGameDisplay_c::instance->layout.findTextBoxByName("T_level_hint");
	nw4r::lyt::TextBox *T_FlyHaxText = dGameDisplay_c::instance->layout.findTextBoxByName("T_FlyHaxText");
	nw4r::lyt::TextBox *T_HideUIAno = dGameDisplay_c::instance->layout.findTextBoxByName("T_HideUIAno");
	nw4r::lyt::TextBox *T_CollViewerAno = dGameDisplay_c::instance->layout.findTextBoxByName("T_CollViewerAno");
	nw4r::lyt::TextBox *T_x_01 = dGameDisplay_c::instance->layout.findTextBoxByName("T_x_01");
	nw4r::lyt::TextBox *T_left_00 = dGameDisplay_c::instance->layout.findTextBoxByName("T_left_00");
	nw4r::lyt::TextBox *T_x_02 = dGameDisplay_c::instance->layout.findTextBoxByName("T_x_02");
	nw4r::lyt::TextBox *T_left_01 = dGameDisplay_c::instance->layout.findTextBoxByName("T_left_01");
	nw4r::lyt::TextBox *T_x_03 = dGameDisplay_c::instance->layout.findTextBoxByName("T_x_03");
	nw4r::lyt::TextBox *T_left_02 = dGameDisplay_c::instance->layout.findTextBoxByName("T_left_02");
	nw4r::lyt::TextBox *T_x_04 = dGameDisplay_c::instance->layout.findTextBoxByName("T_x_04");
	nw4r::lyt::TextBox *T_left_03 = dGameDisplay_c::instance->layout.findTextBoxByName("T_left_03");
	nw4r::lyt::TextBox *T_coin_00 = dGameDisplay_c::instance->layout.findTextBoxByName("T_coin_00");

	if(enableDebugMode) {	//If the debug mode is enabled
		dAcPy_c *player = dAcPy_c::findByID(0);	//Get the Player 1
		
		// B + Anything
		if ((GetActiveRemocon()->heldButtons == 0x402) && (nowPressed & 0x402)) { // B + UP // Changes the player's powerup
			int playerPowerup = ((CheckExistingPowerup(player) == 7) ? 0 : (CheckExistingPowerup(player) + 1));          //increment the player's powerup id by 1. If 7 -> 0
			dAcPy_c__ChangePowerupWithAnimation(player, playerPowerup);                                                  //change it
		}
		if ((GetActiveRemocon()->heldButtons == 0x401) && (nowPressed & 0x401)) { // B + DOWN // Beats the current level
			ExitStage(WORLD_MAP, 0, BEAT_LEVEL, MARIO_WIPE);                                                             //exit the current stage and beat it
		}
		if ((GetActiveRemocon()->heldButtons == 0x408) && (nowPressed & 0x408)) { // B + LEFT // Turn on the Fly Hack
			playerPos = GetSpecificPlayerActor(0)->pos;
			enablePlayerDebug = !enablePlayerDebug;
		}
		if ((GetActiveRemocon()->heldButtons == 0x404) && (nowPressed & 0x404)) { // B + RIGHT // Spawns a Star
			int enitemsettings = 0 | (1 << 0) | (2 << 18) | (4 << 9) | (2 << 10) | (8 << 16);                            //setting the settings
			dStageActor_c *Star = dStageActor_c::create(EN_ITEM, enitemsettings, &player->pos, 0, 0);                    //creates the Star
		}
		if ((GetActiveRemocon()->heldButtons == 0x600) && (nowPressed & 0x600)) { // B + ONE // Enables the Trailer Mode (Makes the music stops)
			isTrailerMode = !isTrailerMode;                                                                              //disable or enable switching
			ExitStage(WORLD_MAP, 0, EXIT_LEVEL, MARIO_WIPE);
		}
		if ((nowHeld & WPAD_B) && (nowPressed & WPAD_MINUS)) {					// B + MINUS // Enables the HideUI mode
			hideUIMode = !hideUIMode;
		}
		if ((nowHeld & WPAD_B) && (nowPressed & WPAD_PLUS)) {					// B + PLUS // Turn On/Off the collision viewer
			enableCollisionMode = !enableCollisionMode;
		}

		// A + Anything
		if ((nowHeld & WPAD_A) && (nowPressed & WPAD_MINUS)) {					// A + MINUS // Spawn the yoshi's egg on now mario pos
			playerPos = GetSpecificPlayerActor(0)->pos;
			dStageActor_c *spawner = CreateActor(330, 0, playerPos, 0, 0);
		}
		if ((nowHeld & WPAD_A) && (nowPressed & WPAD_PLUS)) {					// A + PULS	//1UP
			playerPos = GetSpecificPlayerActor(0)->pos;
			oneUPPopup(playerPos);
			addRest(0, 1, true);	// addrest for mario(1P)
		}
		if ((nowHeld & WPAD_A) && (nowPressed & WPAD_UP)) {						// A + UP // Change language to English
			if(SetGameLanguage != 3){
				SetGameLanguage += 1;
			}
			else{SetGameLanguage = 0;}
		}
		if ((nowHeld & WPAD_A) && (nowPressed & WPAD_TWO)) {					// A + TWO // DisplayScoreAddPopup
			DisplayScoreAddPopup(playerPos, counter, 0, false);
			OSReport("id:%d\n", counter);
			counter += 0x1;
		}
		if ((nowHeld & WPAD_A) && (nowPressed & WPAD_ONE)) {
			DisplayScoreAddPopup(playerPos, counter, 0, false);
			OSReport("id:%d\n", counter);
			counter -= 0x1;
		}

		//Extra commands
		if ((nowHeld & WPAD_MINUS) && (nowPressed & WPAD_PLUS)) {				// Minus + PLUS // Turn On/Off the hideDebugtxtMode
			hideDebugtxtMode = !hideDebugtxtMode;
		}/*
		if ((nowHeld & WPAD_MINUS) && (nowPressed & WPAD_TWO)) {				// Minus + TWO // Show how to use DebugMode
				dMsgBoxManager_c::instance->showMessage(
				settings & 0xFFFFFF, // message ID
				false, // cannot cancel
				delay // delay
				);
		}//*/

		if(enablePlayerDebug) {
			int nowHeld = Remocon_GetButtons(GetActiveRemocon());
			int mult = (nowHeld & WPAD_ONE) ? 8 : 4;
			if(nowHeld & WPAD_UP) {
				playerPos.y += mult;
			}
			else if(nowHeld & WPAD_DOWN) {
				playerPos.y -= mult;
			}

			if(nowHeld & WPAD_RIGHT) {
				playerPos.x += mult;
			}
			else if(nowHeld & WPAD_LEFT) {
				playerPos.x -= mult;
			}
			GetSpecificPlayerActor(0)->pos = playerPos;
		}

		if(!isTrailerMode){
			if (SetGameLanguage == 0){	// EN messages by wakanameko
				T_level_hint->SetString(((enableDebugMode) ? L"Debug Mode" : L" "));
				T_HideUIAno->SetString(((enableDebugMode) ? L"Hide UI Mode : B & -" : L" "));
				T_FlyHaxText->SetString(((enablePlayerDebug) ? L"Flyhack is turned ON" : L" "));
				T_CollViewerAno->SetString(((enableCollisionMode) ? L"Collision Viewer is turned ON" : L" "));
			}
			if (SetGameLanguage == 1){	// JP messages by wakanameko
				T_level_hint->SetString(((enableDebugMode) ? L"デバッグモード" : L" "));
				T_HideUIAno->SetString(((enableDebugMode) ? L"UIをかくす : B & -" : L" "));
				T_FlyHaxText->SetString(((enablePlayerDebug) ? L"フライチート:オン" : L" "));
				T_CollViewerAno->SetString(((enableCollisionMode) ? L"あたりはんていひょうじ:オン" : L" "));
			}
			if (SetGameLanguage == 2){	// DE messages by Vadenimo
				T_level_hint->SetString(((enableDebugMode) ? L"Debug-Modus" : L" "));
				T_HideUIAno->SetString(((enableDebugMode) ? L"UI verbergen: B & -" : L" "));
				T_FlyHaxText->SetString(((enablePlayerDebug) ? L"Flughack: AN" : L" "));
				T_CollViewerAno->SetString(((enableCollisionMode) ? L"Kollisionsbetrachtung: AN" : L" "));
			}
			if (SetGameLanguage == 3){	// IT messages by Jacopo Plays
				T_level_hint->SetString(((enableDebugMode) ? L"Modalitá Degub" : L" "));
				T_HideUIAno->SetString(((enableDebugMode) ? L"Rimuovi UI : B & -" : L" "));
				T_FlyHaxText->SetString(((enablePlayerDebug) ? L"Il Volo é ATTIVATO" : L" "));
				T_CollViewerAno->SetString(((enableCollisionMode) ? L"Le Collisioni sono ATTIVATE" : L" "));
			}
		}
		
		if(hideDebugtxtMode){
			// Text Boxes
			T_level_hint->alpha = 0;
			T_FlyHaxText->alpha = 0;
			T_HideUIAno->alpha = 0;
			T_CollViewerAno->alpha = 0;
		}
	}

	//Welcome to wakanameko-chan's poor code!
	if(hideUIMode){
		// Text Boxes
		T_score_00->alpha = 0;
		T_time_00->alpha = 0;
		T_level_hint->alpha = 0;
		T_FlyHaxText->alpha = 0;
		T_HideUIAno->alpha = 0;
		T_CollViewerAno->alpha = 0;
		T_x_01->alpha = 0;
		T_left_00->alpha = 0;
		T_x_02->alpha = 0;
		T_left_01->alpha = 0;
		T_x_03->alpha = 0;
		T_left_02->alpha = 0;
		T_x_04->alpha = 0;
		T_left_03->alpha = 0;
		T_coin_00->alpha = 0;

		// Picture Boxes
		P_marioIcon_00->size.x = 0.0f;
		P_base_01->size.x = 0.0f;
		P_baseS_01->size.x = 0.0f;
		P_luijiIcon_00->size.x = 0.0f;
		P_kinoB_00->size.x = 0.0f;
		P_kinoY_00->size.x = 0.0f;
		P_coin_00->size.x = 0.0f;
		P_collectOff_00->size.x = 0.0f;
		P_collection_00->size.x = 0.0f;
		P_collectOff_01->size.x = 0.0f;
		P_collection_01->size.x = 0.0f;
		P_collectOff_02->size.x = 0.0f;
		P_collection_02->size.x = 0.0f;
		P_timer_00->size.x = 0.0f;
	}
	if(!hideUIMode){
		// Text Boxes
		T_score_00->alpha = 255;
		T_time_00->alpha = 255;
		T_level_hint->alpha = 255;
		T_FlyHaxText->alpha = 255;
		T_HideUIAno->alpha = 255;
		T_CollViewerAno->alpha = 255;
		T_x_01->alpha = 255;
		T_left_00->alpha = 255;
		T_x_02->alpha = 255;
		T_left_01->alpha = 255;
		T_x_03->alpha = 255;
		T_left_02->alpha = 255;
		T_x_04->alpha = 255;
		T_left_03->alpha = 255;
		T_coin_00->alpha = 255;

		// Picture boxes
		P_marioIcon_00->size.x = 48.0f;
		P_base_01->size.x = 228.0f;
		P_baseS_01->size.x = 231.0f;
		P_luijiIcon_00->size.x = 46.0f;
		P_kinoB_00->size.x = 48.0f;
		P_kinoY_00->size.x = 48.0f;
		P_coin_00->size.x = 40.0f;
		P_collectOff_00->size.x = 64.0f;
		P_collection_00->size.x = 64.0f;
		P_collectOff_01->size.x = 64.0f;
		P_collection_01->size.x = 64.0f;
		P_collectOff_02->size.x = 64.0f;
		P_collection_02->size.x = 64.0f;
		P_timer_00->size.x = 32.0f;
	}

	return orig_val; //Return what the original onExecute was supposed to return
}


/****************************/
/**Hexadecimal coin counter**/
/****************************/
/*
void dGameDisplay_c::doHexCoin() { //Make the coin counter display in hexadecimal if the debug mode is enabled. Why ? Because how do i know if it's enabled otherwise ?
	nw4r::lyt::TextBox *stupidcoin = dGameDisplay_c::instance->layout.findTextBoxByName("T_coin_00");					//get the coin textbox
	char str[2];																										//make a char that will contain the text to display
	sprintf(str, ((enableDebugMode && !isTrailerMode || !hideDebugtxtMode) ? "%02X" : "%02d"), dGameDisplay_c::instance->coins);				//insert the number in the char win three characters
	wchar_t nyeh[2];																									//make a wchar_t
	nyeh[0] = str[0];																									//put the char's content in the wchar_t
	nyeh[1] = str[1];
	stupidcoin->SetString(nyeh, 0, 2);																					//write it to the textbox
}//*/