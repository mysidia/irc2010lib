/*
 * IRC Resolver header
 * Copyright (c) 2001 ***REMOVED***
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the authors nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

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
