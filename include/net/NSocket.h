/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: NSocket.h 83 2007-07-12 12:59:06Z acs/pk $
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

#  include <compat_types.h>

#  include <memory>

#  include <string>
#  include <Val2Type.h>
#  include <unistd.h>
#  if (defined(_MSC_VER) || defined(__MINGW32_VERSION))
#    include <winsock2.h>
#    include <ws2tcpip.h>
#    define DWORD   uint32_t
#  else
#    include <netdb.h>
#    include <sys/types.h>
#    include <sys/socket.h>
#    include <netinet/tcp.h>
     #include <netinet/in.h>
     #include <arpa/inet.h>
#  endif

#include <abstract/ISocket.h>

#  define CLIENT_SOCKET           (uint64_t)0x08000000ULL
#  define SERVER_SOCKET           (uint64_t)0x80000000ULL
#  define SRV_INADDR_A            (uint64_t)0x00000004ULL

#  define OPT_IPPROTO_TCP         (uint64_t)0x00000010ULL
#  define OPT_IPPROTO_UDP         (uint64_t)0x00000020ULL

#  define OPT_KEEPALIVE           (uint64_t)0x00000040ULL
#  define OPT_TCP_NODELAY         (uint64_t)0x00000100ULL

#  define OPT_SOCK_DAGRAM         (uint64_t)0x00000800ULL
#  define OPT_SOCK_STREAM         (uint64_t)0x00001000ULL


#  define SRV_TCP_ANY_IF  (SERVER_SOCKET|OPT_SOCK_STREAM|OPT_IPPROTO_TCP|SRV_INADDR_A)
#  define SRV_TCP_UNI_IF  (SERVER_SOCKET|OPT_SOCK_STREAM|OPT_IPPROTO_TCP)
#  define SRV_UDP_NKA     (SERVER_SOCKET|OPT_SOCK_DAGRAM|OPT_IPPROTO_UDP)

#  define CLN_TCP_KA_TND  (CLIENT_SOCKET|OPT_SOCK_STREAM|OPT_IPPROTO_TCP|OPT_KEEPALIVE|OPT_TCP_NODELAY)
#  define CLN_TCP_KA_TD   (CLIENT_SOCKET|OPT_SOCK_STREAM|OPT_IPPROTO_TCP|OPT_KEEPALIVE)
#  define CLN_TCP_NKA_TND (CLIENT_SOCKET|OPT_SOCK_STREAM|OPT_IPPROTO_TCP|OPT_TCP_NODELAY)
#  define CLN_TCP_NKA_TD  (CLIENT_SOCKET|OPT_SOCK_STREAM|OPT_IPPROTO_TCP)
#  define CLN_UDP_KA      (CLIENT_SOCKET|OPT_SOCK_DAGRAM|OPT_IPPROTO_UDP|OPT_KEEPALIVE)
#  define CLN_UDP_NKA     (CLIENT_SOCKET|OPT_SOCK_DAGRAM|OPT_IPPROTO_UDP)


namespace itc
{
  namespace net
  {
    /**
     *@brief Socket attributes
     **/
    struct SockMemberAttr
    {
      SOCKET mSocket;
      int mPort;
      struct sockaddr mAddr;
      socklen_t mSockAL; // sockaddr length

      explicit SockMemberAttr()
        : mSocket(INVALID_SOCKET), mSockAL(sizeof(struct sockaddr))
      {
      }

      explicit SockMemberAttr(const SockMemberAttr & ref)
        : mSocket(ref.mSocket), mPort(ref.mPort), mSockAL(sizeof(ref.mAddr))
      {
        memcpy(&mAddr, &(ref.mAddr), mSockAL);
      }

      const SockMemberAttr & operator=(const SockMemberAttr & ref)
      {
        mSocket = ref.mSocket;
        mPort = ref.mPort;
        mSockAL = sizeof(ref.mAddr);
        memcpy(&mAddr, &(ref.mAddr), mSockAL);
        return *this;
      }
      ~SockMemberAttr()
      {
        mSocket=-1;
      }
    };

