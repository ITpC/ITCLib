/**
 *  Copyright 2018, Pavel Kraynyukhov <pavel.kraynyukhov@gmail.com>
 * 
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 *  $Id: default_out_adapter.h December 27, 2020 2:05 PM $
 * 
 **/

#ifndef __DEFAULT_OUT_ADAPTER_H__
#  define __DEFAULT_OUT_ADAPTER_H__

#include <string_view>
#include <string>
#include <abstract/ILogFormatter.h>
#include <abstract/ILogOutAdapter.h>
#include <fstream>
#include <system_error>
#include <tsbqueue.h>
#include <memory>
#include <sys/mutex.h>
#include <abstract/Runnable.h>
#include <algorithm>
#include <sys/sched_yield.h>

namespace flog
{
  class defaultOutAdapter : public flog::ILogOutAdapter<std::string>, public itc::abstract::IRunnable
  {
   private:
    using queue=itc::tsbqueue<std::string,itc::sys::mutex>;
    std::ofstream     mOutStream;
    queue             mQueue;
    std::atomic<bool> valid;
    std::atomic<bool> mayClose;
    
    void process_messages()
    {
      std::queue<std::string> messages;
      mQueue.recv<queue::SWAP>(messages);
      while(!messages.empty())
      {
        auto message{messages.front()};
        mOutStream << message;
        messages.pop();
      }
    }
    
   public:
    defaultOutAdapter(std::string_view file_name) 
    : mOutStream(file_name.data(),std::ios_base::app|std::ios_base::binary|std::ios_base::out),
    mQueue{},valid{true},mayClose{false}
    {
      if(!mOutStream.is_open())
        throw std::system_error(errno,std::system_category(),std::string(file_name.data(),file_name.size()));
    }
    
    void post(std::string&& ref){
      mQueue.send(std::move(ref));
    }
    
    void execute()
    {
      while(valid.load())
      {
        process_messages();
      }
      
      // write outstanding messages
      
      while(!mQueue.empty())
      {
        process_messages();
      }
      mOutStream.flush();
      mOutStream.close();
      mayClose.store(true);
    }
    void onCancel()
    {
      shutdown();
    }
    void shutdown()
    {
      valid.store(false);
      YIELD();
      mQueue.send("flog::defaultOutAdapter::shutdown()");
      YIELD();
      while(!mayClose.load()) YIELD();
    }
    ~defaultOutAdapter()
    {
      shutdown();
    }
  };
}

#endif /* __DEFAULT_OUT_ADAPTER_H__ */

