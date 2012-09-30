#ifndef _CCITT_H
#define _CCITT_H

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

typedef struct ccitt_item_s	{
	void		(*i_init)();	 /* Init routine		  */
	void		(*i_action)();	 /* Action routine		  */
	char   *	i_legend;	/* Descriptive text		 */
}	ccitt_item_t;

typedef ccitt_item_t *  CCITT_ITEM;

/*
 *------------------------------------------------------------------------
 * Shape of a CCITT menu
 *------------------------------------------------------------------------
 */

typedef struct ccitt_menu_s	{
	char   *	m_title;	/* Name of the menu		 */
	int		m_nitem;	/* Number of items on menu	 */
	CCITT_ITEM	m_item;		/* List of items on menu	 */
	char   *	m_prompt;	/* Prompt for the menu		 */
}	ccitt_menu_t, *CCITT_MENU;

#define MENUSIZE(m)	( sizeof( (m) ) / sizeof( (m)[0] ) )

/*
 *------------------------------------------------------------------------
 * Function manifest
 *------------------------------------------------------------------------
 */

void		ccitt_center( int, char const *, ... );/* Centered text	 */
CCITT_ITEM	ccitt_menu( CCITT_MENU, int, int );/* Process a menu	 */
void		ccitt_msg( char const *, ... );/* Show string on bottom	 */
void		ccitt_paint( CCITT_MENU, int, int );/* Display a menu	 */
void		ccitt_printf( int, int, char const *, ... );/* Show at X,Y */
char const *	ccitt_prompt( char const *, ... );/* Prompt and get ans  */
int             ccitt_stricmp( char const *, char const *); /* Ignore case */
int		ccitt_subtitle( int, int, char const *, ... );/* Subtitle (centered) */
int             ccitt_title( char const *, char const *, char const * );/* Title */

#endif	/* _CCITT_H */
