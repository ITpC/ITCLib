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
      mutable std::atomic<bool> valid;
      mutable sem_t semaphore;
      
     public:
      

      explicit POSIXSemaphore(ulong def_val = 0): valid{false}
      {
        if(sem_init(&semaphore, 0, def_val))
          throw std::system_error(errno, std::system_category(), "RawPosixSemaphore::RawPosixSemaphore(): semaphore initialization failed");
        valid.store(true);
      }

      explicit POSIXSemaphore(const POSIXSemaphore&) = delete;
      explicit POSIXSemaphore(POSIXSemaphore&) = delete;

      const bool wait(void) const
      {
        return valid.load()&&(sem_wait(&semaphore) != -1);
      }

      const bool post(void) const
      {
        return valid.load()&&(sem_post(&semaphore) == 0);
      }

      void justwait(const ::timespec& timeout)
      {
        if(valid.load())
        {
          if(sem_timedwait(&semaphore, &timeout) == -1)
            throw std::system_error(errno,std::system_category());
        }else
          throw std::system_error(EACCES,std::system_category(),"RawPosixSemaphore::justwait(): an attempt to access an invalid semaphore");
      }
      
      const bool timed_wait(const ::timespec& timeout) const noexcept
      {
        if(valid.load())
        {
          return (sem_timedwait(&semaphore, &timeout) == 0);
        }
        return false;
      }

      const bool try_wait(void) const
      {
        return valid.load()&&(sem_trywait(&semaphore) != -1);
      }

      const bool getValue(int& value)
      {
        return valid.load()&&(sem_getvalue(&semaphore, &value) != -1);
      }

      const bool isValid() const
      {
        return valid.load();
      }
      /**
       * In Linux and Solaris a thread cancellation has to be called before 
       * semaphore  will be destroyed. destroy() costs a lot.
       **/
      void destroy() noexcept
      {
        bool test=true;
        while((!valid.compare_exchange_strong(test,false))&&(test));
        if(test)
        {
          // an attempt to release all the waiting threads
          // subscribers must check the return code.
          // limited to 1000 subscribers ...
          for(auto i=0;i<1000;++i)
          {
            sem_post(&semaphore);
          }
          // No error code checked, semaphore may be not destroyed. 
          // There is no way you can handle error here. 
          // Undefined behavior for threads using this semaphore.
          sem_destroy(&semaphore);
        }
      }

      ~POSIXSemaphore() noexcept
      {
        destroy(); 
      }
    };
  }
}

#endif
