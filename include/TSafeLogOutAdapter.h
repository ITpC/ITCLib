/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: TSafeLogOutAdapter.h 54 2007-05-22 08:36:14Z pk$
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 **/

#ifndef TSAFELOGOUTADAPTER_H_
#define TSAFELOGOUTADAPTER_H_

#include <cstdio>
#include <ios>
#include <fstream>
#include <string>
#include <mutex>

namespace itc
{
   
  namespace utils
  {
    class STDOutLogThreadSafeAdapter : public itc::utils::abstract::ILogOutputAdapter
    {
    private:
      std::mutex    mMutex;
      std::string   mFilename;
      std::ios_base::openmode mMode;
      std::ofstream mLogFile;


    public: 
      explicit STDOutLogThreadSafeAdapter(const char* filename, const std::ios_base::openmode& mode=std::ofstream::app)
      : itc::utils::abstract::ILogOutputAdapter(filename),
        mMutex(), mFilename(filename),mMode(std::ofstream::out|mode),
        mLogFile(mFilename,mMode)
      {
        std::lock_guard<std::mutex> synch(mMutex);
        if(!mLogFile.good())
        {
          throw itc::utils::CanNotOpenTheLogException();
        }
      }

      explicit STDOutLogThreadSafeAdapter(STDOutLogThreadSafeAdapter& p)
      :   itc::utils::abstract::ILogOutputAdapter(NULL)
      {
        std::lock_guard<std::mutex> sync(mMutex);
        std::lock_guard<std::mutex> nsync(p.mMutex);
        mFilename=p.mFilename;
        mMode=p.mMode;
        mLogFile.open(mFilename,mMode);
        if(!mLogFile.good())
        {
          throw itc::utils::CanNotOpenTheLogException();
        }
      }

      void post(const std::shared_ptr<std::vector<char>>& pMessage)
      {
        std::lock_guard<std::mutex> synch(mMutex);
        mLogFile << pMessage->data();
      }
      inline void flush()
      {
        std::lock_guard<std::mutex> synch(mMutex);
        mLogFile.flush(); 
      }

      ~STDOutLogThreadSafeAdapter()
      {
        flush();
        std::lock_guard<std::mutex> synch(mMutex);
        mLogFile.close();
      }
    };
  }
}

#endif /*TSAFELOGOUTADAPTER_H_*/
