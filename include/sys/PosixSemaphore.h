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


#  include <pthread.h>
#  include <semaphore.h>
#  include <errno.h>
#  include <time.h>
#  include <string.h>

#  include <ITCException.h>
#  include <sys/Types.h>
#  include <sys/prototypes.h>
#  include <GAINTLock.h>
#  include <atomic>

namespace itc
{
  namespace sys
  {
    /**
     * POSIX.1-2001 and POSIX.1-2004 specificatios (IEEE Std 1003.1, 2001,2004 Edition)
     * is a bit ambigous about sem_wait/sem_destroy/sem_get_value.
     * 
     * 1. EINVAL parameter became  optional since 2001.
     * 
     * An undefined behavior at destroying semaphores, on those some threads are waiting is a plague.
     * The implementations are not required to define fixed behavior or condition in above case. 
     * In addition EBUSY return value is specified as an option.
     * 
     * There are 3 problems in most of implementations:
     * 2. sem_getvalue(sem_t* sem,int* val) have dual behavior (by specification Edition 2001) in returning 
     * of value if there are threads waiting on semaphore. Either negative value set to val or 0.
     *    First behavior is exceptionally soficient for any needs, however second one sux.
     * 
     * 3. sem_destroy() has undefined behavior on trys to destroy a semmaphore on which another threads are blocking. 
     *    returning -1 and set errno to EBUSY is optional :(
     * 
     * 4. sem_wait() Edition 2004 says nothing about behavior related to (3) and EINVAL became an option.
     *    Good news returning -1 and seting errno to EINTR is not an option and we can use it to our adventages.
     *  
     * Linux implementation:
     *  Linux kernel 2.6.16 and glibc-2.4 (latest i have tested) have second behavior for sem_getvalue. 
     *  sem_count is defined as unsigned int and never drops below 0. No additional count 
     *  for threads waiting on semaphore. You can never get the value of threads waiting.      
     *  sem_destroy always return 0 and makes nothing more. Just return 0 !!!. 
     *  There is no code for semaphore invalidation. conclusion: it sux.
     * 
     * Pthread-win32 implementation is much better in this case. It has specified EBUSY and safe behavior 
     * for semaphores destroying. sem_getvalue returns negative value in case there are threads blocking on 
     * semaphore - :aplause.  
     * 
     * FreeBSD implementation conforms to POSIX.1-1996 and has same sucking behavior for sem_getvalue as linux has.
     * The safety of sem_destroy is just same as Pthread-win32 has. Instead of destroying semaphore that is 
     * blocked by other threads -1 is returned and errno set to EBUSY. 
     * sem_wait is non interraptible :(
     * Conclusion: better then nothing.
     * 
     * 
     * Solaris 9 (SunOS 5.9) implementation:
     * sem_destroy - undefined behavior by destroying of semaphores those are blocked by another threads.
     * sem_getvalue - set value to 0 or positive number - same sux as under linux.
     * sem_wait - interraptible.
     * 
     * 
     * However sem_wait is defined as a cancellation point. so that should not be a problem to break 
     * sem_wait if we use pthread_cancel() with pthread_cleanup_push() and pthread_cleanup_pop().
     * This should be done in the thread not in the semaphore classes. Take itc::sys::CancelableThread 
     * for this purposes.
     * 
     **/

#  if defined(_MSC_VER) && (defined(__MINGW32_VERSION))
#    define __PTPS__

    /**
     * Like RawPosixSemaphore. Should be used with pthread for win32 (best pthread 
     * realization ever). 
     * 
     **/

    class PosixSemaphore
    {
     private:
      sem_t semaphore;
     public:

      explicit PosixSemaphore(ulong def_val = 0)
      {
        if(sem_init(&semaphore, 0, def_val))
          throw ITCException(errno, exceptions::Can_not_initialize_semaphore);
      }

      explicit PosixSemaphore(const PosixSemaphore& ref)
      {
        // seak idea to make a copy of semaphore.
        throw ITCException(errno, exceptions::Can_not_copy_semaphore);
      }

      inline void wait(void)
      {
        if(sem_wait(&semaphore) == -1)
        {
          throw ITCException(exceptions::Can_not_wait_on_semaphore, EINVAL);
        }
      }

