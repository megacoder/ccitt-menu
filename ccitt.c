/*
 *------------------------------------------------------------------------
 *			 COPYRIGHT NOTICE
 *
 *	 Copyright (C) 1993 VME Microsystems International Corporation
 *	 International copyright secured.  All rights reserved.
 *------------------------------------------------------------------------
 *	SCCS/s.ccitt.c 1.1 07/27/93 12:43:27
 *------------------------------------------------------------------------
 *	CCITT Menu Driver Module
 *------------------------------------------------------------------------
 */

#ifndef lint
static char _ccitt_c_sccs_id[] = "@(#)ccitt.c 1.1 12/09/29 VMIC";
#endif	/* lint */

#include <curses.h>
#include <ctype.h>
#include <varargs.h>

#include <ccitt.h>

#define min(x,y)	(( (x) < (y) ) ? (x) : (y))
#define max(x,y)	(( (x) > (y) ) ? (x) : (y))
#define	bound(x,l,u)	( ((x) <= (l)) ? (l) : ((x) >= (u) ? (u) : (x)) )

/*
 *------------------------------------------------------------------------
 * Standard library definitions
 *------------------------------------------------------------------------
 */

extern long strtol();			/* Convert ASCII to binary	 */

/*
 *------------------------------------------------------------------------
 * ccitt_open:
 *------------------------------------------------------------------------
 */

int
ccitt_open()
{
	initscr();
	cbreak();
	noecho();
	return( 0 );
}

/*
 *------------------------------------------------------------------------
 * ccitt_close:
 *------------------------------------------------------------------------
 */

void
ccitt_close()
{
	endwin();
}

/*
 *------------------------------------------------------------------------
 * ccitt_center: pad string with leading spaces to place string in mid-screen
 *
 * Call like this: ccitt_center( row, fmt [, arg...] ), sorta like printf().
 *------------------------------------------------------------------------
 */

void
ccitt_center(va_alist)
va_dcl
{
	va_list ap;			/* Walks down arg list		 */
	char   *fmt;			/* sprintf()-like format	 */
	int	row;			/* Row on crt for the line	 */
	int	len;			/* Length of formatted buffer	 */
	char	buf[COLS + 1];		/* Where we format the line	 */

	va_start(ap);
	row = va_arg(ap, int);
	fmt = va_arg(ap, char *);
	vsprintf(buf, fmt, ap);
	va_end(ap);
	len = strlen(buf);
	crtpos(row, 1);
	crtcel();
	crtpos(row, max(1, (COLS - len) / 2));
	crtput(buf);
}

/*
 *------------------------------------------------------------------------
 * ccitt_menu: display and process a menu
 *------------------------------------------------------------------------
 */

CCITT_ITEM
ccitt_menu(menu, origin, lines)
CCITT_MENU menu;			/* Menu to be processed		 */
int	origin;				/* First line for submenu	 */
int	lines;				/* Line limit for menu		 */
{
	char   *eos;			/* What terminates the input num */
	CCITT_ITEM it;			/* Walks down item list		 */
	CCITT_ITEM lit;			/* Last item table entry + 1	 */
	int	choice;			/* What item number user chose	 */
	CCITT_ITEM candidate;		/* Candidate matching item	 */
	char   *reply;			/* Points to prompt reply	 */

	/* Make sure this menu is on the screen				 */
	ccitt_paint(menu, origin, lines);
	/* Get the number of the menu item to execute			 */
	for (;;) {
		reply = ccitt_prompt("%s> ",
		 menu->m_prompt ? menu->m_prompt : "Enter item number");
		/* Ignore null responses, you can't default on a menu!	 */
		if (*reply == '\0')
			continue;
		/* Attempt to parse the reply as the numeric item number */
		choice = (int) strtol(reply, &eos, 0);
		candidate = menu->m_item + choice - 1;
		/*
		 * Permit numeric selection if it's within range of 1..N
		 */
		if (choice > 0 && choice <= menu->m_nitem && !*eos) {
			return (candidate);
		}
		/* Well, maybe they typed the legend itself		 */
		candidate = NULL;
		for(it=menu->m_item, lit = it + menu->m_nitem; it < lit; ++it) {
			if (ccitt_stricmp(reply, it->i_legend) == 0) {
				if (candidate)
					goto Reject;
				candidate = it;
			}
		}
		if (candidate)
			return (candidate);
Reject:
		/* Hiss, Boo!						 */
		ccitt_msg("Unknown item = %s", reply);
		(void) ccitt_prompt(
				    "Please enter an item number or its title; press ENTER> ");
		ccitt_msg((char *) NULL);
	}
}

