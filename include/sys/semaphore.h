/**
 * Copyright Pavel Kraynyukhov 2007 - 2018.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * 
 *  $Id: sys/semaphore.h October 27, 2018 1:33 PM $
 * 
 *  EMail: pavel.kraynyukhov@gmail.com
 * 
 **/


#ifndef __USERLAND_SEMAPHORE_H__
#  define __USERLAND_SEMAPHORE_H__

#include <atomic>
#include <system_error>
#include <time.h>

#include <sys/PosixSemaphore.h>

namespace itc
{
  namespace sys
  {
    class semaphore
    {
     private:
      mutable std::atomic<int64_t>  counter;
      std::atomic<bool>             mayRun;
      
     public:
      
      explicit semaphore() : counter{0},mayRun{true}
      {
      }
      
      semaphore(const semaphore&)=delete;
      semaphore(semaphore&)=delete;
      
      const bool post() const
      {
        ++counter;
        return mayRun.load();
      }
      
      const bool try_wait() const
      {
        if(mayRun.load())
        {
          if(counter.fetch_sub(1)>0)
            return true;
          else 
          {
            ++counter;
            return false;
          }
        }else{
          throw std::system_error(ENOENT,std::system_category(),"Semaphore is destroyed");
        }
      }
      
      void wait()
      {
        static thread_local const struct timespec pause{0,100000};
        
        while(!try_wait())
        {
          nanosleep(&pause,nullptr);
        }
      }
      
      void destroy()
      {
        mayRun.store(false);
      }
      
      const int64_t sub(const size_t value)
      {
        if(mayRun.load())
        {
          return counter.fetch_sub(value);
        }else{
          throw std::system_error(ENOENT,std::system_category(),"Semaphore is destroyed");
        }
      }
      
      ~semaphore()
      {
        destroy();
      }
    };
  }
}

#endif /* __USERLAND_SEMAPHORE_H__ */

