#include "irclib.h"
#include "dns.h"
#include <assert.h>
ID("$Id: ctest.c,v 1.1 2001/10/31 01:23:19 mysidia Exp $");

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

int funFin ( IRC(dns_query)*q, char *r, void *d )
{
	if (d)
		printf("DNS: %s -> %s\n", q->ip, r);
	else
		printf("DNS: %s -> ???\n", q->ip);
///
}


int conDone( IRC(Socket) * sock, int errcode )
{
	if (errcode)
		printf("Connection failed, code #%d\n", errcode);
	else
		printf("Connection with %X established\n", sock->addr.s_addr);
}

int main()
{
	IRC(Socket) *testCli;
	struct in_addr addr;

	LibIrcInit();
	testCli = IRC(socket_make)();

	if (testCli == NULL)
		abort();

	addr.s_addr = INADDR_ANY;

	if (IRC(socket_bind)(testCli, 0, addr) < 0)
		abort();
	query_dns(0, "localhost", funFin, NULL);

	addr.s_addr = inet_addr("127.0.0.1");
	IRC(socket_connect)(testCli, 3030, addr, conDone);

	IRC(SystemLoop)();
}
