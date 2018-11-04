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

#include <system_error>
#include <sys/time.h>

namespace itc
{
  namespace sys
  {
    class Nap
    {
    public:
      explicit Nap()=default;
      Nap(const Nap&)=delete;
      Nap(Nap&)=delete;
      
      void nanosleep(const uint32_t nsec)
      {
        struct timespec pause{0,nsec};
        struct timespec save{0,0};
        
        if(nsec >= 1000000000)
        {
          pause.tv_sec=nsec/1000000000;
          pause.tv_nsec=pause.tv_sec*1000000000-nsec;
        }
        
        while(::nanosleep(&pause,&save) == -1)
        {
          if(errno == EINTR)
          {
            pause.tv_sec=save.tv_sec;
            pause.tv_nsec=save.tv_nsec;
            save={0,0};
            continue;
          }else{
            break;
          }
        }
      }
      
      void usleep(const uint32_t usec)
      {
        this->nanosleep(usec*1000);
      }
      
      void sleep(const uint32_t sec)
      {
        this->nanosleep(sec*1000000000);
      }
    };
  }
}


#endif	/* SEMSLEEP_H */

