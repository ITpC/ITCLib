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
#include <sys/sched_yield.h>

namespace itc
{
  namespace sys
  {
    /**
     * User-land semaphore with fallback to POSIX semaphore on max attempts 
     * to decrement a semaphore's counter in wait()
     **/
    template <const size_t max_attempts_before_fallback=25> class semaphore
    {
     private:
      mutable std::atomic<int64_t>  counter;
      mutable std::atomic<size_t>   pending;
      std::atomic<bool>             valid;
      POSIXSemaphore                fallback;
      
      
     public:
      
      explicit semaphore() : counter{0},pending{0},valid{true},fallback()
      {
      }
      
      semaphore(const semaphore&)=delete;
      semaphore(semaphore&)=delete;
      
      const bool post() const
      {
        if(valid.load())
        {
          counter.fetch_add(1);

          if(pending.load() >0)
            fallback.post();

          return valid.load();
        }else{
          throw std::system_error(EOWNERDEAD,std::system_category(),"This semaphore is being destroyed");
        }
      }
      
      const bool timed_wait(const struct timespec waiton) const
      {
        if(try_wait())
          return true;
        else{
          pending.fetch_add(1);
          fallback.timed_wait(waiton);
          pending.fetch_sub(1);
          return try_wait();
        }
      }
      
      const bool try_wait() const
      {
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
        size_t attempts=0;
        while(!try_wait())
        {
          if(++attempts >= max_attempts_before_fallback)
          {
            pending.fetch_add(1);
            if((!fallback.wait())||(!valid.load()))
            {
              pending.fetch_sub(1);
              throw std::system_error(EOWNERDEAD,std::system_category(),"This semaphore is being destroyed");
            }
            pending.fetch_sub(1);
            attempts=0;
          }
        }
      }
      
      void destroy() noexcept
      {
        if(valid.load())
        {
          valid.store(false);
          // release pending (up to 1000 threads);
          // Can't simply increase counter and fallback.post() pending times, because of ABA problem.
          counter.fetch_add(1000);
          for(size_t i=0;i<1000;++i) fallback.post();
        
          fallback.destroy();
        }
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
      }
    };
  }
}

#endif /* __USERLAND_SEMAPHORE_H__ */

