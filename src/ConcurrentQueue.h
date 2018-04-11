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

#ifndef _CONCURRENT_QUEUE_H_
#define _CONCURRENT_QUEUE_H_

#include <queue>
#include <pthread.h>

/**
 * Thin wrapper class around std::queue providing an thread-safe pop_front
 */
template <typename T>
class ConcurrentQueue: public std::queue<T>
{
public:
    ConcurrentQueue()
        : std::queue<T>()
    {
        pthread_mutex_init(&m_lock, NULL);
    }

    ConcurrentQueue(const ConcurrentQueue& other)
        : std::queue<T>(other)
    {
        pthread_mutex_init(&m_lock, NULL);
    }

    ConcurrentQueue(const ConcurrentQueue&& other)
        : std::queue<T>(std::move(other))
    {
        pthread_mutex_init(&m_lock, NULL);
    }

    virtual ~ConcurrentQueue() = default;

    ConcurrentQueue<T>& operator=(const ConcurrentQueue<T>& other) = default;
    ConcurrentQueue<T>& operator=(ConcurrentQueue<T>&& other) = default;

    T pop_front()
    {
        if (this->empty())
            return T();
        pthread_mutex_lock(&m_lock);
        T t = this->front();
        this->pop();
        pthread_mutex_unlock(&m_lock);
        return t;
    }

private:
    pthread_mutex_t m_lock;
};

#endif
