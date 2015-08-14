/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: CancelableThread.h 27 2011-02-06 13:25:43Z pk $
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 **/

#ifndef CLEANABLETHREAD_H_
#define CLEANABLETHREAD_H_

#include <memory>

#include <InterfaceCheck.h>
#include <abstract/Runnable.h>
#include <sys/Thread.h>
#include <sys/PosixSemaphore.h>
#include <abstract/Cleanable.h>
#include <mutex>

namespace itc
{
  namespace sys
  {

    template <typename TRunnable> class CancelableThread : public Thread, ::itc::abstract::Cleanable
    {
    private:
      std::atomic<bool> isfinished;
      std::shared_ptr<TRunnable> mRunnable;
      
      friend void* invoke(Thread*);
      friend void cleanup_handler(::itc::abstract::Cleanable*);

    public:

      explicit CancelableThread(const std::shared_ptr<TRunnable>& ref)
      : Thread(), isfinished(false), mRunnable(ref)
      {
        typedef ::itc::abstract::IRunnable RunnableInterface;
        static_assert(
          std::is_base_of<RunnableInterface, TRunnable>::value,
          "Wrong template parameter, - TRunnable is not derived from ::itc::abstract::IRunnable"
          );
        getLog()->debug(__FILE__, __LINE__, "Starting thread with Id: %jx", this->getThreadId());
        begin();
        getLog()->debug(__FILE__, __LINE__, "Started thread with Id: %jx", this->getThreadId());
      }

      const std::shared_ptr<TRunnable>& getRunnable() const
      {
        return mRunnable;
      }

      void run()
      {
        pthread_cleanup_push((void (*)(void*))cleanup_handler, this);
        if (TRunnable * ptr = mRunnable.get())
        {
          pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
          pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
          ptr->execute();
        }
        pthread_cleanup_pop(0);
      }

      void cleanup()
      {
        if (TRunnable * ptr = mRunnable.get())
        {
          ptr->onCancel();
        }
      }

      virtual ~CancelableThread() noexcept // gcc 4.7.4 compat
      {
        if(!isfinished) // destructor double call from std::shared_ptr. why ?????
        {
          isfinished=true;
          if (mRunnable.get() != nullptr)
          {
            mRunnable.get()->shutdown();
          }
          getLog()->debug(__FILE__, __LINE__, "Calling cancel() for TID: %jx", this->getThreadId());
          cancel();
          getLog()->debug(__FILE__, __LINE__, "Calling finish() for TID: %jx", this->getThreadId());
          finish();
          getLog()->debug(__FILE__, __LINE__, "finished TID: %jx", this->getThreadId());
          getLog()->flush();
        }
      }
    };
  }
}
#endif /*CLEANABLETHREAD_H_*/
