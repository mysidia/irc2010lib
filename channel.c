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
#include "irclib.h"
#include "channel.h"

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
