/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: IPv4addrHelper.h 20 Апрель 2015 г. 17:02 pk$
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 **/

#ifndef __IPV4ADDRHELPER_H__
#  define	__IPV4ADDRHELPER_H__

#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netdb.h>
#  include <unistd.h>
#  include <iostream>
#  include <string>
#  include <cstring>
#  include <arpa/inet.h>
#  include <set>
#  include <functional>
#  include <algorithm>

namespace itc
{
  namespace net
  {

    class IPv4AddrHelper
    {
     public:

      explicit IPv4AddrHelper()=default;
      ~IPv4AddrHelper()=default;

      /**
       * @brief converts string IPv4 address or FQDN into set of uint32_t values.
       * A value is an IPv4 address in network order.
       * 
       * @param fqdn - string representation of an IPv4 address or FQDN
       * 
       * @return std::set<uint32_t> set of IPv4 adresses represented as a
       * uint32_t values in network order.
       */
      std::set<uint32_t> str2nset(const std::string& fqdn)
      {
        std::set<uint32_t> retaddrs;
        char hostname[fqdn.length() + 1];
        hostname[fqdn.length()] = 0;

        memcpy(hostname, fqdn.c_str(), fqdn.length());
        struct addrinfo* aiptr;
        struct addrinfo* save;
        getaddrinfo(hostname, NULL, NULL, &aiptr);
        save = aiptr;

        while(aiptr)
        {
          uint32_t ipaddr = (((struct sockaddr_in*)(aiptr->ai_addr))->sin_addr.s_addr);

          retaddrs.insert(ipaddr);
          aiptr = aiptr->ai_next;
        }
        freeaddrinfo(save);
        return retaddrs;
      }

      /**
       * @brief converts the ip address from uint32_t network order representation
       * into a c++ string. No DNS checks are involved. It is merely a conversion.
       * 
       * @param ipaddr network order representation of an IPv4 address.
       * @return std::string value representing a dot separated 4 octets of an IPv4 address .
       */
      const std::string ipv4toStr(const uint32_t& ipaddr)
      {
        char addr[16];
        memset(addr,0,16);
        inet_ntop(AF_INET,&ipaddr,addr,16);
        return addr;
      }
      
      /**
       * @brief converts uint32_t value representing an IPv4 address in network
       * order into a string representation of this address.  A DNS lookup is
       * involved. 
       * 
       * @param addr an uint32_t value in network order
       * @return std::string - a string represening the IPv4 address
       * @exception std::logic_error with getnameinfo error.
       */
      const std::string n2str(const uint32_t& addr)
      {
        sockaddr_in sa;
        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = addr;
        memset(sa.sin_zero, 0, 8);
        char hostname[65];
        memset(hostname, 0, 65);
        int ret = getnameinfo((struct sockaddr*)&sa, sizeof(sa), hostname, 64, NULL, 0, NI_NAMEREQD);

        if(ret == 0)
        {
          return hostname;
        }
        throw std::logic_error(gai_strerror(ret));
      }
    };
  }
}

#endif	/* __IPV4ADDRHELPER_H__ */

