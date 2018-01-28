/**
 * Copyright Pavel Kraynyukhov 2007 - 2018.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: Cleanable.h 54 2007-05-22 08:36:14Z Pavel Kraynyukhov $
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 * 16.01.2018 - license bump.
 **/

#ifndef CLEANABLE_H_
#define CLEANABLE_H_

namespace itc {
    namespace abstract {

        class Cleanable {
        public:
            virtual void cleanup() = 0;
        protected:
            virtual ~Cleanable()=default;
        };
    }
}


#endif /*CLEANABLE_H_*/
