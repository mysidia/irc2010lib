# $Id: Makefile.am,v 1.9 2001/10/31 01:23:19 mysidia Exp $
#
#
DEFS = -D_IRCLIB @DEFS@

bin_PROGRAMS = stest ctest
stest_SOURCES = stest.c
stest_LDADD = libirc.la -levent -ladns

ctest_SOURCES = ctest.c
ctest_LDADD = libirc.la -levent -ladns

lib_LTLIBRARIES = libirc.la
libirc_la_SOURCES = match.c mem.c sock.c server.c irc.c channel.c dns.c

DISTCLEANFILES = config.guess config.sub ltconfig ltmain.sh libtool
