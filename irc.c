/*
 *  Primary Interface
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
 *  @file irc.c
 *  @brief IRC primary interface
 */

#include "irclib.h"
#include "dns.h"

ID("$Id: irc.c,v 1.12 2002/01/30 02:30:48 mysidia Exp $");
time_t CTime;

/**
 * Library initiation
 */
void LibIrcInit()
{
	event_init();		/* Init event system */
	IRC(dns_init)();	/* Init dns system */
}

/**
 * @brief The system event loop
 */
int IRC(SystemLoop)()
{
	CTime = time(NULL);

	if ( event_dispatch() < 0 )
		abort();
}

/**
 * @brief Split a buffer into an IRC message
 * \param mb Message buffer
 * mb->command = command to execute
 * mb->prefix = [optional] sender
 * mb->numarg = count of command arguments
 * mb->args = command arguments
 * \param buf Buffer to split along spaces, : after space or MAXPARA
 * parameters so far signals end of command/argument splitting
 */
void IRC(MakeMessage)(IRC(Message)*mb, char *buf)
{
	char *p, *m;
	int i, done = 0, prefix = 0;

	mb->command = mb->prefix = NULL;
	p = buf;

	/*
	 * No leading spaces
	 */
	while(isspace(*p))
		p++;
	m = p;

	for( i = 0; ; )
	{
		if (*p == '\0' || (isspace(*p) && !done)) {
			if (i == 0) {
				if (prefix) {
					mb->prefix = m;
					prefix = 0;
					i = -1;
				}
				else
					mb->command = m;
			}
			else
				mb->args[i - 1] = m;


			if (*p == '\0')
				break;
			m = p;

			/*
			 * No leading (or trailing) spaces
			 */
			while(*p && isspace(*p))
				p++;
			m[0] = '\0';

			m = p;
			m[-1] = '\0';

			/*
			 * Arg limit.
			 */
			if (i++ >= IRC_MAXARGS - 1)
				done = 1;
			continue;
		}

		if (*p == ':') {
			if (i > 0)
				done = 1;
			else
				prefix = 1;
			m = ++p;
		}

		while (*p && ( done || isspace(*p) == 0 ))
			p++;
	}

	mb->numarg = i;
} /**/
