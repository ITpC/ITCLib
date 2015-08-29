#include <string>
#include <stdio.h>
#include <Val2Type.h>
#include <limits>
#include <iostream>
#include <string.h>
#include <boost/lexical_cast.hpp>

class nfmt 
{
private:
  static std::string mOut;
  static void fit2field(std::string& out, const size_t length, const char prepsym,itc::utils::Bool2Type<true>& align_right)
  {
      if(length == out.length()) return;
      if(length<out.length())
      {
        out.resize(length,0);
        return;
      }
      out.insert(0,length-out.length(),prepsym);
  }

  static void fit2field(std::string& out, const size_t length, const char appsym, itc::utils::Bool2Type<false>& align_right)
  {
      if(length == out.length()) return;
      if(length<out.length())
      {
        out.resize(length,0);
        return;
      }
      out.append(length-out.length(),appsym);
  }

  public:
  template <const size_t length, const char sym=' ', const bool alright=false> static std::string& field(std::string& out)
  {
    itc::utils::Bool2Type<alright> align_right;
    fit2field(out,length,sym,align_right);
    return out;
  }

  template<const size_t field_width,const size_t precision> static std::string& to_string(const double& val)
  {
    mOut.resize(field_width,0);
    snprintf(const_cast<char*>(mOut.data()),field_width,std::string("%-"+boost::lexical_cast<std::string>(field_width)+"."+boost::lexical_cast<std::string>(precision)+"f").c_str(),val);
    return mOut;
  }
  
  template<const size_t field_width,const size_t precision> static std::string& to_string(const long double& val)
  {
    mOut.resize(field_width,0);
    snprintf(const_cast<char*>(mOut.data()),field_width,std::string("%-"+boost::lexical_cast<std::string>(field_width)+"."+boost::lexical_cast<std::string>(precision)+"Lf").c_str(),val);
    return mOut;
  }
  
  template<const size_t precision> static std::string& to_string(const double& val)
  {
    char buff[3000]={};
    snprintf(buff,3000,std::string("%'."+boost::lexical_cast<std::string>(precision)+"f").c_str(),val);
    mOut.clear();
    mOut.append(buff);
    return mOut;
  }
  
  template<const size_t precision> static std::string& to_string(const long double& val)
  {
    char buff[5000]={};
    snprintf(buff,5000,std::string("%'."+boost::lexical_cast<std::string>(precision)+"Lf").c_str(),val);
    mOut.clear();
    mOut.append(buff);
    return mOut;
  }
  template <typename INTEGER> static std::string& to_string(const INTEGER& val)
  {
    mOut=boost::lexical_cast<std::string>(val);
    return mOut;
  }
private:
  static void to_hex(const int val,const size_t limit,char* out)
  {
    snprintf(out,limit,"0x%x",val);
  }

  static void to_hex(const long int val,const size_t limit,char* out)
  {
    snprintf(out,limit,"0x%jx",val);
  }

  static void to_hex(const long long int val,const size_t limit,char* out)
  {
    snprintf(out,limit,"0x%llx",val);
  }

  static void to_hex(const unsigned int val,const size_t limit,char* out)
  {
    snprintf(out,limit,"0x%x",val);
  }

  static void to_hex(const unsigned long int val,const size_t limit,char* out)
  {
    snprintf(out,limit,"0x%jx",val);
  }

  static void to_hex(const unsigned long long int val,const size_t limit,char* out)
  {
    snprintf(out,limit,"0x%llx",val);
  }
public:
  template<typename T> static std::string& to_hex(const T& val)
  {
    char buff[sizeof(T)*2+3]={};
    to_hex(val,sizeof(T)*2+2,buff);
    mOut.clear();
    mOut.append(buff);
    return mOut;
  }

  template<typename T> static std::string& p2hex(const T* val)
  {
    char buff[sizeof(T)*2+3]={};
    snprintf(buff,sizeof(T)*2+2,"%p",val);
    mOut.clear();
    mOut.append(buff);
    return mOut;
  }

};
std::string nfmt::mOut;
