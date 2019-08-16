/*
 *  Sockets interface
 *  Copyright C 2001 Mysidia.  All Rights Reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  As a special exception, if other files instantiate generics from this
 *  unit, or you link this unit with other files to produce an executable,
 *  this unit does not by itself cause the resulting executable to be
 *  covered by the GNU General Public License. This exception does not
 *  however invalidate any other reasons why the executable file might be
 *  covered by the GNU General Public License.
 *
 */
/**
 * @file sock.c
 * @brief Socket Utilities
 */

#include "irclib.h"
#include <stddef.h>

ID("$Id: sock.c,v 1.35 2004/03/28 09:58:52 mysidia Exp $");

void IrcLibEventSocket(int fd, short evType, void *p);
void IrcLibEventListener(int fd, short evType, void *p);
void IrcLibEventSockWrite(int fd, short evType, void *p);

int IRCUnknown(IrcSocket* q, IrcMessage* t)
{
	if (t->command)
		IrcSend(q, "Unknown: [%s]\r\n", t->command);
}

IrcMsgTab IrcLibDefaultClientTable[] =
{
	{ NULL, IRCUnknown }
};

/********************************************************************/

/**
 * @brief Make a connection endpoint non-blocking
 * @param File descriptor of endpoint
 * @return 0 on success, -1 on failure
 */
int LibIrcSockNonBlock(int listenDesc)
{
	int optionValue;

#ifdef SYSV
	optionValue = 1;

	if (ioctl(listenDesc, FIONBIO, &optionValue) < 0)
		return -1;
#else
	if ((optionValue = fcntl(listenDesc, F_GETFL, 0)) == -1)
		return -1;
#ifdef O_NONBLOCK
	if (fcntl(listenDesc, F_SETFL, optionValue | O_NONBLOCK) == -1)
#else
	if (fcntl(listenDesc, F_SETFL, optionValue | O_NDELAY) == -1)
#endif
		return -1;
#endif
	return 0;
}

IrcSocket *Ircsocket_make()
{
	IrcSocket *sockLink;
	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (fd == -1)
		return NULL;
	sockLink = oalloc(sizeof(IrcSocket));

	sockLink->fd = fd;
	sockLink->func = IrcLibDefaultSockHandler;
	sockLink->readFunc = IrcLibReadPackets;

	sockLink->periodic = NULL;

#ifdef LIST_ENTRY_INIT
	LIST_ENTRY_INIT(sockLink, socket_list);
#endif

	return sockLink;
}

void IrcLibdelCon(IrcSocket *q)
{
	if ((q->flags & IRCSOCK_INBOUND) != 0)
		LIST_REMOVE(q, socket_list);
//	LibIrcSocketDelEvents(q);
}

void IrcLibFreeSocket(IrcSocket *q)
{
	struct event *ev = q->theEvent;

	if (ev) {
		event_del(ev);
		free(ev);
	}

	free(q);
}

void IrcLibAddCon(IrcListener *li, IrcSocket *q)
{
	LIST_INSERT_HEAD(&(li->links), q, socket_list);
	IrcSocketAddEvents(q);
}


int Ircsocket_bind(IrcSocket *theSocket, int portNum, struct in_addr addr)
{
	struct sockaddr_in sa;
	int reuseAddr;

	bzero(&sa, sizeof(sa));
	sa.sin_port = htons(portNum);
	sa.sin_addr = addr;
	sa.sin_family = AF_INET;

	reuseAddr = 1;
	setsockopt(theSocket->fd, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(int));

	LibIrcSockNonBlock(theSocket->fd);

	if (bind(theSocket->fd, (struct sockaddr *) &sa, sizeof(struct sockaddr_in)) < 0) {
		perror("bind");
		return -1;
	}

	theSocket->flags |= IRCSOCK_BOUND;

	return 0;
}

