# $Id: Makefile.am,v 1.10 2001/11/17 06:38:10 mysidia Exp $
#
#
DEFS = -D_IRCLIB @DEFS@

bin_PROGRAMS = stest ctest
stest_SOURCES = stest.c
stest_LDADD = libirc.la -levent -ladns

ctest_SOURCES = ctest.c
ctest_LDADD = libirc.la -levent -ladns

lib_LTLIBRARIES = libirc.la
libirc_la_SOURCES = match.c mem.c sock.c server.c irc.c channel.c dns.c hash.c

DISTCLEANFILES = config.guess config.sub ltconfig ltmain.sh libtool
