/*
 *  Sockets interface
 *  Copyright C 2001 ***REMOVED***.  All Rights Reserved.
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

#include "irclib.h"
#include <stddef.h>
#include <event.h>

void irclibEventSocket(int fd, short evType, void *);

/********************************************************************/

/**
 * @brief Make a connection endpoint non-blocking
 * @param File descriptor of endpoint
 * @return 0 on success, -1 on failure
 */
int doNonBlock(int listenDesc)
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

Socket *make_socket()
{
	Socket *sockLink;
	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (fd == -1)
		return NULL;
	sockLink = oalloc(sizeof(Socket));

	sockLink->fd = fd;
	return;
}

int socket_bind(Socket *theSocket, int portNum)
{
	struct sockaddr_in sa;
	int reuseAddr;

	bzero(&sa, sizeof(sa));
	sa.sin_port = htons(portNum);
	sa.sin_addr.s_addr = 0x100007f; /*INADDR_ANY;*/
	sa.sin_family = AF_INET;

	reuseAddr = 1;
	setsockopt(theSocket->fd, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(int));

	doNonBlock(theSocket->fd);

	if (bind(theSocket->fd, (struct sockaddr *) &sa, sizeof(struct sockaddr_in)) < 0) {
		perror("bind");
		return -1;
	}
	return 0;
}

Listener *socket_listen(Socket *theSocket)
{
	Listener *port;

	if ( listen(theSocket->fd, 5) < 0 ) {
		perror("listen");
		return NULL;
	}

	port = oalloc(sizeof(Listener));

	port->sock = theSocket;
	port->topFd = theSocket->fd;

	return port;
}

void socket_addevents(Socket *theSocket)
{
	struct event *p = oalloc(sizeof(struct event));

	event_set(p, theSocket->fd, EV_READ, irclibEventSocket, theSocket);
	event_add(p, NULL);
}

/**
 * @brief Read packets from a connection and buffer them for
 *        processing
 * @param ptrLink Pointer to the connection endpoint
 * @return -1 if the connection is closing,
 *          0 on no data yet or data read
 */
int
IrcLibReadPackets(Socket *ptrLink)
{
	char sockbuf[8192], *b;
	int k, kt = 0;

	k = read(ptrLink->fd, sockbuf, 8192);

	while(k > 0)
	{
		kt += k;
		b = IrcLibShove(ptrLink, sockbuf, k);

		if (b && *b) {
			int len;

			strcpy(sockbuf, b);
			len = strlen(b);

			if (len >= SOCKBUFSIZE)
				return -1;

			k = read(ptrLink->fd, sockbuf + len, 8192 - len);
			if (k > 0)
				k += len;
		}
		else
			k = read(ptrLink->fd, sockbuf, 8192);
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


void irclibEventListener(Listener *li)
{
	Socket *p;
	struct sockaddr_in sai;
	int pFd, alen, ipOk;

	if ((pFd = accept(li->sock->fd, (struct sockaddr *)&sai, &alen)) != -1)
	{
		ipOk = 1;

		//if (sai.sin_addr.s_addr == 0x1000007f)
		//	ipOk = 1;

		if (doNonBlock(pFd) != -1 && ipOk) 
		{
			if (pFd > li->topFd)
				li->topFd = pFd;
			p = (Socket *)oalloc(sizeof(Socket));
			IrcLibaddCon(li, p);
			p->fd = pFd;
			p->addr = sai.sin_addr;
		}
		else
			close(pFd);
	}
}


void irclibEventSocket(int fd, short evType, void *p)
{
	Socket *q = (Socket *)p;

	char buf[SOCKBUFSIZE];

	if (IrcLibReadPackets(q) < 0)
	{
		close(q->fd);
		delCon(q);
		freeCon(q);
		return;
	}

	while (IrcLibPop(q, buf)) {
	}
}

