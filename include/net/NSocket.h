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
#  include <ITCError.h>
#  include <ITCException.h>
#  include <boost/shared_ptr.hpp>
#  include <boost/weak_ptr.hpp>

#  include <string>
#  include <Val2Type.h>
#  if (defined(_MSC_VER) || defined(__MINGW32_VERSION))
#    include <winsock2.h>
#    include <ws2tcpip.h>
#    define DWORD   uint32_t
#  else
#    include <netdb.h>
#    include <sys/types.h>
#    include <sys/socket.h>
#    include <netinet/tcp.h>
#    include <netinet/sctp.h>
#  endif

#  define CLIENT_SOCKET           (uint64_t)0x08000000ULL
#  define SERVER_SOCKET           (uint64_t)0x80000000ULL
#  define SRV_INADDR_A            (uint64_t)0x00000004ULL

#  define OPT_IPPROTO_SCTP        (uint64_t)0x00000008ULL
#  define OPT_IPPROTO_TCP         (uint64_t)0x00000010ULL
#  define OPT_IPPROTO_UDP         (uint64_t)0x00000020ULL

#  define OPT_KEEPALIVE           (uint64_t)0x00000040ULL
#  define OPT_SCTP_NODELAY        (uint64_t)0x00000080ULL
#  define OPT_TCP_NODELAY         (uint64_t)0x00000100ULL

#  define OPT_SCTP_PR_SCTP_RTX    (uint64_t)0x00000200ULL
#  define OPT_SCTP_SEQPACKET      (uint64_t)0x00000400ULL
#  define OPT_SOCK_DAGRAM         (uint64_t)0x00000800ULL
#  define OPT_SOCK_STREAM         (uint64_t)0x00001000ULL




#  define CLN_SCTP_UNI    (CLIENT_SOCKET|OPT_SOCK_STREAM|OPT_IPPROTO_SCTP|OPT_SCTP_NODELAY)
#  define CLN_SCTP_MANY   (CLIENT_SOCKET|OPT_SCTP_SEQPACKET|OPT_IPPROTO_SCTP|OPT_SCTP_NODELAY)

#  define SRV_TCP_ANY_IF  (SERVER_SOCKET|OPT_SOCK_STREAM|OPT_IPPROTO_TCP|SRV_INADDR_A)
#  define SRV_TCP_UNI_IF  (SERVER_SOCKET|OPT_SOCK_STREAM|OPT_IPPROTO_TCP)
#  define SRV_UDP_NKA     (SERVER_SOCKET|OPT_SOCK_DAGRAM|OPT_IPPROTO_UDP)
#  define SRV_SCTP_UNI    (SERVER_SOCKET|OPT_SOCK_STREAM|OPT_IPPROTO_SCTP|SRV_INADDR_A|OPT_SCTP_NODELAY)
#  define SRV_SCTP_MANY   (SERVER_SOCKET|OPT_SCTP_SEQPACKET|OPT_IPPROTO_SCTP|SRV_INADDR_A|OPT_SCTP_NODELAY)

#  define CLN_TCP_KA_TND  (CLIENT_SOCKET|OPT_SOCK_STREAM|OPT_IPPROTO_TCP|OPT_KEEPALIVE|OPT_TCP_NODELAY)
#  define CLN_TCP_KA_TD   (CLIENT_SOCKET|OPT_SOCK_STREAM|OPT_IPPROTO_TCP|OPT_KEEPALIVE)
#  define CLN_TCP_NKA_TND (CLIENT_SOCKET|OPT_SOCK_STREAM|OPT_IPPROTO_TCP|OPT_TCP_NODELAY)
#  define CLN_TCP_NKA_TD  (CLIENT_SOCKET|OPT_SOCK_STREAM|OPT_IPPROTO_TCP)
#  define CLN_UDP_KA      (CLIENT_SOCKET|OPT_SOCK_DAGRAM|OPT_IPPROTO_UDP|OPT_KEEPALIVE)
#  define CLN_UDP_NKA     (CLIENT_SOCKET|OPT_SOCK_DAGRAM|OPT_IPPROTO_UDP)

