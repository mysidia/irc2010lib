/*
 *  Sample/Test Client
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
 */

#include "irclib.h"
#include "dns.h"
#include <assert.h>
ID("$Id: ctest.c,v 1.5 2004/03/28 09:58:52 mysidia Exp $");

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

int funFin (IrcDnsQuery *q, char *r, void *d, void* x)
{
	if (d)
		printf("DNS: %s -> %s\n", q->ip, r);
	else
		printf("DNS: %s -> ???\n", q->ip);
///
}


int conDone(IrcSocket* sock, int errcode )
{
	if (errcode)
		printf("Connection failed, code #%d\n", errcode);
	else
		printf("Connection with %X established\n", sock->addr.s_addr);
}

int main()
{
	IrcSocket *testCli;
	struct in_addr addr;

	LibIrcInit();
	testCli = Ircsocket_make();

	if (testCli == NULL)
		abort();

	addr.s_addr = INADDR_ANY;

	if (Ircsocket_bind(testCli, 0, addr) < 0)
		abort();
	query_dns(0, "localhost", funFin, NULL);

	addr.s_addr = inet_addr("127.0.0.1");
	Ircsocket_connect(testCli, 3030, addr, conDone);

	IrcSystemLoop();
}
