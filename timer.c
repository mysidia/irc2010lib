/*
 *  Timed Event Implementation
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
 * @file timer.c
 * @brief Timed events
 *
 * An IRC client (or server) periodically needs to schedule events
 * to occur (ex, idle checks)
 *
 * Two types of timed events to exist [at least]:
 *       - Global timed event
 *       - Timed event to be iterated over each socket
 */

/*
typedef int timerCallBack (void *pData);
typedef int timerCallBackSocket (IRC(Socket)*, void *pData);

*/


ircTimer* ircMakeTimer (timerCallBack* cb, struct timeval* tv) {
	ircTimer* t = oalloc(sizeof(ircTimer));

	t->count = irctCOUNT_TO;
	t->getCurrentInterval = time;
	t->func = cb;

	return t;
}

void ircFreeTimer (ircTimer* z) {
	free (z);
	return;
}


ircTimer* ircTimerScheduleSession(
        struct IrcLibSession* ses,
        struct timeval tv,
        timerCallBackSocket *func
)
{
	ircTimer* t = ircMakeTimer(func,&tv);
	return t;
}

ircTimer* ircTimerScheduleSocket(struct _ircsocket*,
                        struct timeval tv, timerCallBack *func)
{
	ircTimer* t = ircMakeTimer(func,&tv);
	return t;
}

ircTimer* ircTimerScheduleGlobal(struct timeval tv, timerCallBackSocket *func)
{
	ircTimer* t = ircMakeTimer(func,&tv);
	return t;
}

void ircDoSessionTimers(struct IrcLibSession* ses)
{
}

void ircDoSocketTimers(struct _ircsocket*)
{
}

void ircDoTimers()
{
}

void ircUpdateSocketTimers(struct _ircsocket*, int n)
{
}

void ircUpdateSessionTimers(struct IrcLibSession* ses, int n)
{
}

void ircUpdateTimers(int n)
{
}




