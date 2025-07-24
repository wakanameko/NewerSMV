#ifndef __KERONPA_H
#define __KERONPA_H

#include <daEnemy_c.h>

/* All Addresses PAL Rev1 */

class daEnKeronpa_c : public dEn_c {
public:
	void daEnKeronpa_c::build()	// 80a45fb0
	int onCreate();		// unk型	// 80a46ef0 daEnKeronpa_c::create(dActor_c *param_1)
	int onExecute();	// unk型	// 80a46fa0	daEnKeronpa_c::execute(daEnKeronpa_c *param_1)
	int onDraw();		// unk型	// 80a47140	daEnKeronpa_c::draw(daEnKeronpa_c *param_1)
	int onDelete();		// unk型	// 80a47280 daEnKeronpa_c::doDelete(daEnKeronpa_c *param_1)

	USING_STATES(daEnKeronpa_c);
	REF_NINTENDO_STATE(DieFumi);
	REF_NINTENDO_STATE(DieBigFall);
	REF_NINTENDO_STATE(Move);
	DECLARE_STATE(Attack);
	REF_NINTENDO_STATE(AttackSt);
	REF_NINTENDO_STATE(AttackMd);
	REF_NINTENDO_STATE(AttackMdBoyon);
	REF_NINTENDO_STATE(Explode);
	
	public: static dActor_c *build();
};

#endif

