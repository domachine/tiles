/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.cc
 * Copyright (C) Thomas Witte 2009 <t-witte@gmx.net>
 * 
 * main.cc is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * main.cc is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//#define GP2X

#include <allegro.h>
#include <string>
#include "map.h"

#ifdef GP2X
	#include <sys/mman.h>
	#include <allegro/platform/aintlnx.h>
#endif

#ifdef GP2X
void gp2x_init() {
	unsigned long gp2x_dev;
	static volatile unsigned long *gp2x_memregl;
	volatile unsigned short *gp2x_memregs;

	int mmsp2_blit_base;
	static struct MAPPED_MEMORY mmsp2_blit;

	gp2x_dev = open("/dev/mem", O_RDWR); 
	gp2x_memregl = (unsigned long *)mmap(0, 0x10000, PROT_READ|PROT_WRITE, MAP_SHARED, gp2x_dev, 0xc0000000);
	gp2x_memregs = (unsigned short *)gp2x_memregl;

	{
    	//the following is to work around bug in allegro library which causes it to lock up
     	//if HW blitter hasn't done anything since GP2X was switched on.

     	gp2x_memregs[0x90a >> 1] = 0xffff; // Enable all video and sound devices
     	gp2x_memregs[0x904 >> 1] |= 1<<10; // Enable FASTIO for the hardware blitter

     	#define MESGDSTCTRL 0x0000
     	#define MESG_BSTBPP 0x60
     	#define MESG_DSTBPP_16 (1<<5)
     	#define MESG_DSTENB (1<<6)
     	#define MESGDSTADDR 0x0004
     	#define MESGSRCCTRL 0x000c
     	#define MESG_INVIDEO (1<<8)
     	#define MESGPATCTRL 0x0020
     	#define MESGSIZE 0x002c
     	#define MESG_HEIGHT 16
     	#define MESGCTRL 0x0030
     	#define MESG_XDIR_POS (1<<8)
     	#define MESG_YDIR_POS (1<<9)
     	#define MESG_FFCLR (1<<10)
     	#define MESGSTATUS 0x0034
     	#define MESG_BUSY (1<<0)

     	#define mmsp2_blit_putl(addr,value) (*(unsigned long volatile *)(mmsp2_blit_base+(addr)) = (value))
     	#define mmsp2_blit_getl(addr) (*(unsigned long volatile *)(mmsp2_blit_base+(addr)))

     	mmsp2_blit.base = 0xe0020000;
     	mmsp2_blit.size = 0x100;
     	mmsp2_blit.perms = PROT_READ|PROT_WRITE;
     	mmsp2_blit.data = mmap(0, 0x100, PROT_READ|PROT_WRITE, MAP_SHARED, gp2x_dev, 0xe0020000);

     	if (mmsp2_blit.data == MAP_FAILED)
     	exit(EXIT_FAILURE);

     	mmsp2_blit_base = (int)mmsp2_blit.data;

     	mmsp2_blit_putl (MESGDSTCTRL, MESG_DSTBPP_16 | MESG_DSTENB);
     	mmsp2_blit_putl (MESGDSTADDR, 0x3101000);
     	mmsp2_blit_putl (MESGSRCCTRL, MESG_INVIDEO);
     	mmsp2_blit_putl (MESGPATCTRL, 0);
     	mmsp2_blit_putl (MESGSIZE, (1 << MESG_HEIGHT) | 8);
     	mmsp2_blit_putl (MESGCTRL, (MESG_XDIR_POS) | (MESG_YDIR_POS) | 0xaa | MESG_FFCLR);
     	mmsp2_blit_putl (MESGSTATUS, 1);

     	while (mmsp2_blit_getl(MESGSTATUS) & MESG_BUSY);

     	munmap((void *)&mmsp2_blit_base, 0x100);
	}
}

void gp2x_exit() {
	chdir("/usr/gp2x");
	execl("/usr/gp2x/gp2xmenu", "/usr/gp2x/gp2xmenu", NULL);
}

#endif

int main()
{
	#ifdef GP2X	
		gp2x_init();
	#endif
	allegro_init();
	set_color_depth(16);
	#ifdef GP2X
		set_gfx_mode(GFX_AUTODETECT, 320, 240, 0, 0);
	#else
		set_gfx_mode(GFX_AUTODETECT_WINDOWED, 320, 240, 0, 0);
	#endif
	install_joystick(JOY_TYPE_AUTODETECT);
	install_keyboard();
	install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);
	install_timer();
	
	//spiel
	Map map("defaultLevel");
	while(!key[KEY_ESC]) {
		map.update();
		map.draw();
	}
	
	#ifdef GP2X	
		gp2x_exit();
	#endif
	return 0;
}END_OF_MAIN();