    /**
     *@brief Socket template. 
     **/
    template < uint64_t SockOptions, int ListenQueueLength = 0 > class Socket
    : public ::itc::abstract::ISocket, public SockMemberAttr
    {
     private:

      void create(const char* address, struct addrinfo* hints, int port)
      {
#  if defined(_MSC_VER) || defined(__MINGW32_VERSION)
        static WORD wVersionRequested = MAKEWORD(2, 0);
        static WSADATA wsaData;
#  endif
        struct addrinfo *res0;
        char strport[21] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        if(hints == NULL)
        {
          throw std::system_error(EINVAL,std::system_category(),"Socket::create(): Invalid hints are provided");
          //ITCException(exceptions::NetworkException, exceptions::BadSocketHints);
        }

#  if defined(_MSC_VER) || defined(__MINGW32_VERSION)
        if(int err = WSAStartup(wVersionRequested, &wsaData))
        {
          WSACleanup();
          throw std::system_error(ret, std::generic_category(),"Socket::create(): WSAStartup() error");
        }
#  endif

        snprintf(strport, 20, "%u", port);

        if(int ret = getaddrinfo(address, strport, hints, &res0))
        {
          throw std::system_error(ret, std::system_category(), "Socket::create(): getaddrinfo() error");
        }

        if((mSocket = socket(res0->ai_family, res0->ai_socktype, res0->ai_protocol)) == INVALID_SOCKET)
        {
          freeaddrinfo(res0);
#  if defined(_MSC_VER) || defined(__MINGW32_VERSION)
          throw std::system_error(WSAGetLastError(), std::generic_category(),"Socket::create() at socket() - can't create");
#  else
          throw std::system_error(errno, std::system_category(),"Socket::create() at socket() - can't create");
#  endif
        }
        mSockAL = res0->ai_addrlen;
        memcpy(&mAddr, res0->ai_addr, mSockAL);
        freeaddrinfo(res0);
      }

#  define setKeepAlive() \
            { \
                size_t on=1; \
                if(setsockopt(mSocket,SOL_SOCKET,SO_KEEPALIVE,&on,sizeof(on))==INVALID_SOCKET) \
                {\
                        throw std::system_error(errno, std::system_category(), "Socket::setKeepAlive(), - can't set socket option");\
                }\
            }

#  define  setNagelOff(proto) {\
                size_t on=1;\
                if(setsockopt(mSocket,proto,TCP_NODELAY,&on,sizeof(on))==INVALID_SOCKET){ \
                           throw std::system_error(errno, std::system_category(), "Socket::setNagelOff(), - can't set socket option");\
                }\
            }

#  define  ReuseAddr() {\
                size_t on=1;\
                if(setsockopt(mSocket,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&on,sizeof(on))==INVALID_SOCKET){ \
                    throw std::system_error(errno, std::system_category(), "Socket::ReuseAddr(), - can't set socket option");\
                }\
            }

#  define Connect() {\
                if(connect(mSocket,&mAddr,mSockAL)==INVALID_SOCKET)\
                {\
                    throw std::system_error(errno, std::system_category(), "Socket::Connect(), - can't connect");\
                }\
            }

#  define Bind() {\
                if(bind(mSocket,&mAddr,mSockAL)==INVALID_SOCKET)\
                {\
                    throw std::system_error(errno, std::system_category(), "Socket::Bind(), - can't bind socket");\
                }\
            }

