/* 
 * File:   DateFormatter.h
 * Author: pk
 *
 * Created on 23 Февраль 2015 г., 10:40
 */

#ifndef DATEFORMATTER_H
#define	DATEFORMATTER_H

#include <Date.h>
#include <sys/Mutex.h>
#include <sys/SyncLock.h>

namespace itc
{
    namespace utils
    {
       class DateFormatter
       {
       private:
           sys::Mutex       mMutex;
           Date             mDate;
           std::string      mFormatedDate;
       public:
           
           explicit DateFormatter(const Date& ref, const char* fmt)
           : mDate(ref)
           {
                sys::SyncLock synchronize(mMutex);
                static char tbuf[255];
                
                bzero(tbuf,255);
                time_t sec = mDate.getTime().mTimestamp.tv_sec;
                struct tm* ts=localtime(&sec);
                strftime(tbuf,253,fmt,ts);
                mFormatedDate.insert(0,tbuf);
           }
           
           const std::string& toString()
           {
               sys::SyncLock synchronize(mMutex);
               return mFormatedDate;
           }
       };
    }
}

#endif	/* DATEFORMATTER_H */

