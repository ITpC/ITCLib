/**
 *  Copyright 2007-2020, Pavel Kraynyukhov <pavel.kraynyukhov@gmail.com>
 * 
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * 
 * 
 * $Id: Thread.cpp 54 2007-05-22 08:36:14Z Pavel Kraynyukhov $
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 **/


#include <pthread.h>
#include <sys/Thread.h>
#include <sys/Types.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <abstract/Cleanable.h>
#include <itc_log_defs.h>

namespace itc {
    namespace sys {

        void cleanup_handler(::itc::abstract::Cleanable* arg) {
            arg->cleanup();
        }

        void* invoke(Thread* context) {
#if !defined(_MSC_VER) && !defined(__MINGW32_VERSION)
            sigset_t nset;
            sigemptyset(&nset);
            sigaddset(&nset, SIGTERM);
            pthread_sigmask(SIG_BLOCK, &nset, NULL);
#endif
            try{
              context->start.wait();
              context->run();
            }catch(const std::exception& e)
            {
              ITC_FATAL(__FILE__,__LINE__,"Thread::invoke() has failed because of an exception: %s",e.what());
            }
            return NULL;
        }
    }
}