/*
 *------------------------------------------------------------------------
 * ccitt_msg: output string to message line.
 *------------------------------------------------------------------------
 */

void
ccitt_msg(va_alist)
va_dcl
{
	va_list ap;			/* Walks down arg list		 */
	char   *fmt;			/* sprintf()-like format	 */
	char	buf[COLS + 1];		/* Where we format the line	 */

	crtpos(MSG, 1);
	crtcel();
	va_start(ap);
	fmt = va_arg(ap, char *);
	if (fmt) {
		vsprintf(buf, fmt, ap);
		crtput(buf);
	}
	va_end(ap);
}

/*
 *------------------------------------------------------------------------
 * ccitt_paint: display a menu
 *------------------------------------------------------------------------
 */

void
ccitt_paint(menu, origin, lines)
CCITT_MENU menu;			/* Menu to be processed		 */
int	origin;				/* First line for submenu	 */
int	lines;				/* Line limit for menu		 */
{
	int	row;			/* CRT row of next line		 */
	int	itemno = 0;		/* Counts items in the list	 */
	int	margin;			/* Left margin for menu		 */
	int	width;			/* Max legend width		 */
	int	col;			/* Current item column		 */
	int	ncol;			/* Number of item columns req'd	 */
	char   *eos;			/* What terminates the input num */
	CCITT_ITEM it;			/* Walks down item list		 */
	CCITT_ITEM lit;			/* Last item table entry + 1	 */

	/* Clear the screen first, to allow init routine to draw stuff	 */
	crtpos(origin, 1);
	crtces();
	/* Initialize each item with an i_init routine			 */
	for (it = menu->m_item, lit = it + menu->m_nitem; it < lit; ++it) {
		if (it->i_init)
			(*(it->i_init)) (it);
	}
	/* Output the menu title					 */
	row = origin;
	origin = ccitt_subtitle(origin, menu->m_title);
	if ((lines -= (origin - row)) < 1)
		lines = 1;
	/* Compute max permitted width of a legend string		 */
	ncol = (menu->m_nitem + lines - 1) / lines;
	width = 0;
	for (it = menu->m_item, lit = it + menu->m_nitem; it < lit; ++it)
		width = max(width, strlen(it->i_legend));
	width = bound(width, 1, (COLS - 5 * ncol) / ncol);
	margin = ((COLS - ((4 + width) * ncol)) / (ncol + 1));
	/* Paint each menu item						 */
	row = origin;
	col = 0;
	for (it = menu->m_item, lit = it + menu->m_nitem; it < lit; ++it) {
		++itemno;
		ccitt_printf(row, 1 + (margin * (col + 1)) + ((width + 4) * col),
		     "%2d) %-*.*s", itemno, width, width, it->i_legend);
		if (++row >= (origin + lines)) {
			row = origin;
			++col;
		}
	}
}

/*
 *------------------------------------------------------------------------
 * ccitt_printf: output string to screen
 *
 * Call like this: ccitt_printf( row, col, fmt [, arg...] ), like printf().
 *------------------------------------------------------------------------
 */

void
ccitt_printf(va_alist)
va_dcl
{
	va_list ap;			/* Walks down arg list		 */
	char   *fmt;			/* sprintf()-like format	 */
	int	row;			/* Row on crt for the line	 */
	int	col;			/* Column on crt for the line	 */
	char	buf[COLS + 1];		/* Where we format the text	 */

	va_start(ap);
	row = va_arg(ap, int);
	col = va_arg(ap, int);
	fmt = va_arg(ap, char *);
	vsprintf(buf, fmt, ap);
	va_end(ap);
	crtpos(row, col);
	crtcel();
	crtput(buf);
}

/*
 *------------------------------------------------------------------------
 * ccitt_prompt: issue a prompt string and get a one-line reply
 *------------------------------------------------------------------------
 */
static char buf[COLS + 1];		/* I/O buffer			 */

