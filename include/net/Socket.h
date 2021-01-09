/**
 * Copyright Pavel Kraynyukhov 2021.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: Socket.h 83 2021-01-07 07:50:00Z pk $
 * 
 * this is the refactoring of sys/NSocket.h
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 **/

#ifndef __SOCKET_HPP__
#  define __SOCKET_HPP__

#  if !defined(_MSC_VER) && (!defined(__MINGW32_VERSION))
#    define INVALID_SOCKET (-1)
#    define SOCKET_ERROR (-1)
typedef int SOCKET;
#  endif

#include <iostream>

#include <compat_types.h>
#include <memory>
#include <string>
#include <Val2Type.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fmt/core.h>
#include <system_error>
#include <sys/types.h>
#include <ifaddrs.h>
#include <net/if.h>

namespace itc
{
  namespace net
  {
    enum class Side: uint8_t      { UNDEFINED, SERVER, CLIENT };
    enum class Transport: uint8_t { UNDEFINED, TCP, UDP };
    
    /**
     *@brief Socket wrapper class. 
     * windows compatibility removed.
     * IPV6 support added.
     * 
     **/
    class Socket
    {
     private:
      // no need to keep addr structures or port information.
      SOCKET    mSocket;
      Side      mSide;
      Transport mTransport;

      /**
       * @brief should be used only with AF_INET6
       **/
      const int get_iface_index(struct sockaddr_in6 *addr)
      {
        const int family=AF_INET6;
        struct ifaddrs *ifaddr;
        int ret=-1;
        
        if(getifaddrs(&ifaddr) == -1)
        {
          ITC_ERROR(__FILE__,__LINE__,"Error on acquiring network interfaces information {}",strerror(errno));
          return ret;
        }
        
        for(auto *ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next)
        {
           if (ifa->ifa_addr == nullptr)
               continue;

           if(family == ifa->ifa_addr->sa_family)
           {
             uint8_t *fndptr=(addr->sin6_addr.__in6_u.__u6_addr8);
             uint8_t *lstptr=(((sockaddr_in6*)(ifa->ifa_addr))->sin6_addr.__in6_u.__u6_addr8);
             
             
             if(memcmp(fndptr,lstptr,16) != 0)
               continue;
             
             ret=if_nametoindex(ifa->ifa_name);
             if(ret == 0)
             {
               ret=-1;
               ITC_ERROR(__FILE__,__LINE__,"Error [{}] on acquiring network interfaces index for interface {}",strerror(errno),ifa->ifa_name);
             }
             
             break;
           }
        }
           
        freeifaddrs(ifaddr);
        return ret;
      }

      template <Transport TTransport=Transport::UNDEFINED, Side TSide=Side::UNDEFINED>
      void fillout_hints(const char* address, struct addrinfo& hints)
      {
        static_assert(TSide != Side::UNDEFINED,"Undefined designation {SERVER|CLIENT} in fillout_hints");
        static_assert(TTransport != Transport::UNDEFINED,"Undefined transport {TCP|UDP} in fillout_hints");
        
        struct in_addr ipv4addr;
        struct in6_addr ipv6addr;
        memset(&hints, 0, sizeof(hints));

        hints.ai_flags = AI_CANONNAME; // default
        hints.ai_family = AF_UNSPEC;
        
        if(TTransport == Transport::TCP)
        {
          hints.ai_protocol = IPPROTO_TCP;
          hints.ai_socktype = SOCK_STREAM;
        }
        else
        {
          hints.ai_protocol = IPPROTO_UDP;
          hints.ai_socktype = SOCK_DGRAM;
        }

          // fill out the hints
          if((inet_pton(AF_INET, address, &ipv4addr)) == 1) // is IPv4
          {
            hints.ai_family = AF_INET;
            hints.ai_flags = AI_NUMERICSERV|AI_NUMERICHOST;
          }else if((inet_pton(AF_INET6, address, &ipv6addr)) == 1){
            hints.ai_family = AF_INET6;
            hints.ai_flags = AI_NUMERICSERV|AI_NUMERICHOST;
          } // otherwise hints.ai_flags stays AI_CANONNAME 
      }
      
     public:

