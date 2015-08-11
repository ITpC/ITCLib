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
#include <vector>

#include <InterfaceCheck.h>
#include <abstract/LoggerHelpers.h>
#include <Val2Type.h>
#include <memory>
#include <mutex>

namespace itc
{
  namespace utils
  {

    template <typename OutAdapter, bool TSafe = true > class LogOutBuffer
    {
    private:
      typedef std::shared_ptr<std::vector<char>> shared_buff;
      
      std::mutex mMutex;
      size_t mMaxRows;
      std::shared_ptr<OutAdapter> mOutAdapter;

      std::vector<shared_buff> mMessagesBuffer;
      itc::utils::Bool2Type<TSafe> mThreadSafe;

    public:
      typedef typename itc::utils::abstract::ILogOutputAdapter AbstractLogOutAdapter;

      explicit LogOutBuffer(std::shared_ptr<OutAdapter>& pOutAdapter, const size_t pMaxRows = 1000)
      : mMutex(), mMaxRows(pMaxRows), mOutAdapter(pOutAdapter)
      {
        STATIC_CHECKER3MSG(
          CheckRelationship(
          OutAdapter, subclassof, AbstractLogOutAdapter
          ),
          OutAdapter, _is_not_a_subclass_of_, AbstractLogOutAdapter
          );
      }

      void post(const shared_buff& pLogMessage)
      {
        std::lock_guard<std::mutex> sync(mMutex);
        post(mThreadSafe, pLogMessage);
      }

      void flush()
      {
        std::lock_guard<std::mutex> sync(mMutex);
        flush(mThreadSafe); // for external calls or destructor call.
      }

      ~LogOutBuffer()
      {
        flush();
      }

    private:

      void post(itc::utils::Bool2Type < true > threadsafe, const std::shared_ptr<std::vector<char>>&pLogMessage)
      {
        push(pLogMessage);
      }

      void flush(itc::utils::Bool2Type < true > threadsafe)
      {
        pflush();
      }

      void pflush()
      {
        auto it=mMessagesBuffer.begin();

        for (;it!=mMessagesBuffer.end();++it)
        {
          mOutAdapter.get()->post(*it);
        }
        mMessagesBuffer.clear();
        mOutAdapter.get()->flush();
      }

      void push(const std::shared_ptr<std::vector<char>>&pLogMessage)
      {
        if (pLogMessage.get())
        {

          if (mMessagesBuffer.size() >= mMaxRows)
          {
            itc::utils::Bool2Type < true> threadSafe;
            flush(threadSafe);
          }
          mMessagesBuffer.push_back(pLogMessage);
        }
      }

    };
  }
}

#endif /*__LOGOUTBUFFER_H__*/
