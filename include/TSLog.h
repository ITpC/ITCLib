/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: TSLog.h 54 2007-05-22 08:36:14Z pk$
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 **/
 

#ifndef __TSLOG_H__
#define __TSLOG_H__

#include <vector>

#include <abstract/LoggerHelpers.h>
#include <Logger.h>
#include <LogOutBuffer.h>
#include <StdTextLogFormatter.h>
#include <memory>
#include <TSafeLogOutAdapter.h>

namespace itc {

  typedef itc::utils::Logger<itc::utils::STDOutLogThreadSafeAdapter, utils::StdTextLogFormatter, XDEBUG> PrimitiveDebugLog;
  typedef itc::utils::Logger<itc::utils::STDOutLogThreadSafeAdapter, utils::StdTextLogFormatter, XTRACE> PrimitiveTraceLog;
  typedef itc::utils::Logger<itc::utils::STDOutLogThreadSafeAdapter, utils::StdTextLogFormatter, XINFO> PrimitiveInfoLog;
  typedef itc::utils::Logger<itc::utils::STDOutLogThreadSafeAdapter, utils::StdTextLogFormatter, XERROR> PrimitiveErrorLog;
  typedef itc::utils::Logger<itc::utils::STDOutLogThreadSafeAdapter, utils::StdTextLogFormatter, XFATAL> PrimitiveFatalLog;

#if	defined(LOG_DEBUG)
    typedef PrimitiveDebugLog CurrentLogType;
#else
#if defined(LOG_TRACE)
    typedef PrimitiveTraceLog CurrentLogType;
#else
#if defined(LOG_INFO)
    typedef PrimitiveInfoLog CurrentLogType;
#else
#if defined(LOG_ERROR)
    typedef PrimitiveErrorLog CurrentLogType;
#else
    typedef PrimitiveFatalLog CurrentLogType;
#endif
#endif
#endif
#endif	
  
  extern "C"
  {
      std::shared_ptr<CurrentLogType> getLog();
  }

}
#endif /*__TSLOG_H__*/
