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
#include "fighter.h"
#include "iohelper.h"
#include <iostream>
#include <sstream>

//Tabellen für Levelup
int xptable[] = {
	0,		0,		32,		96,		208,	400,	672,	1056,	1552,	2184,
	2976,	3936,	5080,	6432,	7992,	9784,	11840,	14152,	16736,	19616,
	22832,	26360,	30232,	34456,	39056,	44072,	49464,	55288,	61568,	68304,
	75496,	83184,	91384,	100088,	109344,	119136,	129504,	140464,	152008,	164184,
	176976,	190416,	204520,	219320,	234808,	251000,	267936,	285600,	304040,	323248,
	343248,	364064,	385696,	408160,	431488,	455680,	480776,	506760,	533680,	561528,
	590320,	620096,	650840,	682600,	715368,	749160,	784016,	819920,	856920,	895016,
	934208,	974536,	1016000,1058640,1102456,1147456,1193648,1241080,1289744,1339672,
	1390872,1443368,1497160,1552264,1608712,1666512,1725688,1786240,1848184,1911552,
	1976352,2042608,2110320,2179504,2250192,2322392,2396128,2471400,2548224,2637112,
	2637112
};

int hptable[] = {
	0,		11,		12,		14,		17,		20,		22,		24,		26,		27,
	28,		30,		35,		39,		44,		50,		54,		57,		61,		65,
	67,		69,		72,		76,		79,		82,		86,		90,		95,		99,
	100,	101,	102,	102,	103,	104,	106,	107,	108,	110,
	111,	113,	114,	116,	117,	119,	120,	122,	125,	128,
	130,	131,	133,	134,	136,	137,	139,	142,	144,	145,
	147,	148,	150,	152,	153,	155,	156,	158,	160,	162,
	160,	155,	151,	145,	140,	136,	132,	126,	120,	117,
	113,	110,	108,	105,	102,	100,	98,		95,		92,		90,
	88,		87,		85,		83,		82,		80,		83,		86,		88,		0
};

int mptable[] = {
	0,		0,		0,		5,		5,		6,		6,		7,		8,		8,
	9,		9,		10,		10,		10,		10,		10,		11,		11,		11,
	11,		11,		12,		12,		12,		12,		12,		13,		13,		13,
	13,		13,		14,		14,		14,		14,		14,		15,		15,		15,
	15,		15,		16,		16,		16,		16,		16,		17,		17,		17,
	16,		15,		14,		13,		12,		11,		10,		9,		8,		7,
	6,		5,		5,		6,		6,		7,		7,		7,		8,		8,
	8,		8,		8,		7,		7,		7,		6,		6,		6,		6,
	5,		5,		5,		5,		5,		5,		5,		6,		6,		6,
	6,		6,		7,		8,		9,		10,		11,		12,		13,		0
};

//ende

Fighter::Fighter(Fight *f, Character c, string name, PlayerSide side, int dir) {
	parent = f;
	this->c = c;
	atb = 0;
	itc = 0;
	step = 0;
	poisoncounter = 0;
	condemnedcounter = 0;
	this->side = side;
	direction = dir;
	texttoshow = "";
	textremframes = 0;
	textcol = 0;
	current_animation = NORMAL;
	ts.current = FighterTileset::NORMAL;
	ts.saved = FighterTileset::NORMAL;

	spritename = name;
	laden(name);

	menu.set_parent(this);
}

Fighter::~Fighter() {
	for(int j = FighterTileset::NORMAL; j < FighterTileset::MAGIC_WAIT; j++)
		for(unsigned int i = 0; i < ts.imgs[j].size(); i++)
			imageloader.destroy(ts.imgs[j][i]);
}

