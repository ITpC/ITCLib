/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: strbuff.h 1 11 Август 2015 г.-15:02 pk$
 * 
 * EMail: <pavel.kraynyukhov@gmail.com>
 * 
 **/


#ifndef STRBUFF_H
#define	STRBUFF_H

#include <cstdint>
#include <list>
#include <utility>
#include <string>
#include <string.h>
#include <tuple>
#include <boost/lexical_cast.hpp>

namespace itc
{
  
  class strbuff
  {
  private:
    std::list<std::string> mStrBuff;
    
  public:  
    strbuff() : mStrBuff()  {}
    
    strbuff(const std::string& _str) 
    {
      mStrBuff.push_back(_str);
    }
    
    strbuff(const strbuff& ref)
    {
      mStrBuff=ref.mStrBuff;
    }
    
    strbuff& operator=(const strbuff& ref)
    {
      mStrBuff=ref.mStrBuff;
      return (*this);
    }

    void getstr(std::string& result)
    {
      size_t strsize=0;
      for(auto it=mStrBuff.rbegin();it!=mStrBuff.rend();++it)
      {
        strsize+=it->length();
      }
      
      result.resize(strsize+1,0);
      size_t i=0;
      for(auto it=mStrBuff.begin();it!=mStrBuff.end();i+=it->length())
      {
        memcpy(&(result[i]),it->c_str(),it->length());
      }
    }    


    auto begin() -> decltype(mStrBuff.begin())
    {
      return mStrBuff.begin();
    }
    
    auto end() -> decltype(mStrBuff.end())
    {
      return mStrBuff.end();
    }

    auto rbegin() -> decltype(mStrBuff.rbegin())
    {
      return mStrBuff.rbegin();
    }
    
    auto rend() -> decltype(mStrBuff.rend())
    {
      return mStrBuff.rend();
    }
    
    strbuff& append(const std::string& _val)
    {
      mStrBuff.push_back(_val);
      return (*this);
    }

    strbuff& append(const char* _val)
    {
      mStrBuff.push_back(_val);
      return (*this);
    }

    strbuff& append(const long double& _val)
    {
      mStrBuff.push_back(std::to_string(_val));
      return (*this);
    }

    template <typename T> strbuff& append(T _val)
    {
      static_assert(std::is_integral<T>::value||std::is_floating_point<T>::value, "Integral or floating pointer types are expected");

      mStrBuff.push_back(boost::lexical_cast<std::string>(_val));
      return (*this);
    }
    
    auto clear() -> decltype(mStrBuff.clear())
    {
      return mStrBuff.clear();
    }
  };
  
  template<typename Tuple, std::size_t N>  struct Appender
  {
    static void append(strbuff& buff, const Tuple& t) 
    {
     Appender<Tuple, N-1>::append(buff,t);
     buff.append(std::get<N-1>(t));
    }
  };

  template<typename Tuple> struct Appender<Tuple, 1>
  {
    static void append(strbuff& buff,const Tuple& t) 
    {
     buff.append(std::get<0>(t));
    }
  };
}



#endif	/* STRBUFF_H */

