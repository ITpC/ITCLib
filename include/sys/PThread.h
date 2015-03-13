/**
 * Copyright (c) 2007, Pavel Kraynyukhov.
 *  
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without a written agreement
 * is hereby granted under the terms of the General Public License version 2
 * (GPLv2), provided that the above copyright notice and this paragraph and the
 * following two paragraphs and the "LICENSE" file appear in all modified or
 * unmodified copies of the software "AS IS" and without any changes.
 *
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING
 * LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
 * DOCUMENTATION, EVEN IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATIONS TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 * 
 * 
 * $Id: CancelableThread.h 27 2011-02-06 13:25:43Z pk $
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 **/
#ifndef __PCTHREAD_H__
#   define __PCTHREAD_H__

#   include <memory>

#   include <InterfaceCheck.h>
#   include <abstract/Runnable.h>

#   include <sys/Thread.h>
#   include <sys/Semaphore.h>
#   include <abstract/Cleanable.h>
#   include <abstract/Runnable.h>
#   include <sys/Mutex.h>
#   include <sys/SyncLock.h>
#   include <TSLog.h>

namespace itc
{
    enum PThreadState {STARTING,RUNNING,DONE,CANCEL};

    namespace sys
    {
        /**
         * @brief Implemantation of the thread intended to be used with the 
         * ThreadPool. It executes a Runnable (implementation of the abstract::Runnable)
         * and stays on hold until the Runnable object is assigned with
         * setRunnable(). The the instance of this class, keeps tracking the
         * state of the executuion as defined in PThreadState.
         * The objective of the PThread is to run a task onece and be returned to
         * the ThreadPool.
         * 
         * Do not use this class to implement long running threads, those have 
         * implemented an endless loop inside the Runnable object.
         *
         * See itc::sys::CancelableThread if you wish to implement independent
         * thread class, which is running outside of the ThreadPool and whos
         * lifecycle is determined by your purposes.
         * 
         **/
        class PThread : public Thread, abstract::Cleanable
        {
        public:
            typedef abstract::IRunnable* RunnablePointer;
            typedef std::shared_ptr<abstract::IRunnable> TaskType;

        private:
            friend void* invoke(Thread*);
            friend void cleanup_handler(abstract::Cleanable*);

            Mutex        mMutex;
            TaskType  mRunnable;
            Semaphore     mTask;
            PThreadState mState;

            inline void setState(PThreadState state)
            {
                SyncLock synchronize(mMutex);
                mState=state;
            }
            
            inline bool ok2Run()
            {
                mTask.wait();
                if(getState()!=CANCEL)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }

            inline void mainLoop()
            {
                while(ok2Run())
                {
                    ::itc::getLog()->debug(__FILE__, __LINE__, "Thread [%jx] -> PThread::mainLoop()", this->getThreadId());
                    SyncLock synchronize(mMutex);
                    if(mState!=CANCEL) // double check
                    {
                        mState=RUNNING;
                        if (RunnablePointer ptr = mRunnable.get())
                        {
                            ptr->execute();
                        }
                        mState=DONE;                        
                    }
                    ::itc::getLog()->debug(__FILE__, __LINE__, "Thread [%jx] <- PThread::mainLoop()", this->getThreadId());
                }
            }

        public:
            explicit PThread()
            : Thread(), mMutex(),mState(STARTING)
            {
                ::itc::getLog()->debug(__FILE__, __LINE__, "Thread [%jx] ->  PThread::Pthread()", this->getCurrentThrId());
                SyncLock synchronize(mMutex);
                begin();
                ::itc::getLog()->debug(__FILE__, __LINE__, "Thread [%jx] <-  PThread::Pthread()", this->getCurrentThrId());;
            }

            inline PThreadState getState()
            {
                SyncLock synchronize(mMutex);
                return mState;
            }

            inline TaskType getRunnable()
            {
                SyncLock synchronize(mMutex);
                return mRunnable;
            }

            inline void setRunnable(const TaskType& ref)
            {
                SyncLock synchronize(mMutex);
                mRunnable=ref;
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
                SyncLock synchronize(mMutex);
                if (RunnablePointer ptr = mRunnable.get())
                {
                    ptr->onCancel();
                }
            }

            virtual ~PThread() noexcept // be 4.7.x compat
            {
                setState(CANCEL);
                mTask.post();
                ::itc::getLog()->debug(__FILE__, __LINE__, "Thread [%jx] calling PThread::~Pthread() for thread [%jx]",this->getCurrentThrId(),this->getThreadId());
                if (RunnablePointer ptr = mRunnable.get())
                {
                    ptr->shutdown();
                }
                ::itc::getLog()->debug(__FILE__, __LINE__, "Thread [%jx], PThread::~Pthread() -> Calling cancel() for thread: [%jx]", this->getCurrentThrId(), this->getThreadId());
                mTask.post();
                cancel();
                ::itc::getLog()->debug(__FILE__, __LINE__, "Thread [%jx], PThread::~Pthread() -> Calling finish() for thread: [%jx]", this->getCurrentThrId(), this->getThreadId());
                mTask.post();
                finish();
                ::itc::getLog()->debug(__FILE__, __LINE__, "Thread [%jx], PThread::~Pthread() -> finished thread: [%jx]", this->getCurrentThrId(), this->getThreadId());
            }
        };
    }
}
#endif /*__PCTHREAD_H__*/
