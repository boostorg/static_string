//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
// Copyright (c) 2019 Krystian Stasiowski (sdkrystian at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/static_string
//

#ifndef BOOST_STATIC_STRING_DETAIL_STATIC_STRING_HPP
#define BOOST_STATIC_STRING_DETAIL_STATIC_STRING_HPP

#include <boost/static_string/config.hpp>
#include <iterator>
#include <type_traits>
#include <limits>

namespace boost {
namespace static_string {
namespace detail {

template<std::size_t, typename, typename>
class basic_static_string;

// At minimum an integral type shall not qualify as an iterator (Agustin Berge)
template<class T>
using is_input_iterator =
    std::integral_constant<bool,
        ! std::is_integral<T>::value>;

// Find the smallest width integral type that can hold a value as large as N (Glen Fernandes)
template<std::size_t N>
using smallest_width =
    typename std::conditional<(N <= (std::numeric_limits<unsigned char>::max)()), unsigned char,
    typename std::conditional<(N <= (std::numeric_limits<unsigned short>::max)()), unsigned short,
    typename std::conditional<(N <= (std::numeric_limits<unsigned int>::max)()), unsigned int,
    typename std::conditional<(N <= (std::numeric_limits<unsigned long>::max)()), unsigned long,
    typename std::conditional<(N <= (std::numeric_limits<unsigned long long>::max)()), unsigned long long,
    void>::type>::type>::type>::type>::type;

// std::is_nothrow_convertible is C++20
template<typename To>
void is_nothrow_convertible_helper(To) noexcept;

// MSVC is unable to parse this inline, so a helper is needed
template<typename From, typename To, typename = 
    decltype(is_nothrow_convertible_helper<To>(std::declval<From>()))>
struct is_nothrow_convertible_msvc_helper
{
  static const bool value =
      noexcept(is_nothrow_convertible_helper<To>(std::declval<From>()));
};

template<typename From, typename To, typename = void>
struct is_nothrow_convertible
    : std::false_type { };

template<typename From, typename To>
struct is_nothrow_convertible<From, To, typename std::enable_if<
    is_nothrow_convertible_msvc_helper<From, To>::value>::type>
        : std::true_type { };

// Optimization for using the smallest possible type
template<std::size_t N, typename CharT, typename Traits>
class static_string_base_zero
{
public:
  BOOST_STATIC_STRING_CPP11_CONSTEXPR
  static_string_base_zero() noexcept { };

  BOOST_STATIC_STRING_CPP11_CONSTEXPR
  static_string_base_zero(std::size_t n) noexcept : size_(n) { }

  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  CharT*
  data_impl() noexcept
  {
    return data_;
  }

  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  CharT const*
  data_impl() const noexcept
  {
    return data_;
  }

  BOOST_STATIC_STRING_CPP11_CONSTEXPR
  std::size_t
  size_impl() const noexcept
  {
    return size_;
  }

  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  std::size_t
  set_size(std::size_t n) noexcept
  {
    return size_ = n;
  }

  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  void
  term_impl() noexcept
  {
    Traits::assign(data_[size_], 0);
  }

  smallest_width<N> size_{0};
#ifdef BOOST_STATIC_STRING_ALLOW_UNINIT_MEM
  CharT data_[N + 1];
#else
  CharT data_[N + 1]{};
#endif
};

// Optimization for when the size is 0
template<typename CharT, typename Traits>
class static_string_base_zero<0, CharT, Traits>
{
public:
  BOOST_STATIC_STRING_CPP11_CONSTEXPR
  static_string_base_zero() noexcept {  }

  BOOST_STATIC_STRING_CPP11_CONSTEXPR
  static_string_base_zero(std::size_t) noexcept { }

  // not possible to constexpr with the static there
  CharT*
  data_impl() const noexcept
  {
    static CharT null{};
    return &null;
  }

  BOOST_STATIC_STRING_CPP11_CONSTEXPR
  std::size_t
  size_impl() const noexcept
  {
    return 0;
  }

  BOOST_STATIC_STRING_CPP11_CONSTEXPR
  std::size_t
  set_size(std::size_t) noexcept
  {
    return 0;
  }

  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  void
  term_impl() noexcept
  {

  }
};

// Optimization for storing the size in the last element
template<std::size_t N, typename CharT, typename Traits>
class static_string_base_null
{
public:
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  static_string_base_null() noexcept { set_size(0); }

  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  static_string_base_null(std::size_t n) noexcept { set_size(n); }

  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  CharT*
  data_impl() noexcept
  {
    return data_;
  }

  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  CharT const*
  data_impl() const noexcept
  {
    return data_;
  }

