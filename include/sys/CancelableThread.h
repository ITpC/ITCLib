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

#include <assert.h>

#include <memory>
#include <atomic>

//#include <InterfaceCheck.h>
#include <abstract/Runnable.h>
#include <sys/Thread.h>
#include <abstract/Cleanable.h>
#include <itc_log_defs.h>

namespace itc
{
  namespace sys
  {

    template <typename TRunnable> class CancelableThread : public Thread, public ::itc::abstract::Cleanable
    {
    public:
      typedef std::weak_ptr<TRunnable> RunnableWeakPtr;
      
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
        begin();
      }

      CancelableThread(const CancelableThread&) = delete;
      CancelableThread(CancelableThread&) = delete;
      
      const std::shared_ptr<TRunnable>& getRunnable() const
      {
        return mRunnable;
      }

      void run() final
      {
        pthread_cleanup_push((void (*)(void*))cleanup_handler, this);
        if (mRunnable.get() != nullptr)
        {
          pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
          pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
          try{
            mRunnable.get()->execute();
          }catch(const std::exception& e)
          {
            ITC_ERROR(__FILE__,__LINE__,"Exception in itc::abstract::IRunnable::execute(): %s",e.what());
          }
        }
        pthread_cleanup_pop(0);
      }

      void cleanup() final
      {
        if(!isfinished.load())
        {
          isfinished.store(true);
          auto ptr{mRunnable.get()};
          if ( ptr != nullptr)
          {
            ptr->onCancel();
            mRunnable.reset();
          }
        }
      }

      ~CancelableThread() 
      {
        if(!isfinished.load())
        {
          try{
            cleanup();
          }catch(const std::exception& e)
          {
            ITC_ERROR(__FILE__,__LINE__,"Exception in itc::abstract::IRunnable::~Specific_Destructor(): %s",e.what());
          }
          cancel();
          finish();
        }
      }
    };
  }
}
#endif /*CLEANABLETHREAD_H_*/
