/*
 *  Channel Routines
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
 * @file channel.c
 * @brief Channel Routines
 */

#include "irclib.h"
#include "channel.h"
#include "hash.h"

/**********************************************************************/

char * IRC(ChanCgetName) (IRC(ChannelName) *cn)
{
	return ( cn )->name;
}

const IRC(ChannelName) IRC(ChanMakeTempName) (const char *name)
{
	IRC(ChannelName) tmp;

	tmp.name = (char *)name;
	return tmp;
}

IRC(ChannelName)* IRC(ChanMakeName) (const char *name)
{
	IRC(ChannelName) *tmp;

	tmp = oalloc(sizeof(IRC(ChannelName)));
	( tmp )->name = str_dup(name);

	return tmp;
}

void IRC(ChanFreeName) (IRC(ChannelName) *p)
{
	if (( p )->name)
		free(p->name);
	free(p);
}

void IRC(ChanCsetName) (IRC(ChannelName) *cn, const char *str)
{
	if ( ( cn )->name )
		free (( cn )->name);

	if ( str )
		cn->name = str_dup(str);
	else
		cn->name = NULL;
}

void IRC(ChanJoin) (IRC(Ses)so, IRC(ChannelName) *chan_name)
{
	IrcSend(so.sock, "JOIN %s" IEOL, IRC(ChanCgetName)(chan_name));
}

void IRC(ChanPart) (IRC(Ses)so, IRC(ChannelName) *chan_name)
{
	IrcSend(so.sock, "PART %s" IEOL, IRC(ChanCgetName)(chan_name));
}

//void IRC(ChanAddUser) ( IRC(Channel)* cl, IRC(User)* us )
//{
//}

//void IRC(SendAddBan) (IRC(ChannelName) *chan_name, IRC(ChannelBan) *bNew)
//void IRC(SendDelBan) (IRC(ChannelName) *chan_name, IRC(ChannelBan) *bOld)

//void IRC(SendCUserModePlus) (IRC(ChannelName) *chan_name, IRC(ChannelUser) *opNew, IRC(ChannelMode) toAdd)
//void IRC(SendCUserModeLess) (IRC(ChannelName) *chan_name, IRC(ChannelUser) *opOld, IRC(ChannelMode) toDel)

//void IRC(SendKick) (IRC(ChannelName) *chan_name, IRC(ChannelUser) *target);
//void IRC(SendInvite) (IRC(ChannelName) *chan_name, IRC(NickName) *target);
//BanMask* IRC(GetBanMask) (IRC(UserHost) *a) 

/**********************************************************************/

#define BIT(x)	(1 << (x))

static struct { char fl; int mask1, mask2; }
IrcModeBitMap[] =
{
	{ 'a', BIT( 0), BIT( 0) },	{ 'b', BIT( 1), BIT( 0) },
	{ 'c', BIT( 2), BIT( 0) },	{ 'd', BIT( 3), BIT( 0) },
	{ 'e', BIT( 4), BIT( 0) },	{ 'f', BIT( 5), BIT( 0) },
	{ 'g', BIT( 6), BIT( 0) },	{ 'h', BIT( 7), BIT( 0) },
	{ 'i', BIT( 8), BIT( 0) },	{ 'j', BIT( 9), BIT( 0) },
	{ 'k', BIT(10), BIT( 0) },	{ 'l', BIT(11), BIT( 0) },
	{ 'm', BIT(12), BIT( 0) },	{ 'n', BIT(13), BIT( 0) },
	{ 'o', BIT(14), BIT( 0) },	{ 'p', BIT(15), BIT( 0) },
	{ 'q', BIT(16), BIT( 0) },	{ 'r', BIT(17), BIT( 0) },
	{ 's', BIT(18), BIT( 0) },	{ 't', BIT(19), BIT( 0) },
	{ 'u', BIT(20), BIT( 0) },	{ 'v', BIT(21), BIT( 0) },
	{ 'w', BIT(22), BIT( 0) },	{ 'x', BIT(23), BIT( 0) },
	{ 'y', BIT(24), BIT( 0) },	{ 'z', BIT(25), BIT( 0) },
	{ 'A', BIT(26), BIT( 0) },	{ 'B', BIT(26), BIT( 0) },
	{ 'C', BIT(27), BIT( 0) },	{ 'D', BIT(28), BIT( 0) },
	{ 'E', BIT(29), BIT( 0) },	{ 'F', BIT(30), BIT( 0) },
	{ 'G', BIT(31), BIT( 0) },	{ 'H', BIT(0),  BIT( 1) },
	{ 'I', BIT(0),  BIT( 2) },	{ 'J', BIT(0),  BIT( 3) },
	{ 'K', BIT(0),  BIT( 4) },	{ 'L', BIT(0),  BIT( 5) },
	{ 'M', BIT(0),  BIT( 6) },	{ 'N', BIT(0),  BIT( 7) },
	{ 'O', BIT(0),  BIT( 8) },	{ 'P', BIT(0),  BIT( 9) },
	{ 'Q', BIT(0),  BIT(10) },	{ 'R', BIT(0),  BIT(11) },
	{ 'S', BIT(0),  BIT(12) },	{ 'T', BIT(0),  BIT(13) },
	{ 'U', BIT(0),  BIT(14) },	{ 'V', BIT(0),  BIT(15) },
	{ 'W', BIT(0),  BIT(16) },	{ 'X', BIT(0),  BIT(17) },
	{ 'Y', BIT(0),  BIT(18) },	{ 'Z', BIT(0),  BIT(19) },
	{ '\0' }
};

