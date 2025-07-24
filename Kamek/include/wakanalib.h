#ifndef WAKANALIB_H
#define WAKANALIB_H 
/*VERSION 1.0.1 SMV*/
/* By wakanameko */
#include <common.h>

// increase rests
extern "C" void addRest(int playerIDRest, int howMany, bool reallyAddRest);	// reallyAddRest is always true
extern "C" void incRestAll(bool reallyAddRest);	// reallyAddRest is always true

// stage funcs
extern "C" void oneUPPopup(Vec pos);   // set the position (x,y)
extern "C" void AddScore(int howMany, int playerID);    // from Newer1.30
extern "C" void DisplayScoreAddPopup(Vec pos, int smallScoreID, int playerIDuint, bool wantToShowThisForever);  // smallScoreID list is here -> https://note.com/reaz0n23/n/nb504e036da6d#143de39b-221f-4909-94f8-cc11e4dbdfdd
enum smallScoreIDList{
    NORMAL_100 = 0,
    NORMAL_200 = 1,
    NORMAL_400 = 2,
    NORMAL_800 = 3,
    NORMAL_1000 = 4,
    NORMAL_2000 = 5,
    NORMAL_4000 = 6,
    NORMAL_8000 = 7,
    REDCOIN_1 = 8,
    REDCOIN_2 = 9,
    REDCOIN_3 = 10,
    REDCOIN_4 = 11,
    REDCOIN_5 = 12,
    REDCOIN_6 = 13,
    REDCOIN_7 = 14,
    REDCOIN_8 = 15,
    REST_1UP = 16,
    REST_2UP = 17,
    REST_3UP = 18,
    REST_4UP = 19,
};

// zone settings
extern "C" float zoneZoomFloat;    // Default is 2.0

//
extern "C" char aP_rcha_2;     //"P_rcha"

// daEnDpakkunBase_c::
extern "C" void pakkunYoshiFumiJumpSet(dEn_c *apThis, daPlBase_c *unk);    // 2nd argument is unknown

// daBrosIceball_c::
extern "C" void daBrosIceball__executeState_Move_deleteRequest(dEn_c *self);
extern "C" void daBrosIceball__spawnMisshitEffect(dEn_c *self);

// related World Map
// dScStage_c
    extern "C" uint m_goalType;    // 0: Normal Exit, 1: Secret Exit

class dWmLib{
public:
    static void HandleCourseCompletion(bool isSecretExit, bool isSuperGuide, int world, int level);
};

#endif /* WAKANALIB_H */
