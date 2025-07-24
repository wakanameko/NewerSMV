#ifndef __MESSAGEWINDOW_H
#define __MESSAGEWINDOW_H

#include <common.h>
#include <game.h>
#include <profile.h>
#include <sfx.h>
#include "layoutlib.h"

class dMessageWindow_c : public dStageActor_c {
	public:
		dMessageWindow_c() : acState(this, &StateID_Init) { }

		int onCreate();
		int onExecute();
		int onDelete();
		int onDraw();

		m2d::EmbedLayout_c layout;
		nw4r::lyt::Pane *rootPane;
		dStateWrapper_c<dMessageWindow_c> acState;

		bool layoutLoaded;
		bool isVisible;

		u32 timer;
		u32 textAnim;
		bool isShown;

		nw4r::lyt::TextBox *T_text[2];

		enum Animation {
			APPEAR = 0,
			WAIT = 1,
			END = 2,
		};

		// added entries //
		int messageType;
		uint category;
		uint textID;
		bool isdebug;
		float baseline;
		nw4r::snd::SoundHandle handleMSGWindow;

		void showMessageWindow(int category, int textID, bool debug);
		void hideMessageWindow();
		// end of it //

		static dMessageWindow_c *instance;
		static dActor_c *build();

		USING_STATES(dMessageWindow_c);
		DECLARE_STATE(Init);
		DECLARE_STATE(Wait);
		DECLARE_STATE(End);
};


#endif /* __MESSAGEWINDOW_H */