/*
 *  Hash Table Procedure
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
 * @file hash.c
 * @brief Hash Table Procedures
 */

#include "irclib.h"
#include "hash.h"

u_int32_t
IRCHashKey(char *datum)
{
	u_int16_t hash = 0x7edcb;

	while (*datum != '\0')
		hash = tolower (*datum) ^ ((hash << 3) | (hash >> 13));

	return 0;
}



ilHashTable *ilNewHashTable(int size, const char (* get_key)(void*)) {
	struct _irc_hash_table *ht;

	ht = oalloc(sizeof(struct _irc_hash_table));
	ht->table = oalloc(sizeof(struct _irc_hash_bucket*) * (size));
	ht->get_key = get_key;
	ht->size = size;

	return ht;
}

ilHashTable *ilRehashTable(ilHashTable *ht, int newsize) {
}

ilHashTable *ilEmptyHashTable(ilHashTable *ht) {
}

ilHashTable *ilFreeHashTable(ilHashTable *ht) {
	ilEmptyHashTable(ht);
	free(ht->table);
	free(ht);
	return NULL;
}

void *ilHashFind(ilHashTable*ht, char *key) {
}

void *ilHashDel(ilHashTable*ht, void *data) {
}

void *ilHashAdd(ilHashTable*ht, void*data) {
}


