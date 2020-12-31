/**
 *  Copyright 2018, Pavel Kraynyukhov <pavel.kraynyukhov@gmail.com>
 * 
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 * 
 *  $Id: itc_log_defs.h January 1, 2021 1:24 AM $
 * 
 **/

#ifndef __ITC_LOG_DEFS_H__
#  define __ITC_LOG_DEFS_H__

#include <string_view>
#include <flog.h>

const std::string_view out_fmt{"{}:{} {}"};

#define ITC_TRACE(f,l,mfmt,args...) FLG_TRACE(fmt::format(out_fmt,f,l,mfmt),args)
#define ITC_DEBUG(f,l,mfmt,args...) FLG_DEBUG(fmt::format(out_fmt,f,l,mfmt),args)
#define ITC_INFO(f,l,mfmt,args...)  FLG_INFO(fmt::format(out_fmt,f,l,mfmt),args)
#define ITC_ERROR(f,l,mfmt,args...) FLG_ERROR(fmt::format(out_fmt,f,l,mfmt),args)
#define ITC_FATAL(f,l,mfmt,args...) FLG_FATAL(fmt::format(out_fmt,f,l,mfmt),args)


#endif /* __ITC_LOG_DEFS_H__ */

