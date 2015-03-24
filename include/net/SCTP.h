/* 
 * File:   SCTP.h
 * Author: pk
 *
 * Created on 5 Август 2014 г., 20:16
 */

#ifndef SCTP_H
#  define	SCTP_H
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <netinet/sctp.h>
#  include <net/NSocket.h>

/**
 * OPTIONS of the SCTP for sysctl:
 * addip_enable = 1 // 0 by default
 * association_max_retrans = 10 // is default
 * max_init_retransmits = 8 // is default
 * path_max_retrans = 5 // is default
 * rcvbuf_policy = 1 // every association has its own buffer, default 0
 * rto_min // 1 is default, 4 should be used for VSAT (this is a speculation)
 * sack_timeout // 200ms is default, must be at least 600ms-750ms for VSAT (not a speculation)
 * sndbuf_policy  // 0 is default, 1 gives separate buffer to every association
 * OPTIONS for the SCTP socket (getsockopt, setsockopt)
 * SCTP_RTOINFO - retransmission timout (struct sctp_rtoinfo)
 * SCTP_ASSOCINFO - varios association params (struct sctp_assocparams)
 * SCTP_INITMSG - protocol parameters for the default association initialization (struct sctp_initmsg)
 * SCTP_NODELAY - turn on/off Nagle-like algorithm on SCTP socket
 * SCTP_AUTOCLOSE - for one-to-many: autoclose sockets those not have been sent or have not sent any
 * packets longer then number of specified seconds. By default it is 0, which effectively turns this option off.
 * SCTP_DISABLE_FRAGMENTS - not 0 will  disable fragmentation (Message > PMTU will be not send and error will
 * be indicated to the user.
 * SCTP_PEER_ADDR_PARAMS  - heartbeat options
 **/

namespace itc
{

  /**
   * abandoned due to lack of time. nothing here.
   **/
  template <int options = 0, int lqueuesize = 0 > class SCTPSocket
  {
   private:
    int listenSocket;
    struct sockaddr_in server_addr;

  };
}


#endif	/* SCTP_H */

