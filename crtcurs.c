/*
 *------------------------------------------------------------------------
 *                       COPYRIGHT NOTICE
 *
 *       Copyright (C) 1993 VME Microsystems International Corporation
 *	 International copyright secured.  All rights reserved.
 *------------------------------------------------------------------------
 *	SCCS/s.crtcurs.c 1.2 07/27/93 12:43:33
 *------------------------------------------------------------------------
 *	CCITT Menu VDU Handler
 *------------------------------------------------------------------------
 */

#include <curses.h>
#include <ctype.h>

#include <crtcurs.h>

/*
 *------------------------------------------------------------------------
 * crtini: prepare VDU for formatted screen operation
 *------------------------------------------------------------------------
 */

void
crtini(
	void
)
{

	initscr();
	cbreak();
	noecho();
}

/*
 *------------------------------------------------------------------------
 * crtfin: perform any cleanup required when leaving formatted screen I/O
 *------------------------------------------------------------------------
 */

void
crtfin(
	void
)
{

	endwin();
}

/*
 *------------------------------------------------------------------------
 * crtpos: move cursor to specified row and column address (1-based)
 *------------------------------------------------------------------------
 */

void
crtpos(
	int	row,			/* Line on screen (1-based)	 */
	int	col			/* Column on screen (1-based)	 */
)
{

	move(row - 1, col - 1);
}

/*
 *------------------------------------------------------------------------
 * crtflush: force any queued I/O out to the screen
 *------------------------------------------------------------------------
 */

void
crtflush(
	void
)
{

	refresh();
}

/*
 *------------------------------------------------------------------------
 * crtclr: clear entire screen
 *------------------------------------------------------------------------
 */

void
crtclr(
	void
)
{

	clear();
}

/*
 *------------------------------------------------------------------------
 * crtces: clear from cursor to end of screen
 *------------------------------------------------------------------------
 */

void
crtces(
	void
)
{

	clrtobot();
}

/*
 *------------------------------------------------------------------------
 * crtcel: clear from cursor to end of line (don't care where cursor is left)
 *------------------------------------------------------------------------
 */

void
crtcel(
	void
)
{

	clrtoeol();
}

/*
 *------------------------------------------------------------------------
 * crtput: write string to screen at current cursor address
 *------------------------------------------------------------------------
 */

void
crtput(
	char const   *	s
)
{

	addstr(s);
}

/*
 *------------------------------------------------------------------------
 * getstring: private function for handling erase characters and repainting
 *------------------------------------------------------------------------
 */

static	void
getstring(
	char   *	s
)
{
	register char	*str = s;	/* Local string address copy	 */
	register int	count = 0;	/* Number of valid characters	 */
	int	y;			/* Current line position	 */
	int	x;			/* Current column position	 */
	register int    c;		/* Character being processed	 */

	/* Loop accepting string and performing editing until \n is seen */

	for (;;) {
		c = getch();
		if (c == erasechar() || c == '\b') {
			if (count > 0) {
				getyx(stdscr, y, x);
				move(y, x - 1);
				delch();
				if( iscntrl( *(str-1) ) )	{
					/*
					 * Control characters take two
					 * screen positions.
					 */
					move( y, x - 2 );
					delch();
				}
				refresh();
				str--;
				count--;
			}
		} else if (c == '\n') {
			addch(c);
			refresh();
			*str = 0;
			break;		/* LOOP EXIT */
		} else if (c == '\f') {
			touchwin(stdscr);
			refresh();
		} else {
			addch(c);
			refresh();
			*str++ = c;
			count++;
		}
	}
}

/*
 *------------------------------------------------------------------------
 * crtget: accept one line of input from the screen
 *------------------------------------------------------------------------
 */

void
crtget(
	char *		s
)
{
	/* Read a string from the crt terminal */

	crtflush();
	getstring(s);
}
