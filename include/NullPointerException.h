/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: NullPointerException.h 54 2007-05-22 08:36:14Z Pavel Kraynyukhov $
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 **/
 

#ifndef NULLPOINTEREXCEPTION_H_
#define NULLPOINTEREXCEPTION_H_

#include <ITCException.h>

namespace itc
{
    typedef TITCException<exceptions::NullPointerException> NullPointerException;
}

typedef itc::NullPointerException NullPointerException;

#endif /*NULLPOINTEREXCEPTION_H_*/
