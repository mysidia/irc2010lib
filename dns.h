#ifndef __dns_h__
#define __dns_h__
#include <adns.h>

void IRC(dns_init)();

struct _dnscallent;
struct _dnsquery;

typedef int DnsCallBack (struct _dnsquery*, char *result, void *data);

struct _dnscallent {
	void *data;
	DnsCallBack *func;
	LIST_ENTRY(_dnscallent) call_lst;
};


struct _dnsquery
{
	adns_query query;
	char *ip;

	LIST_HEAD(, _dnscallent) calls;
	LIST_ENTRY(_dnsquery) query_lst;
};

typedef struct _dnsquery IRC(dns_query);
typedef struct _dnscallent IRC(dns_call);

#endif
