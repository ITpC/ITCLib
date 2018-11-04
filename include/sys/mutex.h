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
      std::atomic<bool>      valid;
      std::atomic<pthread_t> mLock;
      
      /*
       * Linux sched_yield() works only for threads with SCHED_FIFO and SCHED_RR
       * policies (real-time), therefore replacing with context switch made by
       * nanosleep(3). 
       */
      inline void sched_yield()
      {
        static thread_local struct timespec pause{0,1};
        nanosleep(&pause,nullptr);
      }
    public:
      explicit mutex():valid{true},mLock{0}
      {
      }
      mutex(const mutex&)=delete;
      mutex(mutex&)=delete;
      
      void lock()
      {
        if(valid.load())
        {
          pthread_t unused=0;
          while(!mLock.compare_exchange_strong(unused,pthread_self()))
          {
            this->sched_yield();
            unused=0;
          }
        }else{
          throw std::system_error(EOWNERDEAD,std::system_category(), "itc::mutex::lock() - This mutex is being destroyed");
        }
      }
      
      void unlock() // Do not check for validity. if mutex is not yet destroyed, let the thread to unlock it.
      {
        pthread_t current=pthread_self();
        
        if(!mLock.compare_exchange_strong(current,0))
        {
          throw std::system_error(EACCES, std::system_category(), "An attempt to unlock the mutex owned by other thread");
        }
      }
      
      const bool try_lock()
      {
        if(valid.load())
        {
          pthread_t unused=0;
          return mLock.compare_exchange_strong(unused,pthread_self());
        }
        return false;
      }
      ~mutex()
      {
        valid.store(false);
      }
    };
  }
}

#endif /* __USERLAND_MUTEX_H__ */

