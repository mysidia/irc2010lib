/*
 *  Timer header
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
 * @file timer.h
 * @brief Timers Header
 */

#define IEOL "\r\n"

struct _ircsocket;
struct IrcLibSession;

typedef int timerCallBack (void *pData);
typedef int timerCallBackSocket (struct _ircsocket*, void *pData );

 enum ircTimerCountType {
	irctCOUNT_TO,
	irctCOUNT_DOWN
 };


struct _ircTimer {
	timer_t			timeSpec; /* Time @ or to interval */
	timerCallBack 	*	func;
	enum ircTimerCountType	count;	/* Count from or to an interval */

	timer_t			(* getCurrentInterval)(time_t*);
	LIST_ENTRY(_ircTimer)	timer_lst;
};
typedef struct _ircTimer ircTimer;

ircTimer* ircTimerScheduleSession(
	struct IrcLibSession* ses,
	struct timeval tv,
	timerCallBackSocket *func
);

ircTimer* ircTimerScheduleSocket(struct _ircsocket*,
			struct timeval tv, timerCallBack *func);

ircTimer* ircTimerScheduleGlobal(struct timeval tv, timerCallBackSocket *func);

void ircDoSessionTimers(struct IrcLibSession* ses);
void ircDoSocketTimers(struct _ircsocket*);
void ircDoTimers();

/* Take into account the passage of n intervals */
void ircUpdateSocketTimers(struct _ircsocket*, int n);
void ircUpdateSessionTimers(struct IrcLibSession* ses, int n);
void ircUpdateTimers(int n);

ircTimer*	ircMakeTimer (timerCallBack*, struct timeval*);
void		ircFreeTimer (ircTimer*);

void		deleteGlobalTimers ();