IrcListener *IrcMakeListener(IrcSocket *theSocket)
{
	IrcListener *port;

	if ( listen(theSocket->fd, 5) < 0 ) {
		perror("listen");
		return NULL;
	}

	port = oalloc(sizeof(IrcListener));

	port->sock = theSocket;
	port->sock->func = IrcLibDefaultListenHandler;
	port->sock->readFunc = IrcLibReadPackets;
	port->sock->periodic = NULL;
	port->sock->port = port;

	port->topFd = theSocket->fd;
	LIST_INIT(&port->links);
	port->sock->flags |= IRCSOCK_LISTEN;

	return port;
}

void IrcSocketAddEvents(IrcSocket *theSocket)
{
	struct event *p;
	int foundRead = 0, foundWrite = 0;

	if (theSocket->theEvent == NULL)
		p  = oalloc(sizeof(struct event));
	else {
		p = theSocket->theEvent;
		event_del(p);
	}

	if ((theSocket->flags & IRCSOCK_WRITE) == 0)
		event_set(p, theSocket->fd, EV_READ | EV_WRITE | EV_TIMEOUT, IrcLibEventSocket, theSocket);
	else
		event_set(p, theSocket->fd, EV_READ | EV_TIMEOUT, IrcLibEventSocket, theSocket);

	theSocket->tv.tv_usec = 10000;
	theSocket->tv.tv_sec = 0;

	event_add(p, &theSocket->tv);

	theSocket->theEvent = p;
}

void IrcListenerAddEvents(IrcListener *thePort)
{
	struct event *p; 

	if (thePort->sock->theEvent == NULL)
		p = oalloc(sizeof(struct event));
	else {
		p = thePort->sock->theEvent;
		event_del(p);
	}

	thePort->sock->tv.tv_sec = 1;
	thePort->sock->tv.tv_usec = 0;

	event_set(p, thePort->sock->fd, EV_READ | EV_TIMEOUT, IrcLibEventListener, thePort);
	event_add(p, &thePort->sock->tv);
	thePort->sock->theEvent = p;
}

/**
 * @brief Read packets from a connection and buffer them for
 *        processing
 * @param ptrLink Pointer to the connection endpoint
 * @return -1 if the connection is closing,
 *          0 on no data yet or data read
 */
int
IrcLibReadPackets(IrcSocket *ptrLink)
{
	char sockbuf[8192], *b;
	int k, kt = 0, msgl;

	if (ptrLink->tailBuf != NULL) {
		b = sockbuf;
		strcpy(b, ptrLink->tailBuf);

		free(ptrLink->tailBuf);
		ptrLink->tailBuf = NULL;

		if (*sockbuf != '\0')
			goto tailbuf;
	}

	k = read(ptrLink->fd, sockbuf, sizeof(sockbuf));

	while(k > 0)
	{
		kt += k;
		b = IrcBufShove(&ptrLink->inBuf, sockbuf, k, &msgl);

		tailbuf:

		if (b && *b) {
			int len;

			if (b != sockbuf)
				strcpy(sockbuf, b);
			len = strlen(b);

			if (len >= SOCKBUFSIZE || msgl >= 8000)
				return -1;

			k = read(ptrLink->fd, sockbuf + msgl, sizeof(sockbuf) - msgl);
			if (k > 0) {
				k += msgl;
				b = NULL;
			}
		}
		else {
			k = read(ptrLink->fd, sockbuf, sizeof(sockbuf));
		}
	}

	if (b && *b) {
		ptrLink->tailBuf = strdup(b);
	
		if (ptrLink->tailBuf == NULL)
			abort();
	}

	if (k == 0)
		return -1;
	if (k == -1) {
		if (errno == EINTR || errno == EWOULDBLOCK)
			return 0;
		return -1;
	}
	return 0;
}

/**
 * Read binary data
 */
int
IrcLibReadBinary(IrcSocket *ptrLink)
{
	char sockbuf[8192], *b;
	int k, kt = 0;
	size_t msgl;

	if (ptrLink->tailBuf != NULL) {
		b = sockbuf;
		strcpy(b, ptrLink->tailBuf);

		free(ptrLink->tailBuf);
		ptrLink->tailBuf = NULL;
	}

	k = read(ptrLink->fd, sockbuf, sizeof(sockbuf));

	while(k > 0)
	{
		kt += k;
		IrcBufShoveBinary(&ptrLink->inBuf, sockbuf, k, &msgl);
		k = read(ptrLink->fd, sockbuf, sizeof(sockbuf));
	}

	if (k == 0)
		return -1;
	if (k == -1) {
		if (errno == EINTR || errno == EWOULDBLOCK)
			return 0;
		return -1;
	}
	return 0;
}

