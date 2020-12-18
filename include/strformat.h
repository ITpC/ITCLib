/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: strformat.h 1 11 Август 2015 г.-15:45 pk$
 * 
 * EMail: <pavel.kraynyukhov@gmail.com>
 * 
 **/

#ifndef STRFORMAT_H
#define	STRFORMAT_H


#include <cstdint>
#include <string>
#include <strbuff.h>


namespace itc
{
  /**
   * @brief safe string formatting class much like sprintf, but with following 
   * differences: only one flag is required %% - which is capable to care of  
   * integral, floating or C-string or C++ string type; It is typesafe; 
   * Amount of flags must not be equal to amount of function arguments; There is
   * no hard limit on resulting formatted string (just max size of std::string);
   * It is ~2.7 times slower then snprintf; result is always appending to out 
   * string.
   * 
   * @usage std::string result("Append afterwards:"); 
   * itc::strformat formatter; 
   * formatter.format(result,"integer: %%, float %%, no idea: %%",1,0.3f,"it's a
   * c-string");
   * std::cout << result;
   * 
   **/
  class strformat
  {
  private:
    strbuff mBuff;
  public:
    
    
    template <typename...Args> void concat(std::string& result, Args...args)
    {
      auto tpl(std::tie(args...));
      
      Appender<decltype(tpl),sizeof...(Args)> ap;
      
      ap.append(mBuff,tpl);
      
      mBuff.getstr(result);
      mBuff.clear();
    }
    

    template <typename...Args> std::string& format(std::string& result, const std::string& fmt,  Args...args)
    {
      auto tpl(std::tie(args...));
      
      Appender<decltype(tpl),sizeof...(Args)> ap;
      
      ap.append(mBuff,tpl);
      
      size_t oldpos=0;
      size_t pos=fmt.find("%%");
      
      auto it=mBuff.begin();

      
      while((pos != std::string::npos) && (it != mBuff.end()))
      {
        result.append(fmt.substr(oldpos,pos-oldpos));
        result.append(*it); 
        ++it; oldpos=pos+2;
        
        pos=fmt.find("%%",oldpos);
      }
      
      if(oldpos < fmt.length())
      {
        result.append(fmt.substr(oldpos));
      }
      
      while(it!=mBuff.end())
      {
        result.append(*it);
        ++it;
      }
      mBuff.clear();
      return result;
    }
  };
}



#endif	/* STRFORMAT_H */

