/**
 * Copyright (c) 2007-2015, Pavel Kraynyukhov.
 *  
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without a written agreement
 * is hereby granted under the terms of the General Public License version 2
 * (GPLv2), provided that the above copyright notice and this paragraph and the
 * following two paragraphs and the "LICENSE" file appear in all modified or
 * unmodified copies of the software "AS IS" and without any changes.
 *
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING
 * LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
 * DOCUMENTATION, EVEN IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATIONS TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 * 
 * 
 * $Id: ITCError.cpp 54 2007-05-22 08:36:14Z Pavel Kraynyukhov $
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 **/

#include <errno.h>
#include <string.h>
#if defined(_MSC_VER) || defined(__MINGW32_VERSION)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#endif

#include <ITCException.h>

#if defined(_MSC_VER)
static const char FAR * ITCStrError(int err)
#else

static const char * ITCStrError(int err)
#endif
{
  switch(err)
  {
    case exceptions::Can_not_create_mutex: return "Can't create mutex ";
    case exceptions::Can_not_initialize_mutex: return "Can't init mutex ";
    case exceptions::Can_not_lock_mutex: return "Can't lock mutex ";
    case exceptions::Can_not_unlock_mutex: return "Can't unlock mutex ";
    case exceptions::Can_not_destroy_mutex: return "Can't destroy mutex ";
    case exceptions::Can_not_copy_mutex: return "Can't copy mutex ";

    case exceptions::Can_not_initialize_semaphore: return "Can't init semaphore ";
    case exceptions::Can_not_get_semaphore_value: return "Can't get semaphore value";
    case exceptions::Can_not_wait_on_semaphore: return "Can't wait on semaphore";
    case exceptions::Can_not_post_on_semaphore: return "Can't post on semaphore";
    case exceptions::Can_not_destroy_semaphore: return "Can't destroy semaphore";
    case exceptions::Can_not_copy_semaphore: return "Can't copy semaphore";

    case exceptions::Can_not_create_thread: return "Can't create thread ";

    case exceptions::Can_not_create_key: return "Can't create key, see is a keyfile exists";
    case exceptions::Can_not_create_conditional: return "Can't create conditional object";
    case exceptions::Can_not_create_reusable_object: return "Can't create reusable object";

    case exceptions::Can_not_open_queue: return "MQueue::MQueue(): can't open POSIX Message Queue";

    case exceptions::Timed_wait_out_of_range: return "PosixSemaphore::timedWait(): nanosecond value of timespec is out of range ( is < 0 or > 1000 millions)";
    case exceptions::Timed_wait_deadlock: return "PosixSemaphore::timedWait(): deadlock condition detected";

    case exceptions::NetworkException: return "Network error";

    case exceptions::Can_not_open_socket: return "Can't open socket";
    case exceptions::Can_not_bind_socket: return "Can't bind socket";
    case exceptions::Can_not_listen_on_socket: return "Can't listen on socket";

    case exceptions::InvalidSocketException: return "Given filedescriptor of the socket is invalid (-1)";
    case exceptions::GAI_Exception: return "Socket, getaddrinfo error";
    case exceptions::BadSocketHints: return "Bad socket hints provided for itc::net::Socket::create() method";
    case exceptions::BadIPAddress: return "Bad connection address";
    case exceptions::MutexException: return "Mutex error";
    case exceptions::BufferOverflowAttempt: return "Buffer overflow attempt";
    case exceptions::IndexOutOfRange: return "Array index out of range";
    case exceptions::NullPointerException: return "NullPointerException";
    case exceptions::bz2ConfigError: return "Programmer error. Bz2 confiuration error in call to function BZ2_bzCompressInit().";
    case exceptions::bz2ParamError: return "Programmer error. Bz2 parameters error in call to function BZ2_bzCompressInit(), BZ2bzDecompressInit() or one of BufToBuff functions.";
    case exceptions::bz2MemError: return "BZ2_bzCompressInit() - not enough RAM";
    case exceptions::bz2DataIntegrityError: return "bz2 data integrity error";
    case exceptions::Can_not_assign_runnable: return "Can not assign a runnable object";
    case exceptions::FileNotFound: return "File not found";
    case exceptions::CanNotWrite: return "Can not write to file or directory";
    case exceptions::FileIsAlreadyOpen: return "This file is already open";
    case exceptions::UnsupportedFileLength: return "Unsupported file fength";
    case exceptions::KeyIsNotSet: return "The key isn't set";
    case exceptions::WrongFileFormat: return "Either the file format is wrong, the file is corrupt, or a password is incorrect";
    case exceptions::ExternalLibraryException: return "an external library threw an exception or error a call to library function returned the error";
    case exceptions::InvalidException: return "Invalid exception";
    case exceptions::ApplicationException: return "Application runtime error";
    case exceptions::LuaScriptError: return "Error in the lua script";
    case exceptions::InitializationForbidden: return "Initialization is forbidden";
    case exceptions::NoEPoll: return "epoll_create() error";
    case exceptions::EPollCTLError: return "epoll_ctl() error";
    case exceptions::MDBEAccess: return "this user does not have permission to access the environment files";
    case exceptions::MDBEAgain: return "the environment was locked by another process";
    case exceptions::MDBInvalid: return "the environment file headers are corrupted";
    case exceptions::MDBNotFound: return "the specified database doesn't exist in the environment and MDB_CREATE was not specified";
    case exceptions::MDBVersionMissmatch: return "the version of the LMDB library doesn't match the version that created the database environment";
    case exceptions::MDBClosed: return "the database environment is already closed";
    case exceptions::MDBGeneral: return "LMDB general error";
    case exceptions::MDBMapResized: return "another process wrote data beyond this MDB_env's mapsize and this environment's map must be resized as well. See mdb_env_set_mapsize().";
    case exceptions::MDBPanic: return "a fatal error occurred earlier and the environment must be shut down.";
    case exceptions::MDBReadersFull: return "a read-only transaction was requested and the reader lock table is full. See mdb_env_set_maxreaders().";
    case exceptions::MDBTooMany: return "too many databases have been opened. See mdb_env_set_maxdbs().";
    case exceptions::MDBEInval: return "an invalid parameter was specified, or the environment is already open.";
    case exceptions::ConfigSyntax: return "Syntaxis error in config stream";
    case exceptions::Reflection: return "Error in config reflection";
    case exceptions::UndefinedType: return "Type name is undefined for this typecode";
    case exceptions::ITCGeneral: return "ITCFramework general exception";
    case exceptions::Can_not_begin_txn: return "Transaction may not begin";
    case exceptions::MDBEnvWrong: return "Attempt to interact with extraneous LMDB environment";
    case exceptions::MDBTxnNULL: return "Transaction handle is NULL";
    case exceptions::MDBInvalParam: return "an invalid parameter was specified";
    case exceptions::MDBMapFull: return "the database is full, see mdb_env_set_mapsize().";
    case exceptions::MDBTxnFull: return "the transaction has too many dirty pages.";
    case exceptions::MDBTEAccess: return "an attempt was made to write in a read-only transaction.";
    default:
      return "UNKNOWN ITC ERROR OR EXCEPTION, Exception code is not on the library";
  }
}