#  define Listen() {\
                if(listen(mSocket,ListenQueueLength)==INVALID_SOCKET)\
                {\
                    throw std::system_error(errno, std::system_category(), "Socket::Listen(), - can't listen on socket");\
                }\
            }
     protected:
      void open(const char* address, const int port, const itc::utils::SizeT2Type<CLN_TCP_KA_TND>& fictive)
      {
        struct addrinfo hints;

        if(address == NULL)
        {
          throw std::system_error(errno, std::system_category(), "Socket::open(), - bad IP address");
        }

        memset(&hints, 0, sizeof(hints));

        hints.ai_flags = AI_CANONNAME;
        hints.ai_family = AF_UNSPEC;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_socktype = SOCK_STREAM;

        create(address, &hints, port);
        setKeepAlive();
        setNagelOff(IPPROTO_TCP);
        Connect();
      }
      
      void open(const char* address, const int port, const itc::utils::SizeT2Type<CLN_TCP_KA_TD>& fictive)
      {
        struct addrinfo hints;

        if(address == NULL)
        {
          throw std::system_error(errno, std::system_category(), "Socket::open(), - bad IP address");
        }

        memset(&hints, 0, sizeof(hints));

        hints.ai_family = AF_UNSPEC;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_socktype = SOCK_STREAM;

        create(address, &hints, port);
        setKeepAlive();
        Connect();
      }

      void open(const char* address, const int port, const itc::utils::SizeT2Type<CLN_TCP_NKA_TND>& fictive)
      {
        struct addrinfo hints;

        if(address == NULL)
        {
          throw std::system_error(errno, std::system_category(), "Socket::open(), - bad IP address");
        }

        memset(&hints, 0, sizeof(hints));

        hints.ai_flags = AI_CANONNAME;
        hints.ai_family = AF_UNSPEC;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_socktype = SOCK_STREAM;

        create(address, &hints, port);
        setNagelOff(IPPROTO_TCP);
        Connect();
      }

      void open(const char* address, const int port, const itc::utils::SizeT2Type<CLN_TCP_NKA_TD>& fictive)
      {
        struct addrinfo hints;

        if(address == NULL)
        {
          throw std::system_error(errno, std::system_category(), "Socket::open(), - bad IP address");
        }

        memset(&hints, 0, sizeof(hints));
        hints.ai_flags = AI_CANONNAME;
        hints.ai_family = AF_UNSPEC;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_socktype = SOCK_STREAM;

        create(address, &hints, port);
        Connect();
      }

      void open(const char* address, const int port, const itc::utils::SizeT2Type<SRV_TCP_ANY_IF>& fictive)
      {
        struct addrinfo hints;

        memset(&hints, 0, sizeof(hints));

        hints.ai_flags = AI_NUMERICSERV;
        hints.ai_family = AF_UNSPEC;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_socktype = SOCK_STREAM;

        create(address, &hints, port);
        ReuseAddr();
        Bind();
        setKeepAlive();
        setNagelOff(IPPROTO_TCP);
        Listen();
      }


      void open(const std::string& address, const int port)
      {
        open(address.c_str(), port, mSockOptions);
      }

      void open(const char* address, const int port)
      {
        open(address, port, mSockOptions);
      }
      
     private:
      void gpnm(std::string& out)
      {
        
        if(mSocket != INVALID_SOCKET)
        {
          struct sockaddr_in peer;
          socklen_t paddrlen; 

          paddrlen = sizeof(struct sockaddr);
          if(::getpeername(mSocket, (struct sockaddr*)(&peer), &paddrlen))
          {
            out.resize(0);
            throw std::system_error(errno,std::system_category(),"Socket::gpnm(string&) - exception on getpeername()");
          }
          
          out=inet_ntoa(peer.sin_addr);
          
        }else{
          throw std::system_error(EINVAL, std::system_category(), "Socket::gpnm(string&) - this socket is invalid" );
        }
      }

      void gpnm(uint32_t& out)
      {
        if(mSocket != INVALID_SOCKET)
        {
          struct sockaddr_in saddr;
          socklen_t saddrlen;

          saddrlen = sizeof(saddr);

          if(::getpeername(mSocket, (struct sockaddr*)(&saddr), &saddrlen))
          {
            throw std::system_error(errno,std::system_category(),"Socket::gpnm(uint32_t&) - exception on getpeername()");
          }

          out = saddr.sin_addr.s_addr;
        } else throw std::system_error(errno, std::system_category(), "Socket::gpnm(), - Invalid server Socket");
      }

      void getpeeraddr(std::string& out, const itc::utils::SizeT2Type<SRV_TCP_ANY_IF>& fictive)
      {
      }

      void getpeeraddr(std::string& out, const itc::utils::SizeT2Type<SRV_TCP_UNI_IF>& fictive)
      {
      }


      void getpeeraddr(uint32_t& out, const itc::utils::SizeT2Type<SRV_TCP_ANY_IF>& fictive)
      {
      }

      void getpeeraddr(uint32_t& out, const itc::utils::SizeT2Type<SRV_TCP_UNI_IF>& fictive)
      {
      }

      void getpeeraddr(std::string& out, const itc::utils::SizeT2Type<CLN_TCP_NKA_TD>& fictive)
      {
        gpnm(out);
      }

      void getpeeraddr(std::string& out, const itc::utils::SizeT2Type<CLN_TCP_NKA_TND>& fictive)
      {
        gpnm(out);
      }

      void getpeeraddr(std::string& out, const itc::utils::SizeT2Type<CLN_TCP_KA_TD>& fictive)
      {
        gpnm(out);
      }

      void getpeeraddr(std::string& out, const itc::utils::SizeT2Type<CLN_TCP_KA_TND>& fictive)
      {
        gpnm(out);
      }

      void getpeeraddr(std::string& out, const itc::utils::SizeT2Type<CLN_UDP_KA>& fictive)
      {
        gpnm(out);
      }

      void getpeeraddr(std::string& out, const itc::utils::SizeT2Type<CLN_UDP_NKA>& fictive)
      {
        gpnm(out);
      }

      void getpeeraddr(uint32_t& out, const itc::utils::SizeT2Type<CLN_TCP_NKA_TD>& fictive)
      {
        gpnm(out);
      }

      void getpeeraddr(uint32_t& out, const itc::utils::SizeT2Type<CLN_TCP_NKA_TND>& fictive)
      {
        gpnm(out);
      }

      void getpeeraddr(uint32_t& out, const itc::utils::SizeT2Type<CLN_TCP_KA_TD>& fictive)
      {
        gpnm(out);
      }

      void getpeeraddr(uint32_t& out, const itc::utils::SizeT2Type<CLN_TCP_KA_TND>& fictive)
      {
        gpnm(out);
      }

      void getpeeraddr(uint32_t& out, const itc::utils::SizeT2Type<CLN_UDP_KA>& fictive)
      {
        gpnm(out);
      }

      void getpeeraddr(uint32_t& out, const itc::utils::SizeT2Type<CLN_UDP_NKA>& fictive)
      {
        gpnm(out);
      }

      /**
       * Accepts incoming connections and set the Socket with appropriate FD and sockaddr
       *
       * @note  This method is Serverside, MultiHost and TCP only.
       *           defined for KEEP_ALIVE and TCP_NODELAY options
       * @param ref - Client socket casted to SockMemberAttr
       * @param fictive - a fictive definition for compile time check of the server socket properties.
       * @return 0 on success  errno on fail
       *
       **/
      int accept(Socket<CLN_TCP_KA_TND>& ref, const itc::utils::SizeT2Type<SRV_TCP_ANY_IF>& fictive)
      {
        int err = 0;

        ref.mSocket = ::accept(mSocket, &(ref.mAddr), &(ref.mSockAL));

        if(ref.mSocket == INVALID_SOCKET)
        {
#  if defined(_MSC_VER)|| defined(__MINGW32_VERSION)
          err = WSAGetLastError();
#  else
          err = errno;
#  endif
        }
        return err;
      }

      /**
       * Accepts incoming connections and set the Socket with appropriate FD and sockaddr
       *
       * @note  This method is Serverside, MultiHost and TCP only.
       *           defined for KEEP_ALIVE and TCP_NODELAY options
       * @param ref - Client socket casted to SockMemberAttr
       * @param fictive - a fictive definition for compile time check of the server socket properties.
       * @return 0 on success  errno on fail
       *
       **/
      int accept(std::shared_ptr< Socket<CLN_TCP_KA_TND> >& ref, const itc::utils::SizeT2Type<SRV_TCP_ANY_IF>& fictive)
      {
        int err = 0;

        ref.get()->mSocket = ::accept(mSocket, &(ref.get()->mAddr), &(ref.get()->mSockAL));

        if(ref.get()->mSocket == INVALID_SOCKET)
        {
#  if defined(_MSC_VER)|| defined(__MINGW32_VERSION)
          err = WSAGetLastError();
#  else
          err = errno;
#  endif
        }
        return err;
      }
      
      int accept(std::shared_ptr< Socket<CLN_TCP_NKA_TD> >& ref, const itc::utils::SizeT2Type<SRV_TCP_ANY_IF>& fictive)
      {
        int err = 0;

        ref.get()->mSocket = ::accept(mSocket, &(ref.get()->mAddr), &(ref.get()->mSockAL));

        if(ref.get()->mSocket == INVALID_SOCKET)
        {
#  if defined(_MSC_VER)|| defined(__MINGW32_VERSION)
          err = WSAGetLastError();
#  else
          err = errno;
#  endif
        }
        return err;
      }
