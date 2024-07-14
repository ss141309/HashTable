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

#include <stdbool.h>

#include "s8.h"
#include "utils.h"

bool s8cmp(const s8 str1, const s8 str2) {
  if (str1.len != str2.len) {
    return false;
  }

  for (size i = 0; i < str1.len; i++) {
    if (str1.data[i] != str2.data[i]) {
      return false;
    }
  }

  return true;
}
