/**
 * Copyright (c) 2007, Pavel Kraynyukhov.
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
 * $Id: ITCException.h 101 2007-08-12 13:07:51Z acs $
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 **/

#ifndef __ITCException_H__
#  define __ITCException_H__

#  include <pthread.h>
#  include <exception>
#  include <string>
#  include <cstring>
#  include <cerrno>
#  include <vector>
#  include <sstream>
#  include <iostream>
#  include <ITCError.h>
#  include <InterfaceCheck.h>
#  include <memory>

// using namespace std::tr1;

#  ifdef PtW32Catch
#    define CATCH  PtW32Catch
#  else
#    define CATCH  catch(...)
#  endif

namespace exceptions
{

  enum Exception
  {
    Can_not_create_mutex = 2001,
    Can_not_initialize_mutex = 2002,
    Can_not_lock_mutex = 2003,
    Can_not_unlock_mutex = 2004,
    Can_not_destroy_mutex = 2006,
    Can_not_copy_mutex = 2007,

    Can_not_copy_semaphore = 2014,
    Can_not_initialize_semaphore = 2015,
    Can_not_get_semaphore_value = 2016,
    Can_not_wait_on_semaphore = 2017,
    Can_not_post_on_semaphore = 2018,
    Can_not_destroy_semaphore = 2019,

    Can_not_create_thread = 2020,

    Can_not_create_reusable_object = 2030,
    Can_not_create_key = 2038,
    Can_not_create_conditional = 2039,

    Can_not_open_socket = 2041,
    Can_not_bind_socket = 2042,
    Can_not_listen_on_socket = 2043,

    Can_not_open_queue = 2054,
    PQIsOutOfSync = 2055,
    QueueOutOfSync = 2056,

    Timed_wait_out_of_range = 2062,
    Timed_wait_deadlock = 2063,

    NetworkException = 2100,
    InvalidSocketException = 2101,
    GAI_Exception = 2102,
    BadSocketHints = 2103,
    BadIPAddress = 2104,
    MutexException = 2105,
    
    ITCGeneral=2112,
    Can_not_begin_txn=2113,

    NoEPoll = 2151,
    EPollCTLError = 2152,

    BufferOverflowAttempt = 2200,
    IndexOutOfRange = 2201,
    NullPointerException = 2202,
    InitializationForbidden = 2203,
    ImplementationForbidden = 2204,

    Can_not_assign_runnable = 2300,

    bz2ConfigError = 2350,
    bz2ParamError = 2351,
    bz2MemError = 2352,
    bz2InternalError = 2353,
    bz2DataIntegrityError = 2354,

    FileNotFound = 2500,
    CanNotWrite = 2501,
    FileIsAlreadyOpen = 2502,
    UnsupportedFileLength = 2503,
    KeyIsNotSet = 2504,
    WrongFileFormat = 2505,

    LuaScriptError = 2515,

    MDBVersionMissmatch = 2600,
    MDBInvalid = 2601,
    MDBNotFound = 2602,
    MDBEAccess = 2603,
    MDBEAgain = 2604,
    MDBClosed = 2605,
    MDBPanic = 2606,
    MDBGeneral = 2607,
    MDBMapResized = 2608,
    MDBReadersFull = 2609,
    MDBTooMany = 2610,
    MDBEInval = 2611,
    MDBEnvWrong = 2612,
    MDBTxnNULL = 2613,
    MDBInvalParam = 2614,
    MDBMapFull = 2615,
    MDBTxnFull = 2616,
    MDBTEAccess = 2617,
    MDBKeyNotFound = 2618,
    MDBWriteFailed = 2619,

    ConfigSyntax = 2700,
    
    Reflection = 2720,
    UndefinedType = 2721,
    InvalidTypecast = 2722,
    
    LPQStart = 2800,

    ExternalLibraryException = 2998,
    InvalidException = 2999,
    ApplicationException = 3000
  };
}


