#include "irclib.h"
#include <assert.h>

int main()
{
	IrcSocket *p;
	IrcListener *q;
	struct in_addr addr;

	init_irc();

	p = LibIrc_socket_make();

	if (p == NULL)
		abort();

	addr.s_addr = INADDR_ANY;

	if (LibIrc_socket_bind(p, 3030, addr) < 0)
		abort();
	if ((q = LibIrc_socket_listen(p)) == NULL)
		abort();
	printf("Listening on %d\n", 3030);

	LibIrc_listen_addevents(q);
	
	start_irc();

//	sleep (6000);
}