char   *
ccitt_prompt(va_alist)
va_dcl
{
	va_list ap;			/* Walks down arg list		 */
	char   *reply;			/* First non-whitespace in buf	 */
	char   *bp;			/* End of string + 1		 */
	char   *fmt;			/* sprintf()-like format	 */

	va_start(ap);
	fmt = va_arg(ap, char *);
	vsprintf(buf, fmt, ap);
	va_end(ap);
	crtpos(BOTTOM, 1);
	crtcel();
	crtput(buf);
	crtget(buf);
	/* Drop leading white space					 */
	for (reply = buf; *reply && isspace(*reply); ++reply);
	/* Drop trailing white space					 */
	for (bp = reply; *bp; ++bp);
	while (bp > reply && isspace(*(bp - 1)))
		*--bp = '\0';
	return (reply);
}

/*
 *------------------------------------------------------------------------
 * ccitt_stricmp: case independant compare two word lists, like strcmp()
 *------------------------------------------------------------------------
 */

int
ccitt_stricmp(l, r)
register char *l;			/* Left string			 */
register char *r;			/* Right string			 */
{
	register int lc;		/* Left character		 */
	register int rc;		/* Right character		 */

	for (;;) {
		/* Find next word in left string			 */
		while ((lc = *l) && isspace(lc))
			++l;
		if (!lc)
			return (0);	/* As much as I checked matched	 */
		/* Find next word in right string			 */
		while ((rc = *r) && isspace(rc))
			++r;
		if (!rc)
			return (-1);	/* Out of words in right string	 */
		/* Compare characters until they differ			 */
		while (lc && toupper(lc) == toupper(rc)) {
			lc = *++l;
			rc = *++r;
			if (isspace(lc) || isspace(rc))
				break;
		}
		/* If we hit end the NULL on left string, we matched	 */
		if (!lc)
			return (0);	/* OK as far as I checked	 */
		/* Change break whitespace to a space			 */
		if (isspace(lc))
			lc = ' ';
		if (isspace(rc))
			rc = ' ';
		if (lc != rc)
			return (-1);
	}
	/* NOTREACHED							 */
}

/*
 *------------------------------------------------------------------------
 * ccitt_subtitle: put subtitle at center of screen
 *------------------------------------------------------------------------
 */

int
ccitt_subtitle(va_alist)
va_dcl
{
	va_list ap;			/* Walks down arg list		 */
	int	origin;			/* First line for subtitle	 */
	int	len;			/* Length of subtitle string	 */
	char   *fmt;			/* sprintf()-like format	 */
	char	buf[COLS + 1];		/* Holds formatted line		 */

	/*
	 * Pick off arguments: ccitt_subtitle( origin, fmt [, arg ] )
	 */
	va_start(ap);
	origin = va_arg(ap, int);
	fmt = va_arg(ap, char *);
	vsprintf(buf, fmt, ap);
	va_end(ap);
	/*
	 * Compute center of screen for this string and put it there.
	 */
	len = strlen(buf);
	crtpos(origin, 1);
	crtcel();
	crtpos(origin, max(1, (COLS - len) / 2));
	crtput(buf);
	return (origin + 2);		/* Point to next usable line	 */
}

/*
 *------------------------------------------------------------------------
 * ccitt_title: put title info at top of screen
 *------------------------------------------------------------------------
 */

int
ccitt_title(title, version, date)
char   *title;				/* Title of menu		 */
char   *version;			/* Software version number	 */
char   *date;				/* Generation date		 */
{
	register int origin = 1;	/* Origin for banner		 */

	ccitt_center(origin, title, version, date);

	++origin;
	crtpos(origin, 1);
	crtcel();

	return (origin + 1);		/* Return first usable row	 */
}

#ifdef	test

void
fini()
{
	ccitt_close();
	exit( 0 );
}

char	*me = "ccitt_test";

char	sitem_1[ COLS+1 ];
char	sitem_2[ COLS+1 ];
char	sitem_3[ COLS+1 ];

void
painter( ip )
CCITT_ITEM	ip;
{
	static	int	cycle;
	sprintf( ip->i_legend, "%d-th Item", ++cycle );
}

ccitt_item_t	main_items[] = {
	{ painter, NULL, sitem_1 },
	{ painter, NULL, sitem_2 },
	{ painter, fini, sitem_3 },
};

ccitt_menu_t main_menu = {
	"Main Menu Title", MENUSIZE(main_items), main_items, "Yeah"
};

int
main( argc, argv )
int	argc;
char	**argv;
{
	CCITT_ITEM	ip;
	if( ccitt_open() )	{
		fprintf( stderr, "%s: cannot open CRT\n", me );
		exit(1);
	}
	for( ; ; )	{
		ip = ccitt_menu( main_menu, 1, LINES );
		if( ip->i_action ) ip->i_action( ip );
	}
}
#endif	/* test */
