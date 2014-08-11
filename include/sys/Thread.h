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
 * $Id: Thread.h 101 2007-08-12 13:07:51Z acs $
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 **/

#ifndef	__THREAD_H__
#define	__THREAD_H__

#include <pthread.h>
#include <sched.h>

#include <memory>

#include <ITCException.h>
#include <sys/Types.h>

#include <abstract/Cleanable.h>
#include <abstract/Runnable.h>
#include <sys/Semaphore.h>
#include <sys/prototypes.h>
#include <InterfaceCheck.h>

#include <TSLog.h>



namespace itc {
    namespace sys {
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
        class Thread {
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
            Thread() : start() {
                register int ret = this->create();
                if (ret) throw ITCException(ret, exceptions::Can_not_create_thread);
            }

            inline int create() {
                return ::pthread_create(&TID, NULL, (thread_t) invoke, this);
            }

            inline int send_signal(int signo) {
                return ::pthread_kill(TID, signo);
            }

            inline int cancel() {
                return ::pthread_cancel(TID);
            }

            inline int detach() {
                return ::pthread_detach(TID);
            }

            inline pthread_t getThreadId() const {
                return TID;
            }

            inline pthread_t getCurrentThrId() {
                return ::pthread_self();
            }

            inline void yield() const {
                YIELD();
            }



        protected:
            virtual ~Thread() = 0;
            virtual void run() = 0;

            inline void begin() {
                start.post();
            }

            inline int finish() {
                return ::pthread_join(TID, NULL);
            }
        };
    }
}

#endif
