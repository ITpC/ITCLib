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
 * $Id: AtomicDigital.h 54 2007-05-22 08:36:14Z Pavel Kraynyukhov $
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 **/


#ifndef __ATOMIC_DIG_H__
#define __ATOMIC_DIG_H__
#include <pthread.h>
#include <sys/types.h>
#include <compat_types.h>
#include <sys/SyncLock.h>
#include <stdint.h>


namespace itc {
    namespace sys {

        template <typename DIG> class AtomicDigital {
        private:
            Mutex mMutex;
            DIG mDIG;
        public:

            explicit AtomicDigital(const DIG value = 0) : mMutex() {
                SyncLock sync(mMutex);
                mDIG = value;
            }

            explicit AtomicDigital(AtomicDigital& ref) : mMutex() {
                SyncLock syncL(mMutex);
                mDIG = DIG(ref);
            }

            DIG operator=(AtomicDigital& ref) {
                SyncLock sync(mMutex);
                return mDIG = DIG(ref);
            }

            inline int operator++() {
                SyncLock sync(mMutex);
                return ++mDIG;
            }

            inline int operator--() {
                SyncLock sync(mMutex);
                return --mDIG;
            }

            inline int operator++(int) //postfix operator
            {
                SyncLock sync(mMutex);
                register DIG ret = mDIG++;
                return ret;
            }

            inline int operator--(int) //postfix operator
            {
                SyncLock sync(mMutex);
                register DIG ret = mDIG++;
                return ret;
            }

            inline DIG operator+=(const DIG change) {
                SyncLock sync(mMutex);
                return (mDIG += change);
            }

            inline DIG operator-=(const DIG change) {
                SyncLock sync(mMutex);
                return (mDIG -= change);
            }

            inline DIG operator+(const DIG change) {
                SyncLock sync(mMutex);
                return mDIG + change;
            }

            inline DIG operator-(DIG change) {
                SyncLock sync(mMutex);
                return mDIG - change;
            }

            inline DIG operator=(const DIG change) {
                SyncLock sync(mMutex);
                return (mDIG = change);
            }

            inline bool operator!() {
                SyncLock sync(mMutex);
                return (!mDIG);
            }

            inline bool operator==(const DIG value) {
                SyncLock sync(mMutex);
                return (mDIG == value);
            }

            inline bool operator==(AtomicDigital& ref) {
                SyncLock syncL(mMutex);
                return (mDIG == DIG(ref));
            }

            inline bool operator<(AtomicDigital& ref) {
                SyncLock sync(mMutex);
                return (mDIG < DIG(ref));
            }

            inline bool operator<(const DIG value) {
                SyncLock sync(mMutex);
                return (mDIG < value);
            }

            inline bool operator>(AtomicDigital& ref) {
                SyncLock sync(mMutex);
                return (mDIG > DIG(ref));
            }

            inline bool operator>(const DIG value) {
                SyncLock sync(mMutex);
                return (mDIG > value);
            }

            inline bool operator>=(AtomicDigital& ref) {
                SyncLock sync(mMutex);
                return (mDIG >= DIG(ref));
            }

            inline bool operator>=(const DIG value) {
                SyncLock sync(mMutex);
                return (mDIG >= value);
            }

            inline bool operator<=(AtomicDigital& ref) {
                SyncLock sync(mMutex);
                return (mDIG <= DIG(ref));
            }

            inline bool operator<=(const DIG value) {
                SyncLock sync(mMutex);
                return (mDIG <= value);
            }

            inline bool operator!=(AtomicDigital& ref) {
                SyncLock sync(mMutex);
                return (mDIG != DIG(ref));
            }

            inline bool operator!=(const DIG value) {
                SyncLock sync(mMutex);
                return (mDIG != value);
            }

            inline operator DIG() {
                SyncLock sync(mMutex);
                return mDIG;
            }
        };

        typedef AtomicDigital<uint64_t> AtomicUInt64;
        typedef AtomicDigital<int64_t> AtomicInt64;
        typedef AtomicDigital<int32_t> AtomicInt32;
        typedef AtomicDigital<int> AtomicInt;
        typedef AtomicDigital<unsigned int> AtomicUInt;
        typedef AtomicDigital<long int> AtomicLInt;
        typedef AtomicDigital<long unsigned int> AtomicLUInt;
        typedef AtomicDigital<size_t> AtomicSizeT;
    }
}


#endif
