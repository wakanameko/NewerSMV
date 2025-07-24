#include <common.h>
#include <game.h>
#include <stage.h>

bool isEventTriggeredGiantLightBlock(int eventID);

bool isEventTriggeredGiantLightBlock(int eventID) {
	u64 eventFlag = ((u64)1 << (eventID - 1));
	if(dFlagMgr_c::instance->flags & eventFlag) {
        return true;
	}
	return false;
}