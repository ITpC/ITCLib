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
 * $Id:$
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 **/

#ifndef TSAFELOGOUTADAPTER_H_
#define TSAFELOGOUTADAPTER_H_

#include <sys/Mutex.h>
#include <cstdio>

namespace itc
{
   
    namespace utils
    {
        class STDOutLogThreadSafeAdapter : public itc::utils::abstract::ILogOutputAdapter
        {
        private:
            sys::Mutex          mMutex;
            FILE*               mLogFile;
            
        public:
            
            explicit STDOutLogThreadSafeAdapter(const char* filename, const char* wmode="a")
            : itc::utils::abstract::ILogOutputAdapter(filename), mMutex(),mLogFile(NULL)
            {
                itc::sys::SyncLock synch(mMutex);
                mLogFile=fopen(filename,wmode);
                if(mLogFile==NULL)
                {
                    throw itc::utils::CanNotOpenTheLogException();
                }
            }
            
            explicit STDOutLogThreadSafeAdapter(const STDOutLogThreadSafeAdapter& p)
            :   itc::utils::abstract::ILogOutputAdapter(NULL)
            {
                itc::sys::SyncLock synch(mMutex);
                mLogFile=p.mLogFile;
            }
            
            void post(const std::string& pMessage)
            {
                itc::sys::SyncLock sync(mMutex);
                fprintf(mLogFile,"%s",pMessage.c_str());
            }
            
            inline void flush()
            {
                itc::sys::SyncLock sync(mMutex);
                fflush(mLogFile); 
            }
            
            ~STDOutLogThreadSafeAdapter()
            {
                flush();
            }
        };
    }
}

#endif /*TSAFELOGOUTADAPTER_H_*/
