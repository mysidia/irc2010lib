# $Id: Makefile.am,v 1.4 2001/10/23 23:05:24 mysidia Exp $
#
#
DEFS = -D_IRCLIB @DEFS@

bin_PROGRAMS = stest
stest_SOURCES = stest.c
stest_LDADD = libirc.la -levent

lib_LTLIBRARIES = libirc.la
libirc_la_SOURCES = match.c mem.c sock.c server.c irc.c