IrcChannelMode IrcLibFlagToMode(char flag)
{
	IrcChannelMode mode;

	if (flag != '-' && flag != '+') 
	{
		int i = 0; 

		for(i = 0; IrcModeBitMap[i].fl; i++)
			if (IrcModeBitMap[i].fl == flag)
				break;
		if (IrcModeBitMap[i].fl == '\0') {
			mode.mode[0] = 0;
			mode.mode[1] = 0;
			return mode;
		}

		mode.mode[0] = mode.mode[0];
		mode.mode[1] = mode.mode[1];
	}
	else {
		mode.mode[0] = 0;
		mode.mode[1] = 0;
	}

	return mode;
}

void IrcLibModeSetMode(IrcChannelMode *q, const char *flagsToSet,
	const char *para[], int paraArgc,
	int (* paramHandler)(char mode, const char *))
{
	int what = 1, onarg = 0;
	IrcChannelMode v;

	while(flagsToSet && *flagsToSet) {
		switch(*flagsToSet)
		{
		case '-': what = 0; break;
		case '+': what = 1; break;

		case 'o':
			if (paraArgc-- > 0)
				paramHandler(*flagsToSet, para[onarg++]);
			break;
		default:
			v = IrcLibFlagToMode(*flagsToSet);
			if (what) {
				q->mode[0] |= v.mode[0];
				q->mode[1] |= v.mode[1];
			}
			else {
				q->mode[0] &= ~v.mode[0];
				q->mode[1] &= ~v.mode[1];
			}
		}

		flagsToSet++;
	}
}

const char *IrcModeString(IrcChannelMode *v)
{
	static char resBuf[512];
	char *p = resBuf;
	int i = 0;

	if (v->mode[0] == 0 && v->mode[1] == 0)
		return strcpy(resBuf, "+");
	*resBuf = '\0';

	for(i = 0; IrcModeBitMap[i].fl; i++)
	{
		if (v->mode[0] & IrcModeBitMap[i].mask1)
			*p++ = IrcModeBitMap[i].fl;
		if (v->mode[1] & IrcModeBitMap[i].mask2)
			*p++ = IrcModeBitMap[i].fl;
	}
	return resBuf;
}

/********************************************************************/

const char *IrcChanHkeyPred(void *item) {
	return ((IrcChannel *)item)->channelName->name;
}

void IrcAddChannel ( ircHashTable **chanHash, IrcChannel *cl )
{
	if (*chanHash == 0) {
		*chanHash = ircNewHashTable(256, IrcChanHkeyPred);
		if (*chanHash == 0) {
			fprintf(stderr, "FATAL: Resources exhausted\n");
			exit(11);
		}
	}
	ircHashAdd(*chanHash, cl);
}

IrcChannel *IrcFindChannel ( ircHashTable **chanHash, IRC(ChannelName) *cn )
{
	if (*chanHash == 0)
		return 0;
	return ircHashFind(*chanHash, cn->name);
}

void IrcDelChannel ( ircHashTable **chanHash, IrcChannel *cl )
{
	int z = ircHashDel(*chanHash, cl);

	if ( z == 0 )
		abort();
}

void IRC(SendModeChange) (
			  IRC(Ses) so, 
			  IRC(ChannelName) *chan_name,
			  IRC(ChannelMode) orig,
                          IRC(ChannelMode) newmode
)
{
	char buf[512], tmp[2];
	int i, a, b, pm=0;

	buf[0] = '\0';
	tmp[1] = '\0';

	// Calculate difference and send change

	for(i = 0; i < (sizeof IrcModeBitMap) / (sizeof *IrcModeBitMap); i++)
	{
		if (IrcModeBitMap[i].mask1) {
			a = (orig.mode[0] & IrcModeBitMap[i].mask1) ? 1 : 0;
			b = (newmode.mode[0] & IrcModeBitMap[i].mask1) ? 1 : 0;
		}
		else {
			a = (orig.mode[1] & IrcModeBitMap[i].mask2) ? 1 : 0;
			b = (newmode.mode[1] & IrcModeBitMap[i].mask2) ? 1 : 0;
		}

		
		tmp[1] = IrcModeBitMap[i].fl;

		if (a && !b) {
			if (pm >= 0) {
				strcat(buf, "-");
				pm = -1;
			}
			strcat(buf, tmp);
		}

		if (!a && b) {
			if (pm <= 0) {
				strcat(buf, "+");
				pm = 1;
			}
			strcat(buf, tmp);
		}
	}

	// IrcModeBitMap[].mask1  mask2   fl(letter)
	IrcSend("MODE %s %s" IEOL, IRC(ChanCgetName)(chan_name),
		buf);
}

/********************************************************************/

