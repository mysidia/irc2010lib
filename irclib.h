/*
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

#if 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <time.h>

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

#include <values.h>

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

#ifdef HAVE_SYS_QUEUE_H
#include <sys/queue.h>
#elif defined(HAVE_QUEUE_H)
#include <queue.h>
#endif

#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <event.h>
#include <stdarg.h>

int match(const char*, const char*);
void *oalloc(size_t);

#define SOCKBUFSIZE 1024
#define IRCBUFSIZE 512
#define IRCBUFSZ   512

#define NICKLEN		30
#define HOSTLEN		63
#define USERLEN		8


#define IRCSOCK_WRITE	0x1

struct _ircbf
{
        struct _ircbq *firstEls, *lastEls;
};

typedef struct _ircbf IrcBuf;
typedef struct _ircbf IrcLibSocketBuf;


struct _ircsocket
{
#ifdef _IRCLIB
	int fd;
	int flags;
	struct event *theEvent;
	struct timeval tv;
	struct in_addr addr;

	IrcLibSocketBuf inBuf;
	IrcLibSocketBuf outBuf;
#else
	char __fd__[INTBITS];
	char __flags__[INTBITS];
	char __event__[sizeof(struct event *)];
	char __tv__[sizeof(struct timeval)];
	char __addr__[sizeof(struct in_addr)];
	char __inbuf__[sizeof(IrcLibSocketBuf)];
	char __outbuf__[sizeof(IrcLibSocketBuf)];
#endif

	time_t lasttime;
	struct _irclistener *port;
	int (* func)(struct _ircsocket*, char *);
	int (* periodic)(struct _ircsocket*);

	LIST_ENTRY(_ircsocket)	socket_list;
};

typedef struct _ircsocket IrcSocket;

struct _irclistener
{
	int topFd;

	IrcSocket *sock;
	LIST_HEAD(, _ircsocket)	links;
}; typedef struct _irclistener IrcListener;


struct _irccon
{
};

int LibIrcSockNonBlock(int listenDesc);
IrcSocket *LibIrc_socket_make();
int LibIrc_socket_bind(IrcSocket *theSocket, int portNum, struct in_addr addr);
IrcListener *LibIrcMakeListener(IrcSocket *theSocket);
void LibIrcSocketAddEvents(IrcSocket *theSocket);
void LibIrcListenerAddEvents(IrcListener *);

int IrcLibReadPackets(IrcSocket *ptrLink);
void IrcLibEventListener(int fd, short evType, void *p);
void IrcLibEventSocket(int fd, short evType, void *p);
void IrcLibEventFlushSockets(int fd, short evType, void *p);
char *IrcLibShove(IrcBuf *t, char *textIn, size_t textLen);
int IrcLib_pop(IrcBuf *t, char cmd[IRCBUFSIZE], int);
int IrcLibBufIsEmpty(IrcBuf *t);
void IrcBufMakeEmpty(IrcBuf *t);

void LibIrcInit();
int LibIrcSystemLoop();
void IrcSend(IrcSocket *, const char *, ...);
int IrcLibDefaultSockHandler(IrcSocket *, char *);
int IrcLibDefaultListenHandler(IrcSocket *, char *);
int IrcLibDefaultClientHandler(IrcSocket *, char *);
extern time_t CTime;

#define IrcLibPop IrcLib_pop
