#include "irclib.h"
#include <assert.h>

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
	
	IRC(SystemLoop)();

//	sleep (6000);
}
