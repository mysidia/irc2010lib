# $Id: Makefile.am,v 1.7 2001/10/27 19:01:53 mysidia Exp $
#
#
DEFS = -D_IRCLIB @DEFS@

bin_PROGRAMS = stest
stest_SOURCES = stest.c
stest_LDADD = libirc.la -levent -ladns

lib_LTLIBRARIES = libirc.la
libirc_la_SOURCES = match.c mem.c sock.c server.c irc.c channel.c dns.c

DISTCLEANFILES = config.guess config.sub ltconfig ltmain.sh libtool
