/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 * $Id: ClientSocketsFactory.h 11 25 Февраль 2015 г., 11:44 $
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 **/
 
#ifndef EPOLL_H
#define	EPOLL_H

#include <unistd.h>
#include <string.h>

#include <cstdint>
#include <memory>
#include <map>
#include <vector>
#include <mutex>

#include <sys/epoll.h>
#include <sys/synclock.h>

#include <TSLog.h>
#include <ITCException.h>
#include <Val2Type.h>

namespace itc
{
  namespace sys
  {
    enum POLLTYPE{PTYPE_POLLIN,PTYPE_POLLOUT};

    template <
      POLLTYPE PT_Value, 
      int mode=EPOLLONESHOT|EPOLLRDHUP|EPOLLPRI|EPOLLERR|EPOLLHUP> 
    class ePoll
    {
    public:
      typedef struct epoll_event            ePollEventType;
      typedef std::vector<ePollEventType>   ePollEvents;
      typedef std::pair<int,uint32_t>       FDEventPair;
      typedef std::shared_ptr<ePollEvents> ePollEvSPtr;

    private:
      std::mutex          mMutex;
      size_t              mMaxEvents;
      ePollEvents         events;
      ePollEvents         revents;
      bool                mDestroy;
      int                 mPollFD;
      
      
    public:
     
      explicit ePoll(const size_t& nevents)
      : mMutex(),mMaxEvents(nevents),events(mMaxEvents),
        revents(mMaxEvents),mDestroy(false)
      {
          SyncLock sync(mMutex);
          memset(revents.data(),0,sizeof(ePollEventType)*(revents.size()));
          if((mPollFD = epoll_create1(0))<0)
          {
              throw TITCException<exceptions::NoEPoll>(errno);
          }
      }

      void add(const int& fd)
      {
        SyncLock sync(mMutex);
        utils::Int2Type<PT_Value> fictive;
        add(fd,fictive);
        itc::getLog()->debug(__FILE__,__LINE__,"fd %d added to ePoll watch list",fd);
      };

      const int getfd() const
      {
        return mPollFD;
      }
      
      std::vector<FDEventPair> poll(const int ms)
      {
        SyncLock sync(mMutex);
        if(!mDestroy)
        {
          revents.clear();
          int ret=epoll_wait(mPollFD, revents.data(), mMaxEvents ,ms);
          if(ret>0)
          {
            std::vector<FDEventPair> tmp;
            for(size_t i=0; i<ret; ++i)
            {
              int fd=revents[i].data.fd;
              uint32_t levents=revents[i].events;
              tmp.push_back(FDEventPair(fd,levents));
              getLog()->debug(__FILE__,__LINE__,"An event %u for fd %d is recieved",levents,revents[i].data.fd);
            }
            return tmp;
          }
          else if(ret == -1)
          {
            ::itc::getLog()->error(__FILE__,__LINE__,"in ePoll()::poll() on epoll_wait(): %s",strerror(errno));
            return std::vector<FDEventPair>(0);
          }
          else if(ret == 0)
          {
            ::itc::getLog()->error(__FILE__,__LINE__,"in ePoll()::poll() nothing to poll");
            return std::vector<FDEventPair>(0);
          }
        }
      }

      ~ePoll()
      {
          SyncLock sync(mMutex);
          mDestroy=true;
      }

    private:

      void add(int fd, const utils::Int2Type<PTYPE_POLLIN>& polltype)
      {
        struct epoll_event event;

        event.events=EPOLLIN|mode;

        if(!mDestroy)
        {
          if(epoll_ctl(mPollFD,EPOLL_CTL_ADD,fd,&event)==-1)
          {
            throw TITCException<exceptions::EPollCTLError>(errno);
          }
        }
      }

      void add(const int& fd, const utils::Int2Type<PTYPE_POLLOUT>& polltype)
      {
        struct epoll_event event;

        event.events=EPOLLOUT|mode;

        if(!mDestroy)
        {
          if(epoll_ctl(mPollFD,EPOLL_CTL_ADD,fd,&event)==-1)
          {
            throw TITCException<exceptions::EPollCTLError>(errno);
          }
        }
      }

      void del(const int& fd)
      {
        // inclompatible to kernels before 2.6.9, the event array is NULL here.
        if(!mDestroy)
        {
          if(epoll_ctl(mPollFD,EPOLL_CTL_DEL,fd,(epoll_event*)0)==-1)
          {
              throw TITCException<exceptions::EPollCTLError>(errno);
          }
        }
      }
    };
  }    
}

#endif	/* EPOLL_H */

