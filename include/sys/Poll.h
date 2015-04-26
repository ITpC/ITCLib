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
      std::mutex mPollMutex;
      std::mutex mModMutex;
      std::atomic<nfds_t> mMaxFDs;
      std::atomic<nfds_t> mWatchFDs;
      std::atomic<bool> mModified;
      std::map<int, pollfd> mAll;
      std::vector<pollfd> mFDs;

     public:

      Poll(const size_t max)
        :mPollMutex(), mModMutex(), mMaxFDs(max), mWatchFDs(0), mFDs(mMaxFDs)
      {

      }

      void clear()
      {
        SyncLock sync1(mModMutex);
        SyncLock sync2(mPollMutex);
        mFDs.clear();
        mAll.clear();
      }

      void poll(int ms, retvector& result)
      {
        SyncLock sync1(mPollMutex);
        if(mModified)
        {
          SyncLock sync2(mModMutex);
          mFDs.clear();
          mWatchFDs = 0;
          for(std::map<int, pollfd>::iterator it = mAll.begin();it != mAll.end();++it)
          {
            mFDs.push_back(it->second);
            ++mWatchFDs;
          }
          mModified = false;
        }
        int ret = ::poll(mFDs.data(), nfds_t(mWatchFDs), ms);
        if(ret > 0)
        {
          for(size_t i = 0;i < mWatchFDs;++i)
          {
            if(mFDs[i].revents != 0)
            {
              result.push_back(pollFDEVENTPair(mFDs[i].fd, mFDs[i].revents));
            }
          }
        }
        else
        {
          if(ret == -1)
          {
            getLog()->error(__FILE__, __LINE__, "Poll:poll() error: %s", strerror(errno));
          }
        }
      }

      void poll(retvector& result)
      {
        std::atomic<int> ret(0);
        
        do{
          SyncLock sync1(mPollMutex);
          
          if(mModified)
          {
            SyncLock sync2(mModMutex);
            mFDs.clear();
            mWatchFDs = 0;
            for(std::map<int, pollfd>::iterator it = mAll.begin();it != mAll.end();++it)
            {
              mFDs.push_back(it->second);
              ++mWatchFDs;
            }
            mModified = false;
          }
          ret = ::poll(mFDs.data(), nfds_t(mWatchFDs), 10); // renew list every 10 ms if necessary

          if(ret > 0)
          {
            for(size_t i = 0;i < mWatchFDs;++i)
            {
              if(mFDs[i].revents != 0)
              {
                result.push_back(pollFDEVENTPair(mFDs[i].fd, mFDs[i].revents));
              }
            }
          }
          else
          {
            if(ret == -1)
            {
              getLog()->error(__FILE__, __LINE__, "Poll:poll() error: %s", strerror(errno));
            }
          }
        }while(ret == 0);
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
          mModified = true;
          mWatchFDs = mAll.size();
        }
      }

      void poll_ctl_modify(const int fd, const short events)
      {
        SyncLock sync(mModMutex);
        if(mAll.size() < mMaxFDs)
        {
          mAll[fd].fd = fd;
          mAll[fd].events = events;
          mModified = true;
          mWatchFDs = mAll.size();
        }
      }

      void poll_ctl_disable(const int fd)
      {
        SyncLock sync(mModMutex);
        std::map<int, pollfd>::iterator it = mAll.find(fd);
        if(it != mAll.end())
        {
          mAll[fd].fd = -fd;
          mModified = true;
        }
      }

      void poll_ctl_enable(const int fd)
      {
        SyncLock sync(mModMutex);
        std::map<int, pollfd>::iterator it = mAll.find(fd);

        if(it != mAll.end())
        {
          mAll[fd].fd = fd;
          mModified = true;
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
          mAll[fd].fd = fd;
          mAll[fd].events = events;
          mModified = true;
          mWatchFDs = mAll.size();
        }
      }
    };
  }
}

#endif	/* __POLL_H__ */

