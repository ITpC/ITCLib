/**
 * public domain 
 * variants found somewhere on stackoverflow
 * 
 *  $Id: classEnumCasts.h August 20, 2019 10:27 PM $
 * 
 **/

#ifndef __CLASSENUMCASTS_H__
#  define __CLASSENUMCASTS_H__

namespace utils
{
  template<typename T, typename E> constexpr typename std::enable_if<
    std::is_integral<T>::value, T
  >::type to_integral(E e) noexcept
  {
    return static_cast<T>( e );
  }

  // will not work for 8-bit integral types (signed or unsigned)
  template <typename E> constexpr auto to_underlying(E e) noexcept
  {
    return static_cast<typename std::underlying_type<E>::type>(e);
  }

  template< typename E , typename T> constexpr typename std::enable_if<
    std::is_enum<E>::value &&
    std::is_integral<T>::value &&
    std::is_same<
      T,
      typename std::underlying_type<E>::type
    >::value,E
  >::type to_enum(T value) noexcept
  {
     return static_cast<E>( value );
  }
}

#endif /* __CLASSENUMCASTS_H__ */

