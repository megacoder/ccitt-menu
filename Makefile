#########################################################################
#                       COPYRIGHT NOTICE
#
#       Copyright (C) 1993 VME Microsystems International Corporation
#	International copyright secured.  All rights reserved.
#########################################################################
#	@(#)Makefile 1.1 12/09/29 VMIC
#########################################################################
#	CCITT Menu Maintenance Script
#########################################################################
#
# Some brain-dead implementations of make(1) use the $SHELL environment
# variable to choose the command interpreter instead of correctly using
# the Bourne shell.  The SHELL macro definition below fixes that; don't
# change it even if you normally use a different shell interactively.
#
SHELL	=/bin/sh
#
#
#
HFILES  =ccitt.h 
#
#
#
CFILES  =ccitt.c crtcurs.c 
#
#
#
CC	=cc
#
#
#
CFLAGS	=-I. -g
#
#
#
LDLIBS	=-lcurses -ltermcap
#
#
#
OBS	=$(CFILES:.c=.o)
#
#
#
all:	ccitt
#
#
#
clean:
	rm -f *.o *.BAK lint tags core install-* ccitt refs
#
#
#
clobber: clean
#
#
#
ccitt.o: ccitt.h ccitt.c
#
#
#
ccitt: ccitt.h ccitt.h crtcurs.o
	$(CC) $(CFLAGS) $(LDFLAGS) -Dtest -o ccitt ccitt.c crtcurs.o $(LDLIBS)
