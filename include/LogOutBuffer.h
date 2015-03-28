/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: LogOutBuffer.h 56 2007-05-22 09:05:13Z pk $
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 **/

#ifndef __LOGOUTBUFFER_H__
#define __LOGOUTBUFFER_H__
#include <string>
#include <vector>

#include <InterfaceCheck.h>
#include <abstract/LoggerHelpers.h>
#include <Val2Type.h>
#include <memory>
#include <mutex>

namespace itc {
    namespace utils {

        template <typename OutAdapter, bool TSafe = true > class LogOutBuffer {
        private:
            std::mutex mMutex;
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
                std::lock_guard<std::mutex> sync(mMutex);
                push(pLogMessage);
            }

            inline void post(itc::utils::Bool2Type < true > threadsafe, const std::string& pLogMessage) {
                std::lock_guard<std::mutex> sync(mMutex);
                push(pLogMessage);
            }

            inline void post(itc::utils::Bool2Type < false > threadsafe, const char* pLogMessage) {
                push(pLogMessage);
            }

            inline void post(itc::utils::Bool2Type < false > threadsafe, const std::string& pLogMessage) {
                push(pLogMessage);
            }

            inline void flush(itc::utils::Bool2Type < true > threadsafe) {
                std::lock_guard<std::mutex> sync(mMutex);
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
