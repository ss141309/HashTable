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

#pragma once

#include "s8.h"
#include "utils.h"

#define HT_DEFAULT_CAPACITY 1024
#define MAX_LOAD_FACTOR 0.5

typedef struct {
  s8 key;
  s8 val;
  bool is_occupied;
  bool is_deleted;
} KvPair;

typedef struct {
  usize len;
  usize capacity;
  KvPair *kv_pairs;
} HashTable;

HashTable ht_create(usize ht_capacity);
void ht_destroy(HashTable *ht);
bool ht_insert(HashTable *ht, s8 key, s8 val);
bool ht_delete(HashTable *ht, const s8 key);
s8 ht_get(HashTable *ht, const s8 key);
