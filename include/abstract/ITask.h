/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: ITask.h 54 2007-05-22 08:36:14Z Pavel Kraynyukhov $
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 * 16.01.2018 - license bump.
 **/

#ifndef ITASK_H_
#define ITASK_H_

#include <abstract/Cleanable.h>

namespace itc {
    namespace abstract {

        struct ITask : public Cleanable {

            ITask() {
            }
            virtual bool Do() = 0;
        protected:

            virtual ~ITask() {
            }
        };

        template <typename Data> class ITaskWithData : public ITask {
        public:

            ITaskWithData(const Data& ref) : ITask() {
                setData(ref);
            }
        protected:

            virtual ~ITaskWithData() {
            }
            virtual void setData(const Data&) = 0;
        };
    }
}
#endif /*ITASK_H_*/
