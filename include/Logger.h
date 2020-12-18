/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: Logger.h 56 2007-05-22 09:05:13Z Pavel Kraynyukhov $
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 **/

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <Val2Type.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#include <exception>
#include <memory>

#include <abstract/LoggerHelpers.h>
#include <abstract/ILogFormatter.h>
#include <StdTextLogFormatter.h>
#include <LogOutBuffer.h>
#include <sys/synclock.h>

#include <sys/time.h>

#include <chrono>

#define XDEBUG 0
#define XTRACE 1
#define XINFO 2
#define XERROR 3
#define XFATAL 4

#ifndef MAX_BUFF_SIZE
#define MAX_BUFF_SIZE 256
#endif

#define LL2STR(x) (\
            (x==XDEBUG) ? "DEBUG" : (\
                (x==XTRACE) ? "TRACE" : (\
                   (x==XINFO) ? "INFO" : (\
                       (x==XERROR) ? "ERROR" : (\
                           (x==XFATAL) ? "FATAL" : "UNDEFINED" )\
                       )\
                   )\
                )\
             )



namespace itc
{
  namespace utils
  {
    template <
    typename TOutAdapter, typename TFormatter, int LOGLEVEL = XDEBUG
    > class Logger
    {
    private:
      using strtimebuff=char[21];
      
      Int2Type<LOGLEVEL> mLogLevel;

      std::shared_ptr<TOutAdapter> mOutAdapter;
      LogOutBuffer<TOutAdapter> mOutBuffer;
      
      const char* mLogLevelStr;
      const char* mAppName;

    public:
      typedef itc::utils::abstract::ILogOutputAdapter AbstractLogOutAdapter;
      typedef itc::utils::abstract::ILogFormatter AbstractLogFormatter;
      typedef typename std::shared_ptr<TOutAdapter> SharedTOutAdapter;
      

      explicit Logger(SharedTOutAdapter& pOutAdapter, const char* pAppName)
      : mOutAdapter(pOutAdapter), mOutBuffer(mOutAdapter),
      mLogLevelStr(LL2STR(LOGLEVEL)), mAppName(pAppName)
      {
        static_assert(
          std::is_base_of<AbstractLogOutAdapter, TOutAdapter>::value, 
          "Wrong template parameter, - OutAdapter is not derived from AbstractLogOutAdapter"
        );

        static_assert(
          std::is_base_of<AbstractLogFormatter, TFormatter>::value, 
          "Wrong template parameter, - TFormatter is not derived from AbstractLogFormatter"
        );

        TFormatter aFormatter;
        strtimebuff timestr;
        getCurrTimeStr(timestr);
        mOutBuffer.post(true,
          aFormatter.format(
            MAX_BUFF_SIZE, "%s - %s: Logger::Logger() [ %s ]:%d: start logging of level %s\n",
            timestr, mAppName, __FILE__, __LINE__, mLogLevelStr
          )
        );
      }

    private:
      void getCurrTimeStr(strtimebuff& tbuf)
      {
        auto clck=std::chrono::system_clock::now();
        
        auto seconds=std::chrono::duration_cast<std::chrono::seconds>(
          std::chrono::time_point_cast<std::chrono::seconds>(clck).time_since_epoch()
        ).count();
        
        size_t msec=std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::time_point_cast<std::chrono::milliseconds>(clck).time_since_epoch()
        ).count()-(seconds*1000);
        

        auto ts=std::chrono::system_clock::to_time_t(clck);
        
        struct tm* td = localtime(&ts);

        if (td == NULL)
        {
          throw std::bad_alloc();
        }
        
        strftime(tbuf, 15, "%Y%m%d%H%M%S", td);
        snprintf(tbuf + 14, 6, ".%03zuZ", msec);
      }

      void publish(const bool flush, const char* method, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        TFormatter aFormatter;
        strtimebuff timestr;
        getCurrTimeStr(timestr);
        mOutBuffer.post(flush, aFormatter.format(MAX_BUFF_SIZE, "%s - [%s:%s] - %s:%zu: %s\n", timestr, method, mLogLevelStr, pFilename, pLineNumber, message->data()));
        if(flush) mOutBuffer.flush();
      }
      void trace(Int2Type<XDEBUG> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message) { }

      void trace(Int2Type<XTRACE> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        publish(false,"TRACE", pFilename,pLineNumber,message);
      }

