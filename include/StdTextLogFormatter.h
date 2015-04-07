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

#include <abstract/ILogFormatter.h>


namespace itc
{
    namespace utils
    {
        class StdTextLogFormatter : public itc::utils::abstract::ILogFormatter
        {
        private:
            std::mutex     mMutex;
            std::string    mPreparedRecord;
            
        public:
            explicit StdTextLogFormatter()
            : ILogFormatter(),mMutex()
            {
                std::lock_guard<std::mutex> sync(mMutex);
            }
            
            inline void format(const size_t pMaxMsgLength, const char* pFormat, ...)
            {
                std::lock_guard<std::mutex> sync(mMutex);
                
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
                std::lock_guard<std::mutex> sync(mMutex);
                
                if(pFormat)
                {
                    std::unique_ptr<char> buf(new char[pMaxMsgLength +1]);
                    if(buf.get())
                    {
                      vsnprintf(buf.get(),pMaxMsgLength,pFormat,args);
                      mPreparedRecord=buf.get();
                    }
                }
            }

            inline const std::string& getFormattedMessage()
            {
                std::lock_guard<std::mutex> sync(mMutex);
                return mPreparedRecord;
            }
        };
    }
}
#endif /*__STDTEXTLOGFORMATTER_H__*/
