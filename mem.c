/*
 *  Memory management
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

/*! 
 * \fn void * oalloc (size_t size)
 * \brief Clean and allocate a memory area.
 * \param size How many bytes of memory are to be allocated
 *
 * Allocate n bytes of memory, check that is was properly allocated
 * and clean it.  This function is like calloc(), but will shut
 * down services cleanly if it fails to allocate
 */
void *
oalloc(size_t iMemLen)
{
	void *vBuf;

	if ((vBuf = calloc(iMemLen, 1)) == (void *)0)
		abort();
	return vBuf;
}

void IrcFreeSocket(IrcSocket *q)
{
	if (q->theEvent) {
		event_del(q->theEvent);
		free(q->theEvent);
	}

	free(q);
}

