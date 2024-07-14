/*
 * Copyright (C) 2024 समीर सिंह Sameer Singh
 * This file is part of Bitcask.
 *
 * Bitcask is free software: you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Bitcask is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with Bitcask. If not, see
 * <https://www.gnu.org/licenses/>.
 */

#include <stdint.h>

#include "ht.h"
#include "rapidhash.h"
#include "s8.h"
#include "utils.h"

static void insert(HashTable *ht, const KvPair kv_pair);
static inline usize hash(const usize ht_capacity, const s8 key);
static inline f64 getCurrLoadFactor(const HashTable ht);
static inline bool rehashAndGrow(HashTable *ht);
static inline void copyKeyToHt(HashTable *ht, KvPair kv_pair, usize index, int inc_len);

HashTable ht_create(usize ht_capacity) {
  abort_if(ht_capacity == 0 || (ht_capacity & (ht_capacity - 1)) != 0, ERR_INVALID_SIZE);

  HashTable ht = {0};
  ht.len = 0;
  ht.capacity = ht_capacity;

  ht.kv_pairs = calloc(ht_capacity, sizeof(KvPair));
  abort_if(ht.kv_pairs == NULL, ERR_OUT_OF_MEMORY);

  return ht;
}

void ht_destroy(HashTable *ht) {
  if (ht->kv_pairs) free(ht->kv_pairs);
}

bool ht_insert(HashTable *ht, s8 key, s8 val) {
  return_value_if(!rehashAndGrow(ht), false, "Rehash failed.\n");

  KvPair kv_pair = {
    .key = key,
    .val = val,
    .is_occupied = false,
    .is_deleted = false
  };

  insert(ht, kv_pair);
  return true;
}

static void insert(HashTable *ht, const KvPair kv_pair) {
  usize index = hash(ht->capacity, kv_pair.key);

  if (!ht->kv_pairs[index].is_occupied) {
    copyKeyToHt(ht, kv_pair, index, 1);
    return;
  }

  while (ht->kv_pairs[index].is_occupied) {
    if (s8cmp(kv_pair.key, ht->kv_pairs[index].key)) {
      copyKeyToHt(ht, kv_pair, index, 0);
      return;
    }
    index = (index + 1) & (ht->capacity - 1);
  }

  copyKeyToHt(ht, kv_pair, index, 1);
}

s8 ht_get(HashTable *ht, const s8 key) {
  usize index = hash(ht->capacity, key);
  s8 val = {.data = NULL, .len = -1};

  while (ht->kv_pairs[index].is_occupied) {
    if (s8cmp(key, ht->kv_pairs[index].key) &&
        !ht->kv_pairs[index].is_deleted) {
      return ht->kv_pairs[index].val;
    }

    index = (index + 1) & (ht->capacity - 1);
  }

  return val;
}

bool ht_delete(HashTable *ht, const s8 key) {
  usize index = hash(ht->capacity, key);
  while (ht->kv_pairs[index].is_occupied) {
    if (s8cmp(key, ht->kv_pairs[index].key)) {
      ht->kv_pairs[index].is_deleted = true;
      ht->len -= 1;
      return true;
    }

    index = (index + 1) & (ht->capacity - 1);
  }

  return false;
}

static inline usize hash(const usize ht_capacity, const s8 key) {
  // hash % ht_capacity
  return rapidhash(key.data, key.len) & (ht_capacity - 1);
}

static inline f64 getCurrLoadFactor(const HashTable ht) {
  return (f64) ht.len / ht.capacity;
}

static inline void copyKeyToHt(HashTable *ht, const KvPair kv_pair,
                               const usize index, const int inc_len) {
  memcpy(ht->kv_pairs + index, &kv_pair, sizeof(KvPair));
  ht->len += inc_len;
  ht->kv_pairs[index].is_occupied = true;
}

static inline bool rehashAndGrow(HashTable *ht) {
  if (getCurrLoadFactor(*ht) < MAX_LOAD_FACTOR)
    return true;

  return_value_if(ht->capacity > SIZE_MAX / 2, false, ERR_ARITHEMATIC_OVERFLOW);
  const usize new_ht_capacity = ht->capacity * 2;

  HashTable new_ht = ht_create(new_ht_capacity);
  return_value_if(new_ht.kv_pairs == NULL, false, ERR_OUT_OF_MEMORY);

  for (usize i = 0; i < ht->capacity; i++) {
    if (ht->kv_pairs[i].is_occupied && !ht->kv_pairs[i].is_deleted) {
      insert(&new_ht, ht->kv_pairs[i]);
    }
  }

  free(ht->kv_pairs);
  ht->len = new_ht.len;
  ht->capacity = new_ht.capacity;
  ht->kv_pairs = new_ht.kv_pairs;
  return true;
}
