#ifndef __dns_h__
#define __dns_h__
#include <adns.h>

void IRC(dns_init)();

struct _dnscallent;
struct _dnsquery;

typedef int DnsCallBack (struct _dnsquery*, char *result, void *data);

/**
 * An internal DNS callback entry
 */
struct _dnscallent {
	void *data;
	DnsCallBack *func;
	LIST_ENTRY(_dnscallent) call_lst;
};


/**
 * A DNS query that is pending
 */
struct _dnsquery
{
	adns_query query;
	char *ip;
	int rev;

	LIST_HEAD(, _dnscallent) calls;
	LIST_ENTRY(_dnsquery) query_lst;
};

typedef struct _dnsquery IRC(dns_query);
typedef struct _dnscallent IRC(dns_call);


int query_dns(int rev, char *, DnsCallBack*, void *);


#endif