void Fighter::laden(string name) {
	string path;
	path = string("Fights/Fighters/") + name + string("/");

	FileParser parser(path + name, "Fighter");
	string sections[] = {"normal", "wound", "hit", "critical", "attack", "attack_exec", "attack_wait",
						"magic", "magic_exec", "magic_wait"};

	for(int j = FighterTileset::NORMAL; j <= FighterTileset::MAGIC_WAIT; j++) {
		deque< deque<string> > ret = parser.getsection(sections[j]);
		for(unsigned int i = 0; i < ret.size(); i++)
			ts.imgs[j].push_back(imageloader.load(path + ret[i][0]));
	}

	c.defensive = true;
	if(parser.getvalue("Fighter", "defensive", 0) == 0) c.defensive = false;
	c.name = parser.getstring("Fighter", "name", c.name);
	c.hp = parser.getvalue("Fighter", "hp", c.hp);
	c.curhp = c.hp;
	c.mp = parser.getvalue("Fighter", "mp", c.mp);
	c.curmp = c.mp;
	c.speed = parser.getvalue("Fighter", "speed", c.speed);
	c.vigor = parser.getvalue("Fighter", "vigor", c.vigor);
	c.stamina = parser.getvalue("Fighter", "stamina", c.stamina);
	c.mpower = parser.getvalue("Fighter", "mpower", c.mpower);
	c.apower = parser.getvalue("Fighter", "apower", c.apower);
	c.mdefense = parser.getvalue("Fighter", "mdefense", c.mdefense);
	c.adefense = parser.getvalue("Fighter", "adefense", c.adefense);
	c.mblock = parser.getvalue("Fighter", "mblock", c.mblock);
	c.ablock = parser.getvalue("Fighter", "ablock", c.ablock);
	c.level = parser.getvalue("Fighter", "level", c.level);
	c.hitrate = parser.getvalue("Fighter", "hitrate", c.hitrate);
	c.xp = parser.getvalue("Fighter", "xp", c.xp);
	c.levelupxp = parser.getvalue("Fighter", "levelupxp", c.levelupxp);

	//Elemente
	string elements[] = {"none", "Heal", "Death", "Bolt", "Ice", "Fire", "Water", "Wind", "Earth", "Poison", "Pearl"};
	string rt;
	for(int i = 0; i < 11; i++) {
		rt = parser.getstring("Elements", elements[i], "normal");
		if(rt == "weak") c.elements[i] = Character::WEAK;
		else if(rt == "absorb") c.elements[i] = Character::ABSORB;
		else if(rt == "immune") c.elements[i] = Character::IMMUNE;
		else if(rt == "resist") c.elements[i] = Character::RESISTANT;
		else c.elements[i] = Character::NORMAL;
	}

	//Statuse
	string status[] = {"Dark", "Zombie", "Poison", "MTek", "Clear", "Imp", "Petrify", "Wound", "Condemned", "NearFatal", "Image", "Mute", "Berserk", "Muddle", "Seizure", "Sleep", "Dance", "Regen", "Slow", "Haste", "Stop", "Shell", "Safe", "Reflect", "Morph", "Float", "Life3"};
	for(int i = 0; i < 27; i++) {
		rt = parser.getstring("Status", status[i], "normal");
		if(rt == "immune") c.status[i] = Character::IMMUNE;
		else if(rt == "suffering") c.status[i] = Character::SUFFERING;
		else c.status[i] = Character::NORMAL;
	}

	//Special
	string special[] = {"0MPDeath", "Human", "CritHitIfImp", "Undead", "HardToRun", "AtkFirst", "BlockSuplex", "CantRun", "CantScan", "CantSketch", "SpecialEvent", "CantControl", "TrueKnight", "Runic", "RemovableFloat"};
	for(int i = 0; i < 15; i++) {
		rt = parser.getstring("Special", special[i], "0");
		if(rt == "0") c.special[i] = false;
		else if(rt == "1") c.special[i] = true;
		else c.special[i] = false;
	}

	deque< deque<string> > menu_items = parser.getsection("Menu");
	menu.set_items(menu_items);
}

