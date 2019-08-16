/*
 *  Test server for binary protocols (in this case Socks)
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
 */
#include "irclib.h"
#include "dns.h"
#include <assert.h>
ID("$Id: sockmon.c,v 1.4 2004/08/30 04:20:58 mysidia Exp $");

IrcSocket *testCli;

int readSocks(IrcSocket *cl, char *buf)
{
	IrcListener *q;
	IrcSocket *p;
	static time_t lasttime = 0;
	int port;
	struct in_addr ia;


if (buf && cl && (buf[0] == 4 || buf[0] == 5)) {
	
	memcpy(&port, buf + 2, 2);
	memcpy(&ia.s_addr, buf+4, 4);
	port = ntohs(port);

//	ia.s_addr = (buf[4] << 24) | (buf[5] << 16) |
//                    (buf[6] <<  8) | (buf[7] << 0);

	printf("<<[%X %X] [%d] [%u.%u.%u.%u]>>\n", buf[0], buf[1],
                port,
             	(ia.s_addr & 0x000000FF), 
                (ia.s_addr & 0x0000FF00) >> 8,
                (ia.s_addr & 0x00FF0000) >> 16, 
                (ia.s_addr & 0xFF000000) >> 24);
}

//	q = cl->port;

//	for(p = q->links.lh_first; p; p = p->socket_list.le_next) {
//		if ((CTime - lasttime) > 3)
//			IrcSend(p, "Hi!\r\n");
//	}

	lasttime = time(NULL);
}

int incomingSocks(IrcSocket *cl, char *buf)
{
	cl->func = readSocks;
	cl->readFunc = IrcLibReadBinary;
}

int conDone( IrcSocket * sock, int errcode )
{
	if (errcode)
		printf("Connection %d\n", errcode);
	else
		printf("Connection with %X established\n", sock->addr.s_addr);
}

int main()
{
	IrcSocket *p;
	IrcListener *q;
	struct in_addr addr;

	LibIrcInit();
	p = Ircsocket_make();
	p->readFunc = IrcLibReadBinary;

	if (p == NULL)
		abort();

	addr.s_addr = INADDR_ANY;

	if (Ircsocket_bind(p, 1080, addr) < 0)
		abort();
	if ((q = IrcMakeListener(p)) == NULL)
		abort();
	printf("Listening on %d\n", 1080);

	IrcListenerAddEvents(q);

	q->sock->func = incomingSocks;

	addr.s_addr = inet_addr("127.0.0.1");

	IrcSystemLoop();

//	sleep (6000);
}

/*************************************************************************/
