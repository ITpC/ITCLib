/**
 *  Copyright 2018, Pavel Kraynyukhov <pavel.kraynyukhov@gmail.com>
 * 
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 *  $Id: spinlock.h November 13, 2018 11:41 AM $
 * 
 **/

#ifndef __SPINLOCK_H__
#  define __SPINLOCK_H__

#include <atomic>
#include <stdexcept>

namespace itc
{
  namespace sys
  {
    class spinlock
    {
     private:
      std::atomic_flag slock;
      
     public:
      explicit spinlock() : slock{ATOMIC_FLAG_INIT} {}
      spinlock(spinlock&)=delete;
      spinlock(const spinlock&)=delete;

      const bool try_lock()
      {
        throw std::logic_error("spinlock::try_lock() may not be implemented" );
      }

      void lock()
      {
        while (slock.test_and_set(std::memory_order_acquire));
      }

      void unlock()
      {
        slock.clear(std::memory_order_release);
      }
    };
  }
}
#endif /* __SPINLOCK_H__ */

