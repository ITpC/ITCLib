/**
 *  Copyright 2020, Pavel Kraynyukhov <pavel.kraynyukhov@gmail.com>
 * 
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 *  $Id: flog_common.h December 27, 2020 3:35 AM $
 * 
 **/

#ifndef __FLOG_COMMON_H__
#  define __FLOG_COMMON_H__


#define FLOG_XTRACE 0
#define FLOG_XDEBUG 1
#define FLOG_XINFO  2
#define FLOG_XERROR 3
#define FLOG_XFATAL 4

#if !defined(FLOG_TRACE) && !defined(FLOG_DEBUG) && !defined(FLOG_INFO) && !defined(FLOG_ERROR) && !defined(FLOG_FATAL)
#define LOG_LEVEL FLOG_XINFO
#define FLOG_INFO
#elif defined(FLOG_TRACE)
#define LOG_LEVEL FLOG_XTRACE
#elif defined(FLOG_DEBUG)
#define LOG_LEVEL FLOG_XDEBUG
#elif defined(FLOG_ERROR)
#define LOG_LEVEL FLOG_XERROR
#elif defined(FLOG_FATAL)
#define LOG_LEVEL FLOG_XFATAL
#else
#define LOG_LEVEL FLOG_XINFO
#endif

namespace flog
{
  enum class LogLevel : uint8_t { TRACE, DEBUG, INFO, ERROR, FATAL };
}

#define LOG_LEVEL_STR() (\
  (LOG_LEVEL==FLOG_XDEBUG) ? "DEBUG" : (\
    (LOG_LEVEL==FLOG_XTRACE) ? "TRACE" : (\
     (LOG_LEVEL==FLOG_XINFO) ? "INFO" : (\
        (LOG_LEVEL==FLOG_XERROR) ? "ERROR" : (\
          (LOG_LEVEL==FLOG_XFATAL) ? "FATAL" : "UNDEFINED" )\
        )\
      )\
    )\
  )

#define LOG_MSG_TYPE(X) (\
  (X==flog::LogLevel::DEBUG) ? "DEBUG" : (\
    (X==flog::LogLevel::TRACE) ? "TRACE" : (\
     (X==flog::LogLevel::INFO) ? "INFO" : (\
        (X==flog::LogLevel::ERROR) ? "ERROR" : (\
          (X==flog::LogLevel::FATAL) ? "FATAL" : "UNDEFINED" )\
        )\
      )\
    )\
  )


#endif /* __FLOG_COMMON_H__ */

