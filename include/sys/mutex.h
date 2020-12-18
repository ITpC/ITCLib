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
#include <sys/sched_yield.h>
#include <usecount.h>

namespace itc {
  namespace sys {
    
    class mutex
    {
    private:
      std::atomic<bool>              valid;
      mutable std::atomic<size_t>    counter;
      std::atomic<pthread_t>         mLock;
      
    public:
      explicit mutex():valid{true},counter{0},mLock{0}
      {
      }
      mutex(const mutex&)=delete;
      mutex(mutex&)=delete;
      
      void lock()
      {
        usecount uc(&counter);
        if(valid.load())
        {
          pthread_t unused=0;
          static thread_local auto current=pthread_self();
          
          while(!mLock.compare_exchange_strong(unused,current))
          {
            unused=0;
            itc::sys::sched_yield(10);
          }
        }else{
          throw std::system_error(EOWNERDEAD,std::system_category(), "itc::mutex::lock() - This mutex is being destroyed");
        }
      }
      
      void unlock() // Do not check for validity. if mutex is not yet destroyed, let the thread to unlock it.
      {
        usecount uc(&counter);
        if(valid.load())
        {
          static thread_local auto current=pthread_self();
        
          if(!mLock.compare_exchange_strong(current,0))
          {
            throw std::system_error(EACCES, std::system_category(), "An attempt to unlock the mutex owned by other thread");
          }
        }else{
          throw std::system_error(EOWNERDEAD,std::system_category(), "itc::mutex::lock() - This mutex is being destroyed");
        }
      }
      
      void unlock_destroy()
      {
        usecount uc(&counter);
        if(valid.compare_exchange_strong(true,false))
        {
          
          static thread_local auto current=pthread_self();
        
          if(!mLock.compare_exchange_strong(current,0))
          {
            throw std::system_error(EACCES, std::system_category(), "An attempt to unlock the mutex owned by other thread");
          }
        }else{
          throw std::system_error(EOWNERDEAD,std::system_category(), "itc::mutex::lock() - This mutex is being destroyed");
        }
      }
      
      const bool try_lock()
      {
        usecount uc(&counter);
        if(valid.load())
        {
          pthread_t unused=0;
          static thread_local auto current=pthread_self();
          return mLock.compare_exchange_strong(unused,current);
        }
        return false;
      }
      
      const bool busy() const
      {
        usecount uc(&counter);
        return (mLock.load() != 0) || (!valid.load());
      }
      
      const bool isvalid() const
      {
        usecount uc(&counter);
        return valid.load();
      }
      
      ~mutex()
      {
        valid.store(false);
        while((mLock.load() != 0)&&(counter.load() != 0));
      }
    };
  }
}

#endif /* __USERLAND_MUTEX_H__ */

