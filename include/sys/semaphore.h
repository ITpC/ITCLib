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
#include <sys/PosixSemaphore.h>
#include <iostream>
#include <usecount.h>
#include <sys/sched_yield.h>

namespace itc
{
  namespace sys
  {
    /**
     * User-land semaphore with fallback to POSIX semaphore on max attempts 
     * to decrement a semaphore's counter in wait()
     **/
    class semaphore
    {
     private:
      const size_t max_attempts_before_fallback;
      POSIXSemaphore                fallback;
      mutable std::atomic<int64_t>  counter;
      mutable std::atomic<size_t>   pending;
      mutable std::atomic<size_t>   inuse;
      std::atomic<bool>             valid;
      
     public:
      
      explicit semaphore(const size_t mabf=10)
      : max_attempts_before_fallback{mabf}, fallback(),
        counter{0},pending{0},inuse{0},valid{true}
      {
      }
      
      semaphore(const semaphore&)=delete;
      semaphore(semaphore&)=delete;
      
      const bool post() const
      {
        usecount uc(&inuse);
        
        if(valid.load())
        {
          counter.fetch_add(1);

          if(pending.load() >0)
            fallback.post();
        }
        
        return valid.load();
      }
      
      const bool timed_wait(const struct timespec waiton) const
      {
        usecount uc(&inuse);
        
        if(valid.load())
        {
          if(try_wait())
            return true;
          else{
            usecount pc(&pending);
            fallback.timed_wait(waiton);
            return try_wait();
          }
        }else{
          throw std::system_error(EOWNERDEAD,std::system_category(),"This semaphore is being destroyed");
        }
      }
      
      const bool try_wait() const
      {
        usecount uc(&inuse);
        
        if(valid.load())
        {
          if(counter.fetch_sub(1)>0)
            return true;
          else 
          {
            counter.fetch_add(1);
            return false;
          }
        }else{
          throw std::system_error(EOWNERDEAD,std::system_category(),"This semaphore is being destroyed");
        }
      }
      
      void wait()
      {
        usecount uc(&inuse);
        
        size_t attempts=0;
        
        while(!try_wait())
        {
          if(++attempts >= max_attempts_before_fallback)
          {
            if(valid.load())
            {
              usecount pc(&pending);
              if(!fallback.wait())
              {
                throw std::system_error(EOWNERDEAD,std::system_category(),"This semaphore is being destroyed");
              }
            } else {
              throw std::system_error(EOWNERDEAD,std::system_category(),"This semaphore is being destroyed");
            }
          }
        }
      }
      
      void destroy() noexcept
      {
          valid.store(false);
          counter.fetch_add(1000);
          fallback.destroy();
      }
      
      const int64_t sub(const size_t value)
      {
        if(valid.load())
        {
          return counter.fetch_sub(value);
        }else{
          throw std::system_error(EOWNERDEAD,std::system_category(),"This semaphore is being destroyed");
        }
      }
      
      const int64_t get_value() const noexcept
      {
        return counter.load();
      }
      
      ~semaphore() noexcept
      {
        destroy();
        //while(inuse.load()){itc::sys::sched_yield(10);};
      }
    };
  }
}

#endif /* __USERLAND_SEMAPHORE_H__ */

