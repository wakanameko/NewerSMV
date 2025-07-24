#ifndef NEWER_H
#define NEWER_H 

extern int lastLevelIDs[];

// from SLLW thanks to rsm
void WriteJPWorldNameToTextBox(nw4r::lyt::TextBox *tb, int world); 
void GetJapaneseWorldName(wchar_t *output, int world);
void WriteWorldNameToTextBox(nw4r::lyt::TextBox *tb, int world);

// edited by wakanameko. void WriteAsciiToTextBox(nw4r::lyt::TextBox *tb, const char *source);
void WriteAsciiToTextBox(nw4r::lyt::TextBox *tb, const wchar_t *source);

int getUnspentStarCoinCount();
int getStarCoinCount();
void getNewerLevelNumberString(int world, int level, wchar_t *dest);

#endif /* NEWER_H */
