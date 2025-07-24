#ifndef __MINIGAME_H
#define __MINIGAME_H

#include <common.h>
#include <game.h>

// Externs
extern "C" void *MakeMarioEnterDemoMode();
extern "C" void *MakeMarioExitDemoMode();
extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);
extern "C" void *StopBGMMusic();
extern "C" void *StartBGMMusic();
// extern "C" void *SoundRelatedClass;
extern "C" void PlaySoundWithFunctionB4(void *spc, nw4r::snd::SoundHandle *handle, int id, int unk);
extern "C" void doBGMStuff(void*, u32); // Lowers volume
extern "C" void doBGMStuff2(void*); // Puts volume back to how it was
extern "C" u8 NextStageActorDisableFlags;
extern bool hideUIMode;

// THIS IS A DUMB NAME
extern bool NoMichaelBuble;


#endif