/**
 * Copyright Pavel Kraynyukhov 2007 - 2018.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *  
 * $Id: Runnable.h 101 2007-08-12 13:07:51Z acs $
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 * 16.01.2018 - license bump.
 **/

#ifndef RUNNABLE_H_
#define RUNNABLE_H_


namespace itc {
    namespace abstract {

        /** 
         * @brief Runnable interface. Inherit this interface to run your tasks
         * with help of itc::sys::PThread and itc::sys::Cancelable<T> as a 
         * thread
         * 
         **/
        class IRunnable {
        public:
          virtual void execute() = 0;
          virtual void onCancel() = 0;
          virtual void shutdown() = 0;
          virtual ~IRunnable(){};
        };
    }
}
#endif /*RUNNABLE_H_*/
