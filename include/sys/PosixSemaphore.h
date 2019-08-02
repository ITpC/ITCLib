/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: PosixSemaphore.h 101 2007-08-12 13:07:51Z acs/pk $
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 **/


#ifndef __PosixSemaphore_H__
#  define __PosixSemaphore_H__

#  include <errno.h>
#  include <signal.h>

#  include <atomic>
#  include <system_error>

#  include <pthread.h>
#  include <semaphore.h>
#  include <errno.h>
#  include <time.h>
#  include <string.h>


#  include <sys/Types.h>
#  include <sys/prototypes.h>
#  include <usecount.h>

#include <iostream>

namespace itc
{
  namespace sys
  {

    /**
     * This class is safe to use with the itc::sys::CancelableThread class. 
     * It is simple raw POSIX semaphores wrapper. Without pthread_cancel
     * you can experience undefined behavior by destroying instances of this
     * class when other threads waiting on this semaphore.
     * 
     **/
    class POSIXSemaphore
    {
     private:
      mutable std::atomic<bool>    valid;
      mutable std::atomic<size_t>  counter;
      mutable sem_t                semaphore;
      
     public:
      

      explicit POSIXSemaphore(ulong def_val = 0): valid{false},counter{0}
      {
        if(sem_init(&semaphore, 0, def_val))
          throw std::system_error(errno, std::system_category(), "RawPosixSemaphore::RawPosixSemaphore(): semaphore initialization failed");
        valid.store(true);
      }

      explicit POSIXSemaphore(const POSIXSemaphore&) = delete;
      explicit POSIXSemaphore(POSIXSemaphore&) = delete;

      const bool wait(void) const
      {
        usecount uc(&counter);
        return valid.load()&&(sem_wait(&semaphore) != -1)&&valid.load();
      }

      const bool post(void) const
      {
        usecount uc(&counter);
        return valid.load()&&(sem_post(&semaphore) == 0)&&valid.load();
      }

      void justwait(const ::timespec& timeout)
      {
        usecount uc(&counter);
        if(valid.load())
        {
          if((sem_timedwait(&semaphore, &timeout) == -1)||(!valid.load()))
            throw std::system_error(errno,std::system_category());
        }else
          throw std::system_error(EACCES,std::system_category(),"RawPosixSemaphore::justwait(): an attempt to access an invalid semaphore");
      }
      
      const bool timed_wait(const ::timespec& timeout) const noexcept
      {
        usecount uc(&counter);
        return valid.load()&&(sem_timedwait(&semaphore, &timeout) == 0)&&valid.load();
      }

      const bool try_wait(void) const
      {
        usecount uc(&counter);
        return valid.load()&&(sem_trywait(&semaphore) != -1)&&valid.load();
      }

      const bool getValue(int& value)
      {
        usecount uc(&counter);
        return valid.load()&&(sem_getvalue(&semaphore, &value) != -1)&&valid.load();
      }
      
      const auto get_usage() const
      {
        return counter.load();
      }
      
      const bool isValid() const
      {
        return valid.load();
      }
      /**
       * In Linux and Solaris a thread cancellation has to be called before 
       * semaphore  will be destroyed. Prevent undefined behavior on sem_destroy
       * by waiting for all threads to finish
       **/
      void destroy() noexcept
      {
        valid.store(false);
        while(get_usage() > 0)
        {
            sem_post(&semaphore);
        }
        sem_destroy(&semaphore);
      }

      ~POSIXSemaphore() noexcept
      {
        destroy(); 
      }
    };
  }
}

#endif