/**========================**/
/**
       * Accepts incoming connections and set the Socket with appropriate FD and sockaddr
       *
       * @note  This method is Serverside, MultiHost and TCP only.
       *           defined for KEEP_ALIVE option
       * @param ref - Client socket casted to SockMemberAttr
       * @param fictive - a fictive definition for compile time check of the server socket properties.
       * @return 0 on success  errno on fail
       *
       **/
      int accept(Socket<CLN_TCP_KA_TD>& ref, const itc::utils::SizeT2Type<SRV_TCP_ANY_IF>& fictive)
      {
        int err = 0;

        ref.mSocket = ::accept(mSocket, &(ref.mAddr), &(ref.mSockAL));

        if(ref.mSocket == INVALID_SOCKET)
        {
#  if defined(_MSC_VER)|| defined(__MINGW32_VERSION)
          err = WSAGetLastError();
#  else
          err = errno;
#  endif
        }
        return err;
      }
      
      int accept(Socket<CLN_TCP_NKA_TD>& ref, const itc::utils::SizeT2Type<SRV_TCP_ANY_IF>& fictive)
      {
        int err = 0;

        ref.mSocket = ::accept(mSocket, &(ref.mAddr), &(ref.mSockAL));

        if(ref.mSocket == INVALID_SOCKET)
        {
#  if defined(_MSC_VER)|| defined(__MINGW32_VERSION)
          err = WSAGetLastError();
#  else
          err = errno;
#  endif
        }
        return err;
      }

      /**
       * Accepts incoming connections and set the Socket with appropriate FD and sockaddr
       *
       * @note  This method is Serverside, MultiHost and TCP only.
       *           defined for KEEP_ALIVE option
       * @param ref - Client socket casted to SockMemberAttr
       * @param fictive - a fictive definition for compile time check of the server socket properties.
       * @return 0 on success  errno on fail
       *
       **/
      int accept(std::shared_ptr< Socket<CLN_TCP_KA_TD> >& ref, const itc::utils::SizeT2Type<SRV_TCP_ANY_IF>& fictive)
      {
        int err = 0;

        ref.get()->mSocket = ::accept(mSocket, &(ref.get()->mAddr), &(ref.get()->mSockAL));

        if(ref.get()->mSocket == INVALID_SOCKET)
        {
#  if defined(_MSC_VER)|| defined(__MINGW32_VERSION)
          err = WSAGetLastError();
#  else
          err = errno;
#  endif
        }
        return err;
      }
