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

#ifndef _ENCODER_THREAD_H_
#define _ENCODER_THREAD_H_

#include <queue>
#include <pthread.h>
#include <string>
#include <memory>

#include "ConcurrentQueue.h"

class EncoderThread
{
private:
    std::shared_ptr<ConcurrentQueue<std::string>> queue;
    pthread_t pthread;

public:
    explicit EncoderThread(const std::shared_ptr<ConcurrentQueue<std::string>>& q);
    ~EncoderThread() = default;

    static void* thread(void* userData);

    void start();
    void wait() const;
};

#endif