/**
 * lookupAppError(size_t) must be defined in user application.
 **/

#  if defined(_MSC_VER)
extern const char FAR * lookupAppError(size_t);
#  else
extern const char * lookupAppError(size_t);
#  endif

#  if defined(_MSC_VER)
extern const char FAR * WSAStrError(int);
#  else
extern const char * WSAStrError(int);
#  endif



#  if defined(_MSC_VER)
extern const char FAR * itcstrerror(int);
#  else
extern const char* itcstrerror(int);
#  endif

struct ITCErrno
{
  int error;
  exceptions::Exception msgno;

  ITCErrno(const int err, const exceptions::Exception msg) : error(err), msgno(msg)
  {
  }

  ITCErrno(const ITCErrno & val) : error(val.error), msgno(val.msgno)
  {
  }

  ITCErrno() : error(EINVAL), msgno(exceptions::InvalidException)
  {
  }

  inline ITCErrno& operator=(const ITCErrno & mterr)
  {
    error = mterr.error;
    msgno = mterr.msgno;
    return(*this);
  }

  ~ITCErrno()
  {
  }
};

/**
 * @brief General purpose exceptions tamplate for ITCLib and ITCFramework 
 * it should be fast in cascade. However it is about 33% (gcc 4.1) slower
 * on throw, then std::exception or std::bad_alloc.
 * 
 * TODO: In gcc 4.3 std::basic_string has been rewritten, so test throwing
 * performance with gcc 4.3
 * 
 * TODO: use it everywhere. 
 *
 **/

template <
const exceptions::Exception Excpt = exceptions::InvalidException
> class TITCException : public std::exception
{
 private:
  ITCErrno mErrno;
  std::shared_ptr<std::string> mWhat;

 public:

    /*    TITCException() throw () : mErrno(EINVAL, Excpt), mWhat(new std::string())
      {
      }
   */
  explicit TITCException(const int ErrVal = EINVAL) throw() : mErrno(ErrVal, Excpt), mWhat(new std::string())
  {
  }

  TITCException(const TITCException<Excpt>& ref) throw()
    : mErrno(ref.mErrno), mWhat(ref.mWhat)
  {
  }

  virtual const char* what() const throw()
  {
    mWhat.get()->append("Exception - ").append(itcstrerror(mErrno.msgno)).append(", ").append(itcstrerror(mErrno.error));
    return mWhat.get()->c_str();
  }

  inline int getErrno() const
  {
    return mErrno.error;
  }

  inline exceptions::Exception getMsgno() const
  {
    return mErrno.msgno;
  }

  virtual ~TITCException() throw()
  {
  }
};

/**
 * @brief General purpose exception class for ITCLib and ITCFramework 
 *
 **/
class ITCException : public std::exception
{
 private:
  ITCErrno mErrno;
  //    exceptions::Exception mExcept;
  std::shared_ptr<std::string> mWhat;
 public:

  ITCException() throw()
    : mErrno(EINVAL, exceptions::InvalidException),
    mWhat(
    new std::string()
    )
  {
  }

  ITCException(
    const int pErrVal = EINVAL,
    const exceptions::Exception pExcept = exceptions::InvalidException
    ) throw()
    : mErrno(pErrVal, pExcept),
    mWhat(
    new std::string()
    )
  {
  }

  ITCException(const ITCException& ref)throw()
    : mErrno(ref.mErrno), mWhat(ref.mWhat)
  {
  }

  virtual const char* what() const throw()
  {
    mWhat.get()->append("Exception - ").append(itcstrerror(mErrno.msgno)).append(", ").append(itcstrerror(mErrno.error));
    return mWhat.get()->c_str();
  }

  inline int getErrno() const
  {
    return mErrno.error;
  }

  inline exceptions::Exception getMsgno() const
  {
    return mErrno.msgno;
  }

  virtual ~ITCException() throw()
  {
  }
};

#endif
