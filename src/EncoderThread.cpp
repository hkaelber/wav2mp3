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

#include "EncoderThread.h"
#include "FileEncodeJob.h"
#include "utils.h"

#include <cinttypes>

EncoderThread::EncoderThread(std::shared_ptr<ConcurrentQueue<std::string>> q)
    : queue(q)
{
}

void* EncoderThread::thread(void *userData)
{
    DBG("Enter thread 0x%" PRIxPTR, current_thread_id());
    EncoderThread* instance = static_cast<EncoderThread*>(userData);
    while (!instance->queue->empty()) {
        std::string nextFile;
        nextFile = instance->queue->pop_front();

        DBG("Thread 0x%" PRIxPTR ": Processing next file %s ...",
              current_thread_id(), nextFile.c_str());
        FileEncodeJob encoder(nextFile);
        if (!encoder.process()) {
            ERR("Could not encode file %s", nextFile.c_str());
        }
    }
    return NULL;
}

void EncoderThread::start()
{
    pthread_create(&pthread, NULL, EncoderThread::thread, this);
}

void EncoderThread::wait() const
{
    DBG("Waiting for thread 0x%" PRIxPTR, current_thread_id());
    pthread_join(pthread, NULL);
    DBG("Done waiting for thread 0x%" PRIxPTR, current_thread_id());
}