      void setKeepAlive()
      {
        size_t on=1;
        if(setsockopt(mSocket,SOL_SOCKET,SO_KEEPALIVE,&on,sizeof(on))==INVALID_SOCKET)
        {
          throw std::system_error(errno, std::system_category(), "Socket::setKeepAlive(), - can't set keepalive socket option");
        }
      }
      
      void unsetKeepAlive()
      {
        size_t off=0;
        if(setsockopt(mSocket,SOL_SOCKET,SO_KEEPALIVE,&off,sizeof(off))==INVALID_SOCKET)
        {
          throw std::system_error(errno, std::system_category(), "Socket::unsetKeepAlive(), - can't unset keepalive socket option");
        }
      }

      void setNagelOn(int proto)
      {
        size_t on=1;
        if(setsockopt(mSocket,proto,TCP_NODELAY,&on,sizeof(on))==INVALID_SOCKET)
        {
          throw std::system_error(errno, std::system_category(), "Socket::setNagelOff(), - can't turn on Nagel's protocol socket option");
        }
      }
      
      void setNagelOff(int proto)
      {
        size_t off=0;
        if(setsockopt(mSocket,proto,TCP_NODELAY,&off,sizeof(off))==INVALID_SOCKET)
        {
          throw std::system_error(errno, std::system_category(), "Socket::setNagelOff(), - can't turn off Nagel's protocol socket option");
        }
      }

      void setReuseAddr()
      {
        size_t on=1;
        if(setsockopt(mSocket,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&on,sizeof(on))==INVALID_SOCKET)
        {
            throw std::system_error(errno, std::system_category(), "Socket::ReuseAddr(), - can't reuse address");
        }
      }
      
      const auto getFamily() const
      {
        if(mSocket != INVALID_SOCKET)
        {
          struct sockaddr_storage saddr;
          socklen_t addrlen=sizeof(struct sockaddr_storage);
        
          if(::getsockname(mSocket,(sockaddr*)(&saddr),&addrlen))
            throw std::system_error(errno,std::system_category(),"In Socket::getFamily()::getsockname()");
          return ((sockaddr*)(&saddr))->sa_family;
        }
        throw std::system_error(EINVAL,std::system_category(),"Socket::getFamily() - socket is invalid");
      }
      
      int getpeername(struct sockaddr_storage* saddr)
      {
        if(mSocket == INVALID_SOCKET)
          throw std::system_error(EINVAL, std::system_category(), "Socket::getpeername(), - mSocket is invalid");
        
        socklen_t saddrlen=sizeof(struct sockaddr_storage);

        if(::getpeername(mSocket, (sockaddr*)(saddr), &saddrlen))
          throw std::system_error(errno,std::system_category(),"In Socket::getpeername()");

        switch(((sockaddr*)(saddr))->sa_family)
        {
          case AF_INET6:
            return AF_INET6;
          case AF_INET:
            return AF_INET;
          default:
            throw std::system_error(EAFNOSUPPORT, std::system_category(), "In Socket::getpeername()");
        }
      }
      
      std::vector<uint8_t> getpeerip()
      {
        if(mSocket != INVALID_SOCKET)
        {
          struct sockaddr_storage saddr;
          int family=getpeername(&saddr);

          switch(family)
          {
            case AF_INET6:
            {
              std::vector<uint8_t> out;
              uint8_t *ptr{((sockaddr_in6*)(&saddr))->sin6_addr.__in6_u.__u6_addr8};
              for(size_t i=0;i<16;++i)
              {
                out.push_back(ptr[i]);
              }
              return out;
            }
            break;
            case AF_INET:
            {
              std::vector<uint8_t> out;
              uint8_t *ptr{(uint8_t*)(&(((sockaddr_in*)(&saddr))->sin_addr.s_addr))};
              for(size_t i=0;i<4;++i)
              {
                out.push_back(ptr[i]);
              }
              return out;
            }
            break;
            default:
              throw std::system_error(EAFNOSUPPORT, std::system_category(), "In Socket::getpeerip()");
          }
        } else throw std::system_error(EINVAL, std::system_category(), "Socket::getpeerip(), - mSocket is invalid");
      }
      
