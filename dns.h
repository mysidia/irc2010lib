#ifndef __dns_h__
#define __dns_h__
#include <adns.h>

void IRC(dns_init)();

struct _dnsquery
{
	adns_query query;
	char *ip;

	LIST_ENTRY(_dnsquery) query_lst;
};

typedef struct _dnsquery IRC(dns_query);

#endif
