/*
 * Library Header
 * Copyright (c) 2001 ***REMOVED***
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the authors nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/**
 * @file irclib.h
 * @brief Library Header
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <time.h>

#define NUM_MODES 52

#define IRC(x)	IrcLib##x

#define IRCLIB_MMP(ma,mi,pl) \
        (((ma) << 16) + ((mi) << 8) + (pl))

#define _IRCLIB IRCLIB_MMP(0,0,2)

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#ifdef HAVE_VALUES_H
#include <values.h>
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

/*#ifdef HAVE_SYS_QUEUE_H
#include <sys/queue.h>
#elif defined(HAVE_QUEUE_H)
#include <queue.h>
#endif*/

#include "queue.h"
#include "hash.h"
#include "timer.h"

#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef HAVE_EVENT_H
#include <event.h>
#endif

#include <stdarg.h>

#define IRC_MAXARGS 15

int match(const char*, const char*);
void *oalloc(size_t);
char *str_dup(const char *);
void str_free(char *s);

struct IRC(_Message);
struct _ircsocket;

#define SOCKBUFSIZE 1024
#define IRCBUFSIZE 512
#define IRCBUFSZ   512

#define NICKLEN		30
#define HOSTLEN		63
#define USERLEN		8

#define IRCSOCK_BOUND		0x1
#define IRCSOCK_LISTEN		0x2
#define IRCSOCK_INCONN		0x4
#define IRCSOCK_ESTAB		0x8
#define IRCSOCK_WRITE		0x10
#define IRCSOCK_INBOUND		0x20

/**
 * Irc socket message buffers
 */
struct _ircbf
{
        struct _ircbq *firstEls, *lastEls;
};

typedef struct _ircbf IrcBuf;
typedef struct _ircbf IrcLibSocketBuf;

#ifdef _IRCLIB_INTERNAL
#define PRIV_MEM(type, var, ne) \
		type var
#define PRIV_MEM_AR(type, var, ne) \
		type var[ne]
#else
#define PRIV_MEM(type, var, ne) \
		type _##__LINE__##_##var##__
#define PRIV_MEM_AR(type, var, ne) \
		type _##__LINE__##_##var##__ [ne]
#endif

struct IRC(_MsgTab)
{
	const char *name;
	int (* handler)(struct _ircsocket*, struct IRC(_Message)*);
};

typedef struct IRC(_MsgTab) IRC(MsgTab);

/**
 * An IRC Connection endpoint
 */
struct _ircsocket
{
	PRIV_MEM(int,			fd,		1);
	PRIV_MEM(int,			flags,		1);
	PRIV_MEM(struct event*,		theEvent,	1);
	PRIV_MEM(struct timeval,	tv,		1);
	PRIV_MEM(struct in_addr,	addr,		1);
	PRIV_MEM(char*,			tailBuf,	1);
	PRIV_MEM(IrcLibSocketBuf,	inBuf,		1);
	PRIV_MEM(IrcLibSocketBuf,	outBuf,		1);
	PRIV_MEM(struct _ircTimer,	timer,		1);
	PRIV_MEM(void*,			exData,		1);

	IRC(MsgTab)	*parser;

	time_t lasttime;
	struct _irclistener *port;
	int (* func)(struct _ircsocket*, char *);
	int (* readFunc)(struct _ircsocket*);
	int (* periodic)(struct _ircsocket*);
	int (* conn)(struct _ircsocket*, int);

	LIST_ENTRY(_ircsocket)	socket_list;
	LIST_HEAD(, ircTimer)	timers;
};

typedef struct _ircsocket IRC(Socket);

/**
 * An IRC Listener
 */
struct _irclistener
{
	int topFd;

	IRC(Socket) *sock;
	LIST_HEAD(, _ircsocket)	links;
}; 
typedef struct _irclistener IRC(Listener);


/**
 * An outbound(?) IRC Connection
 */
struct _irccon
{
};

struct IRC(_Message)
{
	int numarg;
	char *prefix;
	char *command;
	char *args[IRC_MAXARGS];
};
typedef struct IRC(_Message) IRC(Message);

int IRC(SockNonBlock) (int listenDesc);
IRC(Socket) *IRC(socket_make)();
int IRC(socket_bind)(IRC(Socket) *theSocket, int portNum, struct in_addr addr);
IRC(Listener) *IRC(MakeListener)(IRC(Socket) *theSocket);
void IRC(SocketAddEvents)(IRC(Socket) *theSocket);
void IRC(ListenerAddEvents)(IRC(Listener) *);

int IrcLibReadPackets(IRC(Socket) *ptrLink);
int IrcLibReadBinary(IRC(Socket) *ptrLink);
void IrcLibEventListener(int fd, short evType, void *p);
void IrcLibEventSocket(int fd, short evType, void *p);
void IrcLibEventFlushSockets(int fd, short evType, void *p);
char *IRC(BufShove)(IrcBuf *t, char *textIn, size_t textLen);
int IRC(BufDeQueue)(IrcBuf *t, char cmd[IRCBUFSIZE], int);
int IrcLibBufIsEmpty(IrcBuf *t);
void IrcBufMakeEmpty(IrcBuf *t);
char *IrcLibBufShoveBinary(IrcBuf *t, char* text, size_t len);
void IrcSetSockHandler(struct _ircsocket *q, int (* newFunc)(struct _ircsocket*, char *));


void LibIrcInit();
int IRC(SystemLoop)();
void IrcSend(IRC(Socket) *, const char *, ...);
int IrcLibDefaultSockHandler(IRC(Socket) *, char *);
int IrcLibDefaultListenHandler(IRC(Socket) *, char *);
int IrcLibDefaultClientHandler(IRC(Socket) *, char *);

void IRC(socket_connect)
	(IRC(Socket)*, int port, struct in_addr, int (*)(IRC(Socket) *, int));

int IRC(match)(const char *mask, const char *string);

void IRC(MakeMessage)(IRC(Message)*, char*);

extern time_t CTime;

#define IrcLibPop	IrcLib_pop
#define IrcListener	IRC(Listener)
#define IrcSocket	IRC(Socket)
#define IrcDnsQuery	IRC(dns_query)

#ifndef __IDSTRING

#define __IDSTRING(name, string) \
	static const char name[] = string

#endif

//	static const char name[] __attribute__((__unused__)) = (string)

#ifndef __RCSID
#	ifdef _DOXYGEN
#		define ID(x) __RCSID(x);
#	else
#		define ID(x) \
		__RCSID(x); \
		static void use_rcsid() { rcsid; }
#	endif
#	define __RCSID(s) __IDSTRING(rcsid, s)	
#	define __USEVAR(s) __IDSTRING(s, "(null)")
#endif

struct IRC(_modemap_st)
{
	int inuse, takes_arg;
};

struct IRC(_session)
{
	IRC(Socket) *sock;		/* Daemon socket */
	ircHashTable *chanHash;	/* Channels */
	struct IRC(_modemap_st) modemap[NUM_MODES + 1];
	LIST_HEAD(, ircTimer)	timers;
};
typedef struct IRC(_session) IRC(Ses);
typedef struct IRC(_session) IRC(Session);
typedef struct IRC(_modemap_st) IrcModeMap;
