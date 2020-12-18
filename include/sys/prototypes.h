/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: prototypes.h 54 2007-05-22 08:36:14Z Pavel Kraynyukhov $
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 **/


#ifndef PROTOTYPES_H_
#define PROTOTYPES_H_

#include <sys/sched_yield.h>

#if defined(__FreeBSD__)
  #define YIELD	pthread_yield 
#else
  #ifdef __linux__
    #define YIELD	itc::sys::sched_yield
  #else
    #define YIELD sched_yield
  #endif
#endif

#endif /*PROTOTYPES_H_*/
