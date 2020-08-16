/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: ISocket.h December 16, 2017 5:07 PM $
 * 
 * Author: Pavel Kraynyukhov <pavel.kraynyukhov@gmail.com>
 * 
 **/

#ifndef __ISOCEKT_H__
#  define __ISOCEKT_H__

namespace itc
{
  namespace abstract
  {
    /**
     * @brief socket interface
     **/
    class ISocket
    {
     public:
      virtual void open(const std::string& address, const int port) = 0;
      virtual void open(const char* address, const int port) = 0;
      virtual void getpeeraddr(uint32_t& out) = 0;
      virtual void getpeeraddr(std::string& out) = 0;
      virtual int read(uint8_t* inBuffer, unsigned buffSize) = 0;
      virtual int peek(uint8_t* inBuffer, unsigned buffSize) = 0;
      virtual int write(const uint8_t* outBuffer, unsigned buffSize) = 0;
      virtual bool isValid() const = 0;
      virtual const SOCKET getfd() const = 0;
      virtual void setfd(const SOCKET& sock) = 0;
      virtual void close() = 0;

      virtual ~ISocket()=default;
    };
  }
}


#endif /* __ISOCEKT_H__ */

