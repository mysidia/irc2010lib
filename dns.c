/*
 *  DNS Interface
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
/**
 * @file dns.c
 * @brief Resolver Handler
 */

#include "irclib.h"
#include "dns.h"
ID("$Id: dns.c,v 1.8 2001/10/25 04:59:01 mysidia Exp $");

static adns_state dns_state;
static LIST_HEAD(, _dnsquery) queries;

static void IrcLibEventDNSProcess(int fd, short evType, void *pData);
static void check_dns();

void IRC(dns_init)()
{
	int errv;

	errv = adns_init(&dns_state, adns_if_noenv|adns_if_noautosys, 0);
	if (errv) {
		perror("adns_init");
		exit(0);
	}

	IrcLibEventDNSProcess(0, 0, &dns_state);
}

static void IrcLibEventDNSProcess(int fd, short evType, void *pData)
{
	static struct event ev;
	static struct timeval tv;

	adns_processany(*(adns_state *)pData);

	tv.tv_sec = 1;
	tv.tv_usec = 0;

	event_set(&ev, 0, EV_TIMEOUT, IrcLibEventDNSProcess, &dns_state);
	event_add(&ev, &tv);
	check_dns();
}

static char *ip2ptr(char *host)
{
   static char buf[HOSTLEN + 25];
   unsigned short int a, b, c, d;

    if ( sscanf(host, "%hu.%hu.%hu.%hu", &a, &b, &c, &d) < 4 ) {
        return strcpy(buf, host);
    }
    sprintf(buf, "%hu.%hu.%hu.%hu.in-addr.arpa.",
            d, c, b, a);
    return buf;
}

int query_dns(int reverse, char *host, DnsCallBack *func, void *data)
{
	IRC(dns_query) *query;
	IRC(dns_call) *newCall;
	int found = 0;

	if (!host || !*host)
		return -1;

	/* One query will take care of all descriptors from the machine. */
	for(query = queries.lh_first; query; query = query->query_lst.le_next)
		if (strcasecmp(query->ip, host) == 0)
			break;
	if (query)
		found = 1;

	do {
		if (found == 0)
			query = oalloc(sizeof(IRC(dns_query)));

		if (query) {
			newCall = oalloc(sizeof(IRC(dns_call)));
			newCall->func = func;
			newCall->data = data;
			LIST_INSERT_HEAD(&query->calls, newCall, call_lst);

			if (found != 0)
				return 0;
		}

		query->ip = oalloc(strlen(host)+1);
		query->rev = reverse;
		strcpy(query->ip, host);

		if (reverse ? adns_submit(dns_state, ip2ptr(host), adns_r_ptr,
			adns_qf_owner|adns_qf_cname_loose, query, &query->query) :

			adns_submit(dns_state, host, adns_r_addr, adns_qf_owner|adns_qf_cname_loose, query, &query->query)
		    )
		{
			perror("adns_submit");
			if (query->ip)
				free(query->ip);
			free(query);
			return -1;
 		 }

		LIST_INSERT_HEAD(&queries, query, query_lst);
	} while(0);

	check_dns();
	return 0;
}

static void check_dns()
{
	char ip[256];
	char host[256];
	adns_answer *ans;
	void        *ctx;
	int         v = 0;
	struct descriptor_data *d;

	IRC(dns_query) *req, *req_next;
	IRC(dns_call) *call, *call_next;
	int found = 0;

	for(req = queries.lh_first; req; req = req_next)
	{
		req_next = req->query_lst.le_next;
		ctx = NULL;
		ans = NULL;

		v = adns_check(dns_state, &req->query, &ans, &ctx);

		if (!ans || ans->type == adns_r_none)
			continue;

		LIST_REMOVE(req, query_lst);

		if (!ans->nrrs || ans->status == adns_s_nodata || ans->status == adns_s_nxdomain)
		{
			/* DNS Failed */
			for(call = req->calls.lh_first; call; call = call_next)
			{
				call_next = call->call_lst.le_next;

				if (req->rev)
					(* call->func)(req, host, NULL);
				else
					(* call->func)(req, ip, NULL);
				LIST_REMOVE(call, call_lst);
				free(call);
			}
		}
		else
		{

			/* ip -> host */
			if (req->rev) {
				if (ans)
					sprintf(host, "%.100s", ans->rrs.str[0]);
				else
					host[0] = '\0';
				sprintf(ip, "%.100s", req->ip);
			}
			else {
				sprintf(ip, "%.100s", inet_ntoa(ans->rrs.addr->addr.inet.sin_addr));
				sprintf(host, "%.100s", req->ip);
			}

	
			/* DNS Succeeded */
			for(call = req->calls.lh_first; call; call = call_next)
			{
				call_next = call->call_lst.le_next;
	
				if (req->rev)
					(* call->func)(req, host, ans);
				else
					(* call->func)(req, ip, ans);
	
				LIST_REMOVE(call, call_lst);
				free(call);
			}
			if (req->ip)
				free(req->ip);
			free(req);
		}
	}
}
