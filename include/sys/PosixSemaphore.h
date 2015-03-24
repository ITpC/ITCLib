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
 * $Id: PosixSemaphore.h 101 2007-08-12 13:07:51Z acs $
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 **/

#ifndef __PosixSemaphore_H__
#    define __PosixSemaphore_H__

#    include <errno.h>
#    include <signal.h>


#    include <pthread.h>
#    include <semaphore.h>
#    include <errno.h>
#    include <time.h>
#    include <string.h>

#    include <ITCException.h>
#    include <sys/Types.h>
#    include <sys/AtomicBool.h>
#    include <sys/AtomicDigital.h>
#    include <sys/prototypes.h>
#    include <GAINTLock.h>

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
     *  There is no code for semaphore invalidation. conclusion - sux.
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

#   if defined(_MSC_VER) && (defined(__MINGW32_VERSION))
#define __PTPS__
    /**
     * Like RawPosixSemaphore. Should be used with pthread for win32 (best pthread 
     * realisation ever). 
     * 
     **/

    class PosixSemaphore
    {
    private:
        sem_t semaphore;
    public:

        explicit PosixSemaphore(ulong def_val = 0)
        {
            if (sem_init(&semaphore, 0, def_val))
                throw ITCException(errno, exceptions::Can_not_initialize_semaphore);
        }

        explicit PosixSemaphore(const PosixSemaphore& ref)
        {
            // seak idea to make a copy of semaphore.
            throw ITCException(errno, exceptions::Can_not_copy_semaphore);
        }

        inline void wait(void)
        {
            if (sem_wait(&semaphore) == -1)
            {
                throw ITCException(exceptions::Can_not_wait_on_semaphore, EINVAL);
            }
        }

        inline void timedWait(const ::timespec& timeout)
        {
            if (sem_timedwait(&semaphore, &timeout) == -1)
            {
                throw ITCException(exceptions::Can_not_wait_on_semaphore, errno);
            }
        }

        inline int tryWait(void)
        {
            register int ret = sem_trywait(&semaphore);

            if (ret == -1)
            {
                if (errno == EAGAIN)
                {
                    return EAGAIN;
                }
                throw ITCException(EINVAL, exceptions::Can_not_wait_on_semaphore);
            }

            return 0;
        }

        inline void post(void)
        {
            if (sem_post(&semaphore) == -1)
            {
                throw ITCException(exceptions::Can_not_wait_on_semaphore, errno);
            }
        }

        inline int getValue(void)
        {
            register int value; // is on stack ... should be thread safe ...

            if (sem_getvalue(&semaphore, &value))
            {
                throw ITCException(exceptions::Can_not_get_semaphore_value, errno);
            }

            return value;
        }

        inline void destroy()
        {
            while ((sem_destroy(&semaphore) == -1) && (errno == EBUSY))
            {
                int value = 0;
                if (sem_getvalue(&semaphore, &value) != -1)
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
#    else
#define __RPS__
    /**
     * This class is safe to use with the itc::sys::CancelableThread class. 
     * It is simple raw POSIX semaphores wrapper. Without pthread_cancel
     * you can expirience undefined behavior by destroying instances of this
     * class when other threads waiting on this semaphore.
     * 
     **/
    class RawPosixSemaphore
    {
    private:
        sem_t semaphore;
        bool ok;
    public:

        explicit RawPosixSemaphore(ulong def_val = 0) : ok(true)
        {
            if (sem_init(&semaphore, 0, def_val))
                throw ITCException(errno, exceptions::Can_not_initialize_semaphore);
        }

        explicit RawPosixSemaphore(const RawPosixSemaphore&)=delete;
        explicit RawPosixSemaphore(RawPosixSemaphore&)=delete;


        inline void wait(void)
        {
            if (sem_wait(&semaphore))
            {
                throw ITCException(errno, exceptions::Can_not_wait_on_semaphore);
            }
        }

        inline void post(void)
        {
            if (sem_post(&semaphore))
            {
                throw TITCException<exceptions::Can_not_post_on_semaphore > (errno);
            }
        }

        inline void timedWait(const ::timespec& timeout)
        {
            if (sem_timedwait(&semaphore, &timeout))
            {
                throw ITCException(errno, exceptions::Can_not_wait_on_semaphore);
            }
            return;
        }

        inline void tryWait(void)
        {
            if (sem_trywait(&semaphore))
            {
                throw ITCException(errno, exceptions::Can_not_wait_on_semaphore);
            }
        }

        inline int getValue(void)
        {
            register int value;

            if (sem_getvalue(&semaphore, &value))
            {
                throw ITCException(errno, exceptions::Can_not_get_semaphore_value);
            }
            return value;
        }

        /**
         * In Linux and Solaris a cancelation has to be called before semaphore will be destroyed. 
         * if semaphore is destroyed already you wont get any exceptions thrown.
         * 
         * @exception  ITCException(errno,exceptions::Can_not_destroy_semaphore);
         **/
        inline void destroy()
        {
            GAINTLock sync; // Using GAINTLock to avoid race condition.
            // Yes i know destroing of semaphores will be slow.
            // However it will be safe ! Any other safe solutions are wellcome
            if (ok)
            {
                ok = false;
                if (sem_destroy(&semaphore))
                {
                    throw ITCException(errno, exceptions::Can_not_destroy_semaphore);
                }
            }
        }

        ~RawPosixSemaphore()
        {
            destroy();
        }
    };
#   endif

#if defined (__RPS__)
  typedef RawPosixSemaphore Semaphore;
#else 
# if defined (__PTPS__)
    typedef PosixSemaphore Semaphore;
# endif
#endif
  }
}



#endif