#if defined(_MSC_VER)
const char FAR * WSAStrError(int err)
#else

const char * WSAStrError(int err)
#endif
{
#if defined(_MSC_VER) || defined(__MINGW32_VERSION) 
  switch(err)
  {
    case WSAEACCES: return "(10013) Permission denied.  An attempt was made to access a socket in a way forbidden by its access permissions. An example is using a broadcast address for sendto without broadcast permission being set using setsockoptSO_BROADCAST). \n";
    case WSAEADDRINUSE: return "(10048) Address already in use.  Only one usage of each socket address protocol/IP address/port) is normally permitted. This error occurs if an application attempts to bind a socket to an IP address/port that has already been used for an existing socket, or a socket that wasn't closed properly, or one that is still in the process of closing. For server applications that need to bind multiple sockets to the same port number, consider using setsockoptSO_REUSEADDR). Client applications usually need not call bind at all - connect will choose an unused port automatically. When bind is called with a wild-card address involving ADDR_ANY), a WSAEADDRINUSE error could be delayed until the specific address is committed. This could happen with a call to other function later, including connect, listen, WSAConnect or WSAJoinLeaf. \n";
    case WSAEADDRNOTAVAIL: return "(10049) Cannot assign requested address.  The requested address is not valid in its context. Normally results from an attempt to bind to an address that is not valid for the local machine. This can also result from connect, sendto, WSAConnect, WSAJoinLeaf, or WSASendTo when the remote address or port is not valid for a remote machine e.g. address or port 0). \n";
    case WSAEAFNOSUPPORT: return "(10047) Address family not supported by protocol family.  An address incompatible with the requested protocol was used. All sockets are created with an associated address family i.e. AF_INET for Internet Protocols) and a generic protocol type i.e. SOCK_STREAM). This error will be returned if an incorrect protocol is explicitly requested in the socket call, or if an address of the wrong family is used for a socket, e.g. in sendto. \n";
    case WSAEALREADY: return "(10037) Operation already in progress.  An operation was attempted on a non-blocking socket that already had an operation in progress - i.e. calling connect a second time on a non-blocking socket that is already connecting, or canceling an asynchronous request WSAAsyncGetXbyY) that has already been canceled or completed. \n";
    case WSAECONNABORTED: return "(10053) Software caused connection abort.  An established connection was aborted by the software in your host machine, possibly due to a data transmission timeout or protocol error. \n";
    case WSAECONNREFUSED: return "(10061) Connection refused.  No connection could be made because the target machine actively refused it. This usually results from trying to connect to a service that is inactive on the foreign host - i.e. one with no server application running. \n";
    case WSAECONNRESET: return "(10054) Connection reset by peer.  A existing connection was forcibly closed by the remote host. This normally results if the peer application on the remote host is suddenly stopped, the host is rebooted, or the remote host used a 'hard close' see setsockopt for more information on the SO_LINGER option on the remote socket.) This error may also result if a connection was broken due to keep-alive activity detecting a failure while one or more operations are in progress. Operations that were in progress fail with WSAENETRESET. Subsequent operations fail with WSAECONNRESET. \n";
    case WSAEDESTADDRREQ: return "(10039) Destination address required.  A required address was omitted from an operation on a socket. For example, this error will be returned if sendto is called with the remote address of ADDR_ANY. \n";
    case WSAEFAULT: return "(10014) Bad address.  The system detected an invalid pointer address in attempting to use a pointer argument of a call. This error occurs if an application passes an invalid pointer value, or if the length of the buffer is too small. For instance, if the length of an argument which is a struct sockaddr is smaller than sizeofstruct sockaddr). \n";
    case WSAEHOSTDOWN: return "(10064) Host is down.  A socket operation failed because the destination host was down. A socket operation encountered a dead host. Networking activity on the local host has not been initiated. These conditions are more likely to be indicated by the error WSAETIMEDOUT. \n";
    case WSAEHOSTUNREACH: return "(10065) No route to host.  A socket operation was attempted to an unreachable host. See WSAENETUNREACH \n";
    case WSAEINPROGRESS: return "(10036) Operation now in progress.  A blocking operation is currently executing. Windows Sockets only allows a single blocking operation to be outstanding per task or thread), and if any other function call is made whether or not it references that or any other socket) the function fails with the WSAEINPROGRESS error. \n";
    case WSAEINTR: return "(10004) Interrupted function call.  A blocking operation was interrupted by a call to WSACancelBlockingCall. \n";
    case WSAEINVAL: return "(10022) Invalid argument.  Some invalid argument was supplied for example, specifying an invalid level to the setsockopt function). In some instances, it also refers to the current state of the socket - for instance, calling accept on a socket that is not listening. \n";
    case WSAEISCONN: return "(10056) Socket is already connected.  A connect request was made on an already connected socket. Some implementations also return this error if sendto is called on a connected SOCK_DGRAM socket For SOCK_STREAM sockets, the to parameter in sendto is ignored), although other implementations treat this as a legal occurrence. \n";
    case WSAEMFILE: return "(10024) Too many open files.  Too many open sockets. Each implementation may have a maximum number of socket handles available, either globally, per process or per thread. \n";
    case WSAEMSGSIZE: return "(10040) Message too long.  A message sent on a datagram socket was larger than the internal message buffer or some other network limit, or the buffer used to receive a datagram into was smaller than the datagram itself. \n";
    case WSAENETDOWN: return "(10050) Network is down.  A socket operation encountered a dead network. This could indicate a serious failure of the network system i.e. the protocol stack that the WinSock DLL runs over), the network interface, or the local network itself. \n";
    case WSAENETRESET: return "(10052) Network dropped connection on reset.  The connection has been broken due to 'keep-alive' activity detecting a failure while the operation was in progress. It can also be returned by setsockopt if an attempt is made to set SO_KEEPALIVE on a connection that has already failed. \n";
    case WSAENETUNREACH: return "(10051) Network is unreachable.  A socket operation was attempted to an unreachable network. This usually means the local software knows no route to reach the remote host. \n";
    case WSAENOBUFS: return "(10055) No buffer space available.  An operation on a socket could not be performed because the system lacked sufficient buffer space or because a queue was full. \n";
    case WSAENOPROTOOPT: return "(10042) Bad protocol option.  An unknown, invalid or unsupported option or level was specified in a getsockopt or setsockopt call. \n";
    case WSAENOTCONN: return "(10057) Socket is not connected.  A request to send or receive data was disallowed because the socket is not connected and when sending on a datagram socket using sendto) no address was supplied. Any other type of operation might also return this error - for example, setsockopt setting SO_KEEPALIVE if the connection has been reset. \n";
    case WSAENOTSOCK: return "(10038) Socket operation on non-socket.  An operation was attempted on something that is not a socket. Either the socket handle parameter did not reference a valid socket, or for select, a member of an fd_set was not valid. \n";
    case WSAEOPNOTSUPP: return "(10045) Operation not supported.  The attempted operation is not supported for the type of object referenced. Usually this occurs when a socket descriptor to a socket that cannot support this operation, for example, trying to accept a connection on a datagram socket. \n";
    case WSAEPFNOSUPPORT: return "(10046) Protocol family not supported.  The protocol family has not been configured into the system or no implementation for it exists. Has a slightly different meaning to WSAEAFNOSUPPORT, but is interchangeable in most cases, and all Windows Sockets functions that return one of these specify WSAEAFNOSUPPORT. \n";
    case WSAEPROCLIM: return "(10067) Too many processes.  A Windows Sockets implementation may have a limit on the number of applications that may use it simultaneously. WSAStartup may fail with this error if the limit has been reached. \n";
    case WSAEPROTONOSUPPORT: return "(10043) Protocol not supported.  The requested protocol has not been configured into the system, or no implementation for it exists. For example, a socket call requests a SOCK_DGRAM socket, but specifies a stream protocol. \n";
    case WSAEPROTOTYPE: return "(10041) Protocol wrong type for socket.  A protocol was specified in the socket function call that does not support the semantics of the socket type requested. For example, the ARPA Internet UDP protocol cannot be specified with a socket type of SOCK_STREAM. \n";
    case WSAESHUTDOWN: return "(10058) Cannot send after socket shutdown.  A request to send or receive data was disallowed because the socket had already been shut down in that direction with a previous shutdown call. By calling shutdown a partial close of a socket is requested, which is a signal that sending or receiving or both has been discontinued. \n";
    case WSAESOCKTNOSUPPORT: return "(10044) Socket type not supported.  The support for the specified socket type does not exist in this address family. For example, the optional type SOCK_RAW might be selected in a socket call, and the implementation does not support SOCK_RAW sockets at all. \n";
    case WSAETIMEDOUT: return "(10060) Connection timed out.  A connection attempt failed because the connected party did not properly respond after a period of time, or established connection failed because connected host has failed to respond. \n";
    case WSATYPE_NOT_FOUND: return "(10109) Class type not found.  The specified class was not found. \n";
    case WSAEWOULDBLOCK: return "(10035) Resource temporarily unavailable.  This error is returned from operations on non-blocking sockets that cannot be completed immediately, for example recv when no data is queued to be read from the socket. It is a non-fatal error, and the operation should be retried later. It is normal for WSAEWOULDBLOCK to be reported as the result from calling connect on a non-blocking SOCK_STREAM socket, since some time must elapse for the connection to be established. \n";
    case WSAHOST_NOT_FOUND: return "(11001) Host not found.  No such host is known. The name is not an official hostname or alias, or it cannot be found in the databases) being queried. This error may also be returned for protocol and service queries, and means the specified name could not be found in the relevant database. \n";
    case WSA_INVALID_HANDLE: return "(OS dependent) Specified event object handle is invalid.  An application attempts to use an event object, but the specified handle is not valid. \n";
    case WSA_INVALID_PARAMETER: return "(OS dependent) One or more parameters are invalid.  An application used a Windows Sockets function which directly maps to a Win32 function. The Win32 function is indicating a problem with one or more parameters. \n";
      //		case WSAINVALIDPROCTABLE : return "(OS dependent) Invalid procedure table from service provider.  A service provider returned a bogus proc table to WS2_32.DLL. Usually caused by one or more of the function pointers being NULL.) \n";
      //		case WSAINVALIDPROVIDER : return "(OS dependent) Invalid service provider version number.  A service provider returned a version number other than 2.0. \n";
    case WSA_IO_INCOMPLETE: return "(OS dependent) Overlapped I/O event object not in signaled state.  The application has tried to determine the status of an overlapped operation which is not yet completed. Applications that use WSAGetOverlappedResult with the fWait flag set to false) in a polling mode to determine when an overlapped operation has completed will get this error code until the operation is complete. \n";
    case WSA_IO_PENDING: return "(OS dependent) Overlapped operations will complete later.  The application has initiated an overlapped operation which cannot be completed immediately. A completion indication will be given at a later time when the operation has been completed. \n";
    case WSA_NOT_ENOUGH_MEMORY: return "(OS dependent) Insufficient memory available.  An application used a Windows Sockets function which directly maps to a Win32 function. The Win32 function is indicating a lack of required memory resources. \n";
    case WSANOTINITIALISED: return "(10093) Successful WSAStartup not yet performed.  Either the application hasn't called WSAStartup or WSAStartup failed. The application may be accessing a socket which the current active task does not own i.e. trying to share a socket between tasks), or WSACleanup has been called too many times. \n";
    case WSANO_DATA: return "(11004) Valid name, no data record of requested type.  The requested name is valid and was found in the database, but it does not have the correct associated data being resolved for. The usual example for this is a hostname -> address translation attempt using gethostbyname or WSAAsyncGetHostByName) which uses the DNS Domain Name Server), and an MX record is returned but no A record - indicating the host itself exists, but is not directly reachable. \n";
    case WSANO_RECOVERY: return "(11003) This is a non-recoverable error.  This indicates some sort of non-recoverable error occurred during a database lookup. This may be because the database files e.g. BSD-compatible HOSTS, SERVICES or PROTOCOLS files) could not be found, or a DNS request was returned by the server with a severe error. \n";
      //		case WSAPROVIDERFAILEDINIT : return "(OS dependent) Unable to initialize a service provider.  Either a service provider's DLL could not be loaded LoadLibrary failed) or the provider's WSPStartup/NSPStartup function failed. \n";
    case WSASYSCALLFAILURE: return "(OS dependent) System call failure.  Returned when a system call that should never fail does. For example, if a call to WaitForMultipleObjects fails or one of the registry functions fails trying to manipulate theprotocol/namespace catalogs. \n";
    case WSASYSNOTREADY: return "(10091) Network subsystem is unavailable.  This error is returned by WSAStartup if the Windows Sockets implementation cannot function at this time because the underlying system it uses to provide network services is currently unavailable. Users should check: that the appropriate Windows Sockets DLL file is in the current path, that they are not trying to use more than one Windows Sockets implementation simultaneously. If there is more than one WINSOCK DLL on your system, be sure the first one in the path is appropriate for the network subsystem currently loaded.  the Windows Sockets implementation documentation to be sure all necessary components are currently installed and configured correctly. \n";
    case WSATRY_AGAIN: return "(11002) Non-authoritative host not found.  This is usually a temporary error during hostname resolution and means that the local server did not receive a response from an authoritative server. A retry at some time later may be successful. \n";
    case WSAVERNOTSUPPORTED: return "(10092) WINSOCK.DLL version out of range.  The current Windows Sockets implementation does not support the Windows Sockets specification version requested by the application. Check that no old Windows Sockets DLL files are being accessed. \n";
    case WSAEDISCON: return "(10094) Graceful shutdown in progress.  Returned by WSARecv and WSARecvFrom to indicate the remote party has initiated a graceful shutdown sequence. \n";
    case WSA_OPERATION_ABORTED: return "(OS dependent) Overlapped operation aborted.  An overlapped operation was canceled due to the closure of the socket, or the execution of the SIO_FLUSH command in WSAIoctl. \n";
    default: return "UNKNOWN WSA ERROR\n";
  }
#else
  return "NOT A WSA ERROR, WIN32 NOT DEFINED.";
#endif
}

/**
 * lookupAppError(size_t) must be defined in user application.
 **/

#if defined(_MSC_VER)
extern const char FAR * lookupAppError(int);
#else
extern const char * lookupAppError(int);
#endif


#if defined(_MSC_VER)
const char FAR * itcstrerror(int i)
#else

const char* itcstrerror(int i)
#endif
{
  return(
    (i < 0) ? gai_strerror(i) : (
    (i > 30000) ? lookupAppError(i) : (
    (i > 10000) ? WSAStrError(i) : (
    (i > 2000) ? ITCStrError(i) : strerror(i)
    )
    )
    )
    );
}
