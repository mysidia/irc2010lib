# $Id: Makefile.am,v 1.5 2001/10/24 03:51:34 mysidia Exp $
#
#
DEFS = -D_IRCLIB @DEFS@

bin_PROGRAMS = stest
stest_SOURCES = stest.c
stest_LDADD = libirc.la -levent

lib_LTLIBRARIES = libirc.la
libirc_la_SOURCES = match.c mem.c sock.c server.c irc.c channel.c

