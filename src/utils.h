/*
 * Copyright (C) 2018 Holger Kälberer <holger.k@elberer.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _UTILS_H_
#define _UTILS_H_

#include <cstdint>
#include <cstdio>
#include <string>

#define ERR(s, ...) fprintf(stderr, s "\n", ## __VA_ARGS__);
#define LOG(s, ...) fprintf(stderr, s "\n", ## __VA_ARGS__);
#ifdef NDEBUG
#   define DBG(s, ...);
#else
#   define DBG(s, ...) fprintf(stderr, s "\n", ## __VA_ARGS__);
#endif

extern const char PATH_SEPERATOR;

uintptr_t current_thread_id();
long get_num_cores();
extern std::string get_dir_name(const std::string& s);
extern std::string get_file_name(const std::string& s);
extern void hexdump(char *data, int len);

#endif