  BOOST_STATIC_STRING_CPP11_CONSTEXPR
  std::size_t
  size_impl() const noexcept
  {
    return N - data_[N];
  }

  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  std::size_t
  set_size(std::size_t n) noexcept
  {
    return data_[N] = (N - n);
  }

  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  void
  term_impl() noexcept
  {
    Traits::assign(data_[size_impl()], 0);
  }

#ifdef BOOST_STATIC_STRING_ALLOW_UNINIT_MEM
  CharT data_[N + 1];
#else
  CharT data_[N + 1]{};
#endif
};

//#define BOOST_STATIC_STRING_NO_NULL_OPTIMIZATION

// Decides which size optimization to use
// If the size is zero, the object will have no members
// Otherwise, if CharT can hold the max size of the string, store the size in the last char
// Otherwise, store the size of the string using a member of the smallest type possible
template<std::size_t N, typename CharT, typename Traits>
using optimization_base = 
#ifndef BOOST_STATIC_STRING_NO_NULL_OPTIMIZATION
    typename std::conditional<(N <= (std::numeric_limits<CharT>::max)()) && (N != 0), 
        static_string_base_null<N, CharT, Traits>,
        static_string_base_zero<N, CharT, Traits>>::type;
#else
    static_string_base_zero<N, CharT, Traits>;
#endif

template<typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
int
lexicographical_compare(
    CharT const* s1, std::size_t n1,
    CharT const* s2, std::size_t n2) noexcept
{
    if(n1 < n2)
        return Traits::compare(
            s1, s2, n1) <= 0 ? -1 : 1;
    if(n1 > n2)
        return Traits::compare(
            s1, s2, n2) >= 0 ? 1 : -1;
    return Traits::compare(s1, s2, n1);
}

template<typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
int
lexicographical_compare(
    basic_string_view<CharT, Traits> s1,
    CharT const* s2, std::size_t n2) noexcept
{
    return detail::lexicographical_compare<
        CharT, Traits>(s1.data(), s1.size(), s2, n2);
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
int
lexicographical_compare(
    const basic_static_string<N, CharT, Traits>& s1,
    CharT const* s2, std::size_t n2) noexcept
{
    return detail::lexicographical_compare<
        CharT, Traits>(s1.data(), s1.size(), s2, n2);
}

template<typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
int
lexicographical_compare(
    basic_string_view<CharT, Traits> s1,
    basic_string_view<CharT, Traits> s2) noexcept
{
    return detail::lexicographical_compare<CharT, Traits>(
        s1.data(), s1.size(), s2.data(), s2.size());
}

template<std::size_t N, std::size_t M, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
int 
lexicographical_compare(
    const basic_static_string<N, CharT, Traits>& s1, 
    const basic_static_string<M, CharT, Traits>& s2) noexcept
{
  return detail::lexicographical_compare<CharT, Traits>(
    s1.data(), s1.size(), s2.data(), s2.size());
}

// Maximum number of characters in the decimal
// representation of a binary number. This includes
// the potential minus sign.
//
BOOST_STATIC_STRING_CPP11_CONSTEXPR
inline
std::size_t
max_digits(std::size_t bytes)
{
    return static_cast<std::size_t>(
        bytes * 2.41) + 1 + 1;
}

template<typename CharT, class Integer, typename Traits>
inline
CharT*
raw_to_string(
    CharT* buf, Integer x, std::true_type)
{
    if(x == 0)
    {
        Traits::assign(*--buf, '0');
        return buf;
    }
    if(x < 0)
    {
        x = -x;
        for(;x > 0; x /= 10)
            Traits::assign(*--buf ,
                "0123456789"[x % 10]);
        Traits::assign(*--buf, '-');
        return buf;
    }
    for(;x > 0; x /= 10)
        Traits::assign(*--buf ,
            "0123456789"[x % 10]);
    return buf;
}

template<typename CharT, class Integer, typename Traits>
inline
CharT*
raw_to_string(
    CharT* buf, Integer x, std::false_type)
{
    if(x == 0)
    {
        *--buf = '0';
        return buf;
    }
    for(;x > 0; x /= 10)
        Traits::assign(*--buf ,
            "0123456789"[x % 10]);
    return buf;
}

template<
    typename CharT,
    class Integer,
    typename Traits = std::char_traits<CharT>>
inline
CharT*
raw_to_string(CharT* last, std::size_t size, Integer i)
{
    BOOST_STATIC_STRING_ASSERT(size >= max_digits(sizeof(Integer)));
    return raw_to_string<CharT, Integer, Traits>(
        last, i, std::is_signed<Integer>{});
}

template<
    typename Traits,
    typename CharT,
    typename ForwardIterator>
inline
ForwardIterator
find_not_of(
  ForwardIterator first, ForwardIterator last, const CharT* str, std::size_t n) noexcept
{
  for (; first != last; ++first)
    if (!Traits::find(str, n, *first))
      return first;
  return last;
}

} // detail
} // static_string
} // boost

#endif
