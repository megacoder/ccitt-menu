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

#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <curses.h>
#include <ctype.h>
#include <string.h>

#include <ccitt.h>
#include <crtcurs.h>

#define min(x,y)	(( (x) < (y) ) ? (x) : (y))
#define max(x,y)	(( (x) > (y) ) ? (x) : (y))
#define	bound(x,l,u)	( ((x) <= (l)) ? (l) : ((x) >= (u) ? (u) : (x)) )

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
ccitt_center(
	int		row,
	char const *	fmt,
	...
)
{
	va_list		ap;		/* Walks down arg list		 */
	int		len;		/* Computed length of string	 */
	char		buf[COLS + 1];	/* Where we format the line	 */

	va_start( ap, fmt );
	vsprintf( buf, fmt, ap );
	va_end( ap );
	len = strlen( buf );
	crtpos( row, 1 );
	crtcel();
	crtpos( row, max( 1, ( COLS - len ) / 2 ));
	crtput( buf );
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
	char const *	reply;		/* Points to prompt reply	 */

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
ccitt_msg(
	char const *	fmt,		/* Sprintf-like format		 */
	...
)
{
	va_list		ap;		/* Walks down arg list		 */
	char		buf[COLS + 1];	/* Where we format the line	 */

	crtpos(MSG, 1);
	crtcel();
	va_start(ap, fmt);
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
	origin = ccitt_subtitle(origin, strlen(menu->m_title), menu->m_title);
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
ccitt_printf(
	int		row,		/* Row on CRT for the line	 */
	int		col,		/* Column on CRT for the line	 */
	char const *	fmt,		/* Sprintf()-like format	 */
	...
)
{
	va_list		ap;		/* Walks down arg list		 */
	char		buf[COLS + 1];	/* Where we format the text	 */

	va_start(ap, fmt);
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

char const *
ccitt_prompt(
	char const *	fmt,		/* Prompt string to use		 */
	...
)
{
	va_list		ap;		/* Walks down arg list		 */
	char   *	reply;		/* First non-whitespace in buf	 */
	char   *	bp;		/* End of string + 1		 */

	va_start(ap, fmt);
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
	return( reply );
}

/*
 *------------------------------------------------------------------------
 * ccitt_stricmp: case independant compare two word lists, like strcmp()
 *------------------------------------------------------------------------
 */

int
ccitt_stricmp(
	char *		l,		/* Left string			 */
	char *		r		/* Right string			 */
)
{
	int		lc;		/* Left character		 */
	int		rc;		/* Right character		 */

	for( ; ; )	{
		/* Find next word in left string			 */
		while( ( lc = *l ) && isspace( lc ))	{
			++l;
		}
		if( !lc )	{
			return( 0 );	/* As much as I checked matched	 */
		}
		/* Find next word in right string			 */
		while( ( rc = *r ) && isspace( rc ))	{
			++r;
		}
		if( !rc )	{
			return( -1 );	/* Out of words in right string	 */
		}
		/* Compare characters until they differ			 */
		while(lc && toupper( lc ) == toupper(rc))	{
			lc = *++l;
			rc = *++r;
			if(isspace( lc ) || isspace(rc))
				break;
		}
		/* If we hit end the NULL on left string, we matched	 */
		if( !lc )	{
			return( 0 );	/* OK as far as I checked	 */
		}
		/* Change break whitespace to a space			 */
		if(isspace( lc ))	{
			lc = ' ';
		}
		if(isspace( rc ))	{
			rc = ' ';
		}
		if( lc != rc )	{
			return( -1 );
		}
	}
	/* NOTREACHED							 */
}

/*
 *------------------------------------------------------------------------
 * ccitt_subtitle: put subtitle at center of screen
 *------------------------------------------------------------------------
 */

int
ccitt_subtitle(
	int		origin,		/* First line for subtitle	 */
	int		len,		/* Length of subtitle string	 */
	char const *	fmt,		/* sprintf()-like format	 */
	...
)
{
	va_list		ap;		/* Walks down arg list		 */
	char		buf[COLS + 1];	/* Holds formatted line		 */

	/*
	 * Pick off arguments: ccitt_subtitle( origin, fmt [, arg ] )
	 */
	va_start(ap, fmt );
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
ccitt_title(
	char const *	title,		/* Title of menu		 */
	char const *	version,	/* Software version number	 */
	char const *	date		/* Generation date		 */
)
{
	int		origin = 1;	/* Origin for banner		 */

	ccitt_center( origin, title, version, date );

	++origin;
	crtpos( origin, 1 );
	crtcel();

	return( origin + 1 );		/* Return first usable row	 */
}

#ifdef	etest

void
fini(
	void
)
{
	ccitt_close();
	exit( 0 );
}

char	*me = "ccitt_test";

char	sitem_1[ COLS+1 ];
char	sitem_2[ COLS+1 ];
char	sitem_3[ COLS+1 ];

void
painter(
	CCITT_ITEM	ip
)
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
main(
	int		argc,
	char	* *	argv
)
{
	CCITT_ITEM	ip;
	if(ccitt_open() )	{
		fprintf( stderr, "%s: cannot open CRT\n", me );
		exit( 1 );
	}
	for( ; ; )	{
		ip = ccitt_menu( main_menu, 1, LINES );
		if( ip->i_action ) ip->i_action(ip);
	}
}
#endif	/* etest */
