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
IrcDefaultHashKey(const char *datum)
{
	u_int16_t hash = 0x7edcb;

	while (*datum != '\0')
		hash = tolower (*datum) ^ ((hash << 3) | (hash >> 13));

	return 0;
}



ilHashTable *ilNewHashTable(int size, const char* (* get_key)(void*)) {
	struct _irc_hash_table *ht;

	ht = oalloc(sizeof(struct _irc_hash_table));
	ht->table = oalloc(sizeof(struct _irc_hash_bucket*) * (size));
	ht->get_key = get_key;
	ht->size = size;
	ht->hash_name = IrcDefaultHashKey;

	return ht;
}

ilHashTable *ilRehashTable(ilHashTable *ht, int newsize) {
	struct _irc_hash_table nt;
	struct _irc_hash_entry *he;
	int i = 0, size = ht->size;

	if (newsize == 0) {
		fprintf(stderr, "Rehash to size 0 attempted\n");
		abort();
		return NULL;
	}

	nt.table = oalloc(sizeof(struct _irc_hash_bucket*) * (newsize));
	nt.size = newsize;
	nt.get_key = ht->get_key;

	for(i = 0; i < size; i++) {
		for(he = nt.table[i]->first; he; he = he->next) {
			ilHashAdd(&nt, he->item);
		}
	}

	ilEmptyHashTable(ht);
	free(ht->table);
	ht->size = nt.size;
	ht->table = nt.table;

	return ht;
}

ilHashTable *ilEmptyHashTable(ilHashTable *ht) {
	struct _irc_hash_entry* hx;
	int i = 0;

	if (ht->table)
		for(i = 0; i < ht->size; i++) {
			if (ht->table[i] != 0) {
				while(ht->table[i]->first) {
					hx = ht->table[i]->first;
					ht->table[i]->first = hx->next;
					free(hx);
				}

				free(ht->table[i]);
			}
		}
	
}

ilHashTable *ilFreeHashTable(ilHashTable *ht) {
	ilEmptyHashTable(ht);
	free(ht->table);
	free(ht);
	return NULL;
}

int irc_strcmp_hash(const char *left, const char *right) {
	char l, r;
	int i;

	for(i = 0; left[i] && right[i]; i++) {
		l = tolower(left[i]);
		r = tolower(right[i]);

		if (l == r)
			continue;
		if (l < r)
			return -1;
		return 1;
	}
	return left[i] ? 1 : right[i] ? -1 : 0;
}

void *ilHashFind(ilHashTable*ht, char *key) {
	int i = 0;
	struct _irc_hash_entry *he;

	i = ht->hash_name(key) % ht->size;

	if (ht->table[i] == 0 || ht->table[i]->first == 0)
			return 0;

	for(he = ht->table[i]->first; he; he = he->next) {
		const char *c = ht->get_key(he->item);

		if (c != 0 && irc_strcmp_hash(key, c) == 0)
			return he;
	}
	return 0;
}

int ilHashDel(ilHashTable*ht, void *data) {
	int i = 0, found = 0;
	const char *key;
	struct _irc_hash_entry *he, *htmp, *he_next;

	key = ht->get_key(he->item);
	i = ht->hash_name(key) % ht->size;

	if (ht->table[i] == 0 || ht->table[i]->first == 0)
			return 0;

	htmp = 0;

	for(he = ht->table[i]->first; he; he = he_next)
	{
		he_next = he->next;

		if (he->item == data)
		{
			if (htmp)
				htmp->next = he_next;
			else
				ht->table[i]->first = he_next;

			free(he);
			found++;
		}
		else
			htmp = he;
	}
	return found;
}

void *ilHashAdd(ilHashTable *ht, void *data)
{
	int i = 0;
	const char *key;
	struct _irc_hash_entry *he;

	key = ht->get_key(he->item);
	i = ht->hash_name(key) % ht->size;

	if (ht->table[i] == 0)
		ht->table[i] = oalloc(sizeof(struct _irc_hash_bucket));

	he = ht->table[i]->first;

	ht->table[i]->first = oalloc(sizeof(struct _irc_hash_entry));
	ht->table[i]->first->item = data;
	ht->table[i]->first->next = he;

	return ht->table[i]->first;
}