#  define CLN_SCTP_KA_TND_UNI  (CLIENT_SOCKET|OPT_SOCK_STREAM|OPT_IPPROTO_SCTP|OPT_KEEPALIVE|OPT_TCP_NODELAY)
#  define CLN_SCTP_KA_TND_MANY  (CLIENT_SOCKET|OPT_SCTP_SEQPACKET|OPT_IPPROTO_SCTP|OPT_KEEPALIVE|OPT_TCP_NODELAY)

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
      virtual SOCKET getfd() const = 0;
      virtual void setfd(const SOCKET& sock) = 0;
      virtual void close() = 0;

     private:

      ~ISocket()=default;
    };
  }

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
        : mSocket(INVALID_SOCKET), mSockAL(sizeof(mAddr))
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
    };

    /**
     *@brief Socket template. BEWARE SCTP IS NOT WORKING AND NEVER TESTED DUE 
     * TO LACK OF TIME
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
          throw ITCException(exceptions::NetworkException, exceptions::BadSocketHints);
        }

#  if defined(_MSC_VER) || defined(__MINGW32_VERSION)
        if(int err = WSAStartup(wVersionRequested, &wsaData))
        {
          WSACleanup();
          throw ITCException(ret, exceptions::NetworkException);
        }
#  endif

        snprintf(strport, 20, "%u", port);

        if(int ret = getaddrinfo(address, strport, hints, &res0))
        {
          throw ITCException(ret, exceptions::GAI_Exception);
        }

        if((mSocket = socket(res0->ai_family, res0->ai_socktype, res0->ai_protocol)) == INVALID_SOCKET)
        {
          freeaddrinfo(res0);
#  if defined(_MSC_VER) || defined(__MINGW32_VERSION)
          throw ITCException(WSAGetLastError(), exceptions::NetworkException);
#  else
          throw ITCException(errno, exceptions::NetworkException);
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
                        throw ITCException(errno, exceptions::NetworkException);\
                }\
            }

#  define  setNagelOff(proto) {\
                size_t on=1;\
                if(setsockopt(mSocket,proto,TCP_NODELAY,&on,sizeof(on))==INVALID_SOCKET){ \
                           throw ITCException(errno,exceptions::NetworkException);\
                }\
            }

#  define  ReuseAddr() {\
                size_t on=1;\
                if(setsockopt(mSocket,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))==INVALID_SOCKET){ \
                    throw ITCException(errno,exceptions::NetworkException);\
                }\
            }

#  define Connect() {\
                if(connect(mSocket,&mAddr,mSockAL)==INVALID_SOCKET)\
                {\
                    throw ITCException(errno,exceptions::NetworkException);\
                }\
            }

#  define Bind() {\
                if(bind(mSocket,&mAddr,mSockAL)==INVALID_SOCKET)\
                {\
                    throw ITCException(errno,exceptions::NetworkException);\
                }\
            }

#  define Listen() {\
                if(listen(mSocket,ListenQueueLength)==INVALID_SOCKET)\
                {\
                    throw ITCException(errno,exceptions::NetworkException);\
                }\
            }

      void open(const char* address, const int port, const itc::utils::SizeT2Type<CLN_TCP_KA_TND>& fictive)
      {
        struct addrinfo hints;

        if(address == NULL)
        {
          throw ITCException(exceptions::NetworkException, exceptions::BadIPAddress);
        }

        memset(&hints, 0, sizeof(hints));

        hints.ai_flags = AI_CANONNAME;
        hints.ai_family = PF_UNSPEC;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_socktype = SOCK_STREAM;

        create(address, &hints, port);
        setKeepAlive();
        setNagelOff(IPPROTO_TCP);
        Connect();
      }

      void open(const char* address, const int port, const itc::utils::SizeT2Type<CLN_SCTP_KA_TND_UNI>& fictive)
      {
        struct addrinfo hints;

        if(address == NULL)
        {
          throw ITCException(exceptions::NetworkException, exceptions::BadIPAddress);
        }

        memset(&hints, 0, sizeof(hints));

        hints.ai_flags = AI_CANONNAME;
        hints.ai_family = PF_UNSPEC;
        hints.ai_protocol = IPPROTO_SCTP;
        hints.ai_socktype = SOCK_STREAM;

        create(address, &hints, port);
        setKeepAlive();
        setNagelOff(IPPROTO_SCTP);
        Connect();
      }

      void open(const char* address, const int port, const itc::utils::SizeT2Type<CLN_SCTP_KA_TND_MANY>& fictive)
      {
        struct addrinfo hints;

        if(address == NULL)
        {
          throw ITCException(exceptions::NetworkException, exceptions::BadIPAddress);
        }

        memset(&hints, 0, sizeof(hints));

        hints.ai_flags = AI_CANONNAME;
        hints.ai_family = PF_UNSPEC;
        hints.ai_protocol = IPPROTO_SCTP;
        hints.ai_socktype = SOCK_SEQPACKET;

        create(address, &hints, port);
        setKeepAlive();
        setNagelOff(IPPROTO_SCTP);
      }

      void open(const char* address, const int port, const itc::utils::SizeT2Type<CLN_TCP_KA_TD>& fictive)
      {
        struct addrinfo hints;

        if(address == NULL)
        {
          throw ITCException(exceptions::NetworkException, exceptions::BadIPAddress);
        }

        memset(&hints, 0, sizeof(hints));

        hints.ai_family = PF_UNSPEC;
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
          throw ITCException(exceptions::NetworkException, exceptions::BadIPAddress);
        }

        memset(&hints, 0, sizeof(hints));

        hints.ai_flags = AI_CANONNAME;
        hints.ai_family = PF_UNSPEC;
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
          throw ITCException(exceptions::NetworkException, exceptions::BadIPAddress);
        }

        memset(&hints, 0, sizeof(hints));
        hints.ai_flags = AI_CANONNAME;
        hints.ai_family = PF_UNSPEC;
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
        hints.ai_family = PF_UNSPEC;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_socktype = SOCK_STREAM;

        create(address, &hints, port);
        ReuseAddr();
        Bind();
        setKeepAlive();
        setNagelOff(IPPROTO_TCP);
        Listen();
      }

      void open(const char* address, const int port, const itc::utils::SizeT2Type<SRV_SCTP_UNI>& fictive)
      {
        struct addrinfo hints;

        memset(&hints, 0, sizeof(hints));

        hints.ai_flags = AI_NUMERICSERV;
        hints.ai_family = PF_INET;
        hints.ai_protocol = IPPROTO_SCTP;
        hints.ai_socktype = SOCK_STREAM;

        create(address, &hints, port);
        Bind();
        Listen();
      }

      void open(const char* address, const int port, const itc::utils::SizeT2Type<SRV_SCTP_MANY>& fictive)
      {
        struct addrinfo hints;

        memset(&hints, 0, sizeof(hints));

        hints.ai_flags = AI_NUMERICSERV;
        hints.ai_family = PF_INET;
        hints.ai_protocol = IPPROTO_SCTP;
        hints.ai_socktype = SOCK_SEQPACKET;

        create(address, &hints, port);
        Bind();
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

      uint32_t addr2bytes(const char* addr)
      {
        char *p = (char*) addr;
        uint32_t bytes = 0;

        char octets[4][4] = {
          {0, 0, 0, 0},
          {0, 0, 0, 0},
          {0, 0, 0, 0},
          {0, 0, 0, 0}
        };

        size_t it = 0;
        size_t i = 0;
        while(p && (*p))
        {
          if(*p != '.')
          {
            octets[it][i++] = *p;
          }else
          {
            bytes = (bytes << 8) | atoi(octets[it]);
            it++;
            i = 0;
          }
          p++;
        }
        bytes = (bytes << 8) | atoi(octets[it]);
        return bytes;
      }

      void gpnm(std::string& out)
      {
        if(mSocket != INVALID_SOCKET)
        {
          struct sockaddr saddr;
          socklen_t saddrlen;
          char hbuf[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

          saddrlen = sizeof(saddr);
          if(::getpeername(mSocket, &saddr, &saddrlen))
          {
            throw TITCException<exceptions::GAI_Exception>(errno);
          }

          if(int error = ::getnameinfo(&saddr, saddrlen, hbuf, 16, NULL, 0, NI_NUMERICHOST))
          {
            throw ITCException(error, exceptions::GAI_Exception);
          }
          out = std::string(hbuf);
        }
      }

      void gpnm(uint32_t& out)
      {
        if(mSocket != INVALID_SOCKET)
        {
          struct sockaddr saddr;
          socklen_t saddrlen;
          char hbuf[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

          saddrlen = sizeof(saddr);

          if(::getpeername(mSocket, &saddr, &saddrlen))
          {
            throw TITCException<exceptions::GAI_Exception>(errno);
          }

          if(int error = ::getnameinfo(&saddr, saddrlen, hbuf, 16, NULL, 0, NI_NUMERICHOST))
          {
            throw ITCException(error, exceptions::GAI_Exception);
          }
          out = addr2bytes(hbuf);
        }
        else throw ITCException(exceptions::NetworkException, exceptions::InvalidSocketException);
      }

      void getpeeraddr(std::string& out, const itc::utils::SizeT2Type<SRV_TCP_ANY_IF>& fictive)
      {
      }

      void getpeeraddr(std::string& out, const itc::utils::SizeT2Type<SRV_TCP_UNI_IF>& fictive)
      {
      }

      void getpeeraddr(std::string& out, const itc::utils::SizeT2Type<SRV_SCTP_UNI>& fictive)
      {
      }

      void getpeeraddr(std::string& out, const itc::utils::SizeT2Type<SRV_SCTP_MANY>& fictive)
      {
      }

      void getpeeraddr(std::string& out, const itc::utils::SizeT2Type<CLN_SCTP_KA_TND_UNI>& fictive)
      {
        gpnm(out);
      }

      void getpeeraddr(uint32_t& out, const itc::utils::SizeT2Type<SRV_TCP_ANY_IF>& fictive)
      {
      }

      void getpeeraddr(uint32_t& out, const itc::utils::SizeT2Type<SRV_TCP_UNI_IF>& fictive)
      {
      }

      void getpeeraddr(uint32_t& out, const itc::utils::SizeT2Type<SRV_SCTP_UNI>& fictive)
      {
      }

      void getpeeraddr(uint32_t& out, const itc::utils::SizeT2Type<SRV_SCTP_MANY>& fictive)
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
       * Accepts incomming connections and set the Socket with appropriate FD and sockaddr
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
        register int err = 0;

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
       * Accepts incomming connections and set the Socket with appropriate FD and sockaddr
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
        register int err = 0;

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

      int accept(Socket<CLN_SCTP_KA_TND_UNI>& ref, const itc::utils::SizeT2Type<SRV_SCTP_UNI>& fictive)
      {
        register int err = 0;

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

      int accept(Socket<CLN_SCTP_KA_TND_UNI>& ref, const itc::utils::SizeT2Type<SRV_SCTP_MANY>& fictive)
      {
        return -1;
      }

      /**
       * Accepts incomming connections and set the Socket with appropriate FD and sockaddr
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
        register int err = 0;

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
        register unsigned length = 0;
        register int nRead = 0;

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
          nRead = recv(mSocket, &inBuffer[length], buffSize - length, 0);
#  endif
#  if !defined(_MSC_VER) && defined(__CYGWIN__)
          nRead = recv(mSocket, &inBuffer[length], buffSize - length, MSG_NOSIGNAL);
#  endif
#  if !defined(_MSC_VER) && !defined(__CYGWIN__)&& (!defined(_WIN32))
          nRead = recv(mSocket, &inBuffer[length], buffSize - length, MSG_WAITALL);
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
        register unsigned length = 0;
        register int nRead = 0;

        if((buffSize == 0) || (inBuffer == 0))
          return 0;

        if(mSocket == INVALID_SOCKET)
          return INVALID_SOCKET;

        do
        {
          errno = 0;
#  if defined(_MSC_VER) || (defined(__MINGW32_VERSION))
          nRead = recv(mSocket, &inBuffer[length], buffSize - length, MSG_PEEK);
#  endif

#  if !defined(_MSC_VER) && defined(__CYGWIN__)
          nRead = recv(mSocket, &inBuffer[length], buffSize - length, MSG_NOSIGNAL | MSG_PEEK);
#  endif
#  if !defined(_MSC_VER) && !defined(__CYGWIN__) && (!defined(_WIN32))
          nRead = recv(mSocket, &inBuffer[length], buffSize - length, MSG_WAITALL | MSG_PEEK);
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
        register unsigned length = 0;
        register int nSent = 0;

        if((buffSize == 0) || (outBuffer == 0)) // isn't it better to throw an exception here and in 2 priveios cases ?
          return 0;

        if(mSocket == INVALID_SOCKET)
          return INVALID_SOCKET;

        do
        {
#  if defined(__linux__) || defined(__FreeBSD__) || defined(__CYGWIN__)
          nSent = send(mSocket, outBuffer, buffSize, MSG_NOSIGNAL);
#  else
          nSent = send(mSocket, outBuffer, buffSize, 0);
#  endif
          if(nSent <= 0)
          {
            this->close();
            return INVALID_SOCKET;
          }
          length += nSent;
        }while(length != buffSize);
        return nSent;
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

      const Socket & operator=(const Socket& ref)
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
          throw ITCException(exceptions::NetworkException, exceptions::InvalidSocketException);
        }
        mSocket = sock;
        if(::getsockname(mSocket, &mAddr, &mSockAL) == INVALID_SOCKET)
        {
          int error = errno;
          this->close();
          throw ITCException(error, exceptions::NetworkException);
        }
      }

      void close()
      {
        if(mSocket != INVALID_SOCKET)
        {
          shutdown(mSocket, SHUT_RDWR);
          mSocket = INVALID_SOCKET;
        }
      }

     private:
      itc::utils::SizeT2Type<SockOptions> mSockOptions;
    };
  }
}
#endif