void Fighter::update() {
	//Wound status
	if(c.curhp <= 0) {
		if(c.status[Character::ZOMBIE] != Character::SUFFERING) {
			for(int i = 0; i < 27; i++) {
				if(c.status[i] != Character::IMMUNE)
					c.status[i] = Character::NORMAL;
			}
			c.status[Character::WOUND] = Character::SUFFERING;
			atb = 0;
		}
	} else if(c.curhp < c.hp/8) {
		c.status[Character::NEAR_FATAL] = Character::SUFFERING;
	} else {
		c.status[Character::NEAR_FATAL] = Character::NORMAL;
	}

	//Zombie status
	if(c.status[Character::ZOMBIE] == Character::SUFFERING) {
		if(c.status[Character::DARK] != Character::IMMUNE)
			c.status[Character::DARK] = Character::NORMAL;
		if(c.status[Character::POISON] != Character::IMMUNE)
			c.status[Character::POISON] = Character::NORMAL;
		if(c.status[Character::NEAR_FATAL] != Character::IMMUNE)
			c.status[Character::NEAR_FATAL] = Character::NORMAL;
		if(c.status[Character::BERSERK] != Character::IMMUNE)
			c.status[Character::BERSERK] = Character::NORMAL;
		if(c.status[Character::MUDDLE] != Character::IMMUNE)
			c.status[Character::MUDDLE] = Character::NORMAL;
		if(c.status[Character::SLEEP] != Character::IMMUNE)
			c.status[Character::SLEEP] = Character::NORMAL;
	}

	if(atb > 65536) {
		atb = 65536;
		parent->enqueue_ready_fighter(this);
	}

	if(c.status[Character::HASTE] == Character::SUFFERING)
		itc += 180/GAME_TIMER_BPS;
	else if(c.status[Character::SLOW] == Character::SUFFERING)
		itc += 60/GAME_TIMER_BPS;
	else
		itc += 120/GAME_TIMER_BPS;

	if(itc > 255 && c.status[Character::WOUND] != Character::SUFFERING) {
		itc = 0;
		if(c.status[Character::POISON] != Character::SUFFERING)
			poisoncounter = 0;
		if(c.status[Character::POISON] == Character::SUFFERING && random()%8 == 0) {
			int dmg = (c.hp * c.stamina / 1024) + 2;
			if(dmg > 255) dmg = 255;
			dmg = dmg * (random()%32 + 224) / 256;
			if(!is_monster()) dmg /= 2;
			dmg *= (poisoncounter + 1);
			lose_health(dmg);

			poisoncounter++;
			if(poisoncounter > 7) poisoncounter = 0;
			//Giftschaden

		}
		if(c.status[Character::CONDEMNED] == Character::SUFFERING) {
			if(condemnedcounter == 0) {
				Command c(this);
				c.add_target(this);
				c.set_attack("X-Fer");
				c.execute();
			} else {
				condemnedcounter--;
			}
		}
		if(c.status[Character::SEIZURE] == Character::SUFFERING && random()%4 == 0) {
			int dmg = (c.hp * c.stamina / 1024) + 2;
			if(dmg > 255) dmg = 255;
			dmg = dmg * (random()%32 + 224) / 256;
			if(!is_monster()) dmg /= 2;
			lose_health(dmg);
			//Seizureschaden

		}
		if(c.status[Character::REGEN] == Character::SUFFERING && random()%4 == 0) {
			int dmg = (c.hp * c.stamina / 1024) + 2;
			if(dmg > 255) dmg = 255;
			dmg = dmg * (random()%32 + 224) / -256;
			lose_health(dmg);
			//hp durch regen
		}
	}
}

void Fighter::animate() {
	step++;
}

