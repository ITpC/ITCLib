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
      
      explicit semaphore() : counter{0},valid{true}
      {
      }
      
      semaphore(const semaphore&)=delete;
      semaphore(semaphore&)=delete;
      
      const bool post() const
      {
        ++counter;
        
        if(pending.load() >0)
          fallback.post();
        
        return valid.load();
      }
      
      const bool timed_wait(const struct timespec waiton) const
      {
        if(try_wait())
          return true;
        else{
          ++pending;
          fallback.timed_wait(waiton);
          --pending;
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
            ++counter;
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
            ++pending;
            fallback.wait();
            --pending;
            attempts=0;
          }
        }
      }
      
      void destroy() noexcept
      {
        valid.store(false);
        // release all pending (worst case scenario: counter(0), pending(0), 100 waiting threads);
        
        pending.store(pending.load()+100);
        counter.store( (counter.fetch_add(100)>0) ? counter.fetch_add(100) : 100);
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
      
      ~semaphore() noexcept
      {
        destroy();
      }
    };
  }
}

#endif /* __USERLAND_SEMAPHORE_H__ */

