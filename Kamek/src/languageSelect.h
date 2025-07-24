#ifndef __LANGUAGESELECT_H
#define __LANGUAGESELECT_H

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>

void languageSelectDrawFunc();

class dScLanguageSelect_c : public dScene_c {
	public:
		dScLanguageSelect_c();
		~dScLanguageSelect_c();

		int onCreate();
		int onDelete();
		int onExecute();
		int onDraw();

		m2d::EmbedLayout_c layout;
		u8 layoutLoaded, isVisible;

		nw4r::lyt::TextBox *T_SFXTxt_00;
		nw4r::lyt::TextBox *T_SFXSha_00;
		nw4r::lyt::TextBox *T_SFXNumTxt_00;
		nw4r::lyt::TextBox *T_SFXNumSha_00;

		static dScLanguageSelect_c *build();
		static dScLanguageSelect_c *instance;

		void exit();
};


#endif