void Fighter::draw(BITMAP *buffer, int x, int y) {
	scrposx = x;
	scrposy = y;

	int index = (step/SPRITE_ANIMATION_SPEED)%ts.imgs[ts.current].size();

	//Schatten
	ellipsefill(buffer, x, y+ts.imgs[ts.current][index]->h/2, 6, 2, COL_BLACK);

	if(c.status[Character::FLOAT] == Character::SUFFERING) {
		y -= 5;
	}

	//spiegeln, wenn direction = 0(linkss)
	if(c.status[Character::CLEAR] != Character::SUFFERING) {
		if(direction == 0) {
			draw_sprite_h_flip(buffer, ts.imgs[ts.current][index], x-ts.imgs[ts.current][index]->w/2, y-ts.imgs[ts.current][index]->h/2);
		} else {
			draw_sprite(buffer, ts.imgs[ts.current][index], x-ts.imgs[ts.current][index]->w/2, y-ts.imgs[ts.current][index]->h/2);
		}
	}

	//Statusindikatoren zeichnen
	if(c.status[Character::HASTE] == Character::SUFFERING) {
		draw_outline(buffer, ts.imgs[ts.current][index], x-ts.imgs[ts.current][index]->w/2,
					y-ts.imgs[ts.current][index]->h/2, COL_RED, (direction == 0));
	} else if(c.status[Character::SLOW] == Character::SUFFERING) {
		draw_outline(buffer, ts.imgs[ts.current][index], x-ts.imgs[ts.current][index]->w/2,
					y-ts.imgs[ts.current][index]->h/2, COL_WHITE, (direction == 0));
	} else if(c.status[Character::STOP] == Character::SUFFERING) {
		draw_outline(buffer, ts.imgs[ts.current][index], x-ts.imgs[ts.current][index]->w/2,
					y-ts.imgs[ts.current][index]->h/2, COL_PINK, (direction == 0));
	} else if(c.status[Character::SHELL] == Character::SUFFERING) {
		draw_outline(buffer, ts.imgs[ts.current][index], x-ts.imgs[ts.current][index]->w/2,
					y-ts.imgs[ts.current][index]->h/2, COL_GREEN, (direction == 0));
	} else if(c.status[Character::SAFE] == Character::SUFFERING) {
		draw_outline(buffer, ts.imgs[ts.current][index], x-ts.imgs[ts.current][index]->w/2,
					y-ts.imgs[ts.current][index]->h/2, COL_YELLOW, (direction == 0));
	} else if(c.status[Character::REFLECT] == Character::SUFFERING) {
		draw_outline(buffer, ts.imgs[ts.current][index], x-ts.imgs[ts.current][index]->w/2,
					y-ts.imgs[ts.current][index]->h/2, COL_LIGHT_BLUE, (direction == 0));
	} else if(c.status[Character::CLEAR] == Character::SUFFERING) {
		draw_outline(buffer, ts.imgs[ts.current][index], x-ts.imgs[ts.current][index]->w/2,
					y-ts.imgs[ts.current][index]->h/2, COL_BLACK, (direction == 0));
	}

	if(condemnedcounter > 0) {
		textprintf_ex(buffer, font, x, y-25, COL_GREY, -1, "%i", condemnedcounter);
	}
	if(textremframes) {
		textremframes--;
		textout_ex(buffer, font, texttoshow.c_str(), x-10, y-25+textremframes/2, textcol, -1);
	}
}

void Fighter::draw_outline(BITMAP *target, BITMAP *muster, int x, int y, int color, bool flip) {
	for(int i = 0; i < muster->w; i++)
		for(int j = 0; j < muster->h; j++) {
			int c = getpixel(muster, i, j);
			if(getr(c) < 30 && getg(c) < 30 && getb(c) < 30)
				putpixel(target, (flip ? x+muster->w-i : x+i), y+j, color);
		}
}

