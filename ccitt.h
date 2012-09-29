/*
 *------------------------------------------------------------------------
 *			 COPYRIGHT NOTICE
 *
 *	 Copyright (C) 1993 VME Microsystems International Corporation
 *	 International copyright secured.  All rights reserved.
 *------------------------------------------------------------------------
 *	SCCS/s.ccitt.h 1.1 07/27/93 12:43:29
 *------------------------------------------------------------------------
 *	CCITT Menu Structure definitions and prototypes.
 *------------------------------------------------------------------------
 */

#ifndef _CCITT_H
#define _CCITT_H

#ifndef lint
static char _ccitt_h_sccs_id[] = "@(#)ccitt.h 1.1 12/09/29 VMIC";
#endif	/* lint */

/*
 *------------------------------------------------------------------------
 * Size and shape of the CRT or VDU
 *------------------------------------------------------------------------
 */

#define COLS	80			/* Default size of CRT screen	 */
#define LINES	24			/* Default height of CRT screen	 */
#define BOTTOM	(LINES-1)		/* Last usable line for prompts	 */
#define MSG	(LINES)			/* Status line			 */

/*
 *------------------------------------------------------------------------
 * Shape of a CCITT menu item
 *------------------------------------------------------------------------
 */

typedef struct ccitt_item_s {
	void	(*i_init) ();		/* Initialization routine	 */
	void	(*i_action) ();		/* Action routine		 */
	char   *i_legend;		/* Descriptive text		 */
}	ccitt_item_t, *CCITT_ITEM;

/*
 *------------------------------------------------------------------------
 * Shape of a CCITT menu
 *------------------------------------------------------------------------
 */

typedef struct ccitt_menu_s {
	char   *m_title;		/* Name of the menu		 */
	int	m_nitem;		/* Number of items on menu	 */
	CCITT_ITEM m_item;		/* List of items on menu	 */
	char   *m_prompt;		/* Prompt for the menu		 */
}	ccitt_menu_t, *CCITT_MENU;

#define MENUSIZE(m)	( sizeof( (m) ) / sizeof( (m)[0] ) )

/*
 *------------------------------------------------------------------------
 * Function manifest
 *------------------------------------------------------------------------
 */

extern void ccitt_center();		/* Display centered text	 */
extern CCITT_ITEM ccitt_menu();		/* Process a menu		 */
extern void ccitt_msg();		/* Output string at bottom line	 */
extern void ccitt_paint();		/* Display a menu		 */
extern void ccitt_printf();		/* Output string at pos on crt	 */
extern char *ccitt_prompt();		/* Prompt and get response	 */
extern int ccitt_stricmp();		/* Compare words in two strings	 */
extern int ccitt_subtitle();		/* Put subtitle at screen center */
extern int ccitt_title();		/* Put title at top of screen	 */

#undef	P

#endif	/* _CCITT_H */