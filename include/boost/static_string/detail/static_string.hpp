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
#include <cwchar>

namespace boost {
namespace static_string {

template<std::size_t, typename, typename>
class basic_static_string;

namespace detail {

template<std::size_t N>
using static_string = basic_static_string<N, char, std::char_traits<char>>;

template<std::size_t N>
using static_wstring = basic_static_string<N, wchar_t, std::char_traits<wchar_t>>;

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

// void_t for c++11
template<typename...>
using void_t = void;

// Simplified check for if a type is an iterator
template<class T, typename = void>
struct is_iterator : std::false_type { };

template<class T>
struct is_iterator<T, 
    typename std::enable_if<std::is_class<T>::value, 
        void_t<typename T::iterator_category>>::type>
            : std::true_type { };

template<class T>
struct is_iterator<T*, void>
    : std::true_type { };

template<class T, typename = void>
struct is_input_iterator : std::false_type { };

template<class T>
struct is_input_iterator<T, typename std::enable_if<is_iterator<T>::value && 
    std::is_convertible<typename std::iterator_traits<T>::iterator_category, 
        std::input_iterator_tag>::value>::type>
            : std::true_type { };

template<class T, typename = void>
struct is_forward_iterator : std::false_type { };

template<class T>
struct is_forward_iterator<T, typename std::enable_if<is_iterator<T>::value &&
    std::is_convertible<typename std::iterator_traits<T>::iterator_category, 
        std::forward_iterator_tag>::value>::type>
            : std::true_type { };

template<typename T, typename = void>
struct is_subtractable 
    : std::false_type { };

template<typename T>
struct is_subtractable<T, void_t<decltype(std::declval<T&>() - std::declval<T&>())>>
    : std::true_type { };

// constexpr distance for c++14
template<typename InputIt, typename std::enable_if<!is_subtractable<InputIt>::value>::type* = nullptr>
BOOST_STATIC_STRING_CPP14_CONSTEXPR 
typename std::iterator_traits<InputIt>::difference_type
distance(InputIt first, InputIt last)
{
  std::size_t dist = 0;
  for (; first != last; ++first, ++dist);
  return dist;
}

template<typename RandomIt, typename std::enable_if<is_subtractable<RandomIt>::value>::type* = nullptr>
BOOST_STATIC_STRING_CPP14_CONSTEXPR 
typename std::iterator_traits<RandomIt>::difference_type
distance(RandomIt first, RandomIt last)
{
  return last - first;
}

// Copy using traits, respecting iterator rules
template<typename Traits, typename InputIt, typename CharT>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
void
copy_with_traits(InputIt first, InputIt last, CharT* out)
{
  for (; first != last; ++first, ++out)
    Traits::assign(*out, *first);
}

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
  CharT data_[N + 1]{0};
#endif
};

// Optimization for when the size is 0
template<typename CharT, typename Traits>
class static_string_base_zero<0, CharT, Traits>
{
public:
  BOOST_STATIC_STRING_CPP11_CONSTEXPR
  static_string_base_zero() noexcept { }

  BOOST_STATIC_STRING_CPP11_CONSTEXPR
  static_string_base_zero(std::size_t) noexcept { }

  // Modifying the null terminator is UB
  BOOST_STATIC_STRING_CPP11_CONSTEXPR
  CharT*
  data_impl() const noexcept
  {
    return const_cast<CharT*>(&null_);
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
  term_impl() noexcept { }

private:
  static constexpr CharT null_{0};
};

template<typename CharT, typename Traits>
constexpr CharT static_string_base_zero<0, CharT, Traits>::null_;

#ifdef BOOST_STATIC_STRING_NULL_OPTIMIZATION
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
  CharT data_[N + 1]{0};
#endif
};
#endif

// Decides which size optimization to use
// If the size is zero, the object will have no members
// Otherwise, if CharT can hold the max size of the string, store the size in the last char
// Otherwise, store the size of the string using a member of the smallest type possible
template<std::size_t N, typename CharT, typename Traits>
using optimization_base = 
#ifdef BOOST_STATIC_STRING_USE_NULL_OPTIMIZATION
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

template<typename Traits, typename Integer>
inline
char*
integer_to_string(
  char* str_end, Integer value, std::true_type)
{
  if (value == 0)
  {
    Traits::assign(*--str_end, '0');
    return str_end;
  }
  if (value < 0)
  {
    value = -value;
    for (; value > 0; value /= 10)
      Traits::assign(*--str_end, "0123456789"[value % 10]);
    Traits::assign(*--str_end, '-');
    return str_end;
  }
  for (; value > 0; value /= 10)
    Traits::assign(*--str_end, "0123456789"[value % 10]);
  return str_end;
}

template<typename Traits, typename Integer>
inline
char*
integer_to_string(
  char* str_end, Integer value, std::false_type)
{
  if (value == 0)
  {
    Traits::assign(*--str_end, '0');
    return str_end;
  }
  for (; value > 0; value /= 10)
    Traits::assign(*--str_end, "0123456789"[value % 10]);
  return str_end;
}

template<typename Traits, typename Integer>
inline
wchar_t*
integer_to_wstring(
  wchar_t* str_end, Integer value, std::true_type)
{
  if (value == 0)
  {
    Traits::assign(*--str_end, L'0');
    return str_end;
  }
  if (value < 0)
  {
    value = -value;
    for (; value > 0; value /= 10)
      Traits::assign(*--str_end, L"0123456789"[value % 10]);
    Traits::assign(*--str_end, L'-');
    return str_end;
  }
  for (; value > 0; value /= 10)
    Traits::assign(*--str_end, L"0123456789"[value % 10]);
  return str_end;
}

template<typename Traits, typename Integer>
inline
wchar_t*
integer_to_wstring(
  wchar_t* str_end, Integer value, std::false_type)
{
  if (value == 0)
  {
    Traits::assign(*--str_end, L'0');
    return str_end;
  }
  for (; value > 0; value /= 10)
    Traits::assign(*--str_end, L"0123456789"[value % 10]);
  return str_end;
}

template<std::size_t N, typename Integer>
inline
static_string<N>
to_static_string_int_impl(Integer value)
{
  char buffer[N];
  const auto digits_end = std::end(buffer);
  const auto digits_begin = integer_to_string<std::char_traits<char>, Integer>(
    digits_end, value, std::is_signed<Integer>{});
  return static_string<N>(digits_begin, std::distance(digits_begin, digits_end));
}

template<std::size_t N, typename Integer>
inline
static_wstring<N>
to_static_wstring_int_impl(Integer value)
{
  wchar_t buffer[N];
  const auto digits_end = std::end(buffer);
  const auto digits_begin = integer_to_wstring<std::char_traits<wchar_t>, Integer>(
    digits_end, value, std::is_signed<Integer>{});
  return static_wstring<N>(digits_begin, std::distance(digits_begin, digits_end));
}

template<std::size_t N, typename Floating>
inline
static_string<N>
to_static_string_float_impl(Floating value)
{
  char buffer[N + 1];
  std::sprintf(buffer, "%f", value);
  return static_string<N>(buffer);
}

template<std::size_t N, typename Floating>
inline
static_wstring<N>
to_static_wstring_float_impl(Floating value)
{
  wchar_t buffer[N + 1];
  std::swprintf(buffer, N + 1, L"%f", value);
  return static_wstring<N>(buffer);
}

template<
    typename Traits,
    typename CharT,
    typename ForwardIterator>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
ForwardIterator
find_not_of(
  ForwardIterator first, 
  ForwardIterator last, 
  const CharT* str, 
  std::size_t n) noexcept
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

