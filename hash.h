/* $Id: hash.h,v 1.2 2001/12/16 09:53:25 mysidia Exp $ */

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

#include <stddef.h>

struct _hash_entry;
struct _hash_bucket;

struct _hash_entry
{
        void *item;
        struct _hash_bucket *next;
};

struct _hash_table
{
	struct _hash_entry **table;
	int size;
};

typedef struct _hash_table ilHashTable;


ilHashTable *ilNewHashTable(int size);
ilHashTable *ilRehashTable(ilHashTable *, int newsize);
ilHashTable *ilEmptyHashTable(ilHashTable *);
ilHashTable *ilFreeHashTable(ilHashTable *);

void *ilHashFind(ilHashTable*, char *);
void *ilHashDel(ilHashTable*, void *);
void *ilHashAdd(ilHashTable*, char *, void*);
