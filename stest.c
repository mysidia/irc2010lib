/*
 *  Sample/Test Server
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
 */
#include "irclib.h"
#include "dns.h"
#include <assert.h>
ID("$Id: stest.c,v 1.17 2004/03/28 09:58:52 mysidia Exp $");

IrcSocket *testCli;

int Test(IrcSocket *cl)
{
	IrcListener *q;
	IrcSocket *p;
	static time_t lasttime = 0;

	q = cl->port;

	for(p = q->links.lh_first; p; p = p->socket_list.le_next) {
		if ((CTime - lasttime) > 3)
			IrcSend(p, "Hi!\r\n");
	}

	lasttime = time(NULL);
}

int funFin ( IrcDnsQuery *q, char *r, void *d, void* x)
{
	if (d)
		printf("DNS: %s -> %s\n", q->ip, r);
	else
		printf("DNS: %s -> ???\n", q->ip);
///
}


int conDone( IrcSocket *sock, int errcode )
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

	if (p == NULL)
		abort();

	addr.s_addr = INADDR_ANY;

	if (Ircsocket_bind(p, 3030, addr) < 0)
		abort();
	if ((q = IrcMakeListener(p)) == NULL)
		abort();
	printf("Listening on %d\n", 3030);

	IrcListenerAddEvents(q);

	q->sock->periodic = Test; /* periodically Test(...) */

	addr.s_addr = inet_addr("127.0.0.1");

	query_dns(0, "rs.internic.net", funFin, NULL); /* test DNS */

	IrcSystemLoop();

//	sleep (6000);
}