/**==========================*/
      /**
       * Accepts incoming connections and set the Socket with appropriate FD and sockaddr
       *
       * @note  This method is Serverside, UniHost and TCP only.
       *        defined for KEEP_ALIVE and TCP_NODELAY options
       * @param ref - Client socket casted to SockMemberAttr
       * @param fictive - a fictive definition for compile time check of the server socket properties.
       * @return 0 on success  errno on fail
       *
       **/
      int accept(Socket<CLN_TCP_KA_TND>& ref, const itc::utils::SizeT2Type<SRV_TCP_UNI_IF>& fictive)
      {
        int err = 0;

        ref.mSocket = ::accept(mSocket, &(ref.mAddr), &(ref.mSockAL));

        if(ref.mSocket == INVALID_SOCKET)
        {
#  if defined(_MSC_VER)|| defined(__MINGW32_VERSION)
          err = WSAGetLastError();
#  else
          err = errno;
#  endif
        }
        return err;
      }

     public:

      ~Socket()
      {
        this->close();
      }

      int accept(Socket<CLN_TCP_NKA_TD>& ref)
      {
        return this->accept(ref, mSockOptions);
      }

      int accept(Socket<CLN_TCP_KA_TND>& ref)
      {
        return this->accept(ref, mSockOptions);
      }

      int accept(std::shared_ptr< Socket<CLN_TCP_KA_TND> >& ref)
      {
        return this->accept(ref, mSockOptions);
      }
      
      int accept(std::shared_ptr< Socket<CLN_TCP_KA_TD> >& ref)
      {
        return this->accept(ref, mSockOptions);
      }
      int accept(std::shared_ptr< Socket<CLN_TCP_NKA_TD> >& ref)
      {
        return this->accept(ref, mSockOptions);
      }

      int accept(Socket<CLN_TCP_KA_TD>& ref)
      {
        return this->accept(ref, mSockOptions);
      }

      int accept(Socket<CLN_TCP_NKA_TND>& ref)
      {
        return this->accept(ref, mSockOptions);
      }

      void getpeeraddr(uint32_t& out)
      {
        this->getpeeraddr(out, mSockOptions);
      }

      void getpeeraddr(std::string& out)
      {
        this->getpeeraddr(out, mSockOptions);
      }

      /**
       * read a number of characters from a socket
       *
       * @param   inBuffer array to receive data
       * @param   buffSize exact length of data to read
       * @return  length read
       */
      int read(uint8_t* inBuffer, unsigned buffSize)
      {
        unsigned length = 0;
        int nRead = 0;

        if((buffSize == 0) || (inBuffer == nullptr))
          return 0;

        if(mSocket == INVALID_SOCKET)
        {
          return INVALID_SOCKET;
        }

        do
        {
          errno = 0;
#  if defined(_MSC_VER) || (defined(__MINGW32_VERSION))
          nRead = recv(static_cast<void*>(&(inBuffer[length])),  buffSize - length, 0);
#  endif
#  if !defined(_MSC_VER) && defined(__CYGWIN__)
          nRead = recv(static_cast<void*>(&(inBuffer[length])),  buffSize - length, MSG_NOSIGNAL);
#  endif
#  if !defined(_MSC_VER) && !defined(__CYGWIN__)&& (!defined(_WIN32))
          nRead = recv(static_cast<void*>(&(inBuffer[length])), buffSize - length, MSG_WAITALL|MSG_NOSIGNAL);
#  endif
          if(nRead <= 0)
          {
            this->close();
            return INVALID_SOCKET;
          }
          length += nRead;
        }while(length != buffSize);

        return length;
      }

      /**
       * peek a number of characters from a socket
       *
       * @param   inBuffer array to receive data
       * @param   buffSize exact length of data to read
       * @return  length read
       */
      int peek(uint8_t* inBuffer, unsigned buffSize)
      {
        unsigned length = 0;
        int nRead = 0;

        if((buffSize == 0) || (inBuffer == 0))
          return 0;

        if(mSocket == INVALID_SOCKET)
          return INVALID_SOCKET;

        do
        {
          errno = 0;
#  if defined(_MSC_VER) || (defined(__MINGW32_VERSION))
          nRead = recv(static_cast<void*>(&(inBuffer[length])), buffSize - length, MSG_PEEK);
#  endif

#  if !defined(_MSC_VER) && defined(__CYGWIN__)
          nRead = recv(static_cast<void*>(&(inBuffer[length])), buffSize - length, MSG_NOSIGNAL | MSG_PEEK);
#  endif
#  if !defined(_MSC_VER) && !defined(__CYGWIN__) && (!defined(_WIN32))
          nRead = recv(static_cast<void*>(&(inBuffer[length])), buffSize - length, MSG_WAITALL | MSG_PEEK|MSG_NOSIGNAL);
#  endif
          if(nRead <= 0)
          {
            this->close();
            return INVALID_SOCKET;
          }
          length += nRead;
        }while(length != buffSize);

        return length;
      }

      /**
       * write a number of characters to a socket
       *
       * @param   outBuffer array of data
       * @param   buffSize exact length of data to write
       * @return  bytes written
       */
      int write(const uint8_t* outBuffer, unsigned buffSize)
      {
        unsigned length = 0;
        int nSent = 0;

        if((buffSize == 0) || (outBuffer == 0)) // isn't it better to throw an exception here and in 2 previous cases ?
          return 0;

        if(mSocket == INVALID_SOCKET)
          return INVALID_SOCKET;

        do
        {
          if((buffSize - length ) == 0) return length;
#  if defined(__linux__) || defined(__FreeBSD__) || defined(__CYGWIN__)
          nSent = send(mSocket, outBuffer+length, buffSize-length, MSG_NOSIGNAL);
#  else
          nSent = send(mSocket, outBuffer+length, buffSize-length, 0);
#  endif
          if(nSent <= 0)
          {
            this->close();
            return INVALID_SOCKET;
          }
          length += nSent;
        }while(length != buffSize);
        return length;
      }
      
      /** 
       * @brief exposing recv(2)
       * 
       **/
      
      size_t recv(void *buf, size_t len, const int flags)
      {
        return ::recv(mSocket,buf,len,flags);
      }

      template <typename T> ssize_t recv(T* out,const int flags)
      {
        return ::recv(mSocket,static_cast<void*>(out),sizeof(T),flags);
      }
      
      /**
       * /brief read from a socket a packet that is sizeof(T) bytes large
       *
       **/
      template <class T> bool read(T *buff)
      {
        return(this->read((uint8_t*) buff, sizeof(T)) != INVALID_SOCKET);
      }

      /**
       * /brief peek from a socket a packet that is sizeof(T) bytes large
       *
       **/
      template <class T> bool peek(T *buff)
      {
        return(this->peek((uint8_t*) buff, sizeof(T)) != INVALID_SOCKET);
      }

      /**
       * /brief write to a socket a packet that is sizeof(T) bytes large
       *
       **/
      template <class T> bool write(const T *buff)
      {
        return(this->write((const uint8_t*) buff, sizeof(T)) != INVALID_SOCKET);
      }

      Socket() : SockMemberAttr()
      {
      }

      Socket(const SockMemberAttr& ref) : SockMemberAttr(ref)
      {
      }

      Socket(const std::string& addr, const int port) : SockMemberAttr()
      {
        this->open(addr, port);
      }

      Socket(const char* addr, const int port) : SockMemberAttr()
      {
        this->open(addr, port);
      }

      Socket(const Socket& ref) : SockMemberAttr((SockMemberAttr) ref)
      {
      }

      Socket(const SOCKET& ref) : SockMemberAttr()
      {
        setfd(ref);
      }

      const Socket operator=(const Socket& ref)
      {
        return *((SockMemberAttr*)this) = (SockMemberAttr) ref;
      }

      bool isValid() const
      {
        return(mSocket != INVALID_SOCKET);
      }

      SOCKET getfd() const
      {
        return mSocket;
      }

      void setfd(const SOCKET& sock)
      {
        if(sock == INVALID_SOCKET)
        {
          throw std::system_error(errno, std::system_category(), "Socket::setfd(), - invalid socket");
        }
        mSocket = sock;
        if(::getsockname(mSocket, &mAddr, &mSockAL) == INVALID_SOCKET)
        {
          int error = errno;
          this->close();
          throw std::system_error(error, std::system_category(), "Socket::setfd():getsockname(), - invalid socket");
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

     private:
      itc::utils::SizeT2Type<SockOptions> mSockOptions;
    };
  }
 }
#endif

