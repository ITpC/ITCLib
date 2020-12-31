/**
 *  Copyright 2018, Pavel Kraynyukhov <pavel.kraynyukhov@gmail.com>
 * 
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 *  $Id: default_formatter.h December 27, 2020 5:33 AM $
 * 
 **/

#ifndef __DEFAULT_FORMATTER_H__
#  define __DEFAULT_FORMATTER_H__

#include <chrono>
#include <abstract/ILogFormatter.h>
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <string_view>
#include <string>
#include <iostream>

namespace flog
{
  class default_formatter:public flog::ILogFormatter
  {
    
  protected:
    virtual std::string getfmt(flog::LogLevel msgtype, std::string_view subfmt)
    {
      auto now = std::chrono::system_clock::now();
      auto seconds=std::chrono::duration_cast<std::chrono::seconds>(
          std::chrono::time_point_cast<std::chrono::seconds>(now).time_since_epoch()
        ).count();

        size_t msec=std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch()
        ).count()-(seconds*1000);

      auto a_time_t = std::chrono::system_clock::to_time_t(now);

      auto prefmt{
        fmt::format(
          "{}{:03d}Z [{}:{}] {}\n",
          fmt::format("{:%Y%m%d%H%M%S}.",fmt::localtime(a_time_t)),
          msec,LOG_LEVEL_STR(),LOG_MSG_TYPE(msgtype),subfmt
        )
      };
      return prefmt;
    }
  };
}

#endif /* __DEFAULT_FORMATTER_H__ */

