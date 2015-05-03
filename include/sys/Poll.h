/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: Poll.h 25 Апрель 2015 г. 23:36 pk$
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 **/

#ifndef __POLL_H__
#  define	__POLL_H__

#  include <unistd.h>
#  include <string.h>

#  include <cstdint>
#  include <memory>
#  include <map>
#  include <vector>
#  include <mutex>

#  include <sys/epoll.h>
#  include <sys/synclock.h>

#  include <TSLog.h>
#  include <ITCException.h>
#  include <Val2Type.h>

#  include <poll.h>

namespace itc
{
  namespace sys
  {
    typedef std::pair<int, short> pollFDEVENTPair;
    typedef std::vector<pollFDEVENTPair> retvector;

    enum POLL_TYPE
    {
      PTYPEPOLLIN, PTYPEPOLLOUT
    };

    class Poll
    {
     private:
      std::mutex mModMutex;
      std::atomic<nfds_t> mMaxFDs;
      std::map<int, pollfd> mAll;

     public:

      explicit Poll(const size_t max)
        :mModMutex(), mMaxFDs(max)
      {
      }

      void clear()
      {
        SyncLock sync1(mModMutex);
        mAll.clear();
      }

      void fill(std::vector<pollfd>& watchlist)
      {
        SyncLock sync(mModMutex);
        if(!mAll.empty())
        {
          for(std::map<int, pollfd>::iterator it = mAll.begin();it != mAll.end();++it)
          {
            watchlist.push_back(it->second);
          }
        }
      }
      
      const int poll(std::vector<pollfd>& cref, const int ms)
      {
        if(cref.empty()) return 0;
        else return ::poll(cref.data(),cref.size(),ms);
      }

      template <POLL_TYPE ptype = PTYPEPOLLIN> void poll_ctl_add(const int fd)
      {
        SyncLock sync(mModMutex);
        utils::Int2Type<ptype> fictive;
        add(fd, fictive);
      }

      void poll_ctl_delete(const int fd)
      {
        SyncLock sync(mModMutex);
        std::map<int, pollfd>::iterator it = mAll.find(fd);
        if(it != mAll.end())
        {
          mAll.erase(it);
        }
      }

      void poll_ctl_modify(const int fd, const short events)
      {
        SyncLock sync(mModMutex);
        if(mAll.size() < mMaxFDs)
        {
          pollfd tmp;
          tmp.fd=fd;
          tmp.events=events;
          tmp.revents=0;
          mAll.insert(std::pair<int,pollfd>(fd,tmp));
        }
      }

      void poll_ctl_disable(const int fd)
      {
        SyncLock sync(mModMutex);
        std::map<int, pollfd>::iterator it = mAll.find(fd);
        if(it != mAll.end())
        {
          it->second.fd = -fd;
        }
      }

      void poll_ctl_enable(const int fd)
      {
        SyncLock sync(mModMutex);
        std::map<int, pollfd>::iterator it = mAll.find(fd);

        if(it != mAll.end())
        {
          it->second.fd = fd;
        }
      }


     private:

      void add(const int fd, const utils::Int2Type<PTYPEPOLLIN>& polltype)
      {
        static short mode = POLLIN | POLLPRI | POLLRDHUP;
        poll_ctl_insert(fd, mode);
      }

      void add(const int fd, const utils::Int2Type<PTYPEPOLLOUT>& polltype)
      {
        static short mode = POLLOUT | POLLPRI | POLLRDHUP | POLLERR | POLLHUP | POLLNVAL;
        poll_ctl_insert(fd, mode);
      }

      void poll_ctl_insert(const int fd, const short events)
      {
        if(mAll.size() < mMaxFDs)
        {
          pollfd tmp;
          tmp.fd=fd;
          tmp.events=events;
          tmp.revents=0;
          mAll.insert(std::pair<int,pollfd>(fd,tmp));
        }
      }
    };
  }
}

#endif	/* __POLL_H__ */

