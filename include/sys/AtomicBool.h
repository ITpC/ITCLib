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
 * $Id: AtomicBool.h 54 2007-05-22 08:36:14Z Pavel Kraynyukhov $
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 **/

#ifndef ATOMICBOOL_H_
#define ATOMICBOOL_H_

#include <sys/Mutex.h>
#include <sys/SyncLock.h>

namespace itc
{
    namespace sys
    {
        class AtomicBool
        {
        private:
            Mutex   mMutex;
            bool    mBool;
            
        public:
            explicit AtomicBool(const bool val=false) : mMutex()
            {
                SyncLock syncL(mMutex);
                mBool=val;
            }
            
            explicit  AtomicBool(AtomicBool& ref):mMutex()
            {
                SyncLock syncL(mMutex);
                mBool=bool(ref);
            }
            
            bool operator=(AtomicBool& ref)
            {
                SyncLock syncL(mMutex);
                return mBool=bool(ref);
            }
            
            bool operator=(const bool val)
            {
                SyncLock sync(mMutex);
                return mBool=val;
            }
            
            inline operator bool()
            {
                SyncLock sync(mMutex);
                return mBool;
            }

            const bool operator==(const bool val)
            {
                SyncLock sync(mMutex);
                return mBool==val;
            }
            
            const bool operator!=(const bool val)
            {
                SyncLock sync(mMutex);
                return mBool!=val;
            }
        };
    }
}


#endif /*ATOMICBOOL_H_*/
