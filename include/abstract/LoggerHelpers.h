/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: LoggerHelpers.h 23 2010-11-23 12:53:35Z pk $
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 * 16.01.2018 - license bump.
 * 
 **/

#ifndef LOGGERHELPERS_H_
#define LOGGERHELPERS_H_

#include <string>
#include <memory>
#include <vector>

namespace itc
{
  namespace utils
  {
    typedef std::shared_ptr<std::vector<char>> shared_char_vector;
    typedef std::weak_ptr<std::vector<char>> weak_char_vector;
    namespace abstract
    {
      class ILogOutputAdapter
      {
      public:
        explicit ILogOutputAdapter(const std::string& filename) { }
        virtual void post(const shared_char_vector& pMessage) = 0;

      protected:
        virtual ~ILogOutputAdapter() { }
      };
    }
  }
}


#endif /*LOGGERHELPERS_H_*/
