/*
 *  Sample/Test Client
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
 */

#include "irclib.h"
#include "dns.h"
#include <assert.h>
ID("$Id: irctest.c,v 1.1 2004/03/28 09:58:52 mysidia Exp $");

struct
Dns_Pass1 {
	IrcSession* ses;
	int port_no;
};

int myUnknown(IrcSocket* q, IrcMessage* t)
{
	char buf[IRCBUFSZ];

	if (t->command) {
		IrcMessageText(t, buf, sizeof(buf), 0, -1);
			
		printf("[%s %s %s]\n", t->prefix, t->command, buf);
	}
}


IrcMsgTab myTable[] =
{
	        { NULL, myUnknown }
};



int conDone(IrcSocket* sock, int errcode );

int
printSockHandler(IrcSocket *q, char *buf)
{
	        printf("[%s]\n",buf);
}


int Test(IrcSocket *cl)
{
	IrcListener *q;
	IrcSocket *p;
	static time_t lasttime = 0;

	q = cl->port;

	for(p = q->links.lh_first; p; p = p->socket_list.le_next) {
		if ((CTime - lasttime) > 3)
			; //IrcSend(p, "Hi!\r\n");
	}

	lasttime = time(NULL);
}

int dnsFin (IrcDnsQuery *q, char *r, void *answer, void* data )
{
	struct Dns_Pass1 *pass = (struct Dns_Pass1 *) data;
	IrcSession* ses;
	struct in_addr addr;
	
	if (answer) {
		ses = pass->ses;

		printf("DNS: %s port %d -> %s\n", q->ip, pass->port_no, r);
		addr.s_addr = INADDR_ANY;
	        addr.s_addr = inet_addr(r);
		if (Ircsocket_bind(pass->ses->sock, 0, addr) < 0)
		{
			fprintf(stderr, "Error: unable to bind port\n");
			exit(1);
		}
		pass->ses->sock->func = IrcLibDefaultClientHandler;
		pass->ses->sock->parser = myTable;
	        Ircsocket_connect(pass->ses->sock, pass->port_no, addr, conDone);
		Ircsess_setinfo(ses, "testnick", "testuser", "testhost.com", "real name");
		Ircsess_register(ses);
	}
	else {
		printf("Error: unable to resolve %s\n", q->ip);
		exit(1);
	}
///
}


int conDone(IrcSocket* sock, int errcode )
{
	if (errcode)
		printf("Connection failed, code #%d\n", errcode);
	else
		printf("Connection with %X established\n", sock->addr.s_addr);
}

int main(int argc, char** argv)
{
	IrcSession *ses;
	struct in_addr addr;
	int portNo = 6667;
	char *host_name;
	static struct Dns_Pass1 dp1;

	LibIrcInit();
	ses = Irc_session_make();
	if (ses == NULL)
		abort();
	ses->sock = Ircsocket_make();

	if (argc < 2) {
		printf("Syntax: irctest <host> [<port>]\n");
		return;
	}

	if (argc >= 3)
		portNo = atoi(argv[2]);
	host_name = argv[1];

	printf("Resolving %s...\n", host_name);
	dp1.port_no = portNo;
	dp1.ses = ses;
	query_dns(0, host_name, dnsFin, &dp1);
	IrcSystemLoop();
}
