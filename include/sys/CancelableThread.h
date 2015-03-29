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
 * $Id: CancelableThread.h 27 2011-02-06 13:25:43Z pk $
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 **/
#ifndef CLEANABLETHREAD_H_
#    define CLEANABLETHREAD_H_

#    include <memory>

#    include <InterfaceCheck.h>
#    include <abstract/Runnable.h>

#    include <sys/Thread.h>
#    include <sys/Semaphore.h>
#    include <abstract/Cleanable.h>
#    include <abstract/Runnable.h>
#include <iostream>

namespace itc
{
namespace sys
{

template <typename TRunnable> class CancelableThread : public Thread, abstract::Cleanable
{
private:
    friend void* invoke(Thread*);
    friend void cleanup_handler(abstract::Cleanable*);

    std::shared_ptr<TRunnable> mRunnable;

public:

    explicit CancelableThread(const std::shared_ptr<TRunnable>& ref)
    : Thread(), mRunnable(ref)
    {
        typedef abstract::IRunnable RunnableInterface;
        STATIC_CHECKER3MSG(
           CheckRelationship(
             TRunnable, subclassof, RunnableInterface
             ),
           TRunnable, _is_not_a_subclass_of_, RunnableInterface
       );
        begin();
    }
    
    inline std::shared_ptr<TRunnable> getRunnable()
    {
        return mRunnable;
    }
    
    inline void run()
    {
        pthread_cleanup_push((void (*)(void*))cleanup_handler, this);
        if (TRunnable *ptr = mRunnable.get())
        {
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
            pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
            ptr->execute();
        }
        pthread_cleanup_pop(0);
    }

    inline void cleanup()
    {
        if (TRunnable *ptr = mRunnable.get())
        {
            ptr->onCancel();
        }
    }

    virtual ~CancelableThread() noexcept // gcc 4.7.4 compat
    {
        if (TRunnable *ptr = mRunnable.get())
        {
            ptr->shutdown();
        }
        getLog()->debug(__FILE__, __LINE__, "Calling cancel() for TID: %jx\n", this->getThreadId());
        cancel();
        getLog()->debug(__FILE__, __LINE__, "Calling finish() for TID: %jx\n", this->getThreadId());
        finish();
        getLog()->debug(__FILE__, __LINE__, "finished TID: %jx\n", this->getThreadId());
    }
};
}
}
#endif /*CLEANABLETHREAD_H_*/
