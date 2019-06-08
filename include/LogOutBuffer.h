/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: LogOutBuffer.h 56 2007-05-22 09:05:13Z pk $
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 **/

#ifndef __LOGOUTBUFFER_H__
#define __LOGOUTBUFFER_H__
#include <string>
#include <list>

#include <abstract/LoggerHelpers.h>
#include <memory>
#include <mutex>
#include <sys/synclock.h>

namespace itc
{
  namespace utils
  {
    template <typename OutAdapter> class LogOutBuffer
    {
    private:
      itc::sys::mutex mMutex;
      size_t mMaxRows;
      size_t mRowsNow;
      std::shared_ptr<OutAdapter> mOutAdapter;
      std::list<shared_char_vector> mMessagesBuffer;

    public:
      typedef typename itc::utils::abstract::ILogOutputAdapter AbstractLogOutAdapter;

      explicit LogOutBuffer(const std::shared_ptr<OutAdapter>& pOutAdapter, const size_t pMaxRows = 1000)
      : mMutex(), mMaxRows(pMaxRows), mRowsNow(0), mOutAdapter(pOutAdapter)
      {
        static_assert(
          std::is_base_of<AbstractLogOutAdapter, OutAdapter>::value, 
          "Wrong template parameter, - OutAdapter is not derived from AbstractLogOutAdapter"
        );
      }

      void post(const bool _flush, const shared_char_vector& pLogMessage)
      {
        ITCSyncLock sync(mMutex);
        mMessagesBuffer.push_back(pLogMessage);
        if(++mRowsNow >= mMaxRows)
        {
          pflush();
          mRowsNow=0;
        }
        if(_flush) pflush();
      }

      void flush()
      {
        ITCSyncLock sync(mMutex);
        pflush();
      }
      
      ~LogOutBuffer()
      {
        pflush();
      }

    private:
      void pflush()
      {
        auto it=mMessagesBuffer.begin();

        for (;it!=mMessagesBuffer.end();++it)
        {
          mOutAdapter.get()->post(*it);
        }
        mOutAdapter.get()->flush();
        mMessagesBuffer.clear();
      }
    };
  }
}

#endif /*__LOGOUTBUFFER_H__*/
