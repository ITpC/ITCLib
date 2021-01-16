/**
 *  Copyright 2018, Pavel Kraynyukhov <pavel.kraynyukhov@gmail.com>
 * 
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 *  $Id: expose_prioq_container.h September 1, 2019 11:06 AM $
 * 
 **/

#ifndef __EXPOSE_PRIOQ_CONTAINER_H__
#  define __EXPOSE_PRIOQ_CONTAINER_H__

template <typename...Args> auto& expose_container(std::priority_queue<Args...>& q)
{
  struct ExposedQueue : private std::priority_queue<Args...>
  {
    static auto& expose_container(std::priority_queue<Args...>& q)
    {
      return q.*&ExposedQueue::c;
    }
  };
  return ExposedQueue::expose_container(q);
};

#endif /* __EXPOSE_PRIOQ_CONTAINER_H__ */

