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
typedef int timerCallBackSocket (IrcSocket*, void *pData);

*/


static LIST_HEAD(,ircTimer)	globalTimerList;


static void ircDefaultGetInterval(struct timeval* tv)
{
	if ( gettimeofday(tv, NULL) == -1 ) {
		perror("gettimeofday failed");
		abort();
	}
}

ircTimer* ircMakeTimer (timerCallBack* cb, struct timeval* tv) {
	ircTimer* t = oalloc(sizeof(ircTimer));

	t->count = irctCOUNT_TO;
	t->getCurrentInterval = ircDefaultGetInterval;
	t->func = cb;
	t->data = (void *)0;
	t->timeSpec = *tv;

	return t;
}

void ircFreeTimer (ircTimer* z) {
	free (z);
	return;
}


ircTimer* ircTimerScheduleSession(
        struct IrcLibSession* ses,
        struct timeval tv,
        timerCallBack *func
)
{
	ircTimer* t = ircMakeTimer(func,&tv);

	LIST_INSERT_HEAD(&(ses->timers), t, timer_lst);
	return t;
}

ircTimer* ircTimerScheduleSocket(struct _ircsocket* sock,
                        struct timeval tv, timerCallBack *func)
{
	ircTimer* t = ircMakeTimer(func,&tv);

	LIST_INSERT_HEAD(&(sock->timers), t, timer_lst);
	return t;
}

ircTimer* ircTimerScheduleGlobal(struct timeval tv, timerCallBack *func)
{
	ircTimer* t = ircMakeTimer(func,&tv);

	LIST_INSERT_HEAD(globalTimerList, t, timer_lst);
	return t;
}

static void irc_subtract_times(struct timeval *etime, struct timeval *stime)
{
  etime->tv_sec -= stime->tv_sec;
  etime->tv_usec -= stime->tv_usec;
  while ( etime->tv_usec < 0 ) {
    etime->tv_usec += 1000000;
    etime->tv_sec--;
  }
  return;
}

static int ircTimePassed(struct timeval* present, struct timeval* target)
{
	struct timeval tv = *present;

	irc_subtract_times(&tv, target);

	if (tv.tv_sec <= 0 && tv.tv_usec <= 0)
            return 1;

	return 0;
}

static int timerExpiring(ircTimer* t) {
	struct timeval timeInterval;

	switch(t->count)
	{
		default:
		case irctCOUNT_TO:

			(* t->getCurrentInterval)(&timeInterval);

			if (ircTimePassed(t->timeSpec, &timeInterval)) {
				return 1;
			}
			break;
	}
	return 0;
}

static int ircExpireSesTimer(ircTimer* t, IrcLibSession* ses)
{
	ircTimerHandle h;

	h.sock = NULL;
	h.sess = ses;
	(* t->func)(&h, t->data);

	return 1;
}

static int ircExpireSockTmer(ircTimer* t, struct _ircsocket* sock)
{
	ircTimerHandle h;

	h.sock = sock;
	h.sess = NULL;
	(* t->func)(&h, t->data);

	return 1;
}

static int ircExpireGlobalTimer(ircTimer* t)
{
	ircTimerHandle h;

	h.sock = NULL;
	h.sess = NULL;
	(* t->func)(&h, t->data);

	return 1;
}

void ircDoSessionTimers(struct IrcLibSession* ses)
{
	ircTimer *t, *t_next;

	for(t = LIST_FIRST(&ses->timers) ; t ; t = t_next)
	{
		t_next = LIST_NEXT(t, timer_lst);

		if (timerExpiring(t) && ircExpireSesTimer(t,ses)) {
			LIST_REMOVE(t, timer_lst);
			ircFreeTimer(t);
		}
	}
}

void ircDoSocketTimers(struct _ircsocket* sock)
{
	ircTimer *t, *t_next;

	for(t = LIST_FIRST(&(sock->timers)) ; t ; t_next)
	{
		t_next = LIST_NEXT(t, timer_lst);

		if (timerExpiring(t) && ircExpireSockTimer(t,sock)) {
			LIST_REMOVE(t, timer_lst);
			ircFreeTimer(t);
		}
	}
}

void ircDoTimers()
{
	ircTimer *t, *t_next;

	for(t = LIST_FIRST(&(sock->timers)) ; t ; t_next)
	{
		t_next = LIST_NEXT(t, timer_lst);

		if (timerExpiring(t) && ircExpireGlobalTimer(t)) {
			LIST_REMOVE(t, timer_lst);
			ircFreeTimer(t);
		}
	}
}

void ircUpdateSocketTimers(struct _ircsocket*, int n)
{
// Not done
}

void ircUpdateSessionTimers(struct IrcLibSession* ses, int n)
{
// Not done
}

void ircUpdateTimers(int n)
{
// Not done

}

void deleteGlobalTimers ()
{
	 // while (timer exists)  remove timer ...
	// while(LIST_FIRST(globalTimers))  { LIST_REMOVE; ircFreeTimer }
// Not done
}

