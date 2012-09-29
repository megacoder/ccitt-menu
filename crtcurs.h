#ifndef	_CRTCURS_H
#define	_CRTCURS_H

#include <curses.h>
#include <ctype.h>

void crtini( void);
void crtfin( void);
void crtpos(int,int);
void crtflush( void);
void crtclr( void);
void crtces( void);
void crtcel( void);
void crtput( char const   *	s);
void crtget( char *		s);

#endif	/* _CRTCURS_H */
