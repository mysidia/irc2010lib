/* $Id: hash.h,v 1.11 2002/10/12 01:23:59 mysidia Exp $ */

/*
 *  Hash Table Header
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
 * @file hash.h
 * @brief Hash Table header 
 */

#ifndef __hash_h__
#define __hash_h__

#include <stddef.h>

struct _irc_hash_entry;
struct _irc_hash_bucket;

/**
 * An entry in an [irc?] hash table
 */
struct _irc_hash_entry
{
	void *item;
	struct _irc_hash_entry *next;
};

/*! Entire hash bucket */
struct _irc_hash_bucket
{
	struct _irc_hash_entry *first;
};

/*! Entire hash table */
struct _irc_hash_table
{
	struct _irc_hash_bucket **table;
	const char* (* get_key)(void *item);
	int size;
};

typedef struct _irc_hash_table ircHashTable;
void *ircHashFind(ircHashTable*, char *);
void *ircHashAdd(ircHashTable*, void*);

ircHashTable *ircNewHashTable(int size, const char* (* getKeyPred)(void*));
ircHashTable *ircRehashTable(ircHashTable *, int newsize);
ircHashTable *ircEmptyHashTable(ircHashTable *);
ircHashTable *ircFreeHashTable(ircHashTable *);

int ircHashDel(ircHashTable*, void *);

#define ilHashTable	ircHashTable
#define ilHashDel	ircHashDel
#define ilHashFind	ircHashFind
#define ilHashAdd	ircHashAdd
#define ilNewHashTable	ircNewHashTable
#define	ilRehashTable	ircRehashTable
#define ilEmptyHashTable ircEmptyHashTable
#define ilFreeHashTable	ircFreeHashTable
#endif
