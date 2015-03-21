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
 * $Id: Mutex.h 101 2007-08-12 13:07:51Z acs $
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 **/

#ifndef __MUTEX_H__
#define __MUTEX_H__
#include <pthread.h>

#include <ITCException.h>

typedef pthread_mutex_t* pthread_mutex_ptr;

namespace itc {
namespace sys {

template <
        int mutex_attr = PTHREAD_MUTEX_DEFAULT
>class RawMutex {
private:
    pthread_mutex_t mAtomicLock;
public:
    explicit RawMutex()
    {
        if (mutex_attr == PTHREAD_MUTEX_DEFAULT)
        {
            if (int ret = pthread_mutex_init(&mAtomicLock, NULL))
            {
                throw TITCException<exceptions::Can_not_create_mutex>(ret);
            }
        }
        else
        {
            pthread_mutexattr_t aMutexAttr;
            pthread_mutexattr_init(&aMutexAttr);
            pthread_mutexattr_settype(&aMutexAttr, mutex_attr);
            if (int ret = pthread_mutex_init(&mAtomicLock, &aMutexAttr))
            {
                throw ITCException(ret, exceptions::Can_not_create_mutex);
            }
        }
    }

    explicit RawMutex(const RawMutex& ref)=delete;
    explicit RawMutex(RawMutex& ref)=delete;

    ~RawMutex() {
        destroy();
    }

    inline void destroy() {
        /**
         * IEEE Std 1003.1-2001 specification says:
         * Implementations are required  to allow an object to be destroyed and freed
         * and potentially unmapped immediately after the object is unlocked.
         *
         * hope your system follow the specification.
         *
         * glibc-2.4 returns EBUSY if mutex locked, let use this adventage.
         *
         * however i have no idea if this behavior will persist in later glibc releases.
         * seems to be not very clever to use it ...
         *
         **/
#ifdef __linux__
        do {
            pthread_mutex_lock(&mAtomicLock);
            pthread_mutex_unlock(&mAtomicLock);
        } while (pthread_mutex_destroy(&mAtomicLock) == EBUSY);
#else
        pthread_mutex_lock(&mAtomicLock);
        pthread_mutex_unlock(&mAtomicLock);
        pthread_mutex_destroy(&mAtomicLock);
#endif
    }

    inline void Lock()
    {
        if (int ret = pthread_mutex_lock(&mAtomicLock))
        {
            throw TITCException<exceptions::Can_not_lock_mutex>(ret);
        }
    }

    inline void unLock()
    {
        if (int ret = pthread_mutex_unlock(&mAtomicLock))
        {
            throw TITCException<exceptions::Can_not_unlock_mutex>(ret);
        }
    }

    inline bool tryLock() {
        if (int ret = pthread_mutex_trylock(&mAtomicLock))
        {
            if (ret != EBUSY) {
                throw TITCException<exceptions::Can_not_lock_mutex>(ret);
            }
            return false;
        }
        return true;
    }

    inline pthread_mutex_ptr getMutexPtr() {
        return (&mAtomicLock);
    }
};

typedef RawMutex<PTHREAD_MUTEX_DEFAULT> Mutex;
typedef RawMutex<PTHREAD_MUTEX_RECURSIVE> RecursiveMutex;
typedef RawMutex<PTHREAD_MUTEX_ERRORCHECK> SmartMutex;
typedef RawMutex<PTHREAD_MUTEX_NORMAL> NormalMutex;

}
}

#endif
