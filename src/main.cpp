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

#include <pthread.h>
#include <cinttypes>
#include <string.h>

#include "utils.h"
#include "EncoderThread.h"
#include "ConcurrentQueue.h"

#include <dirent.h>

static std::string baseDir;

std::shared_ptr<ConcurrentQueue<std::string>> scan_base_dir(const std::string& path)
{
    std::shared_ptr<ConcurrentQueue<std::string>> q = std::make_shared<ConcurrentQueue<std::string>>();
    DIR* dirp = opendir(path.c_str());
    if (!dirp) {
        ERR("Could not open directory %s: %s (%d)", path.c_str(),
            strerror(errno), errno);
        return q;
    }
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        std::string filename(dp->d_name);
        if (filename.size() > 3 &&
                0 == filename.compare(filename.size() - 4, 4, ".wav")) {
            // FIXME: to also find wave-files with another extension parse the
            // first bytes and act on wave-headers
            q->push(path + PATH_SEPERATOR + filename);
            DBG("Pushed file %s", (path + PATH_SEPERATOR + filename).c_str());
        }
    }
    closedir(dirp);
    return q;
}

void show_usage()
{
    LOG("USAGE:\n"
        "wav2mp3 /path/to/wav/dir");
    exit(1);
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        ERR("Missing argument!");
        show_usage();
    }

    baseDir = argv[1];

    std::shared_ptr<ConcurrentQueue<std::string>> queue = scan_base_dir(baseDir);
    if (!queue || queue->empty()) {
        LOG("Base directory %s does not contain .wav files!", baseDir.c_str());
        return 0;
    }

    // create encoder threads:
    long numCores = get_num_cores();
    long numThreads = std::min(numCores,
                               static_cast<signed long>(queue->size()));
    DBG("Discovered %ld cores, have %" PRIuPTR " files to encode, creating %lu threads",
        numCores, queue->size(), numThreads);
    std::vector<EncoderThread> encoderThreads;
    for (int i = 0; i < numThreads; i++) {
        EncoderThread et(queue);
        et.start();
        encoderThreads.push_back(et);
    }

    for (int i = 0; i < numThreads; i++)
        encoderThreads.at(i).wait();

    return 0;
}
