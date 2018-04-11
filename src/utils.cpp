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

#include <cstdio>
#include <pthread.h>
#include <unistd.h>

#ifdef _WIN32
#   include <windows.h>
#endif

#include "utils.h"

#ifdef _WIN32
const char PATH_SEPERATOR = '\\';
#else
const char PATH_SEPERATOR = '/';
#endif

uintptr_t current_thread_id()
{
#ifdef _WIN32
    return reinterpret_cast<uintptr_t>(pthread_self().p);
#else
    return pthread_self();
#endif
}

long get_num_cores()
{
#ifdef _WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
#else
    return sysconf(_SC_NPROCESSORS_ONLN);
    // FIXME: use std::thread::hardware_concurrency()?
    // Seems to be not available everywhere!?
#endif
}

std::string get_dir_name(const std::string& s)
{
   size_t i = s.rfind(PATH_SEPERATOR, s.length());
   if (i != std::string::npos)
      return s.substr(0, i);

   return std::string();
}

std::string get_file_name(const std::string& s)
{
   size_t i = s.rfind(PATH_SEPERATOR, s.length());
   if (i != std::string::npos)
      return s.substr(i+1, s.length());

   return std::string();
}

int
hexdump_line (char *buffer, char *data, int len, int offset)
{
	int i = 0;
	int j = 0;
	int size = 0;
	int wlen = 0;
	unsigned char *start = (unsigned char *) data;
	if ((!buffer) || (!data) || (len <= 0))
		return -1;

	if (offset >= 0)
		buffer += sprintf (buffer, " 0x%4.4x|", offset);

	if (len > 16)
		len = 16;

	/* Dump hex */
	for (i = 0; i < len; i += 4) {
		wlen = i + 4;
		if (wlen > len)
			wlen = len;

		*buffer++ = ' ';
		for (j = i; j < wlen; j++) {
			buffer += sprintf(buffer, "%2.2X", *start++);
			size++;
		}

		for ( ; j < (i + 4) ; j++)
			buffer += sprintf(buffer, "..");
	}

	for ( ; i < 16 ; i += 4)
		buffer += sprintf(buffer, " ........");

	/* Dump ASCII */

	buffer += sprintf(buffer, "| |");
	for (i = 0; i < len; i++, data++) {
		if (*data >= ' ' && *data < 0x7f)
			*buffer++ = *data;
		else
			*buffer++ = '.';
	}

	for ( ; i < 16 ; i++)
		*buffer++ = '|';

	*buffer++ = '|';
	*buffer = '\0';  /* Terminate the string. */

	return size;
}

void hexdump(char *data, int len)
{
	int done;
	int offset = 0;
	char buffer[64000];

	while (len > 0) {
		done = hexdump_line(buffer, data, len, offset);
		len -= done;
		offset += done;
		data += done;
		DBG("%s", buffer);
	}

}
