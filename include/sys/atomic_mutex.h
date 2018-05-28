/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id$
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 **/

#ifndef ATOMIC_MUTEX_H
#define ATOMIC_MUTEX_H
#include <atomic>

namespace itc {
  namespace sys {
    class AtomicMutex 
    {
    private:
      std::atomic_flag mLock;
    public:
      explicit AtomicMutex():mLock(ATOMIC_FLAG_INIT)
      {
      }
      AtomicMutex(const AtomicMutex&)=delete;
      AtomicMutex(AtomicMutex&)=delete;
      void lock()
      {
        while(mLock.test_and_set(std::memory_order_acquire)) sched_yield();
      }
      void unlock()
      {
        mLock.clear(std::memory_order_release);
      }
      const bool try_lock()
      {
        if(!mLock._M_i)
        {
          while(mLock.test_and_set(std::memory_order_acquire));
          return true;
        }else{
          return false;
        }
      }
    };   
  }
}

#endif /* ATOMIC_MUTEX_H */

