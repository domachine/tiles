/*  Copyright 2009-2010 Thomas Witte

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GAME_MENU_H
#define GAME_MENU_H

#include <allegro.h>
#include <string>
#include "config.h"
#include "menu_base.h"
#include "game.h"

using namespace std;

class GameMenu : public MenuBase {
	public:
		GameMenu(Game *parent);

	protected:
		enum DIALOG_ID {MAIN_DIALOG, ITEM_DIALOG, ITEM_SP_DIALOG, SKILL_DIALOG, SKILL_CH_DIALOG, EQUIP_DIALOG,
						EQUIP_CH_DIALOG, RELIC_DIALOG, RELIC_CH_DIALOG, STATUS_CH_DIALOG, STATUS_DIALOG};
		Game *parent;

		DIALOG *create_dialog(int id);
		DIALOG *create_main_dialog();
		DIALOG *create_ch_chooser(DIALOG_ID id);
		DIALOG *create_status_dialog();
		DIALOG *create_skill_dialog();
		DIALOG *create_equip_dialog();
		DIALOG *create_relic_dialog();
		DIALOG *create_item_dialog();
		DIALOG *create_item_sp_dialog();

		string get_chosen_player();
};

#endif
