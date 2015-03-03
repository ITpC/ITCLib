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
 * $Id: SemSleep.h 54 2015-03-04 00:12:14Z Pavel Kraynyukhov $
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 **/

#ifndef SEMSLEEP_H
#define	SEMSLEEP_H

#include <sys/Semaphore.h>
#include <ITCException.h>

namespace itc
{
  namespace sys
  {
    class SemSleep{
    private:
      itc::sys::Semaphore mSemaphore;
    public:
      explicit SemSleep()
      {
      }
      inline void nanosleep(uint64_t nsec)
      {
        struct timespec anAbstime;
        struct timeval now;

        gettimeofday(&now,NULL);

        anAbstime.tv_nsec=now.tv_usec*1000UL+nsec;
        uint64_t addsec=uint64_t(anAbstime.tv_nsec / 1000000000UL);
        anAbstime.tv_sec=now.tv_sec+addsec;
        anAbstime.tv_nsec -= addsec*1000000000UL;

        while(1)
        {
          try{
            mSemaphore.timedWait(anAbstime);
          }catch(const ITCException& e)
          {
            if(e.getErrno() == EINTR)
              continue;
            if(e.getErrno() == ETIMEDOUT)
              return;
          }
        }
      }
      inline void usleep(uint64_t usec)
      {
        struct timespec anAbstime;
        struct timeval now;

        gettimeofday(&now,NULL);

        anAbstime.tv_nsec=(now.tv_usec+usec)*1000UL;
        uint64_t addsec=uint64_t(anAbstime.tv_nsec / 1000000000UL);
        anAbstime.tv_sec=now.tv_sec+addsec;
        anAbstime.tv_nsec -= addsec*1000000000UL;

        while(1)
        {
          try{
            mSemaphore.timedWait(anAbstime);
          }catch(const ITCException& e)
          {
            if(e.getErrno() == EINTR)
              continue;
            if(e.getErrno() == ETIMEDOUT)
              return;
          }
        }
      }
    };
  }
}


#endif	/* SEMSLEEP_H */