void Fighter::draw_status(BITMAP *buffer, int x, int y, int w, int h) {
	textout_ex(buffer, font, c.name.c_str(), x+5, y+h/2-text_height(font)/2, COL_WHITE, -1);
	char text[10];
	sprintf(text, "%i", c.curhp);
	if(c.status[Character::NEAR_FATAL] == Character::SUFFERING)
		textout_right_ex(buffer, font, text, x+w*2/3, y+h/2-text_height(font)/2, COL_YELLOW, -1);
	else if(c.status[Character::WOUND] == Character::SUFFERING || c.status[Character::ZOMBIE] == Character::SUFFERING)
		textout_right_ex(buffer, font, text, x+w*2/3, y+h/2-text_height(font)/2, COL_RED, -1);
	else
		textout_right_ex(buffer, font, text, x+w*2/3, y+h/2-text_height(font)/2, COL_WHITE, -1);
	rect(buffer, x+w*2/3+2, y+h/2-4, x+w-3, y+h/2+3, COL_WHITE);

	int color;
	if(atb < 65536) {
		if(c.status[Character::HASTE] == Character::SUFFERING)
			color = COL_LIGHT_BLUE;
		else if(c.status[Character::SLOW] == Character::SUFFERING ||
				c.status[Character::STOP] == Character::SUFFERING)
			color = COL_RED;
		else
			color = COL_WHITE;
	} else {
		color = COL_YELLOW;
	}

	rectfill(buffer, x+w*2/3+4, y+h/2-2, x+w*2/3+4+(atb*(w/3-8))/65536, y+h/2+1, color);
}

void Fighter::get_ready() {
	atb = 0;
}

bool Fighter::is_friend() {
	if(parent->get_team(this) == Fight::FRIEND) return true;
	return false;
}

Character Fighter::get_character() {
	return c;
}

void Fighter::override_character(Character o) {
	if(o.name != "") c.name = o.name;
	c.defensive = o.defensive;
	if(o.hp >= 0) c.hp = o.hp;
	if(o.curhp >= 0) c.curhp = o.curhp;
	if(o.mp >= 0) c.mp = o.mp;
	if(o.curmp >= 0) c.curmp = o.curmp;
	if(o.speed >= 0) c.speed = o.speed;
	if(o.vigor >= 0) c.vigor = o.vigor;
	if(o.stamina >= 0) c.stamina = o.stamina;
	if(o.mpower >= 0) c.mpower = o.mpower;
	if(o.apower >= 0) c.apower = o.apower;
	if(o.mdefense >= 0) c.mdefense = o.mdefense;
	if(o.adefense >= 0) c.adefense = o.adefense;
	if(o.mblock >= 0) c.mblock = o.mblock;
	if(o.ablock >= 0) c.ablock = o.ablock;
	if(o.xp >= 0) c.xp = o.xp;
	if(o.levelupxp >= 0) c.levelupxp = o.levelupxp;
	if(o.level >= 0) c.level = o.level;
	if(o.hitrate >= 0) c.hitrate = o.hitrate;
	for(int i = 0; i < 11; i++)
		if(o.elements[i] != Character::NORMAL) {
			for(int j = 0; j < 11; j++)
				c.elements[i] = o.elements[i];
			break;
		}
	for(int i = 0; i < 27; i++)
		if(o.status[i] != Character::NORMAL) {
			for(int j = 0; j < 27; j++) {
				c.status[j] = o.status[j];
			}
			break;
		}
}

void Fighter::lose_health(int hp) {
	if(hp == MAX_DAMAGE+1) {
		show_text("Miss", COL_WHITE, GAME_TIMER_BPS/2);
	} else if(hp == MAX_DAMAGE+2) {
		show_text("Block", COL_WHITE, GAME_TIMER_BPS/2);
	} else if(hp < 0) {
		show_text(to_string(hp*-1), COL_GREEN, GAME_TIMER_BPS/2);
	} else {
		show_text(to_string(hp), COL_WHITE, GAME_TIMER_BPS/2);
	}

	if(hp > MAX_DAMAGE) hp = 0;
	c.curhp -= hp;
	if(c.curhp <= 0) {
		c.curhp = 0;
	}
	if(c.curhp > c.hp) c.curhp = c.hp;
}

