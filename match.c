/*
 *  Matching interfaces
 *  Copyright C 2001 ***REMOVED***. All Rights Reserved.
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


/**
 *  \fn int match(const char *mask, const char *string)
 *  \param mask Pattern to match against
 *  \param string String to match
 *  \brief Matches a string against the specified pattern for wildcards \
 *   as allowed done by IRC clients/servers.
 *
 *  Determine if a given string (name) matches the given mask.
 *
 *  A wildcard match can be made, '*' matches a number of characters,
 *  and '?' matches a single character.
 *
 *  Wildcards can be escaped with a preceding \\ (backslash).
 *
 *  \return 0 if it matches, 1 if it does not match.
 *
 *  \bug match() comes from ircd, it is GPL'ed and services can't be \
 *      distributed while it remains.
 *
 */

/*
 * Written By Douglas A. Lewis <dalewis@cs.Buffalo.EDU>
 *
 * The match procedure is public domain code (from req.c of ircII)
 */

int
IRC(match)(const char *mask, const char *string)
{
        const char  *m = mask,
                *n = string,
                *ma = NULL,
                *na = NULL,
                *mp = NULL,
                *np = NULL;
        int     just = 0,
                pcount = 0,
                acount = 0,
                count = 0;

        for (;;)
        {
                if (*m == '*')
                {
                        ma = ++m;
                        na = n;
                        just = 1;
                        mp = NULL;
                        acount = count;
                }
#if 0 
                else if (*m == '%')
                {
                        mp = ++m;
                        np = n;
                        pcount = count;
                }
#endif
                else if (*m == '?')
                {
                        m++;
                        if (!*n++)
                                return 1;
                }
                else
                {
                        if (*m == '\\')
                        {
                                m++;
                                /* Quoting "nothing" is a bad thing */
                                if (!*m)
                                        return 1;
                        }
                        if (!*m)
                        {
                                /*
                                 * If we are out of both strings or we just
                                 * saw a wildcard, then we can say we have a
                                 * match
                                 */
                                if (!*n)
                                        return 0;
                                if (just)
                                        return 0;
                                just = 0;
                                goto not_matched;
                        }
                        /*
                         * We could check for *n == NULL at this point, but
                         * since it's more common to have a character there,
                         * check to see if they match first (m and n) and
                         * then if they don't match, THEN we can check for
                         * the NULL of n
                         */
                        just = 0;
                        if (tolower(*m) == tolower(*n))
                        {
                                m++;
                                if (*n == ' ')
                                        mp = NULL;
                                count++;
                                n++;
                        }
                        else
                        {

        not_matched:

                                /*
                                 * If there are no more characters in the
                                 * string, but we still need to find another
                                 * character (*m != NULL), then it will be
                                 * impossible to match it
                                 */
                                if (!*n)
                                        return 1;
                                if (mp)
                                {
                                        m = mp;
                                        if (*np == ' ')
                                        {
                                                mp = NULL;
                                                goto check_percent;
                                        }
                                        n = ++np;
                                        count = pcount;
                                }
                                else
        check_percent:

                                if (ma)
                                {
                                        m = ma;
                                        n = ++na;
                                        count = acount;
                                }
                                else
                                        return 1;
                        }
                }
        }
}
