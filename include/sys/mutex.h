/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: ITCLib::include/sys/mutex.h$
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 **/

#ifndef __USERLAND_MUTEX_H__
#define __USERLAND_MUTEX_H__
#include <atomic>
#include <time.h>

namespace itc {
  namespace sys {
    
    class mutex
    {
    private:
      std::atomic<pthread_t> mLock;
    public:
      explicit mutex():mLock{0}
      {
      }
      mutex(const mutex&)=delete;
      mutex(mutex&)=delete;
      
      void lock()
      {
        pthread_t unused=0;
        while(!mLock.compare_exchange_strong(unused,pthread_self()))
        {
          static thread_local const struct timespec pause{0,1000};

          unused=0;
          nanosleep(&pause,nullptr);
        };
      }
      
      void unlock()
      {
        pthread_t current=pthread_self();
        
        if(!mLock.compare_exchange_strong(current,0))
        {
          throw std::system_error(EACCES, std::system_category(), "An attempt to unlock the mutex owned by other thread");
        }
      }
      
      const bool try_lock()
      {
        pthread_t unused=0;
        return mLock.compare_exchange_strong(unused,pthread_self());
      }
    };
  }
}

#endif /* __USERLAND_MUTEX_H__ */

