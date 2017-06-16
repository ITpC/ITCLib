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
#include <Date.h>

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
      std::mutex mMutex;
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
      : mMutex(), mOutAdapter(pOutAdapter), mOutBuffer(mOutAdapter),
      mLogLevelStr(LL2STR(LOGLEVEL)), mAppName(pAppName)
      {
        SyncLock sync(mMutex);
        
        static_assert(
          std::is_base_of<AbstractLogOutAdapter, TOutAdapter>::value, 
          "Wrong template parameter, - OutAdapter is not derived from AbstractLogOutAdapter"
        );

        static_assert(
          std::is_base_of<AbstractLogFormatter, TFormatter>::value, 
          "Wrong template parameter, - TFormatter is not derived from AbstractLogFormatter"
        );

        TFormatter aFormatter;
        mOutBuffer.post(
          aFormatter.format(
            MAX_BUFF_SIZE, "%s - %s: Logger::Logger() [ %s ]:%d: start logging of level %s\n",
            getCurrTimeStr(), mAppName, __FILE__, __LINE__, mLogLevelStr
          )
        );
      }

    private:
      static const char* getCurrTimeStr()
      {
        static char tbuf[22];
        Date aDate;
        Time aTime = aDate.get();

        time_t sec = aTime.mTimestamp.tv_sec;
        time_t msec = aTime.mTimestamp.tv_usec / 1000;

        struct tm* ts = localtime(&sec);

        if (ts == NULL)
        {
          throw std::bad_alloc();
        }
        strftime(tbuf, 22, "%Y%m%d%H%M%S", ts);
        snprintf(tbuf + 14, 6, ".%03jdZ", msec);
        tbuf[21] = 0;

        return tbuf;
      }

      void trace(Int2Type<XDEBUG> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message) { }

      void trace(Int2Type<XTRACE> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        TFormatter aFormatter;
        mOutBuffer.post(aFormatter.format(MAX_BUFF_SIZE, "%s - [%s] - %s:%d: %s\n", getCurrTimeStr(),mLogLevelStr, pFilename, pLineNumber, message->data()));
      }

      void trace(Int2Type<XINFO> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message) { }

      void trace(Int2Type<XERROR> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message) { }

      void trace(Int2Type<XFATAL> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message) { }
      //-----------

      void debug(Int2Type<XTRACE> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        TFormatter aFormatter;
        mOutBuffer.post(aFormatter.format(MAX_BUFF_SIZE, "%s - [%s] - %s:%d: %s\n", getCurrTimeStr(),mLogLevelStr, pFilename, pLineNumber, message->data()));
      }

      void debug(Int2Type<XDEBUG> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        TFormatter aFormatter;
        mOutBuffer.post(aFormatter.format(MAX_BUFF_SIZE, "%s - [%s] - %s:%d: %s\n", getCurrTimeStr(),mLogLevelStr, pFilename, pLineNumber, message->data()));
      }

      void debug(Int2Type<XINFO> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message) { }

      void debug(Int2Type<XERROR> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message) { }

      void debug(Int2Type<XFATAL> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message) { }
      //-------------------

      void fatal(Int2Type<XTRACE> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        TFormatter aFormatter;
        mOutBuffer.post(aFormatter.format(MAX_BUFF_SIZE, "%s - [%s] - %s:%d: %s\n", getCurrTimeStr(),mLogLevelStr, pFilename, pLineNumber, message->data()));
      }

      void fatal(Int2Type<XFATAL> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        TFormatter aFormatter;
        mOutBuffer.post(aFormatter.format(MAX_BUFF_SIZE, "%s - [%s] - %s:%d: %s\n", getCurrTimeStr(),mLogLevelStr, pFilename, pLineNumber, message->data()));
      }

      void fatal(Int2Type<XINFO> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        TFormatter aFormatter;
        mOutBuffer.post(aFormatter.format(MAX_BUFF_SIZE, "%s - [%s] - %s:%d: %s\n", getCurrTimeStr(),mLogLevelStr, pFilename, pLineNumber, message->data()));
      }

      void fatal(Int2Type<XERROR> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        TFormatter aFormatter;
        mOutBuffer.post(aFormatter.format(MAX_BUFF_SIZE, "%s - [%s] - %s:%d: %s\n", getCurrTimeStr(),mLogLevelStr, pFilename, pLineNumber, message->data()));
      }

      void fatal(Int2Type<XDEBUG> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        TFormatter aFormatter;
        mOutBuffer.post(aFormatter.format(MAX_BUFF_SIZE, "%s - [%s] - %s:%d: %s\n", getCurrTimeStr(),mLogLevelStr, pFilename, pLineNumber, message->data()));
      }
      //-------------------

      void error(Int2Type<XTRACE> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        TFormatter aFormatter;
        mOutBuffer.post(aFormatter.format(MAX_BUFF_SIZE, "%s - [%s] - %s:%d: %s\n", getCurrTimeStr(),mLogLevelStr, pFilename, pLineNumber, message->data()));
      }

      void error(Int2Type<XERROR> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        TFormatter aFormatter;
        mOutBuffer.post(aFormatter.format(MAX_BUFF_SIZE, "%s - [%s] - %s:%d: %s\n", getCurrTimeStr(),mLogLevelStr, pFilename, pLineNumber, message->data()));
      }

      void error(Int2Type<XINFO> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        TFormatter aFormatter;
        mOutBuffer.post(aFormatter.format(MAX_BUFF_SIZE, "%s - [%s] - %s:%d: %s\n", getCurrTimeStr(),mLogLevelStr, pFilename, pLineNumber, message->data()));
      }

      void error(Int2Type<XFATAL> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message) { }

      void error(Int2Type<XDEBUG> fictive, const char* pFilename, const size_t pLineNumber, const shared_char_vector& message)
      {
        TFormatter aFormatter;
        mOutBuffer.post(aFormatter.format(MAX_BUFF_SIZE, "%s - [%s] - %s:%d: %s\n", getCurrTimeStr(),mLogLevelStr, pFilename, pLineNumber, message->data()));
      }
      //-------------------

      void info(Int2Type<XTRACE> fictive, const shared_char_vector& message)
      {
        TFormatter aFormatter;
        mOutBuffer.post(aFormatter.format(MAX_BUFF_SIZE, "%s - [%s] - %s\n", getCurrTimeStr(),mLogLevelStr, message->data()));
      }

      void info(Int2Type<XINFO> fictive, const shared_char_vector& message)
      {
        TFormatter aFormatter;
        mOutBuffer.post(aFormatter.format(MAX_BUFF_SIZE, "%s - [%s] - %s\n", getCurrTimeStr(),mLogLevelStr, message->data()));
      }

      void info(Int2Type<XERROR> fictive, const shared_char_vector& message) { }

      void info(Int2Type<XFATAL> fictive, const shared_char_vector& message) { }

      void info(Int2Type<XDEBUG> fictive, const shared_char_vector& message)
      {
        TFormatter aFormatter;
        mOutBuffer.post(aFormatter.format(MAX_BUFF_SIZE, "%s - [%s] - %s\n", getCurrTimeStr(),mLogLevelStr, message->data()));
      }

      //-------------------

      public:
      void trace(const char* pFilename, const size_t pLineNumber, const char* format, ...)
      {
        SyncLock sync(mMutex);
        TFormatter aFormatter;
        
        va_list args;
        va_start(args, format);
        shared_char_vector formattedMessage(aFormatter.format(MAX_BUFF_SIZE, format, args));
        va_end(args);
        
        trace(mLogLevel, pFilename, pLineNumber, formattedMessage);
      }

      void debug(const char* pFilename, const size_t pLineNumber, const char* format, ...)
      {
        SyncLock sync(mMutex);
        TFormatter aFormatter;

        va_list args;
        va_start(args, format);
        shared_char_vector formattedMessage(aFormatter.format(MAX_BUFF_SIZE, format, args));
        va_end(args);
        
        debug(mLogLevel, pFilename, pLineNumber, formattedMessage);
      }

      void error(const char* pFilename, const size_t pLineNumber, const char* format, ...)
      {
        SyncLock sync(mMutex);
        TFormatter aFormatter;

        va_list args;
        va_start(args, format);
        shared_char_vector formattedMessage(aFormatter.format(MAX_BUFF_SIZE, format, args));
        va_end(args);
        
        error(mLogLevel, pFilename, pLineNumber, formattedMessage);
      }

      void fatal(const char* pFilename, const size_t pLineNumber, const char* format, ...)
      {
        SyncLock sync(mMutex);
        TFormatter aFormatter;

        va_list args;
        va_start(args, format);
        shared_char_vector formattedMessage(aFormatter.format(MAX_BUFF_SIZE, format, args));
        va_end(args);
        
        fatal(mLogLevel, pFilename, pLineNumber, formattedMessage);
      }

      void info(const char* format, ...)
      {
        SyncLock sync(mMutex);
        TFormatter aFormatter;

        va_list args;
        va_start(args, format);
        shared_char_vector formattedMessage(aFormatter.format(MAX_BUFF_SIZE, format, args));
        va_end(args);
        info(mLogLevel, formattedMessage);
      }

      void flush()
      {
        SyncLock sync(mMutex);
        mOutBuffer.flush();
      }

      ~Logger()
      {
        flush();
      }
    };

    class CanNotOpenTheLogException : public std::exception
    {
    private:
      int ERRNO;
    public:

      CanNotOpenTheLogException() throw ()
      {
        ERRNO = errno;
      }

      const char* what() const throw ()
      {
        return strerror(ERRNO);
      }
    };
  }
}

#endif /*__LOGGER_H__*/