bool Fighter::lose_mp(int mp) {
	//Bin mir nicht sicher, ob das Mute-verhalten so richtig ist
	if(c.curmp < mp || (mp > 0 && c.status[Character::MUTE] == Character::SUFFERING))
		return false;
	if(mp < 0) {
		show_text(to_string(mp) + "MP", COL_GREEN, GAME_TIMER_BPS/2);
	}
	c.curmp -= mp;
	return true;
}

void Fighter::show_text(string text, int color, int frames) {
	texttoshow = text;
	textcol = color;
	textremframes = frames;
}

int Fighter::get_status(int status) {
	if(status >= 27 || status < 0) return -1;
	return c.status[status];
}

void Fighter::set_status(int status, int state) {
	if(status >= 27 || status < 0 || !(state == Character::NORMAL || state == Character::IMMUNE || state == Character::SUFFERING)) return;
	c.status[status] = state;
}

bool Fighter::get_special(int special) {
	if(special < 15 && special >= 0)
		return c.special[special];
	return false;
}

void Fighter::set_special(int special, bool state) {
	if(special < 15 && special >= 0)
		c.special[special] = state;
}

bool Fighter::has_menu_entry(string name) {
	MenuEntry *e = menu.get_menu_entry("Menu", NULL);

	unsigned int sz = e->submenu.size();
	//Maximale Suchtiefe ist 2 - reicht fürs erste
	for(unsigned int i = 0; i < sz; i++) {
		if(e->submenu[i].text == name)
			return true;
		unsigned int sz2 = e->submenu[i].submenu.size();
		for(unsigned int j = 0; j < sz2; j++) {
			if(e->submenu[i].submenu[j].text == name)
				return true;
		}
	}
	return false;
}

FighterBase::MenuEntry* Fighter::FighterMenu::get_menu_entry(string name, MenuEntry *e) {
	if(!e) e = &menu;
	if(e->text == name)
		return e;

	for(unsigned int i = 0; i < e->submenu.size(); i++) {
		MenuEntry *ret = NULL;
		ret = get_menu_entry(name, &e->submenu[i]);
		if(ret)
			return ret;
	}

	return NULL;
}

Fighter::FighterMenu::FighterMenu() {
	fighter = NULL;
}

Fight *get_parent(Fighter& f) {
	return f.parent;
}

void Fighter::FighterMenu::set_parent(Fighter *fighter) {
	this->fighter = fighter;
}

Fighter::FighterMenu::~FighterMenu() {
}

void Fighter::FighterMenu::set_items(deque< deque<string> > items) {
	//Das Speicherformat gefällt mir überhaupt nicht (nur einfach verschachtelt)…
	//bei gelegenheit ändern
	MenuEntry e, e2;
	menu.text = "Menu";
	for(unsigned int i = 0; i < items.size(); i++) {
		e.submenu.resize(0);
		e.text = items[i][0];
		for(unsigned int j = 1; j < items[i].size(); j++) {
			e2.text = items[i][j];
			e.submenu.push_back(e2);
		}
		menu.submenu.push_back(e);
	}
}

void Fighter::get_screen_position(int *x, int *y) {
	*x = scrposx;
	*y = scrposy;
}

Hero::Hero(Fight *f, Character c, string name, PlayerSide side, int dir)
	: Fighter(f, c, name, side, dir) {
}

int Hero::get_xp(int xp) {
	c.xp += xp;
	c.levelupxp -= xp;

	if(c.xp < xptable[c.level+1])
		return 0;

	while(c.xp >= xptable[c.level+1] && c.level < 100) {
		c.level++;
		c.levelupxp = xptable[c.level+1]-c.xp;

		//HP etc steigern…
		c.hp += hptable[c.level-1];
		if(c.hp > MAX_HP) c.hp = MAX_HP;
		c.mp += mptable[c.level-1];
		if(c.mp > MAX_MP) c.mp = MAX_MP;
	}
	return 1;
}

