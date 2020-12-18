/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: ILogFormatter.h 23 2010-11-23 12:53:35Z pk $
 * 
 * Author: Pavel Kraynyukhov <pavel.kraynyukhov@gmail.com>
 * 
 **/

#ifndef __ILOGFORMATTER_H__
#define __ILOGFORMATTER_H__

#include <stdarg.h>
#include <string>
#include <abstract/LoggerHelpers.h>

namespace itc
{
  namespace utils
  {
    namespace abstract
    {
      class ILogFormatter
      {
      public:
        explicit ILogFormatter() { }
        virtual const shared_char_vector format(const size_t pMaxMsgLength, const char* format, ...) = 0;
        virtual const shared_char_vector format(const size_t pMaxMsgLength, const char* format, va_list args) = 0;

        virtual ~ILogFormatter() { }
      };
    }
  }
}

#endif /*__ILOGFORMATTER_H__*/
