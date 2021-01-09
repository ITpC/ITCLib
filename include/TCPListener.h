/**
 * Copyright Pavel Kraynyukhov 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
  * $Id: TCPListener.h 1 2015-02-28 13:30:33Z pk $
 * 
 * EMail: pavel.kraynyukhov@gmail.com
 * 
 * 06.01.2018 - gracefull shutdown.
 **/

#ifndef __TCPLISTENER_H__
#define	__TCPLISTENER_H__
#include <memory>
#include <atomic>
#include <string>
#include <cstdint>
#include <functional>

#include <sys/mutex.h>
#include <net/Socket.h>
#include <abstract/IController.h>
#include <abstract/Runnable.h>
#include <sys/CancelableThread.h>
#include <Singleton.h>
#include <itc_log_defs.h>



namespace itc
{
  /**
   * \@brief a listener class which supposed to run as a CancelableThread. 
   * Do not run this listener as a ThreadPool runner, it will block the ThreadPool
   * indefinitely. The only task this class does is to accept new inbound 
   * connections and notify the associated view (usually a worker class).
   **/
  class TCPListener: public ::itc::abstract::IRunnable, public ::itc::abstract::IController<std::shared_ptr<net::Socket>>
  {
  private:
    itc::sys::mutex   mMutex;
    std::string       mAddress;
    uint16_t          mPort;
    net::Socket       mServerSocket;
    ViewTypeSPtr      mSocketsHandler;
    
    std::function<bool(const uint32_t)> mFilter;
    
    std::atomic<bool> doRun;
    std::atomic<bool> canDestroy;
    
	public:
   typedef ModelType value_type;
   
    explicit TCPListener(
      const std::string& address,
      const uint16_t port,
      const ViewTypeSPtr& sh, 
      const std::function<bool(const uint32_t)>& _filter=nullptr
    ): mMutex(), mAddress{address}, mPort{port}, mServerSocket(),
       mSocketsHandler{sh},mFilter{_filter}, doRun{true},canDestroy{false}
    {
      if(!mSocketsHandler.lock())
        throw std::runtime_error("The connection handler view does not exists (nullptr)");
      try{
        mServerSocket.open<net::Transport::TCP,net::Side::SERVER>(
          mAddress.c_str(),mPort,true,1000
        );
      }
      catch(std::exception& e)
      {
        ITC_ERROR(__FILE__,__LINE__,"Exception: {}",e.what());
        
        throw std::system_error(
          EINVAL,
          std::system_category(),
          fmt::format("Exception in itc::TCPListener() constructor, caused by Socket::open(): {}",e.what())
        );
      }
    }
    
    TCPListener(const TCPListener&)=delete;
    TCPListener(TCPListener&)=delete;
    
    void execute()
    {
      while(doRun.load())
      {
        try {
          auto newClient{mServerSocket.accept()};
          
          auto peerep{newClient->getpeerendpoint()};

          if(peerep.first.empty())
          {
            newClient->close();
            ITC_ERROR(__FILE__,__LINE__,"Can't receive peer endpoint information, closing connection",nullptr);
          }
          else
          {
            ITC_DEBUG(__FILE__,__LINE__,"New client connection: {}",peerep.first);
          }
          
          if(newClient->getFamily() == AF_INET)
          {
            uint32_t ipv4numeric{newClient->getpeernamev4()};
            
            if(mFilter&&mFilter(ipv4numeric))
            {
              newClient->close();
            }
          }
          
          if(!notify(newClient,mSocketsHandler))
          {
            doRun.store(false);
            break;
          }
        }catch(const std::exception& e)
        {
          ITC_ERROR(__FILE__,__LINE__,"Exception: {}",e.what());
        }
      }
      canDestroy.store(true);
    }
    
    void onCancel()
    {
      this->shutdown();
    }
    void shutdown() final
    {
      doRun.store(false);
      while(!canDestroy.load())
      {
        try{
          ::itc::net::Socket aSocket;
          aSocket.open<net::Transport::TCP,net::Side::CLIENT>(mAddress.c_str(),mPort);
          aSocket.close();
        }
        catch(const std::system_error& e)
        {
          ITC_INFO(__FILE__,__LINE__,"Listener went down, it is safe to shutdown",nullptr);
          break;
        }
        itc::sys::sched_yield(10);
      }
    }
  
    ~TCPListener()
    {
      this->shutdown();
    }
  };
}

typedef std::shared_ptr<itc::TCPListener> TCPListenerSPtr;
typedef itc::sys::CancelableThread<itc::TCPListener> TCPListenerThread;
typedef std::shared_ptr<TCPListenerThread>  TCPListenerThreadSPtr;
typedef std::weak_ptr<TCPListenerThread>  TCPListenerThreadWPtr;

#endif	/* __TCPLISTENER_H__ */

