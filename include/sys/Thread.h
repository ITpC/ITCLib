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
#define	__THREAD_H__

#include <pthread.h>
#include <sched.h>

#include <memory>
#include <system_error>

#include <sys/Types.h>

#include <abstract/Cleanable.h>
#include <abstract/Runnable.h>
#include <sys/semaphore.h>
#include <sys/prototypes.h>
#include <InterfaceCheck.h>
#include <sys/synclock.h>



namespace itc
{
  namespace sys
  {
    class Thread;

    void cleanup_handler(::itc::abstract::Cleanable*);
    void* invoke(itc::sys::Thread*);

    /**
     * This class must be inherited by any other that requires MT functionality.
     * There are 3 obligatory steps to make it proper:
     *      1. implement run() method
     *      2. use begin() method in constructor of derived class, or whenever you want to start thread
     *      3. use finish()	 method in destructor of derived class.
     *
     * So here is sequence: Constructor of this class creates a thread that will call run() method of
     * derived class when Thread::begin() method is called. Without call of begin(), thread will be not activated
     * and persist in sleep state. When thread::finish() called without former call of Thread::begin(),
     * then main thread will stay forever and wait till thread finish it self if Thread::detach() was not called
     * in between. To keep it problem-less follow the 3 steps above.
     * 
     * @exceptions Can_not_create_thread exception is thrown if pthread_create fails, appropriate errno is provided 
     * with exception.
     *   
     **/
    class Thread
    {
    protected:
      friend void* invoke(Thread*);
    private:
      using semaphore=::itc::sys::POSIXSemaphore;
      
      semaphore start;
      pthread_t TID;     

    public:

      /**
       * Default constructor Thread::Thread()
       * Creates a thread in wait state.
       */
      explicit Thread() : start(), TID{0}
      {
        int ret = ::pthread_create(&TID, NULL, (thread_t) invoke, this);
        if (ret) throw std::system_error(ret,std::system_category(),"Can't create thread");
      }

      Thread(const Thread&) = delete;
      Thread(Thread&) = delete;
      
      const int send_signal(int signo)
      {
        return ::pthread_kill(TID, signo);
      }

      const int cancel()
      {
        return ::pthread_cancel(TID);
      }

      const int detach()
      {
        return ::pthread_detach(TID);
      }

      const pthread_t getThreadId() const
      {
        return TID;
      }

      const pthread_t getCurrentThrId() const
      {
        static thread_local auto current{pthread_self()};
        return current;
      }

      void yield() const
      {
        YIELD();
      }

    protected:
      virtual ~Thread() = default;
      virtual void run() = 0;

      void begin()
      {
        start.post();
      }

      int finish()
      {
        return ::pthread_join(TID, NULL);
      }
    };
  }
}

#endif
