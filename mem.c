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
/**
 * @file mem.c
 * @brief Memory Management/Utilities
 */

#include "irclib.h"
ID("$Id: mem.c,v 1.6 2001/10/27 05:05:46 mysidia Exp $");

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



/**
 * \fn char *str_dup(const char *input)
 * \param input String to duplicate
 * \brief Allocates and returns a copy of the supplied string.
 */
char *
str_dup(const char *input)
{
	char *buf;
	int len;

	for (buf = ((char *)(input)); *buf; buf++);
	buf = (char *)((oalloc(1 + (len = (buf - input)))));
	return (char *)(memcpy(buf, input, len + 1));
}

/*!
 * \fn char *strn_dup(const char *input, int max)
 * \param input String to duplicate
 * \param max Maximum length of new string
 * \brief Allocates and returns a copy of part of the supplied string.
 */
char *
strn_dup(const char *input, int max)
{
	char *buf;
	int len;

	for (buf = ((char *)(input));
		 *buf && ((buf - input) < max); buf++);
	len = buf - input;
	buf = (char *)(oalloc(1 + len));
	buf[len] = '\0';
	return (char *)memcpy(buf, input, len);
}

/**
 * \pre  From points to a valid NUL-terminated character array, and
 *       add is a reference to a character array to be changed.
 *
 * \post *Buf area is reallocated to contain its present state plus
 *       the string specified as 'add'
 */
void AppendBuffer(char **buf, const char *add)
{
     char *newbuf, *x;

     /* x = newbuf = new char[ ((buf && *buf ? strlen(*buf) : 0)) + strlen(add) + 4]; */

     x = newbuf = (char *)oalloc(((*buf ? strlen(*buf) : 0)) + strlen(add) + 4);
     if (!newbuf) return;
     if (*buf)
     {
       bzero( newbuf, (*buf ? strlen( *buf ):0) + strlen(add) + 2 );
       memcpy( newbuf, *buf, strlen(*buf) );
       x = newbuf + strlen(*buf);
     } else x = newbuf;
     strcpy(x, add); /* this is ok */
     if (*buf) {
         free(*buf);
     }
     *buf = newbuf;
     return;
}


/**
 * \pre  From points to a valid NUL-terminated character array, and
 *       add is a reference to a character array to be changed.
 *
 * \post *Buf area is reallocated to contain the string specified as
 *       'new'
 */
void SetDynBuffer(char **buf, const char *newStr)
{
	char *newbuf, *x;

	if (newStr) {
		x = newbuf = (char *)oalloc(strlen(newStr) + 1);
		if (!newbuf)
			return;
		strcpy(x, newStr);
	}
	else
		newbuf = NULL;

	if (*buf)
		free(*buf);
	*buf = newbuf;
	return;
}