      inline void timedWait(const ::timespec& timeout)
      {
        if(sem_timedwait(&semaphore, &timeout) == -1)
        {
          throw ITCException(exceptions::Can_not_wait_on_semaphore, errno);
        }
      }

      inline int tryWait(void)
      {
        register int ret = sem_trywait(&semaphore);

        if(ret == -1)
        {
          if(errno == EAGAIN)
          {
            return EAGAIN;
          }
          throw ITCException(EINVAL, exceptions::Can_not_wait_on_semaphore);
        }

        return 0;
      }

      inline void post(void)
      {
        if(sem_post(&semaphore) == -1)
        {
          throw ITCException(exceptions::Can_not_wait_on_semaphore, errno);
        }
      }

      inline int getValue(void)
      {
        register int value; // is on stack ... should be thread safe ...

        if(sem_getvalue(&semaphore, &value))
        {
          throw ITCException(exceptions::Can_not_get_semaphore_value, errno);
        }

        return value;
      }

      inline void destroy()
      {
        while((sem_destroy(&semaphore) == -1) && (errno == EBUSY))
        {
          int value = 0;
          if(sem_getvalue(&semaphore, &value) != -1)
          {
            sem_post_multiple(&semaphore, value); // Cancellation still required, wake all waiting threads !
          }
        }
      }

      ~PosixSemaphore()
      {
        destroy();
      }
    };
#  else
#    define __RPS__

    /**
     * This class is safe to use with the itc::sys::CancelableThread class. 
     * It is simple raw POSIX semaphores wrapper. Without pthread_cancel
     * you can experience undefined behavior by destroying instances of this
     * class when other threads waiting on this semaphore.
     * 
     * Update: all other archs are broken now. Interface change.
     * 
     **/
    class RawPosixSemaphore
    {
     private:
      mutable sem_t semaphore;
      // they say: do not use volatile. no sync here. RIGH! That is what i need.
      // I do not need the sync, I need __sporadically__ correct value of this
      // variable. This variable is changed outside of thread, 
      // by calling destroy()
      volatile bool valid; 
     public:

      explicit RawPosixSemaphore(ulong def_val = 0)
      {
        if(sem_init(&semaphore, 0, def_val))
          throw ITCException(errno, exceptions::Can_not_initialize_semaphore);
        valid=true;
      }

      explicit RawPosixSemaphore(const RawPosixSemaphore&) = delete;
      explicit RawPosixSemaphore(RawPosixSemaphore&) = delete;

      const bool wait(void) const
      {
        return valid&&(sem_wait(&semaphore) != -1);
      }

      const bool post(void) const
      {
        return valid&&(sem_post(&semaphore) != -1);
      }

      const bool timedWait(const ::timespec& timeout) const
      {
        return valid&&(sem_timedwait(&semaphore, &timeout) != -1);
      }

      const bool tryWait(void) const
      {
        return valid&&(sem_trywait(&semaphore) != -1);
      }

      const bool getValue(int& value)
      {
        return valid&&(sem_getvalue(&semaphore, &value) != -1);
      }

      const bool isValid() const
      {
        return valid;
      }
      /**
       * In Linux and Solaris a thread cancellation has to be called before 
       * semaphore  will be destroyed. destroy() costs a lot and switches 
       * thread context twice.
       **/
      void destroy() noexcept
      {
        if(valid)
        {
          valid=false;
          sched_yield();
          // an attempt to release all the waiting threads
          // subscribers must check the return code
          // limited to 1000 subscribers ...
          for(auto i=0;i<1000;++i)
          {
            sem_post(&semaphore);
            sched_yield();
          }
          // No error code checked, semaphore may be not destroyed. 
          // There is no way you can handle error here. undefined behavior.
          sem_destroy(&semaphore); 
        }
      }

      ~RawPosixSemaphore() noexcept
      {
        destroy();
      }
    };
#  endif

#  if defined (__RPS__)
    typedef RawPosixSemaphore Semaphore;
#  else 
#    if defined (__PTPS__)
    typedef PosixSemaphore Semaphore;
#    endif
#  endif
  }
}



#endif
