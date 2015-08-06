/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: StdTextLogFormatter.h 56 2007-05-22 09:05:13Z pk$
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 **/

#ifndef __STDTEXTLOGFORMATTER_H__
#define __STDTEXTLOGFORMATTER_H__

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <string>
#include <memory>
#include <mutex>
#include <vector>

#include <abstract/ILogFormatter.h>


namespace itc
{
    namespace utils
    {
        class StdTextLogFormatter : public itc::utils::abstract::ILogFormatter
        {
        private:
            std::mutex           mMutex;
            std::shared_ptr<std::vector<char>>    mPreparedRecord;
            
        public:
            explicit StdTextLogFormatter()
            : ILogFormatter(),mMutex(),mPreparedRecord(std::make_shared<std::vector<char>>(0))
            {
                std::lock_guard<std::mutex> sync(mMutex);
            }
            
            inline void format(const size_t pMaxMsgLength, const char* pFormat, ...)
            {
                std::lock_guard<std::mutex> sync(mMutex);
                
                if(pFormat)
                {
                  mPreparedRecord->resize(pMaxMsgLength+1,0);
                  memset(mPreparedRecord->data(),0,mPreparedRecord->size());

                  va_list args;
                  va_start(args,pFormat);
                  vsnprintf(mPreparedRecord->data(),pMaxMsgLength,pFormat,args);
                  va_end(args);
                }
            }

            void format(const size_t pMaxMsgLength, const char* pFormat, va_list args)
            {
                std::lock_guard<std::mutex> sync(mMutex);
                
                if(pFormat)
                {
                  mPreparedRecord->resize(pMaxMsgLength+1,0);
                  memset(mPreparedRecord->data(),0,mPreparedRecord->size());
                  vsnprintf(mPreparedRecord->data(),pMaxMsgLength,pFormat,args);
                }
            }

            const std::shared_ptr<std::vector<char>>& getFormattedMessage()
            {
                std::lock_guard<std::mutex> sync(mMutex);
                return mPreparedRecord;
            }
        };
    }
}
#endif /*__STDTEXTLOGFORMATTER_H__*/
