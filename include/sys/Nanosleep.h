/**
 * Copyright Pavel Kraynyukhov 2007 - 2021.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: Nanosleep.h 27 2011-02-06 13:25:43Z pk $
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 **/

#ifndef __CPP_NANOSLEEP_H__
#define	__CPP_NANOSLEEP_H__

#include <system_error>
#include <sys/time.h>

namespace itc
{
  namespace sys
  {
    class Nap
    {
    public:
      explicit Nap()=default;
      Nap(const Nap&)=delete;
      Nap(Nap&)=delete;
      
      void nanosleep(const uint32_t nsec)
      {
        struct timespec pause{0,nsec};
        struct timespec save{0,0};
        
        if(nsec >= 1000000000)
        {
          pause.tv_sec=nsec/1000000000;
          pause.tv_nsec=pause.tv_sec*1000000000-nsec;
        }
        
        while(::nanosleep(&pause,&save) == -1)
        {
          if(errno == EINTR)
          {
            pause.tv_sec=save.tv_sec;
            pause.tv_nsec=save.tv_nsec;
            save={0,0};
            continue;
          }else{
            break;
          }
        }
      }
      
      void usleep(const uint32_t usec)
      {
        this->nanosleep(usec*1000);
      }
      
      void sleep(const uint32_t sec)
      {
        this->nanosleep(sec*1000000000);
      }
    };
  }
}


#endif	/* __CPP_NANOSLEEP_H__ */

