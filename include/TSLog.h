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

#if defined(TSAFE_LOG)
    typedef itc::utils::Logger<itc::utils::STDOutLogThreadSafeAdapter, utils::StdTextLogFormatter, XDEBUG, true > PrimitiveDebugTSLog;
    typedef itc::utils::Logger<itc::utils::STDOutLogThreadSafeAdapter, utils::StdTextLogFormatter, XTRACE, true > PrimitiveTraceTSLog;
    typedef itc::utils::Logger<itc::utils::STDOutLogThreadSafeAdapter, utils::StdTextLogFormatter, XINFO, true > PrimitiveInfoTSLog;
    typedef itc::utils::Logger<itc::utils::STDOutLogThreadSafeAdapter, utils::StdTextLogFormatter, XERROR, true > PrimitiveErrorTSLog;
    typedef itc::utils::Logger<itc::utils::STDOutLogThreadSafeAdapter, utils::StdTextLogFormatter, XFATAL, true > PrimitiveFatalTSLog;
#else
    typedef itc::utils::Logger<itc::utils::STDOutLogThreadUnsafeAdapter, utils::StdTextLogFormatter, XDEBUG, true > PrimitiveDebugTSLog;
    typedef itc::utils::Logger<itc::utils::STDOutLogThreadUnsafeAdapter, utils::StdTextLogFormatter, XTRACE, true > PrimitiveTraceTSLog;
    typedef itc::utils::Logger<itc::utils::STDOutLogThreadUnsafeAdapter, utils::StdTextLogFormatter, XINFO, true > PrimitiveInfoTSLog;
    typedef itc::utils::Logger<itc::utils::STDOutLogThreadUnsafeAdapter, utils::StdTextLogFormatter, XERROR, true > PrimitiveErrorTSLog;
    typedef itc::utils::Logger<itc::utils::STDOutLogThreadUnsafeAdapter, utils::StdTextLogFormatter, XFATAL, true > PrimitiveFatalTSLog;
#endif

#if	defined(LOG_DEBUG)
    typedef PrimitiveDebugTSLog CurrentLogType;
#else
#if defined(LOG_TRACE)
    typedef PrimitiveTraceTSLog CurrentLogType;
#else
#if defined(LOG_INFO)
    typedef PrimitiveInfoTSLog CurrentLogType;
#else
#if defined(LOG_ERROR)
    typedef PrimitiveErrorTSLog CurrentLogType;
#else
    typedef PrimitiveFatalTSLog CurrentLogType;
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
