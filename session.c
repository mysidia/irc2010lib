/*
 *  DNS Interface
 *  Copyright C 2001 Mysidia.  All Rights Reserved.
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
 * @file session.c
 * @brief Session Management
 */

/**
 * Session management
 */
#include "irclib.h"
#include "channel.h"

ID("$Id: session.c,v 1.3 2004/03/28 09:58:52 mysidia Exp $");

/**
 * Create a session
 */
IrcSession* Irc_session_make()
{
	IrcSession* ses = (IrcSession *)oalloc(sizeof(IrcSession));

	LIST_INIT(&ses->timers);
	memset(ses->modemap, 0, sizeof(ses->modemap));
	ses->chanHash = ilNewHashTable(DEFCHANHASHSIZE, IrcChanVoidPtrHashableName);
	ses->sock = NULL;
}

void
Ircsess_setinfo(IrcSession* ses, const char* nick, const char* user, const char* host,
	const char* real)
{
	ses->userData.namp = strdup(nick);
	ses->userData.userp = strdup(user);
	ses->userData.hostp = strdup(host);
	ses->userData.realp = strdup(real);
}

int
Ircsess_register(IrcSession* ses)
{
	if (ses->sock->fd < 0)
		return -1;

	IrcSend(ses->sock, "NICK %s\r\n", ses->userData.namp);
	IrcSend(ses->sock, "USER %s %s %s :%s\r\n", ses->userData.userp, 
			ses->userData.hostp, 
			ses->userData.servp ? ses->userData.servp : "Unknown", 
			ses->userData.realp);
}
					
