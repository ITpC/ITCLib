/**
 *  Copyright 2018, Pavel Kraynyukhov <pavel.kraynyukhov@gmail.com>
 * 
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 *  $Id: sched_yield.h November 6, 2018 1:23 AM $
 * 
 **/

#ifndef __SCHED_YIELD_H__
#  define __SCHED_YIELD_H__

#include <time.h>

namespace itc
{
  namespace sys
  {
    /*
     * Linux sched_yield() works only for threads with SCHED_FIFO and SCHED_RR
     * policies (real-time), therefore replacing with context switch made by
     * nanosleep(3). 
     */
    static void sched_yield(const long int ns=10)
    {
      struct timespec pause{0,ns};
      nanosleep(&pause,nullptr);
    }
  }
}
      

#endif /* __SCHED_YIELD_H__ */