void Hero::update() {
	Fighter::update();

	if(c.status[Character::WOUND] != Character::SUFFERING &&
	c.status[Character::STOP] != Character::SUFFERING &&
	c.status[Character::SLEEP] != Character::SUFFERING &&
	c.status[Character::PETRIFY] != Character::SUFFERING)

	if(atb < 65536) {
		if(c.status[Character::HASTE] == Character::SUFFERING)
			atb += 63*(c.speed+20) / 16;
		else if(c.status[Character::SLOW] == Character::SUFFERING)
			atb += 24*(c.speed+20) / 16;
		else
			atb += 3*(c.speed+20);
	}
}

void Hero::draw(BITMAP *buffer, int x, int y) {
	switch(current_animation) {
		case ATTACK:
			x -= (get_side()-1)*(step*3*PC_RESOLUTION_X/8)/GAME_TIMER_BPS;
			if(ts.saved == FighterTileset::ATTACK_WAIT) {
				ts.current = FighterTileset::ATTACK;
				ts.saved = FighterTileset::NORMAL;
			} else if(ts.saved == FighterTileset::MAGIC_WAIT) {
				ts.current = FighterTileset::MAGIC;
				ts.saved = FighterTileset::NORMAL;
			}
		break;

		case RETURN:
			x -= (get_side()-1)*((GAME_TIMER_BPS/3-step)*3*PC_RESOLUTION_X/8)/GAME_TIMER_BPS;
			ts.current = FighterTileset::ATTACK;
		break;

		case NORMAL:
			if(c.status[Character::WOUND] == Character::SUFFERING) {
				ts.current = FighterTileset::WOUND;
				ts.saved = FighterTileset::WOUND;
			}

			if(ts.saved == FighterTileset::NORMAL) {
				if(	c.status[Character::NEAR_FATAL] == Character::SUFFERING ||
					c.status[Character::POISON] == Character::SUFFERING ||
					c.status[Character::SEIZURE] == Character::SUFFERING) {
					ts.current = FighterTileset::CRITICAL;
				} else {
					ts.current = FighterTileset::NORMAL;
				}
			} else {
				ts.current = ts.saved;
			}
		break;

		case WAIT_TO_CAST_SPELL:
			ts.saved = FighterTileset::MAGIC_WAIT;
			ts.current = FighterTileset::MAGIC_WAIT;
		break;

		case WAIT_TO_ATTACK:
			ts.saved = FighterTileset::ATTACK_WAIT;
		case DEFEND:
		case EVADE:
			ts.current = FighterTileset::ATTACK_WAIT;
		break;

		case ATTACK_IN_PROGRESS:
			x -= (get_side()-1)*(PC_RESOLUTION_X/8);
			if(ts.current == FighterTileset::ATTACK)
				ts.current = FighterTileset::ATTACK_EXEC;
			else if(ts.current == FighterTileset::MAGIC)
				ts.current = FighterTileset::MAGIC_EXEC;
		break;

		case HURT:
			if(c.status[Character::WOUND] != Character::SUFFERING)
				ts.current = FighterTileset::HIT;
		break;
		
		case CHEERING:
			if(((4*step)/GAME_TIMER_BPS)%2 == 0) {
				ts.current = FighterTileset::MAGIC_EXEC;
			} else {
				ts.current = FighterTileset::NORMAL;
			}
		break;

		default:
		break;
	}

	Fighter::draw(buffer, x, y);
}

Monster::Monster(Fight *f, Character c, string name, PlayerSide side, int dir)
	: Fighter(f, c, name, side, dir) {
	laden(name);
}

