#ifndef __dns_h__
#define __dns_h__
#include <adns.h>

void IRC(dns_init)();

struct _dnscallEnt;
struct _dnsquery;

typedef int DnsCallBack (struct _dnsquery*, char *result, void *data);

struct _dnscallEnt{
	void *data;
	DnsCallBack *ptr;
	LIST_ENTRY(_dnscallEnt) call_lst;
};


struct _dnsquery
{
	adns_query query;
	char *ip;

	LIST_HEAD(, _dnscallEnt) calls;
	LIST_ENTRY(_dnsquery) query_lst;
};

typedef struct _dnsquery IRC(dns_query);

#endif
