# $Id: Makefile.am,v 1.6 2001/10/24 06:00:52 mysidia Exp $
#
#
DEFS = -D_IRCLIB @DEFS@

bin_PROGRAMS = stest
stest_SOURCES = stest.c
stest_LDADD = libirc.la -levent -ladns

lib_LTLIBRARIES = libirc.la
libirc_la_SOURCES = match.c mem.c sock.c server.c irc.c channel.c dns.c