void Monster::update() {
	Fighter::update();

	if(c.status[Character::WOUND] != Character::SUFFERING &&
	c.status[Character::STOP] != Character::SUFFERING &&
	c.status[Character::SLEEP] != Character::SUFFERING &&
	c.status[Character::PETRIFY] != Character::SUFFERING)

	if(atb < 65536) {
		if(c.status[Character::HASTE] == Character::SUFFERING) // Berechnungen in der Algorithms FAQ sind offensichtlich falsch…
			atb += 63*(c.speed+20) / 16;
		else if(c.status[Character::SLOW] == Character::SUFFERING)
			atb += 24*(c.speed+20) / 16;
		else
			atb += 3*(c.speed+20);
	}

	if(c.status[Character::WOUND] == Character::SUFFERING ||
		c.status[Character::PETRIFY] == Character::SUFFERING) {
		if(current_animation != DIE) {
			set_animation(DIE);
		} else if(step > GAME_TIMER_BPS/3) {
			parent->defeated_fighter(this);
		}
	}
}

void Monster::draw(BITMAP *buffer, int x, int y) {
	switch(current_animation) {
		case HURT:
			x += step%3-1;
		break;
	
		case DIE:
		{
			set_trans_blender(200, 128, 255, 0);
			int index = (step/SPRITE_ANIMATION_SPEED)%ts.imgs[ts.current].size();

			//spiegeln, wenn direction = 0(links)
			if(direction == 0) {
				BITMAP *temp = create_bitmap(ts.imgs[ts.current][index]->w, ts.imgs[ts.current][index]->h);
				draw_sprite_h_flip(temp, ts.imgs[ts.current][index], 0, 0);
				draw_lit_sprite(buffer, ts.imgs[ts.current][index], x-ts.imgs[ts.current][index]->w/2, y-ts.imgs[ts.current][index]->h/2, 100+step*300/GAME_TIMER_BPS);
				destroy_bitmap(temp);
			} else {
				draw_lit_sprite(buffer, ts.imgs[ts.current][index], x-ts.imgs[ts.current][index]->w/2, y-ts.imgs[ts.current][index]->h/2, 100+step*300/GAME_TIMER_BPS);
			}
			if(textremframes) {
				textremframes--;
				textout_ex(buffer, font, texttoshow.c_str(), x-10, y-25+textremframes/2, textcol, -1);
			}
		}
		return;

		default:
		break;
	}

	Fighter::draw(buffer, x, y);
}

Command Monster::get_command() {
	Command c(this);
	if(com_script.size() == 0 || Fighter::c.status[Character::BERSERK] == Character::SUFFERING) {
		c.set_attack("Battle");
		return c;
	}

	int index = 0;

	while(true) {
		if(com_script[comcounter][index] == "if") {
			//Stimmt zwar nicht, aber das Skriptsystem muss sowieso überarbeitet werden
			if(com_script[comcounter][index+1] == "var") {
				index++;
			}
			if(com_script[comcounter][index+3] == "var") {
				index++;
			}
			index += 5;
			for(unsigned int i = 0; i < com_script[comcounter].size(); i++) {
				if(com_script[comcounter][i] == "else")
					com_script[comcounter].erase(com_script[comcounter].begin()+i);
			}

		} if(com_script[comcounter][index] == "rand") {
			c.set_attack(com_script[comcounter][index + 1 + random()%(com_script[comcounter].size()-index-1)]);
			break;
		} else {
			c.set_attack(com_script[comcounter][index]);
			break;
		}
	}

	comcounter++;
	if(comcounter >= com_script.size()) comcounter = 0;
	return c;
}

void Monster::laden(string name) {
	string path;
	path = string("Fights/Fighters/") + name + string("/");
	FileParser parser(path + name, "Fighter");

	t.xp = parser.getvalue("Treasure", "xp", 0);
	t.gp = parser.getvalue("Treasure", "gp", 0);
	t.dropped_items = parser.get("Treasure", "Dropped");
	t.stolen_items = parser.get("Treasure", "Stolen");
	t.morph_items = parser.get("Treasure", "MorphItems");
	t.morph = parser.getvalue("Treasure", "Morph", 0);

	com_script = parser.getsection("Script");
	comcounter = 0;
}

Monster::Treasure Monster::treasure() {
	return t;
}
