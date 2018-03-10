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
#include <system_error>

#include <sys/synclock.h>

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
      explicit STDOutLogThreadSafeAdapter(const std::string& filename, const std::ios_base::openmode& mode=std::ofstream::app)
      : itc::utils::abstract::ILogOutputAdapter(filename),
        mMutex(), mFilename(filename),mMode(std::ofstream::out|mode),
        mLogFile(mFilename,mMode)
      {
        SyncLock sync(mMutex);
        if(!mLogFile.good())
        {
          throw std::system_error(errno,std::system_category(),"Can't open the log file");
        }
      }

      explicit STDOutLogThreadSafeAdapter(STDOutLogThreadSafeAdapter& p)
      : itc::utils::abstract::ILogOutputAdapter(NULL)
      {
        std::lock_guard<std::mutex> sync(mMutex);
        std::lock_guard<std::mutex> nsync(p.mMutex);
        mFilename=p.mFilename;
        mMode=p.mMode;
        mLogFile.open(mFilename,mMode);
        if(!mLogFile.good())
        {
          throw std::system_error(errno,std::system_category(),"Can't open the log file");
        }
      }

      void post(const shared_char_vector& pMessage)
      {
        SyncLock sync(mMutex);
        mLogFile << pMessage->data();
      }
      
      void flush()
      {
        SyncLock sync(mMutex);
        pflush();
      }

      ~STDOutLogThreadSafeAdapter()
      {
        SyncLock sync(mMutex);
        pflush();
        mLogFile.close();
      }
    private:
      void pflush()
      {
        mLogFile.flush(); 
      }
    };
  }
}

#endif /*TSAFELOGOUTADAPTER_H_*/
