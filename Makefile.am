# $Id: Makefile.am,v 1.13 2002/10/15 00:07:51 mysidia Exp $
#
#
DEFS = -D_IRCLIB_INTERNAL @DEFS@

bin_PROGRAMS = stest ctest sockmon
stest_SOURCES = stest.c
stest_LDADD = libirc.la -levent -ladns

ctest_SOURCES = ctest.c
ctest_LDADD = libirc.la -levent -ladns

sockmon_SOURCES = sockmon.c
sockmon_LDADD = libirc.la -levent -ladns

lib_LTLIBRARIES = libirc.la
libirc_la_SOURCES = match.c mem.c sock.c server.c irc.c channel.c dns.c hash.c \
 proto.c
include_HEADERS = irclib.h proto.h

DISTCLEANFILES = config.guess config.sub ltconfig ltmain.sh libtool
