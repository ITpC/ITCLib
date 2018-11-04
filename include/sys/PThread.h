/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: PThread.h 27 2011-02-06 13:25:43Z pk $
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 **/



#ifndef __PCTHREAD_H__
#  define __PCTHREAD_H__

#  include <memory>

#  include <InterfaceCheck.h>
#  include <abstract/Runnable.h>
#  include <sys/Thread.h>
#  include <sys/semaphore.h>
#  include <abstract/Cleanable.h>
#  include <abstract/Runnable.h>
#  include <TSLog.h>
#  include <mutex>

namespace itc
{

  enum PThreadState
  {
    STARTING, RUNNING, DONE, CANCEL
  };

  namespace sys
  {

    /**
     * @brief Implementation of the thread intended to be used with the 
     * ThreadPool. It executes a Runnable (implementation of the abstract::Runnable)
     * and stays on hold until the Runnable object is assigned with
     * setRunnable(). The instance of this class, keeps tracking of the 
     * execution state as defined in PThreadState.
     * The objective of the PThread is to run a task once and be returned to
     * the ThreadPool.
     * 
     * Do not use this class to implement long running threads, those have 
     * implemented an endless loop inside the Runnable object.
     *
     * See itc::sys::CancelableThread if you wish to implement independent
     * thread class, which is running outside of the ThreadPool and which
     * lifecycle is determined by your purposes.
     * 
     **/
    class PThread : public Thread, ::itc::abstract::Cleanable
    {
     public:
      typedef ::itc::abstract::IRunnable* RunnablePointer;
      typedef std::shared_ptr<::itc::abstract::IRunnable> TaskType;

     private:
      using semaphore=::itc::sys::semaphore<0>; // fallback to posix semaphore
      friend void* invoke(Thread*);
      friend void cleanup_handler(::itc::abstract::Cleanable*);

      std::mutex mMutex;
      TaskType mRunnable;
      semaphore mTask;
      PThreadState mState;

      inline void setState(PThreadState state)
      {
        std::lock_guard<std::mutex> dosync(mMutex);
        mState = state;
      }

      const bool ok2Run()
      {
        try{
          mTask.wait();
          if(getState() != CANCEL)
          {
            return true;
          }else
          {
            return false;
          }
        }catch(...)
        {
          return false;
        }
      }

      inline void mainLoop()
      {
        while(ok2Run())
        {
          ::itc::getLog()->trace(__FILE__, __LINE__, "Thread [%jx] -> PThread::mainLoop()", this->getThreadId());
          std::lock_guard<std::mutex> dosync(mMutex);
          if(mState != CANCEL) // double check
          {
            mState = RUNNING;
            if(RunnablePointer ptr = mRunnable.get())
            {
              ptr->execute();
            }
            mState = DONE;
          }
          ::itc::getLog()->trace(__FILE__, __LINE__, "Thread [%jx] <- PThread::mainLoop()", this->getThreadId());
        }
      }

     public:

      explicit PThread()
        : Thread(), mMutex(), mState(STARTING)
      {
        ::itc::getLog()->trace(__FILE__, __LINE__, "Thread [%jx] ->  PThread::Pthread()", this->getCurrentThrId());
        std::lock_guard<std::mutex> dosync(mMutex);
        begin();
        ::itc::getLog()->trace(__FILE__, __LINE__, "Thread [%jx] <-  PThread::Pthread()", this->getCurrentThrId());
        ;
      }

      PThread(const PThread&)=delete;
      PThread(PThread&)=delete;
      
      const PThreadState getState() const
      {
        return mState;
      }

      inline TaskType getRunnable()
      {
        std::lock_guard<std::mutex> dosync(mMutex);
        return mRunnable;
      }

      inline void setRunnable(const TaskType& ref)
      {
        std::lock_guard<std::mutex> dosync(mMutex);
        mRunnable = ref;
        mTask.post();
      }

      inline void run()
      {
        pthread_cleanup_push((void (*)(void*))cleanup_handler, this);
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
        mainLoop();
        pthread_cleanup_pop(0);
      }

      inline void cleanup()
      {
        setState(CANCEL);
        mTask.post();
        std::lock_guard<std::mutex> dosync(mMutex);
        if(RunnablePointer ptr = mRunnable.get())
        {
          ptr->onCancel();
        }
      }

      virtual ~PThread() noexcept
      {
        setState(CANCEL);
        mTask.post();
        ::itc::getLog()->trace(__FILE__, __LINE__, "Thread [%jx] calling PThread::~Pthread() for thread [%jx]", this->getCurrentThrId(), this->getThreadId());
        if(RunnablePointer ptr = mRunnable.get())
        {
          ptr->shutdown();
        }
        ::itc::getLog()->trace(__FILE__, __LINE__, "Thread [%jx], PThread::~Pthread() -> Calling cancel() for thread: [%jx]", this->getCurrentThrId(), this->getThreadId());
        mTask.post();
        cancel();
        ::itc::getLog()->trace(__FILE__, __LINE__, "Thread [%jx], PThread::~Pthread() -> Calling finish() for thread: [%jx]", this->getCurrentThrId(), this->getThreadId());
        mTask.post();
        finish();
        ::itc::getLog()->trace(__FILE__, __LINE__, "Thread [%jx], PThread::~Pthread() -> finished thread: [%jx]", this->getCurrentThrId(), this->getThreadId());
      }
    };
  }
}
#endif /*__PCTHREAD_H__*/
