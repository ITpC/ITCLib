/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: Val2Type.h 56 2007-05-22 09:05:13Z Pavel Kraynyukhov $
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 **/

#ifndef __INT2TYPE_H__
#define __INT2TYPE_H__

#include <cstdint>

namespace itc
{
    namespace utils
    {
    	template <int val> struct Int2Type
    	{
    		enum { value = val };
    	};
    
    	template <bool val> struct Bool2Type
    	{
    		enum { value = val };
    	};
        
       template <std::size_t val> struct SizeT2Type
    	{
    		enum { value = val };
    	};
    }
}
#endif /*__INT2TYPE_H__*/
