/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: Types.h 54 2007-05-22 08:36:14Z Pavel Kraynyukhov $
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 **/

#ifndef __BASE_TYPES__
#define __BASE_TYPES__
#include <stdlib.h>

typedef void (*intCallBack)(int);
typedef void (*IntStrCallBack)(int,char*);
typedef void (*CharIntCharCallBack)(char*,int,char*);
typedef void (*CharIntIntCharCallBack)(char*,int,int,char*);
typedef void (*PtrCallBack)(void*);
typedef void (*WntPtrCallBack)(int, void*);
typedef void (*WordPtrCallBack)(unsigned, void*);
typedef void (*CallBack)();
typedef void (*(*thread_t)(void *));
typedef void ((*cleanupfunc)(void *));

#endif