      template <Transport TTransport=Transport::UNDEFINED, Side TSide=Side::UNDEFINED>
      void open(const char* address, const uint16_t port,const bool reuse=false,const int backlog=100)
      {
        static_assert(TSide != Side::UNDEFINED,"Can't open a socket with undefined designation {SERVER|CLIENT}");
        
        static_assert(TTransport != Transport::UNDEFINED,"Can't open a socket with undefined transport {TCP|UDP}");
        
        if((TSide == Side::CLIENT)&&(reuse))
        {
          throw std::system_error(EINVAL,std::system_category(),"Can't reuse client side address");
        }
          
        if(address == nullptr)
        {
          throw std::system_error(EINVAL, std::system_category(), "Socket::open(), - nullptr provided in const char* address");
        }
        
        mSide=TSide;
        mTransport=TTransport;
        
        struct addrinfo hints;
        struct addrinfo *ainfo{nullptr};
        std::string strport{fmt::format("{:d}",port)};
        
        fillout_hints<TTransport,TSide>(address,hints);
        
        int ret=0;
        if((ret=getaddrinfo(address, strport.c_str(), &hints, &ainfo)))
        {
          throw std::system_error(ret, std::system_category(), "Socket::create(): getaddrinfo() error");
        }
        
        if(ainfo->ai_family == AF_INET6)
        {
          int idx=get_iface_index((sockaddr_in6*)(ainfo->ai_addr));
          if(idx!=-1)
          {
            ((sockaddr_in6*)(ainfo->ai_addr))->sin6_scope_id=idx;
          }
          else{
            ITC_ERROR(__FILE__,__LINE__,"Can't find interface index: {}",strerror(errno));
          }
        }
        
        if((mSocket = socket(ainfo->ai_family, ainfo->ai_socktype, ainfo->ai_protocol)) == INVALID_SOCKET)
        {
          freeaddrinfo(ainfo);
          throw std::system_error(errno, std::system_category(),"Socket::create() at socket() - can't create");
        }
        
        if(mSide == Side::SERVER)
        {
          if(reuse)
          {
            this->setReuseAddr();
          }
          
          if(::bind(mSocket,ainfo->ai_addr,ainfo->ai_addrlen))
          {
            freeaddrinfo(ainfo);
            throw std::system_error(errno,std::system_category(),"Socket::open()::bind(), - can't bind server socket");
          }
          
          freeaddrinfo(ainfo);
          
          // set default options: No keepalive, Nagel algorithm is on
          if(TTransport == Transport::TCP)
          {
            setNagelOn(IPPROTO_TCP);  
          }
          unsetKeepAlive();
          
          if(::listen(mSocket,backlog)==INVALID_SOCKET)
          {
            throw std::system_error(errno, std::system_category(), "Socket::Listen(), - can't listen on server socket");
          }
        }
        else if(TTransport == Transport::TCP) // attempt to connect
        {
          int  ret{0};
          bool success{false};
          auto save{ainfo};
          
          do{
            ret=::connect(mSocket, ainfo->ai_addr, ainfo->ai_addrlen);
            if(ret < 0)
            {
              if(ainfo->ai_next)
                ainfo=ainfo->ai_next;
              else break;
            }
            else{
              success=true;
              break;
            }
          }while(true);
          
          freeaddrinfo(save);
          
          if(!success)
          {
            throw std::system_error(errno, std::system_category(), "Socket::open() on connect, - can't connect: ");
          }
        }        

      }
            
      uint32_t getpeernamev4()
      {
        struct sockaddr_storage saddr;
        auto family{getpeername(&saddr)};
        switch(family)
        {
          case AF_INET:
            return ((sockaddr_in*)(&saddr))->sin_addr.s_addr;
          default:
            throw std::system_error(EINVAL,std::system_category(),"Not a an AF_INET socket");
        }
      }
      