/**
 * @brief Fired on listener event
 */
void
IrcLibEventListener(int fd, short evType, void *vI)
{
	IrcListener *li = (IrcListener *) vI;
	IrcSocket *p;
	struct sockaddr_in sai;
	int pFd, alen, ipOk;

	CTime = time(NULL);

	if (evType & EV_TIMEOUT)
		if (li->sock->periodic) {
			if ( (* li->sock->periodic)(li->sock) < 0) {
				return;
			}
		}

	if (evType & EV_READ)
	if ((pFd = accept(li->sock->fd, (struct sockaddr *)&sai, &alen)) != -1)
	{
		ipOk = 1;

		//if (sai.sin_addr.s_addr == 0x1000007f)
		//	ipOk = 1;

		if (LibIrcSockNonBlock(pFd) != -1 && ipOk) 
		{
			if (pFd > li->topFd)
				li->topFd = pFd;
			p = (IrcSocket *)oalloc(sizeof(IrcSocket));
			IrcLibAddCon(li, p);
			p->fd = pFd;
			p->addr = sai.sin_addr;
			p->port = li;
			p->flags = IRCSOCK_ESTAB | IRCSOCK_INBOUND;
			p->readFunc = IrcLibReadPackets;
			p->func = IrcLibDefaultClientHandler;
			p->parser = IrcLibDefaultClientTable;

			(* li->sock->func)(p, "");
//XXX debug
			printf("Incoming link from %x\n", sai.sin_addr.s_addr);
		}
		else
			close(pFd);
	}

	IrcListenerAddEvents(li);
}

void Ircsocket_connect
	(IrcSocket*sock, int portNum, struct in_addr addr,
	int (* finHandler)(IrcSocket* sock, int errcode))
{
	struct sockaddr_in sai;

	sai.sin_addr = addr;
	sai.sin_port = htons(portNum);
	sai.sin_family = AF_INET;
	sock->addr = addr;

	if ( connect(sock->fd, (struct sockaddr *) &sai, sizeof(struct sockaddr_in)) == 0)
	{
		sock->conn = finHandler;
		sock->flags = IRCSOCK_BOUND | IRCSOCK_ESTAB;

		(* finHandler)(sock, 0);
		return;
	}

	sock->conn = finHandler;
	sock->flags = IRCSOCK_BOUND | IRCSOCK_INCONN;

	if (errno != EWOULDBLOCK && errno != EINTR && errno != EINPROGRESS) {
		(* finHandler)(sock, errno);
		return;
	}

	IrcSocketAddEvents(sock);
}

/*int (* xxdontCallMe ( int ((* x)()) )) {
      return (int (*)(int ((* x)()))) xxdontCallMe; 
}*/


/**
 * @brief Send a message
 */
void
IrcSend(IrcSocket *s, const char *fmt, ...)
{
	static char buf[IRCBUFSIZE+3];
	va_list ap;
	size_t p;


	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf) - 3, fmt, ap);
	va_end(ap);

	strcat(buf, "\r\n");

	IrcBufShove(&s->outBuf, buf, strlen(buf), &p);
}

/**
 * @brief Fired on socket readable event
 */
