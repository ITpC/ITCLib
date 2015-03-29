/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: DateFormatter.h 1 23 Февраль 2015 г., 10:40 pk $
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 **/


#ifndef DATEFORMATTER_H
#  define	DATEFORMATTER_H

#  include <Date.h>
#  include <mutex>

namespace itc
{
  namespace utils
  {

    class DateFormatter
    {
     private:
      std::mutex mMutex;
      Date mDate;
      std::string mFormatedDate;

     public:

      explicit DateFormatter(const Date& ref, const char* fmt)
        : mDate(ref)
      {
        std::lock_guard<std::mutex> sync(mMutex);
        char tbuf[40];

        bzero(tbuf, 40);
        time_t sec = mDate.getTime().mTimestamp.tv_sec;
        struct tm* ts = localtime(&sec);
        strftime(tbuf, 39, fmt, ts);
        mFormatedDate.assign(tbuf);
      }

      const std::string& toString()
      {
        std::lock_guard<std::mutex> sync(mMutex);
        return mFormatedDate;
      }
    };
  }
}

#endif	/* DATEFORMATTER_H */