      std::pair<std::string,uint16_t> getpeerendpoint()
      {
        if(mSocket != INVALID_SOCKET)
        {
          std::string             str_addr;
          uint16_t                port;
          struct sockaddr_storage saddr;
          
          auto family{getpeername(&saddr)};
          
          switch(family)
          {
            case AF_INET:
              str_addr.resize(16); // max is 15, +1 to store 0
              if(inet_ntop(AF_INET, &(((sockaddr_in*)(&saddr))->sin_addr), str_addr.data(), str_addr.size()))
              {
                port=ntohs(((sockaddr_in*)(&saddr))->sin_port);
              }
              else {
                throw std::system_error(
                  errno,
                  std::system_category(),
                  "In Socket::getpeerendpoint(), error in inet_ntop for IPv4 address:"
                );
              }
              break;
            case AF_INET6:
              str_addr.resize(46); // max is 45, +1 to store 0
              if(inet_ntop(AF_INET6, &(((sockaddr_in6*)(&saddr))->sin6_addr), str_addr.data(), str_addr.size()))
              {
                port=ntohs(((sockaddr_in6*)(&saddr))->sin6_port);
              }else{
                throw std::system_error(
                  errno,std::system_category(),
                "In Socket::getpeerendpoint(), - error in inet_ntop for IPv6 address:"
                );
              }
              break;
            default:
              throw std::system_error(EAFNOSUPPORT,std::system_category(),"In Socket::getpeerendpoint(): ");
          }
          return {str_addr,port};
        }else{
          throw std::system_error(EINVAL, std::system_category(), "In Socket::getpeerendpoint() this->mSocket is invalid" );
        }
      }
      
      std::shared_ptr<Socket> accept() // obsolets ClientSocketsFactory
      {
        if(mSocket == INVALID_SOCKET)
          throw std::system_error(EBADR,std::system_category(),"Socket::accept() - mSocket is invalid");
        
        struct sockaddr_storage saddr;
        socklen_t saddrlen{sizeof(sockaddr_storage)};
        
        std::shared_ptr<Socket> clientSocket=std::make_shared<Socket>();
        clientSocket->mSide=Side::CLIENT;
        clientSocket->mTransport=Transport::TCP;
       
        clientSocket->mSocket = ::accept(mSocket, (sockaddr*)(&saddr), &saddrlen);
        
        if(clientSocket->mSocket == INVALID_SOCKET)
          throw std::system_error(errno,std::system_category(),"Can't accept inbound connection in Socket::accept(), reason:");
        
        return clientSocket;
      }
     
    /** 
     * @brief exposing recv(2)
     * 
     **/
    auto recv(void *buf, size_t len, const int flags=MSG_NOSIGNAL)
    {
      return ::recv(mSocket,buf,len,flags);
    }
    
    /** 
     * @brief exposing send(2)
     * 
     **/
    auto send(void *buf, size_t len, const int flags=MSG_NOSIGNAL)
    {
      return ::send(mSocket,buf,len,flags);
    }

    Socket()
    : mSocket{-1}, mSide{Side::UNDEFINED},
      mTransport{Transport::UNDEFINED}
    {
    }

    Socket(const Socket& ref)
    : mSocket{ref.mSocket}, mSide{ref.mSide},
      mTransport{ref.mTransport}
    {
    }

    Socket(const SOCKET& ref)
    : mSocket{ref}, mSide{Side::UNDEFINED},
      mTransport{Transport::UNDEFINED}
    {
      setfd(ref);
    }

    virtual ~Socket()
    {
      this->close();
    }
        
    Socket& operator=(const Socket& ref)
    {
      mSocket=ref.mSocket;
      mSide=ref.mSide;
      mTransport=ref.mTransport;
      return (*this);
    }

    bool isValid() const
    {
      return(mSocket != INVALID_SOCKET);
    }

    const SOCKET getfd() const
    {
      return mSocket;
    }
    
    void setfd(const SOCKET& sock)
    {
      if(sock == INVALID_SOCKET)
      {
        throw std::system_error(errno, std::system_category(), "Socket::setfd(sock), - sock is invalid");
      }
      
      mSocket = sock;
      
      // validate it is a socket
      
      struct sockaddr_storage addr;
      socklen_t addr_len{INET6_ADDRSTRLEN};
      
      if(::getsockname(mSocket, ((sockaddr*)(&addr)), &addr_len) == INVALID_SOCKET)
      {
        this->close();
        throw std::system_error(errno, std::system_category(), "Socket::setfd():getsockname():");
      }      
    }

    void close()
    {
      if(mSocket != INVALID_SOCKET)
      {
        shutdown(mSocket, SHUT_RDWR);
        ::close(mSocket);
        mSocket = INVALID_SOCKET;
      }
    }
  };
}
}
#endif

