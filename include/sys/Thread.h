/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: Thread.h 101 2007-08-12 13:07:51Z pk $
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 **/


#ifndef	__THREAD_H__
#  define	__THREAD_H__

#  include <pthread.h>
#  include <sched.h>

#  include <memory>

#  include <ITCException.h>
#  include <sys/Types.h>

#  include <abstract/Cleanable.h>
#  include <abstract/Runnable.h>
#  include <sys/PosixSemaphore.h>
#  include <sys/prototypes.h>
#  include <InterfaceCheck.h>

#  include <TSLog.h>



namespace itc
{
  namespace sys
  {
    class Thread;

    void cleanup_handler(::itc::abstract::Cleanable*);
    void* invoke(itc::sys::Thread*);

    /**
     * This class must be inherited by any other that requiers MT functionallity.
     * There are 3 obligatory steps to make it proper:
     *      1. implement run() method
     *      2. use begin() method in constructor of derived class, or whenever you want to start thread
     *      3. use finish()	 method in destructor of derived class.
     *
     * So here is sequence: Constructor of this class creates a thread that will call run() method of
     * derived class when Thread::begin() method is called. Without call of begin(), thread will be not activated
     * and persist in sleep state. When thread::finish() called without former call of Thread::begin(),
     * then main thread will stay forever and wait till thread finish it self if Thread::detach() was not called
     * in between. To keep it problemless follow the 3 steps above.
     * 
     * @exceptions Can_not_create_thread exception emmited if pthread_create fails, appropriate errno is provided 
     * with exception.
     *   
     **/
    class Thread
    {
     protected:
      friend void* invoke(Thread*);
     private:
      pthread_t TID;
      Semaphore start;
     public:

      /**
       * Default constructor Thread::Thread()
       * Creates a thread in wait state.
       */
      Thread() : start()
      {
        int ret = this->create();
        if(ret) throw ITCException(ret, exceptions::Can_not_create_thread);
      }

      inline int create()
      {
        return ::pthread_create(&TID, NULL, (thread_t) invoke, this);
      }

      inline int send_signal(int signo)
      {
        return ::pthread_kill(TID, signo);
      }

      inline int cancel()
      {
        return ::pthread_cancel(TID);
      }

      inline int detach()
      {
        return ::pthread_detach(TID);
      }

      inline pthread_t getThreadId() const
      {
        return TID;
      }

      inline pthread_t getCurrentThrId()
      {
        return ::pthread_self();
      }

      inline void yield() const
      {
        YIELD();
      }



     protected:
      virtual ~Thread() = default;
      virtual void run() = 0;

      inline void begin()
      {
        start.post();
      }

      inline int finish()
      {
        return ::pthread_join(TID, NULL);
      }
    };
  }
}

#endif