void
IrcLibEventSocket(int fd, short evType, void *p)
{
	IrcSocket *q = (IrcSocket *)p;
	char buf[SOCKBUFSIZE];
	int l;

	CTime = time(NULL);

	if ((evType & EV_READ) != 0) 
	{
		if (q->flags & IRCSOCK_INCONN) {
			struct sockaddr_in sai;
			socklen_t nl;

			if (getpeername(q->fd, (struct sockaddr *)&sai, &nl) < 0)
			{
				if (q->conn)
					(* q->conn)(q, errno);
				close(q->fd);
				IrcLibdelCon(q);
				IrcFreeSocket(q);
				return;
			}
		}

		if (((* q->readFunc)(q)) < 0)
		{
	//XXX debug
			printf("Lost link from %x\n", q->addr.s_addr);
			close(q->fd);
			IrcLibdelCon(q);
			IrcFreeSocket(q);
			return;
		}

		while (IrcBufDeQueue(&q->inBuf, buf, q->readFunc == IrcLibReadPackets ? 0 : 2)) {
			if ( (* q->func)(q, buf) < 0 ) {
				close(q->fd);
				IrcLibdelCon(q);
				IrcFreeSocket(q);
				return;
			}
		}
	}


	if (evType & EV_WRITE) {
		if (q->flags & IRCSOCK_INCONN) {
			q->flags &= ~IRCSOCK_INCONN;
			q->flags |= IRCSOCK_ESTAB;

			if (q->conn) {
				(* q->conn)(q, 0);
			}
		}
		q->flags |= IRCSOCK_WRITE;
	}

	if ((evType & EV_TIMEOUT) || (evType & EV_READ)) 
	{
		if (q->periodic && ((CTime - q->lasttime) > 0)) {
			q->lasttime = CTime;

			if ( (* q->periodic)(q) < 0) {
				close(q->fd);
				IrcLibdelCon(q);
				IrcFreeSocket(q);
				return;
			}
		}

		while (IrcBufDeQueue(&q->outBuf, buf, 1)) {
			if ( send(q->fd, buf, strlen(buf), 0) < 0 ) {
				q->flags &= ~IRCSOCK_WRITE;

////				if (errno != EWOULDBLOCK)
////					IrcLib_putback(&q->outBuf, buf);
				break;
			}
		}
		
	}

	IrcSocketAddEvents(q);
}

/**
 * @brief Fired when a socket becomes writable
 */
void
IrcLibEventSockWrite(int fd, short evType, void *p)
{
	IrcSocket *q = (IrcSocket *)p;

	q->flags |= IRCSOCK_WRITE;
}

/////////////

struct _ircbq {
		char *text;	///< Text of the line
		struct _ircbq *next;	///< Pointer to the next line of text
		size_t len;	///< Line length
};

typedef struct _ircbq BufQel;

/////////////

/**
 * @internal
 * @brief Called by shove() to push a piece of the message
 *        before a \\n onto the buffer.
 */
static char *
qPush(IrcBuf *t, char *text, char *sep, size_t *rlen)
{
	BufQel *z;

	z = (BufQel *)oalloc(sizeof(BufQel) + (sep - text + 1));

	*sep = '\0';
	*rlen = z->len = sep - text;
	z->text = (char *)oalloc(z->len + 1);
	strcpy(z->text, text);

	if (t->firstEls == NULL)
		t->firstEls = t->lastEls = z;
	else
		{
			t->lastEls->next = z;
			t->lastEls = z;
		}

	*sep = '\n';

	return (sep + 1);
}

/**
 * @brief Fill the buffer from input data and return
 *        the leftovers
 *
 * @param textIn Text to buffer for processing
 * @param textLen Length of the text (in bytes)
 * @return An offset of textIn after which no text
 *         is buffered.
 * @pre   textIn points to a character array of
 *        size textLen.
 * @post  Each substring ending with a \\n inside
 *        #textIn is now in a buffer element, and
 *        buffer elements are in the order in which
 *        text was input.  If the array returned is
 *        empty then all text has been buffered,
 *        else that which has not been buffered is
 *        contained.
 */

char *
IrcBufShove(IrcBuf *t, char *textIn, size_t textLen, size_t* rlen)
{
	char *p;
	char *text = textIn;

	for(p = text; p < (textIn + textLen); p++) {
		if (*p == '\n' || (*p == '\r' && p[1] == '\n')) {
			text = qPush(t, text, p, rlen);

			while((p<(textIn+textLen)) && *p && (p[1]=='\r'||p[1]=='\n')) {
				p++;
				text++; 
			}
		}
	}

	if (text < (textIn + textLen))
		return text;
	return "";
}


