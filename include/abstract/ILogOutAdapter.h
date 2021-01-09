/**
 *  Copyright 2020, Pavel Kraynyukhov <pavel.kraynyukhov@gmail.com>
 * 
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 *  $Id: IOutAdapter.h December 27, 2020 2:39 AM $
 * 
 **/

#ifndef __IOUTADAPTER_H__
#  define __IOUTADAPTER_H__

namespace flog
{
  template <typename Message> class ILogOutAdapter 
  {
   public:
    ILogOutAdapter()=default;
    virtual void post(Message&&) = 0;
    virtual void flush() =0;
   protected:
     virtual ~ILogOutAdapter()=default;
  };
}

#endif /* __IOUTADAPTER_H__ */

