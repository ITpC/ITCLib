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
 * $Id: StdTextLogFormatter.h 56 2007-05-22 09:05:13Z Pavel Kraynyukhov $
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 **/

#ifndef __STDTEXTLOGFORMATTER_H__
#define __STDTEXTLOGFORMATTER_H__

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <string>
#include <memory>

#include <sys/Mutex.h>
#include <sys/SyncLock.h>
#include <abstract/ILogFormatter.h>


namespace itc
{
    namespace utils
    {
        class StdTextLogFormatter : public itc::utils::abstract::ILogFormatter
        {
        private:
            itc::sys::Mutex     mMutex;
            std::string         mPreparedRecord;
            
        public:
            explicit StdTextLogFormatter()
            : ILogFormatter(),mMutex()
            {
                itc::sys::SyncLock sync(mMutex);
            }
            
            inline void format(const size_t pMaxMsgLength, const char* pFormat, ...)
            {
                itc::sys::SyncLock sync(mMutex);
                
                if(pFormat)
                {
                    std::unique_ptr<char> buf(new char[pMaxMsgLength+1]);
                    memset(buf.get(),0,pMaxMsgLength);
        
                    va_list args;
                    va_start(args,pFormat);
                    vsnprintf(buf.get(),pMaxMsgLength,pFormat,args);
                    va_end(args);
                    mPreparedRecord=buf.get();
                }
            }

            inline void format(const size_t pMaxMsgLength, const char* pFormat, va_list args)
            {
                itc::sys::SyncLock sync(mMutex);
                
                if(pFormat)
                {
                    std::unique_ptr<char> buf(new char[pMaxMsgLength +1]);
                    vsnprintf(buf.get(),pMaxMsgLength,pFormat,args);
                    mPreparedRecord=buf.get();
                }
            }

            inline const std::string& getFormattedMessage()
            {
                itc::sys::SyncLock sync(mMutex);
                return mPreparedRecord;
            }
        };
    }
}
#endif /*__STDTEXTLOGFORMATTER_H__*/
