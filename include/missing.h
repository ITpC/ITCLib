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
 * $Id: missing.h 81 2007-07-01 11:50:43Z Pavel Kraynyukhov $
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 **/

#ifndef __MISSING_H__
#define __MISSING_H__

#include <string.h>
#include <string>
#include <sstream>
#include <ctype.h>
#include <exception>
#include <stdexcept>

namespace itc {
  namespace utils {
    
    auto toupper(const std::string& str)
    {
      std::string out;
      std::for_each(
        str.begin(),str.end(),
        [&out](const auto& ch)
        {
          out.append(1,static_cast<char>(std::toupper(static_cast<unsigned char>(ch))));
        }
      );
      return out;
    }
    auto tolower(const std::string& str)
    {
      std::string out;
      std::for_each(
        str.begin(),str.end(),
        [&out](const auto& ch)
        {
          out.append(1,static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
        }
      );
      return out;
    }
  }
}

#if defined (__MINGW32__) || defined (__CYGWIN__)

char* rindex(const char* x, int c) {
    if (x) {
        int l = strlen(x) - 1;

        if (l == 0) return NULL;

        char* ptr = (char*) (&x[l]);
        do {
            if (ptr && ((*ptr) == c))
                return ptr;
        } while ((--ptr) > x);
        return NULL;
    }
    return NULL;
}

char* index(const char* x, int c) {
    if (x) {
        int len = strlen(x);
        if (!len) return NULL;

        char* ptr = (char*) x;
        while (ptr < (x + len)) {
            if ((*ptr) == c)
                return ptr;
            ptr++;
        }
        return NULL;
    }
    return NULL;
}

char *strndup(const char *s, size_t n) {
    size_t len = strlen(s);
    if (len > n) {
        char *ret = new char[n + 1];
        memset(ret, 0, n + 1);
        memcpy(ret, s, n);
        return ret;
    } else {
        char *ret = new char[len + 1];
        memset(ret, 0, len + 1);
        memcpy(ret, s, len);
        return ret;
    }
}
#endif

const char* itoa(const int i) {
    static char buff[21];
    memset(buff, 0, 21);
    snprintf(buff, 20, "%d", i);
    return buff;
}

const char* ftoa(const float i) {
    static char buff[33];
    memset(buff, 0, 33);
    snprintf(buff, 32, "%f", i);
    return buff;
}

#endif /*__MISSING_H__*/
