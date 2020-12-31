/**
 *  Copyright 2020, Pavel Kraynyukhov <pavel.kraynyukhov@gmail.com>
 * 
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 *  $Id: ILogFormatter.h December 27, 2020 3:01 AM $
 * 
 **/

#ifndef __ILOGFORMATTER_H__
#  define __ILOGFORMATTER_H__

#include <flog_common.h>
#include <string_view>
#include <chrono>
#include <fmt/core.h>
#include <string_view>

namespace flog
{
  class ILogFormatter
  {
   private:
   public:
    ILogFormatter()=default;
    template <typename ... Args> auto format(flog::LogLevel msgtype, std::string_view subfmt, Args&& ... args)
    {
      return fmt::format(getfmt(msgtype,subfmt),std::forward<Args>(args)...);
    }
   protected:
    virtual std::string getfmt(flog::LogLevel,std::string_view)=0;
    virtual ~ILogFormatter()=default;
  };
}

#endif /* __ILOGFORMATTER_H__ */

