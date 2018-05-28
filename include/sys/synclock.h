/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: synclock.h 4 Апрель 2015 г. 1:34 pk$
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 **/

#ifndef __SYNCLOCK_H__
#  define	__SYNCLOCK_H__

#include <mutex>
#include <sys/atomic_mutex.h>

typedef std::lock_guard<std::recursive_mutex> RSyncLock;
typedef std::lock_guard<std::mutex> SyncLock;
typedef std::lock_guard<itc::sys::AtomicMutex> AtomicLock;


#endif	/* __SYNCLOCK_H__ */