      void trace(Int2Type<XINFO> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message) { }

      void trace(Int2Type<XERROR> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message) { }

      void trace(Int2Type<XFATAL> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message) { }
      //-----------

      void debug(Int2Type<XTRACE> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        publish(false,"DEBUG", pFilename,pLineNumber,message);
      }

      void debug(Int2Type<XDEBUG> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        publish(false,"DEBUG", pFilename,pLineNumber,message);
      }

      void debug(Int2Type<XINFO> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message) { }

      void debug(Int2Type<XERROR> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message) { }

      void debug(Int2Type<XFATAL> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message) { }
      //-------------------

      void fatal(Int2Type<XTRACE> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        publish(true,"FATAL", pFilename,pLineNumber,message);
      }

      void fatal(Int2Type<XFATAL> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        publish(true,"FATAL", pFilename,pLineNumber,message);
      }

      void fatal(Int2Type<XINFO> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        publish(true,"FATAL", pFilename,pLineNumber,message);
      }

      void fatal(Int2Type<XERROR> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        publish(true,"FATAL", pFilename,pLineNumber,message);
      }

      void fatal(Int2Type<XDEBUG> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        publish(true,"FATAL", pFilename,pLineNumber,message);
      }
      //-------------------

      void error(Int2Type<XTRACE> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        publish(true,"ERROR", pFilename,pLineNumber,message);
      }

      void error(Int2Type<XERROR> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        publish(true,"ERROR", pFilename,pLineNumber,message);
      }

      void error(Int2Type<XINFO> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        publish(true,"ERROR", pFilename,pLineNumber,message);
      }

      void error(Int2Type<XFATAL> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message) { }

      void error(Int2Type<XDEBUG> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        publish(true,"ERROR", pFilename,pLineNumber,message);
      }
      //-------------------

      void info(Int2Type<XTRACE> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        publish(false,"INFO", pFilename,pLineNumber,message);
      }
      
      void info(Int2Type<XINFO> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        publish(false,"INFO", pFilename,pLineNumber,message);
      }

      void info(Int2Type<XERROR> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message) { }

      void info(Int2Type<XFATAL> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message) { }

      void info(Int2Type<XDEBUG> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        publish(false,"INFO", pFilename,pLineNumber,message);
      }

      //-------------------

      public:
      void trace(const char* pFilename, const size_t pLineNumber, const char* format, ...)
      {
        
        TFormatter aFormatter;
        va_list args;
        va_start(args, format);
        shared_char_vector formattedMessage(aFormatter.format(MAX_BUFF_SIZE, format, args));
        va_end(args);
        
        trace(mLogLevel, pFilename, pLineNumber, formattedMessage);
      }

      void debug(const char* pFilename, const size_t pLineNumber, const char* format, ...)
      {
        TFormatter aFormatter;

        va_list args;
        va_start(args, format);
        shared_char_vector formattedMessage(aFormatter.format(MAX_BUFF_SIZE, format, args));
        va_end(args);
        
        debug(mLogLevel, pFilename, pLineNumber, formattedMessage);
      }

      void error(const char* pFilename, const size_t pLineNumber, const char* format, ...)
      {
        TFormatter aFormatter;

        va_list args;
        va_start(args, format);
        shared_char_vector formattedMessage(aFormatter.format(MAX_BUFF_SIZE, format, args));
        va_end(args);
        
        error(mLogLevel, pFilename, pLineNumber, formattedMessage);
      }

      void fatal(const char* pFilename, const size_t pLineNumber, const char* format, ...)
      {
        TFormatter aFormatter;

        va_list args;
        va_start(args, format);
        shared_char_vector formattedMessage(aFormatter.format(MAX_BUFF_SIZE, format, args));
        va_end(args);
        
        fatal(mLogLevel, pFilename, pLineNumber, formattedMessage);
      }

      void info(const char* pFilename, const size_t pLineNumber, const char* format, ...)
      {
        TFormatter aFormatter;

        va_list args;
        va_start(args, format);
        shared_char_vector formattedMessage(aFormatter.format(MAX_BUFF_SIZE, format, args));
        va_end(args);
        
        info(mLogLevel, pFilename, pLineNumber, formattedMessage);
      }
      
      
      void flush()
      {
        mOutBuffer.flush();
      }

      ~Logger()
      {
        flush();
      }
    };
  }
}

#endif /*__LOGGER_H__*/
