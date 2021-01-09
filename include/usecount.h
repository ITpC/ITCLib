/**
 *  Copyright 2018, Pavel Kraynyukhov <pavel.kraynyukhov@gmail.com>
 * 
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 *  $Id: usecoung.h June 15, 2019 2:49 PM $
 * 
 **/


#ifndef __USECOUNG_H__
#  define __USECOUNG_H__

#include <atomic>
#include <system_error>

struct usecount
{
  std::atomic<size_t>* ptr;

  explicit usecount(std::atomic<size_t>* _ptr) : ptr{_ptr}
  { 
    if(!ptr) throw std::system_error(EINVAL,std::system_category(),"usecount(nullptr)");

    ptr->fetch_add(1);
  }
  ~usecount() noexcept
  {
    if(ptr) ptr->fetch_sub(1);
  }
};

#endif /* __USECOUNG_H__ */