char *
IrcBufShoveBinary(IrcBuf *t, char *textIn, size_t textLen)
{
	BufQel *z;

	z = (BufQel *)oalloc(sizeof(BufQel) + textLen);
	z->len = textLen;
	z->text = (char *)oalloc(textLen + 1);
	strcpy(z->text, textIn);

	if (t->firstEls == NULL)
		t->firstEls = t->lastEls = z;
	else
		{
			t->lastEls->next = z;
			t->lastEls = z;
		}
	return textIn;
}

/**
 * @brief Get buffered text and place it in cmd.
 * @param Buffer to place text in
 * @return 1 if a cmd has been loaded with a buffered item,
 *         0 if there is no text in the buffer.
 * @pre   Cmd is a memory area of type char [] with
 *        a size of at least #IPCBUFSIZE bytes.
 * @post  If an item is buffered for processing,
 *        then the text of the next item will
 *        fill cmd, and be removed from the buffer.
 */
int
IrcBufDeQueue(IrcBuf *t, char cmd[IRCBUFSIZE], int sendcr)
{
	BufQel *f;
	char *cp;

	if (t->firstEls == NULL)
		return 0;

	f = t->firstEls;

	if (t->firstEls == t->lastEls)
		t->firstEls = t->lastEls = NULL;
	else
		t->firstEls = t->firstEls->next;

	strncpy(cmd, f->text, IRCBUFSIZE);
	cmd[IRCBUFSIZE - 1] = '\0';

	if (sendcr != 2)
	{
		cp = cmd + strlen(cmd) - 1;
	
		if (*cp == '\n' || *cp == '\r')
			*cp = '\0';
	
		if ((cp - 1) > cmd && (cp[-1] == '\r' || cp[-1] == '\n')) {
			cp--;
			*cp = '\0';
		}

		if (sendcr) {
			if (*cp != '\0')
				cp++;
			while((cp - cmd) >= (IRCBUFSIZE - 3))
				cp--;
			*cp = '\r';
			*cp++ = '\n';
		}
	}

	free(f->text);
	free(f);

	return 1;
}

/**
 * @brief Is the buffer empty or no?
 * @return False if the buffer contains any text,
 *         not-false if the buffer contains text.
 */
int
IrcLibBufIsEmpty(IrcBuf *t)
{
	if (t->firstEls == NULL)
		return 1;
	return 0;
}

/**
 * @brief Empty the buffer
 * @post  The buffer contains no items.
 */
void
IrcBufMakeEmpty(IrcBuf *t)
{
	char cmd[1025];

	while(IrcBufDeQueue(t, cmd, 2))
		return;
}

void
IrcSetSockHandler(IrcSocket *q, int (* newFunc)(struct _ircsocket*, char *))
{
    q->func = newFunc;
}

int
IrcLibDefaultListenHandler(IrcSocket *q, char *buf)
{
	q->func = IrcLibDefaultClientHandler;
	q->parser = IrcLibDefaultClientTable;
}

int
IrcLibDefaultSockHandler(IrcSocket *q, char *buf)
{
}


int
IrcLibDefaultClientHandler(IrcSocket *q, char *buf)
{
	if (q->parser) {
		IrcMessage m;
		int i;

		IrcMakeMessage(&m, buf);

		if (m.command) {
			for(i = 0; q->parser[i].name; i++)
				if (strcasecmp(q->parser[i].name, m.command) == 0)
					break;
		}
		else
			for(i = 0; q->parser[i].name; i++)
				;

		if (q->parser[i].handler) {
			(* q->parser[i].handler)(q, &m);
		}

		// IrcSend(q, "[prefix = %s]\r\n", m.prefix);
		// IrcSend(q, "[command = %s]\r\n", m.command);

		// for(i = 0; i < m.numarg; i++)
		//	IrcSend(q, "[args.%i = %s]\r\n", i, m.args[i]);
	}
}


