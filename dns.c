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
#include "irclib.h"
#include "dns.h"

static adns_state dns_state;
static LIST_HEAD(, _dnsquery) queries;

void IRC(dns_init)()
{
	int errv;

	errv = adns_init(&dns_state, adns_if_noenv|adns_if_noautosys, 0);
	if (errv) {
		perror("adns_init");
		exit(0);
	}
}

char *ip2ptr(char *host)
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

int query_dns(char *host, DnsCallBack *func)
{
	IRC(dns_query) *query;

	if (!host || !*host)
		return -1;

	/* One query will take care of all descriptors from the machine. */
	for(query = queries.lh_first; query; query = query->query_lst.le_next)
		if (strcasecmp(query->ip, host) == 0)
			break;

//XXX callback should be a list, if query exists, callback should be added
//  to the query.
	if (query)
		return 0;

	do {
		query = oalloc(sizeof(IRC(dns_query)));

		query->ip = oalloc(strlen(host)+1);
		strcpy(query->ip, host);

		if (adns_submit(dns_state, ip2ptr(host), adns_r_ptr,
			adns_qf_owner|adns_qf_cname_loose, query, &query->query))
		{
			perror("adns_submit");
			if (query->ip)
				free(query->ip);
			free(query);
			return -1;
 		 }

		LIST_INSERT_HEAD(&queries, query, query_lst);
	} while(0);

//	check_dns();
	return 0;
}
