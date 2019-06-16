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
      using mutextype=itc::sys::mutex;
      
      mutextype               mMutex;
      std::ios_base::openmode mMode;
      std::string             mFilename;
      std::ofstream           mLogFile;
      
    public: 
      explicit STDOutLogThreadSafeAdapter(const std::string& filename, const std::ios_base::openmode& mode=std::ofstream::app)
      : itc::utils::abstract::ILogOutputAdapter(filename),
        mMutex(),
        mMode(std::ofstream::out|mode),
        mFilename(filename),
        mLogFile(mFilename,mMode)
      {
        std::lock_guard<mutextype> sync(mMutex);
        if(!mLogFile.good())
        {
          throw std::system_error(errno,std::system_category(),"Can't open the log file");
        }
      }

      explicit STDOutLogThreadSafeAdapter(STDOutLogThreadSafeAdapter& p)
      : itc::utils::abstract::ILogOutputAdapter(NULL)
      {
        std::lock_guard<mutextype> sync(mMutex);
        std::lock_guard<mutextype> nsync(p.mMutex);
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
        std::lock_guard<mutextype> sync(mMutex);
        mLogFile << pMessage->data();
      }
      
      void flush()
      {
        std::lock_guard<mutextype> sync(mMutex);
        pflush();
      }

      ~STDOutLogThreadSafeAdapter()
      {
        std::lock_guard<mutextype> sync(mMutex);
        pflush();
        mLogFile.close();
      }
    private:
      void pflush()
      {
        if(mLogFile.good()) mLogFile.flush(); 
      }
    };
  }
}

#endif /*TSAFELOGOUTADAPTER_H_*/
