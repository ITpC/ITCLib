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
 * $Id: Conditional.h 54 2007-05-22 08:36:14Z Pavel Kraynyukhov $
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 **/


#ifndef __CONDITIONAL__H__
#define __CONDITIONAL__H__
#include <pthread.h>
#include <sys/time.h>

#include <sys/Types.h>
#include <sys/SyncLock.h>
#include <sys/Mutex.h>


namespace itc
{
    namespace sys
    {
        /**
         *@name Conditional is a wrapper class manipulating with pthread_cond_t type
         */

        class Conditional
        {
        private:
            pthread_cond_t  mCondition;
            Mutex           mMutex;
        public:
            Conditional() : mMutex()
            { 
                SyncLock sync(mMutex);
                register int ret=pthread_cond_init(&mCondition,NULL);
                if(ret) throw ITCException(ret,exceptions::Can_not_create_conditional);
            }

            inline int wait()
            { 
                SyncLock sync(mMutex);
                return pthread_cond_wait(&mCondition,mMutex.getMutexPtr());
            }

            inline int tryWait(time_t sec, long nsec)
            {
                SyncLock sync(mMutex);
                
                struct timespec anAbstime;
                struct timeval now;
                
                gettimeofday(&now,NULL);
                
                
                anAbstime.tv_nsec=now.tv_usec*1000UL+nsec;
                uint64_t addsec=uint64_t(anAbstime.tv_nsec / 1000000000UL);
                anAbstime.tv_sec=now.tv_sec+sec+addsec;
                anAbstime.tv_nsec -= addsec*1000000000;
                return pthread_cond_timedwait(&mCondition,mMutex.getMutexPtr(),&anAbstime);
            }

            inline int broadcast()
            { 
                return pthread_cond_broadcast(&mCondition);
            }

            inline int signal()
            { 
                return pthread_cond_signal(&mCondition);
            }

            /**
             * is an alias to Conditional::signal()
             **/
            inline int post()
            {
                return this->signal();
            }

            ~Conditional()
            { 
                SyncLock sync(mMutex);
                pthread_cond_destroy(&mCondition);
            }
        };
    }
}
#endif
