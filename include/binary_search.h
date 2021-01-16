#ifndef __BINARY_SEARCH__
#define __BINARY_SEARCH__
namespace itc
{
  namespace utils
  {
    template <typename ForwardIterator, typename T, typename UnaryFunction> void exec_if_found(ForwardIterator first, ForwardIterator last, const T& val, UnaryFunction EXEC, std::forward_iterator_tag)
    {
      first = std::lower_bound(first,last,val);
      if(first!=last && !(val<*first))
          EXEC(first);
    }

    template <typename ForwardIterator, typename T, typename UnaryFunction> void exec_if_found(ForwardIterator first, ForwardIterator last, const T& val, UnaryFunction EXEC)
    {
      exec_if_found(first,last,val,EXEC,typename std::iterator_traits<ForwardIterator>::iterator_category());
    }

    template <typename ForwardIterator, typename T > ForwardIterator binary_search(ForwardIterator first, ForwardIterator last, const T& val, std::forward_iterator_tag)
    {
      first = std::lower_bound(first,last,val);
      if(first!=last && !(val<*first))
          return first;
      return last;
    }
    
    template <typename ForwardIterator, typename T> const bool exists(ForwardIterator first, ForwardIterator last, const T& val)
    {
      return exec_if_found(first,last,val,typename std::iterator_traits<ForwardIterator>::iterator_category()) != last;
    }

    template <typename ForwardIterator, typename T> const ForwardIterator binary_search(ForwardIterator first, ForwardIterator last, const T& val)
    {
      return exec_if_found(first,last,val,typename std::iterator_traits<ForwardIterator>::iterator_category());
    }
  }
}

#endif
