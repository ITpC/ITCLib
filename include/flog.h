/**
 *  Copyright 2018, Pavel Kraynyukhov <pavel.kraynyukhov@gmail.com>
 * 
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 *  $Id: flog.h December 26, 2020 6:12 PM $
 * 
 **/

#ifndef __FLOG_H__
#  define __FLOG_H__

#include <type_traits>
#include <memory>
#include <thread>
#include <flog_common.h>
#include <default_out_adapter.h>
#include <default_formatter.h>
#include <Singleton.h>

namespace flog
{
  

  template <typename TOutAdapter, typename TFormatter> 
  class logger
  {
   private:
    using OutAdapter=std::shared_ptr<TOutAdapter>;
    
    LogLevel          mLogLevel;
    OutAdapter        mOutAdapter;
    TFormatter        mFormatter;
    std::thread       mOutAdapterThread;


   public:
    template <typename...Args> logger(Args&&...args)
    :
#if defined(FLOG_TRACE)
    mLogLevel{LogLevel::TRACE}
#elif defined(FLOG_DEBUG)
    mLogLevel{LogLevel::DEBUG}
#elif defined(FLOG_INFO)
    mLogLevel{LogLevel::INFO}
#elif defined(FLOG_ERROR)
    mLogLevel{LogLevel::ERROR}
#elif defined(FLOG_FATAL)
    mLogLevel{LogLevel::FATAL}
#endif
    ,mOutAdapter{std::make_shared<TOutAdapter>(args...)},mFormatter(),mOutAdapterThread([this](){mOutAdapter->execute();})
    {
      static_assert(std::is_base_of<flog::ILogFormatter, TFormatter>::value, "provided TFormatter class is not derived from flog::ILogFormatter");
      static_assert(std::is_base_of<flog::ILogOutAdapter<std::string>, TOutAdapter>::value, "provided TOutAdapter class is not derived from flog::ILogOutAdapter");
    }
    logger(const logger&)=delete;
    logger(logger&)=delete;
    
    template <typename ... Args> void post(flog::LogLevel msgtype,std::string_view _format,Args&&...args)
    {
      mOutAdapter->post(mFormatter.format(msgtype,_format,std::forward<Args>(args)...));
      if((msgtype == flog::LogLevel::ERROR)||(msgtype == flog::LogLevel::FATAL))
      {
        mOutAdapter->flush();
      }
    }

    ~logger()
    {
      mOutAdapter->flush();
      mOutAdapter->shutdown();
      mOutAdapterThread.join();
    }
  };

}


#ifdef LOG_FILE
#define LOG_NAME LOG_FILE
#else
#define LOG_NAME "flog.log"
#endif

#define semicolon

#define LOGGER itc::Singleton<flog::logger<flog::defaultOutAdapter,flog::default_formatter>>::getInstance(LOG_NAME)
#define FLG_LOG(TYPE,args...) \
  ((int)(TYPE)>=LOG_LEVEL) ? LOGGER->post(TYPE,args) : ((void)0)

#define FLG_INFO(args...) \
  FLG_LOG(flog::LogLevel::INFO,args)

#define FLG_DEBUG(args...) \
  FLG_LOG(flog::LogLevel::DEBUG,args)

#define FLG_TRACE(args...) \
  FLG_LOG(flog::LogLevel::TRACE,args)

#define FLG_ERROR(args...) \
  FLG_LOG(flog::LogLevel::ERROR,args)

#define FLG_FATAL(args...) \
  FLG_LOG(flog::LogLevel::FATAL,args)

#endif /* __FLOG_H__ */

