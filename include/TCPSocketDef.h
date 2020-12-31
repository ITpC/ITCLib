/**
 *  Copyright 2018, Pavel Kraynyukhov <pavel.kraynyukhov@gmail.com>
 * 
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * File:   TCPSocketDef.h
 *
 * Created on 4 Март 2015 г., 2:14
 */

#ifndef TCPSOCKETDEF_H
#define	TCPSOCKETDEF_H

#include <net/NSocket.h>
#include <ClientSocketsFactory.h>

namespace itc
{
	typedef net::Socket<SRV_TCP_ANY_IF,1000> ServerSocket;
	typedef net::Socket<CLN_TCP_KA_TD> ClientSocket;
	typedef ClientSocketsFactory<CLN_TCP_KA_TD> TCPSocketsFactory;
	typedef TCPSocketsFactory::SharedClientSocketPtrType CSocketSPtr;
  typedef TCPSocketsFactory::ClientSocketType  TCPClientSocketType;
}


#endif	/* TCPSOCKETDEF_H */

