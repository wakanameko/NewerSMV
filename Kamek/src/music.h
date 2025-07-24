#ifndef MUSIC_H
#define MUSIC_H 
u8 hijackMusicWithSongName(const char *songName, int themeID, bool hasFast, int channelCount, int trackCount, int *wantRealStreamID);
int hijackSFX(int SFXNum);
#endif /* MUSIC_H */
