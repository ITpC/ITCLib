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
#include <sys/synclock.h>


namespace itc
{
  namespace utils
  {
    
    class StdTextLogFormatter : public itc::utils::abstract::ILogFormatter
    {
    private:
      std::mutex mMutex;

    public:

      
      explicit StdTextLogFormatter()
      : ILogFormatter(), mMutex()
      {
        STDSyncLock sync(mMutex);
      }

      const shared_char_vector format(const size_t pMaxMsgLength, const char* pFormat, ...)
      {
        STDSyncLock sync(mMutex);

        if (pFormat)
        {
          auto tmp=std::make_shared<std::vector<char>>(pMaxMsgLength + 1, 0);

          va_list args;
          va_start(args, pFormat);
          vsnprintf(tmp.get()->data(), pMaxMsgLength, pFormat, args);
          va_end(args);
          return tmp;
        }
        return std::make_shared<std::vector<char>>(0);
      }

      const shared_char_vector format(const size_t pMaxMsgLength, const char* pFormat, va_list args)
      {
        STDSyncLock sync(mMutex);

        if (pFormat)
        {
          auto tmp=std::make_shared<std::vector<char>>(pMaxMsgLength + 1, 0);
          vsnprintf(tmp.get()->data(), pMaxMsgLength, pFormat, args);
          return tmp;
        }
        return std::make_shared<std::vector<char>>(0);
      }
    };
  }
}
#endif /*__STDTEXTLOGFORMATTER_H__*/
