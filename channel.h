/*
 *  Channel Header
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
 * @file channel.h
 * @brief Channel Header
 */

#include "user.h"

#define IEOL "\r\n"
#define DEFCHANHASHSIZE 10

struct _irc_hash_table;

struct _chan_mode_par {
	char fl;
	char *value;
};
typedef struct _chan_mode_par IrcChannelModePar;

struct _chanmode
{
	IrcChannelModePar	*modeparams;
	long mode[2];

	char paramCount;
};
typedef struct _chanmode IrcChannelMode;


struct _channame
{
	char	*name;
};
typedef struct _channame IrcChannelName;

struct _irc_chanhandle
{
	LIST_HEAD(,_chanuser)	*users;
	IrcChannelName	*channelName;
	time_t			created;
	time_t			lastTime;
	IrcChannelMode		mode;
};
typedef struct _irc_chanhandle IrcChannel;

struct _chanuser
{
	int flags;
//	IrcNickName		nick;
};

void IrcAddChannel ( struct _irc_hash_table**, IrcChannel *cl );
IrcChannel *IrcFindChannel ( struct _irc_hash_table**, IrcChannelName *cn );
void IrcDelChannel ( struct _irc_hash_table**, IrcChannel *cl );
char * IrcChanCgetName (IrcChannelName *cn);
char * IrcChanGetName (IrcChannel* cn);
const char * IrcChanVoidPtrHashableName (void* cn);



