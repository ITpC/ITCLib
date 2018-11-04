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
              itc::getLog()->fatal(__FILE__,__LINE__,"Thread::invoke() has failed because of an exception: %s",e.what());
            }
            return NULL;
        }
    }
}
