#include "irclib.h"
#include "dns.h"
#include <assert.h>
ID("$Id: stest.c,v 1.8 2001/10/25 04:54:28 mysidia Exp $");

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


int main()
{
	IRC(Socket) *p;
	IRC(Listener) *q;
	struct in_addr addr;

	LibIrcInit();
	p = IRC(socket_make)();

	if (p == NULL)
		abort();

	addr.s_addr = INADDR_ANY;

	if (IRC(socket_bind)(p, 3030, addr) < 0)
		abort();
	if ((q = IRC(MakeListener)(p)) == NULL)
		abort();
	printf("Listening on %d\n", 3030);

	IRC(ListenerAddEvents)(q);

	q->sock->periodic = Test;

	query_dns(1, "1.2.3.4", funFin, NULL);	
	IRC(SystemLoop)();

//	sleep (6000);
}
