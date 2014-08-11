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
 * $Id: ITCSemaphore.h 101 2007-08-12 13:07:51Z acs $
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 **/


#ifndef __ITCSEMAPHORE_H__
#define __ITCSEMAPHORE_H__

#include <errno.h>

#include <sys/Mutex.h>
#include <sys/SyncLock.h>
#include <sys/prototypes.h>
#include <sys/AtomicDigital.h>
#include <sys/AtomicBool.h>
#include <sys/Nanosleep.h>
#include <ITCException.h>
#include <iostream>

namespace itc {
    namespace sys {

        /**
         * Simulates a semaphore using a Mutex. Its differ to the RawPosixSemaphore or PosixSemaphore defined 
         * for platforms other than Linux and Solaris in performance/safety tradeoff behavior. The favor given
         * to the safety(However bugs are sommewhere here ...). No undefined behavior by destroying. No possibility
         * to use uninitialized semaphore.
         * ITCSemaphore should be absolutelly safe (if no bugs in this realisation are present)
         * usleep used here to suspend threads execution on wait and on destroy. 
         * Solution is unportable, unperformant and bugy as well.
         *  
         * @exception ITCException(Can_not_init_semaphore,EINVAL)
         * @exception ITCException(Can_not_get_semaphore_value, EINVAL)
         * @exception ITCException(Can_not_wait_on_semaphore, EINVAL)
         * @exception Propagate all Mutex exceptions further if any.
         *  
         **/
        class ITCSemaphore {
        private:
            Mutex mMutex;
            Sleep mTimer;
            AtomicSizeT sem_val;
            AtomicInt on_wait_count;
            AtomicBool sem_ok;

        public:

            explicit ITCSemaphore(const size_t pSemVal = 0) {
                SyncLock sync(mMutex);
                sem_val = pSemVal;
                on_wait_count = 0;
                sem_ok = true;
            }

            explicit ITCSemaphore(const ITCSemaphore& ref) {
                throw ITCException(EINVAL, exceptions::Can_not_copy_semaphore);
            }

            /**
             * return sem_val ifit is greater then 0, on_wait_count otherwise or throws an exception 
             * if semaphore is invalid.
             * 
             * @exception ITCException(Can_not_get_semaphore_value, EINVAL)
             **/

            inline int getValue(void) {
                if (sem_ok) {
                    return ( sem_val > size_t(0) ? (int) sem_val : (int) on_wait_count);
                } else throw ITCException(EINVAL, exceptions::Can_not_get_semaphore_value);
            }

            /**
             * increase sem_val if it is valid throws an exception otherwise.
             * 
             **/
            inline void post() {
                if (sem_ok) {
                    sem_val++;
                    return;
                }
                throw ITCException(EINVAL, exceptions::Can_not_post_on_semaphore);
            }

            inline void tryWait() {
                AtomicBool cantry(false);
                {
                    SyncLock sync(mMutex);
                    if (sem_ok && sem_val) {
                        cantry = true;
                    }
                }
                if (cantry) {
                    wait();
                }
            }

            /**
             * Decrease sem_val if semaphore valid
             **/
            inline void wait() {
                { // stack in
                    SyncLock sync(mMutex);
                    if (sem_ok) {
                        if (sem_val) {
                            sem_val--;
                            return;
                        }
                        on_wait_count++;
                    } else throw ITCException(EINVAL, exceptions::Can_not_wait_on_semaphore);
                } // stack out
                // lets start waiting ...
                while (sem_ok) {
                    {
                        SyncLock sync(mMutex);
                        if (sem_val) {
                            sem_val--;
                            on_wait_count--; // indicate no thread wait 
                            return; // semaphore value has been increased by post 
                        }
                    }
                    mTimer.usleep(10); // TODO: what a fucking constants ?????? 
                }
                on_wait_count--; // indicate no thread wait before throw
                throw ITCException(EINVAL, exceptions::Can_not_wait_on_semaphore);
            }

            ~ITCSemaphore() {
                destroy();
            }

        private:

            inline void destroy() {
                if (!sem_ok) {
                    return;
                }

                sem_ok = false;

                while (on_wait_count) // we do'nt need a segmentation fault, so we should wait untill no threads
                {                     // are waiting on semaphore
                    sem_val++;
                }
            }
        };
    }
}



#endif /* __ITCSEMAPHORE_H__ */
