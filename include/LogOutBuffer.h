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
 * $Id: LogOutBuffer.h 56 2007-05-22 09:05:13Z Pavel Kraynyukhov $
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 **/

#ifndef __LOGOUTBUFFER_H__
#define __LOGOUTBUFFER_H__
#include <string>
#include <vector>

#include <sys/Mutex.h>
#include <sys/SyncLock.h>
#include <InterfaceCheck.h>
#include <abstract/LoggerHelpers.h>
#include <Val2Type.h>
#include <memory>

namespace itc {
    namespace utils {

        template <typename OutAdapter, bool TSafe = true > class LogOutBuffer {
        private:
            itc::sys::Mutex mMutex;
            size_t mMaxRows;
            std::shared_ptr<
                OutAdapter
            > mOutAdapter;
            size_t mMessagesInBuff;
            std::vector<std::string> mMessagesBuffer;
            itc::utils::Bool2Type<TSafe> mThreadSafe;

        public:

            typedef typename itc::utils::abstract::ILogOutputAdapter AbstractLogOutAdapter;

            explicit LogOutBuffer(std::shared_ptr<OutAdapter>& pOutAdapter, const size_t pMaxRows = 1000)
            : mMutex(), mMaxRows(pMaxRows), mOutAdapter(pOutAdapter), mMessagesInBuff(0) {
                STATIC_CHECKER3MSG(
                        CheckRelationship(
                        OutAdapter, subclassof, AbstractLogOutAdapter
                        ),
                        OutAdapter, _is_not_a_subclass_of_, AbstractLogOutAdapter
                        );
            }

            inline void post(const std::string& pLogMessage) {
                post(mThreadSafe, pLogMessage);
            }

            inline void post(const char* pLogMessage) {
                post(mThreadSafe, pLogMessage);
            }

            inline void flush() {
                flush(mThreadSafe); // for external calls or destructor call.
            }

            ~LogOutBuffer() {
                flush();
            }

        private:

            inline void post(itc::utils::Bool2Type < true > threadsafe, const char* pLogMessage) {
                itc::sys::SyncLock sync(mMutex);
                push(pLogMessage);
            }

            inline void post(itc::utils::Bool2Type < true > threadsafe, const std::string& pLogMessage) {
                itc::sys::SyncLock sync(mMutex);
                push(pLogMessage);
            }

            inline void post(itc::utils::Bool2Type < false > threadsafe, const char* pLogMessage) {
                push(pLogMessage);
            }

            inline void post(itc::utils::Bool2Type < false > threadsafe, const std::string& pLogMessage) {
                push(pLogMessage);
            }

            inline void flush(itc::utils::Bool2Type < true > threadsafe) {
                itc::sys::SyncLock sync(mMutex);
                pflush();
            }

            inline void flush(itc::utils::Bool2Type < false > threadsafe) {
                pflush();
            }

            inline void pflush() {
                size_t depth = mMessagesBuffer.size();

                for (size_t i = 0; i < depth; i++) {
                    mOutAdapter.get()->post(mMessagesBuffer[i]);
                }
                mMessagesBuffer.clear();
                mOutAdapter.get()->flush();
                mMessagesInBuff = 0;
            }

            inline void push(const std::string& pLogMessage) {
                if (!pLogMessage.empty()) {
                    if (mMessagesInBuff >= mMaxRows) {
                        itc::utils::Bool2Type < false > notThreadSafe;
                        flush(notThreadSafe);
                    }
                    mMessagesBuffer.push_back(pLogMessage);
                    mMessagesInBuff++;
                }
            }

            inline void push(const char* pLogMessage) {
                if (pLogMessage) {
                    std::string tmp(pLogMessage);

                    if (mMessagesInBuff >= mMaxRows) {
                        itc::utils::Bool2Type < false > notThreadSafe;
                        flush(notThreadSafe);
                    }
                    mMessagesBuffer.push_back(tmp);
                    mMessagesInBuff++;
                }
            }
        };
    }
}

#endif /*__LOGOUTBUFFER_H__*/
