/**
 * Copyright (c) 2009, Pavel Kraynyukhov.
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
 * $Id:$
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 **/

#ifndef __SHAREDBUFFER_H__
#    define	__SHAREDBUFFER_H__

#    include <boost/shared_array.hpp>
#    include <ITCException.h>

namespace itc
{
typedef TITCException<exceptions::IndexOutOfRange> IndexOutOfRangeException;

template <typename T> class SharedBuffer
{
private:
    size_t mSize;
    boost::shared_array<T> mBuff;
public:

    explicit SharedBuffer(const size_t& pSize=0)
    : mSize(pSize), mBuff()
    {
        if (pSize != 0)
        {
            mBuff.reset(new T[pSize]);
        }
    }

    SharedBuffer(const size_t& pSize, boost::shared_array<T> const& pBuff)
    : mSize(pSize), mBuff(pBuff)
    {
    }

    SharedBuffer(const size_t& pSize, T* pBuff)
    : mSize(pSize), mBuff(pBuff)
    {
        if (pBuff == NULL)
        {
            mSize = 0;
        }
    }


    SharedBuffer(const SharedBuffer& ref)
    : mSize(ref.mSize), mBuff(ref.mBuff)
    {
    }

    inline const SharedBuffer & operator=(const SharedBuffer& ref)
    {
        mSize = ref.mSize;
        mBuff = ref.mBuff;
        return (*this);
    }

    inline T* get() const
    {
        return mBuff.get();
    }

    inline const size_t size() const
    {
        return mSize;
    }

    T & operator[](const size_t i) const
    {
        if (i >= mSize)
            throw IndexOutOfRangeException(ENODATA);
        return mBuff.get()[i];
    }    
};
}
#endif	/* __SHAREDBUFFER_H__ */

