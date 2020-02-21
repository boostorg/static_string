//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
// Copyright (c) 2019-2020 Krystian Stasiowski (sdkrystian at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/static_string
//

#ifndef BOOST_STATIC_STRING_STATIC_STRING_HPP
#define BOOST_STATIC_STRING_STATIC_STRING_HPP

#include <boost/static_string/config.hpp>
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cwchar>
#include <functional>
#include <initializer_list>
#include <iosfwd>
#include <type_traits>

namespace boost {
namespace static_string {

template<std::size_t, typename, typename>
class basic_static_string;

//------------------------------------------------------------------------------
//
// Aliases
//
//------------------------------------------------------------------------------

template<std::size_t N>
using static_string =
  basic_static_string<N, char, std::char_traits<char>>;

template<std::size_t N>
using static_wstring =
  basic_static_string<N, wchar_t, std::char_traits<wchar_t>>;

template<std::size_t N>
using static_u16string =
  basic_static_string<N, char16_t, std::char_traits<char16_t>>;

template<std::size_t N>
using static_u32string =
  basic_static_string<N, char32_t, std::char_traits<char32_t>>;

//--------------------------------------------------------------------------
//
// Detail
//
//--------------------------------------------------------------------------
#ifndef GENERATING_DOCUMENTATION
namespace detail {

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

// MSVC is unable to parse this as a single expression, so a helper is needed
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

// GCC 4.8, 4.9 workaround for void_t to make the defining-type-id dependant 
template<typename...>
struct void_t_helper
{
  using type = void;
};

// void_t for c++11
template<typename... Ts>
using void_t = typename void_t_helper<Ts...>::type;

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
template<
  typename ForwardIt,
  typename std::enable_if<!is_subtractable<ForwardIt>::value>::type* = nullptr>
BOOST_STATIC_STRING_CPP14_CONSTEXPR 
std::size_t
distance(ForwardIt first, ForwardIt last)
{
  std::size_t dist = 0;
  for (; first != last; ++first, ++dist);
  return dist;
}

template<
  typename RandomIt,
  typename std::enable_if<is_subtractable<RandomIt>::value>::type* = nullptr>
BOOST_STATIC_STRING_CPP14_CONSTEXPR 
std::size_t
distance(RandomIt first, RandomIt last)
{
  return last - first;
}

// Copy using traits, respecting iterator rules
template<typename Traits, typename InputIt, typename CharT>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
void
copy_with_traits(
  InputIt first,
  InputIt last,
  CharT* out)
{
  for (; first != last; ++first, ++out)
    Traits::assign(*out, *first);
}

// Optimization for using the smallest possible type
template<std::size_t N, typename CharT, typename Traits>
class static_string_base
{
private:
  using size_type = smallest_width<N>;
  using value_type = typename Traits::char_type;
  using pointer = value_type*;
  using const_pointer = const value_type*;
public:
  BOOST_STATIC_STRING_CPP11_CONSTEXPR
  static_string_base() noexcept { };

  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  pointer
  data_impl() noexcept
  {
    return data_;
  }

  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  const_pointer
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
    // Functions that set size will throw
    // if the new size would exceed max_size()
    // therefore we can guarantee that this will
    // not lose data.
    return size_ = size_type(n);
  }

  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  void
  term_impl() noexcept
  {
    Traits::assign(data_[size_], value_type());
  }

  size_type size_{0};
#ifdef BOOST_STATIC_STRING_CPP20
  value_type data_[N + 1];
#else
  value_type data_[N + 1]{};
#endif
};

// Optimization for when the size is 0
template<typename CharT, typename Traits>
class static_string_base<0, CharT, Traits>
{
private:
  using value_type = typename Traits::char_type;
  using pointer = value_type*;
public:
  BOOST_STATIC_STRING_CPP11_CONSTEXPR
  static_string_base() noexcept { }

  // Modifying the null terminator is UB
  BOOST_STATIC_STRING_CPP11_CONSTEXPR
  pointer
  data_impl() const noexcept
  {
    return const_cast<pointer>(&null_);
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
  static constexpr const value_type null_{};
};

template<typename CharT, typename Traits>
constexpr
const
typename static_string_base<0, CharT, Traits>::value_type
static_string_base<0, CharT, Traits>::
null_;

template<typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
int
lexicographical_compare(
  const CharT* s1,
  std::size_t n1,
  const CharT* s2,
  std::size_t n2) noexcept
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
  const CharT* s2,
  std::size_t n2) noexcept
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
  const CharT* s2,
  std::size_t n2) noexcept
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
  char* str_end,
  Integer value,
  std::true_type) noexcept
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
  char* str_end,
  Integer value,
  std::false_type) noexcept
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
  wchar_t* str_end,
  Integer value,
  std::true_type) noexcept
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
  wchar_t* str_end,
  Integer value,
  std::false_type) noexcept
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
to_static_string_int_impl(Integer value) noexcept
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
to_static_wstring_int_impl(Integer value) noexcept
{
  wchar_t buffer[N];
  const auto digits_end = std::end(buffer);
  const auto digits_begin = integer_to_wstring<std::char_traits<wchar_t>, Integer>(
    digits_end, value, std::is_signed<Integer>{});
  return static_wstring<N>(digits_begin, std::distance(digits_begin, digits_end));
}

template<std::size_t N>
inline
static_string<N>
to_static_string_float_impl(double value) noexcept
{
  // extra one needed for null terminator
  char buffer[N + 1];
  std::sprintf(buffer, "%f", value);
  // this will not throw
  return static_string<N>(buffer);
}

template<std::size_t N>
inline
static_string<N>
to_static_string_float_impl(long double value) noexcept
{
  // extra one needed for null terminator
  char buffer[N + 1];
  std::sprintf(buffer, "%Lf", value);
  // this will not throw
  return static_string<N>(buffer);
}

template<std::size_t N>
inline
static_wstring<N>
to_static_wstring_float_impl(double value) noexcept
{
  // extra one needed for null terminator
  wchar_t buffer[N + 1];
  std::swprintf(buffer, N + 1, L"%f", value);
  // this will not throw
  return static_wstring<N>(buffer);
}

template<std::size_t N>
inline
static_wstring<N>
to_static_wstring_float_impl(long double value) noexcept
{
  // extra one needed for null terminator
  wchar_t buffer[N + 1];
  std::swprintf(buffer, N + 1, L"%Lf", value);
  // this will not throw
  return static_wstring<N>(buffer);
}

template<typename Traits, typename CharT, typename ForwardIterator>
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

// constexpr search for C++14
template<typename ForwardIt1, typename ForwardIt2, typename BinaryPredicate>
BOOST_STATIC_STRING_CPP14_CONSTEXPR 
inline
ForwardIt1 
search(
  ForwardIt1 first,
  ForwardIt1 last,
  ForwardIt2 s_first,
  ForwardIt2 s_last,
  BinaryPredicate p)
{
  for (; ; ++first)
  {
    ForwardIt1 it = first;
    for (ForwardIt2 s_it = s_first; ; ++it, ++s_it)
    {
      if (s_it == s_last)
        return first;
      if (it == last)
        return last;
      if (!p(*it, *s_it))
        break;
    }
  }
}

template<typename InputIt, typename ForwardIt, typename BinaryPredicate>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
InputIt 
find_first_of(
  InputIt first,
  InputIt last,
  ForwardIt s_first,
  ForwardIt s_last,
  BinaryPredicate p)
{
  for (; first != last; ++first)
    for (ForwardIt it = s_first; it != s_last; ++it)
      if (p(*first, *it))
        return first;
  return last;
}

// Check if a pointer lies within the range [src_first, src_last) 
// without unspecified behavior, allowing it to be used
// in a constant evaluation.
template<typename T>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
bool
ptr_in_range(
  const T* src_first, 
  const T* src_last,
  const T* ptr)
{
#if defined(BOOST_STATIC_STRING_CPP14) && \
defined(BOOST_STATIC_STRING_IS_CONST_EVAL)
  // Our second best option is to use is_constant_evaluated
  // and a loop that checks for equality, since equality for 
  // pointer to object types is never unspecified in this case.
  if (BOOST_STATIC_STRING_IS_CONST_EVAL)
  {
    for (; src_first != src_last; ++src_first)
      if (ptr == src_first)
        return true;
    return false;
  }
#endif
  // We want to make this usable in constant expressions as much as possible
  // while retaining the guarentee that the comparison has a strict total ordering.
  // We also want this to be fast. Since different compilers have differing levels
  // of conformance, we will settle for the best option that is available.
  // We don't care about this in C++11, since this function would have
  // no applications in constant expressions.
#if defined(BOOST_STATIC_STRING_CPP14) && \
defined(BOOST_STATIC_STRING_NO_PTR_COMP_FUNCTIONS) 
  // If library comparison functions don't work,
  // we can use try builtin comparison operators instead.
  return ptr >= src_first && ptr < src_last;
#else
  // Use the library comparison functions if we can't use 
  // is_constant_evaluated or if we don't need to.
  return std::greater_equal<const T*>()(ptr, src_first) &&
    std::less<const T*>()(ptr, src_last);
#endif
}
} // detail
#endif

//--------------------------------------------------------------------------
//
// static_string
//
//--------------------------------------------------------------------------

/** A fixed-capacity string.

    These objects behave like `std::string` except that the storage
    is not dynamically allocated but rather fixed in size, and
    stored in the object itself.

    These strings offer performance advantages when an algorithm
    can execute with a reasonable upper limit on the size of a value.

    @see to_static_string 
*/
template<std::size_t N, typename CharT,
  typename Traits = std::char_traits<CharT>>
class basic_static_string 
#ifndef GENERATING_DOCUMENTATION
  : private detail::static_string_base<N, CharT, Traits>
#endif
{
private:
  template<std::size_t, class, class>
  friend class basic_static_string;
public:
  //--------------------------------------------------------------------------
  //
  // Member types
  //
  //--------------------------------------------------------------------------

  using traits_type       = Traits;
  using value_type        = typename traits_type::char_type;
  using size_type         = std::size_t;
  using difference_type   = std::ptrdiff_t;
  using pointer           = value_type*;
  using reference         = value_type&;
  using const_pointer     = const value_type*;
  using const_reference   = const value_type&;
  using iterator          = value_type*;
  using const_iterator    = const value_type*;
    
  using reverse_iterator =
    std::reverse_iterator<iterator>;
    
  using const_reverse_iterator =
    std::reverse_iterator<const_iterator>;

  /// The type of `string_view_type` returned by the interface
  using string_view_type =
    basic_string_view<value_type, traits_type>;

  //--------------------------------------------------------------------------
  //
  // Constants
  //
  //--------------------------------------------------------------------------

  /// Maximum size of the string excluding any null terminator
  static constexpr size_type static_capacity = N;

  /// A special index
  static constexpr size_type npos = size_type(-1);

  //--------------------------------------------------------------------------
  //
  // Construction
  //
  //--------------------------------------------------------------------------

  /** Construct a `basic_static_string`.

      Construct an empty string
  */
  BOOST_STATIC_STRING_CPP11_CONSTEXPR
  basic_static_string() noexcept
  {
#ifdef BOOST_STATIC_STRING_CPP20
    term();
#endif
  }

  /** Construct a `basic_static_string`.
    
      Construct the string with `count` copies of character `ch`.
    
      The behavior is undefined if `count >= npos`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string(
    size_type count,
    value_type ch)
  {
    assign(count, ch);
  }

  /** Construct a `basic_static_string`.
        
      Construct with a substring (pos, other.size()) of `other`.
  */
  template<std::size_t M>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string(
    const basic_static_string<M, CharT, Traits>& other,
    size_type pos)
  {
    assign(other, pos);
  }

  /** Construct a `basic_static_string`.
    
      Construct with a substring (pos, count) of `other`.
  */
  template<std::size_t M>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string(
    const basic_static_string<M, CharT, Traits>& other,
    size_type pos,
    size_type count)
  {
    assign(other, pos, count);
  }

  /** Construct a `basic_static_string`.
        
      Construct with the first `count` characters of `s`, including nulls.
    */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string(
    const_pointer s,
    size_type count)
  {
    assign(s, count);
  }

  /** Construct a `basic_static_string`.
        
      Construct from a null terminated string.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string(const_pointer s);

  // KRYSTIAN TODO: we can use a better algorithm if this is a forward iterator
  /** Construct a `basic_static_string`.
    
      Construct from a range of characters
  */
  template<class InputIterator
#ifndef GENERATING_DOCUMENTATION
    , typename std::enable_if<
      detail::is_input_iterator<InputIterator>
        ::value>::type* = nullptr
#endif
  >
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string(
    InputIterator first,
    InputIterator last)
  {
    assign(first, last);
  }

  /** Construct a `basic_static_string`.
        
      Copy constructor.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string(const basic_static_string& other) noexcept
  {
    assign(other);
  }

  /** Construct a `basic_static_string`.
        
      Copy constructor.
  */
  template<std::size_t M>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string(
    const basic_static_string<M, CharT, Traits>& other)
  {
    assign(other);
  }

  /** Construct a `basic_static_string`.
        
      Construct from an initializer list
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string(std::initializer_list<value_type> init)
  {
    assign(init.begin(), init.end());
  }

  // KRYSTIAN TODO: this may need to be templated
  /** Construct a `basic_static_string`.
    
      Construct from a `string_view_type`
  */
  explicit
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string(string_view_type sv)
  {
    assign(sv);
  }

  /** Construct a `basic_static_string`.
    
      Construct from any object convertible to `string_view_type`.

      The range (pos, n) is extracted from the value
      obtained by converting `t` to `string_view_type`,
      and used to construct the string.
  */
  template<typename T
#ifndef GENERATING_DOCUMENTATION
    , class = typename std::enable_if<
      std::is_convertible<T, string_view_type>::value>::type
#endif
  >
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string(
    const T& t,
    size_type pos,
    size_type n)
  {
    assign(t, pos, n);
  }

  //--------------------------------------------------------------------------
  //
  // Assignment
  //
  //--------------------------------------------------------------------------

  /** Assign to the string.

      If `*this` and `s` are the same object,
      this function has no effect.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  operator=(const basic_static_string& s) noexcept
  {
    return assign(s);
  }

  /** Assign to the string.
    
      Replace the contents with a copy of `s`

      @throw std::length_error if `s.size() > max_size()`
  */
  template<std::size_t M>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  operator=(const basic_static_string<M, CharT, Traits>& s)
  {
    return assign(s);
  }

  /** Assign to the string.
    
      Replace the contents with those of the null-terminated string `s`

      @throw std::length_error if `traits_type::length(s) > max_size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  operator=(const_pointer s)
  {
    return assign(s);
  }

  /** Assign to the string.
    
      Assign from single character.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  operator=(value_type ch)
  {
    return assign_char(ch,
      std::integral_constant<bool, (N > 0)>{});
  }

  /** Assign to the string.

      Assign from initializer list.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  operator=(std::initializer_list<value_type> ilist)
  {
    return assign(ilist);
  }

  /** Assign to the string.
    
      Assign from `string_view_type`.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  operator=(string_view_type sv)
  {
    return assign(sv);
  }

  /** Replace the contents.
    
      Replace the contents with `count` copies of character `ch`

      @throw std::length_error if `count > max_size()`
      @return `*this`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  assign(
    size_type count,
    value_type ch);

  /** Replace the contents.
    
      Replace the contents with a copy of another `basic_static_string`

      @return `*this`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  assign(const basic_static_string& s) noexcept;

  /** Replace the contents.
    
      Replace the contents with a copy of another `basic_static_string`

      @throw std::length_error if `s.size() > max_size()`
      @return `*this`
  */
  template<std::size_t M>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  assign(const basic_static_string<M, CharT, Traits>& s)
  {
    // VFALCO this could come in two flavors,
    //        N>M and N<M, and skip the exception
    //        check when N>M
    return assign(s.data(), s.size());
  }

  /** Replace the contents.
    
      Replace the contents with a copy of `count` characters starting at `npos` from `s`.

      @throw std::length_error if `count > max_size()`
      @return `*this`
  */
  template<std::size_t M>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  assign(
    const basic_static_string<M, CharT, Traits>& s,
    size_type pos,
    size_type count = npos);

  /** Replace the contents.
    
      Replace the contents with the first `count` characters of `s`, including nulls.

      @throw std::length_error if `count > max_size()`
      @return `*this`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  assign(
    const_pointer s,
    size_type count);

  /** Replace the contents.
    
      Replace the contents with a copy of a null terminated string `s`

      @throw std::length_error if `traits_type::length(s) > max_size()`
      @return `*this`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  assign(const_pointer s)
  {
    return assign(s, traits_type::length(s));
  }

  /** Replace the contents.
    
      Replace the contents with a copy of characters from the range `(first, last)`
      
      @throw std::length_error if `std::distance(first, last) > max_size()`
      @return `*this`
  */
  template<typename InputIterator>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
#ifdef GENERATING_DOCUMENTATION
  basic_static_string&
#else
  typename std::enable_if<
    detail::is_input_iterator<InputIterator>::value,
      basic_static_string&>::type
#endif
  assign(
    InputIterator first,
    InputIterator last);

  /** Replace the contents.
    
      Replace the contents with the characters in an initializer list

      @throw std::length_error if `ilist.size() > max_size()`
      @return `*this`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  assign(
    std::initializer_list<value_type> ilist)
  {
      return assign(ilist.begin(), ilist.end());
  }

  /** Replace the contents.
    
      Replace the contents with a copy of the characters from `string_view_type{t}`

      @throw std::length_error if `string_view_type{t}.size() > max_size()`
  */
  template<typename T>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
  basic_static_string&
#else
  typename std::enable_if<
    std::is_convertible<T, string_view_type>::value &&
      !std::is_convertible<T, const_pointer>::value,
        basic_static_string&>::type
#endif
  assign(const T& t)
  {
    string_view_type ss{t};
    return assign(ss.data(), ss.size());
  }

  /** Replace the contents.
    
      Replace the contents with a copy of the characters from `string_view_type{t}.substr(pos, count)`

      The range `[pos, count)` is extracted from the value
      obtained by converting `t` to `string_view_type`,
      and used to assign the string.

      @throw std::out_of_range if `pos > string_view_type{t}.size()`
      @throw std::length_error if `string_view_type{t}.substr(pos, count).size() > max_size()`
  */

  template<typename T>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
  basic_static_string&
#else
  typename std::enable_if<
    std::is_convertible<T, string_view_type>::value &&
      !std::is_convertible<T, const_pointer>::value,
        basic_static_string&>::type
#endif
  assign(
    const T& t,
    size_type pos,
    size_type count = npos)
  {
    return assign(string_view_type{t}.substr(pos, count));
  }

  //--------------------------------------------------------------------------
  //
  // Element access
  //
  //--------------------------------------------------------------------------

  /** Access specified character with bounds checking.

      @throw std::out_of_range if `pos >= size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  reference
  at(size_type pos)
  {
    BOOST_STATIC_STRING_THROW_IF(
      pos >= size(), std::out_of_range{"pos >= size()"});
    return data()[pos];
  }

  /** Access specified character with bounds checking.

      @throw std::out_of_range if `pos >= size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  const_reference
  at(size_type pos) const
  {
    BOOST_STATIC_STRING_THROW_IF(
      pos >= size(), std::out_of_range{"pos >= size()"});
    return data()[pos];
  }

  /** Access specified character

      Undefined behavior if `pos > size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  reference
  operator[](size_type pos) noexcept
  {
    return data()[pos];
  }

  /** Access specified character.

      Undefined behavior if `pos > size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  const_reference
  operator[](size_type pos) const noexcept
  {
    return data()[pos];
  }

  /** Accesses the first character.

      Undefined behavior if `empty() == true`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  reference
  front() noexcept
  {
    return data()[0];
  }

  /** Accesses the first character.

      Undefined behavior if `empty() == true`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  const_reference
  front() const noexcept
  {
    return data()[0];
  }

  /** Accesses the last character.

      Undefined behavior if `empty() == true`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  reference
  back() noexcept
  {
    return data()[size() - 1];
  }

  /** Accesses the last character.

      Undefined behavior if `empty() == true`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  const_reference
  back() const noexcept
  {
    return data()[size() - 1];
  }

  /// Returns a pointer to the first character of the string.
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  pointer
  data() noexcept
  {
    return this->data_impl();
  }

  /// Returns a pointer to the first character of a string.
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  const_pointer
  data() const noexcept
  {
    return this->data_impl();
  }

  /// Returns a non-modifiable standard C character array version of the string.
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  const_pointer
  c_str() const noexcept
  {
    return data();
  }

  /// Convert a static string to a `string_view_type`
  BOOST_STATIC_STRING_CPP11_CONSTEXPR
  operator string_view_type() const noexcept
  {
    return string_view_type{data(), size()};
  }

  //--------------------------------------------------------------------------
  //
  // Iterators
  //
  //--------------------------------------------------------------------------

  /// Returns an iterator to the beginning.
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  iterator
  begin() noexcept
  {
    return data();
  }

  /// Returns an iterator to the beginning.
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  const_iterator
  begin() const noexcept
  {
    return data();
  }

  /// Returns an iterator to the beginning.
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  const_iterator
  cbegin() const noexcept
  {
    return data();
  }

  /// Returns an iterator to the end.
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  iterator
  end() noexcept
  {
    return &data()[size()];
  }

  /// Returns an iterator to the end.
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  const_iterator
  end() const noexcept
  {
    return &data()[size()];
  }

  /// Returns an iterator to the end.
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  const_iterator
  cend() const noexcept
  {
    return &data()[size()];
  }

  /// Returns a reverse iterator to the beginning.
  BOOST_STATIC_STRING_CPP17_CONSTEXPR
  reverse_iterator
  rbegin() noexcept
  {
    return reverse_iterator{end()};
  }

  /// Returns a reverse iterator to the beginning.
  BOOST_STATIC_STRING_CPP17_CONSTEXPR
  const_reverse_iterator
  rbegin() const noexcept
  {
    return const_reverse_iterator{cend()};
  }

  /// Returns a reverse iterator to the beginning.
  BOOST_STATIC_STRING_CPP17_CONSTEXPR
  const_reverse_iterator
  crbegin() const noexcept
  {
    return const_reverse_iterator{cend()};
  }

  /// Returns a reverse iterator to the end.
  BOOST_STATIC_STRING_CPP17_CONSTEXPR
  reverse_iterator
  rend() noexcept
  {
    return reverse_iterator{begin()};
  }

  /// Returns a reverse iterator to the end.
  BOOST_STATIC_STRING_CPP17_CONSTEXPR
  const_reverse_iterator
  rend() const noexcept
  {
    return const_reverse_iterator{cbegin()};
  }

  /// Returns a reverse iterator to the end.
  BOOST_STATIC_STRING_CPP17_CONSTEXPR
  const_reverse_iterator
  crend() const noexcept
  {
    return const_reverse_iterator{cbegin()};
  }

  //--------------------------------------------------------------------------
  //
  // Capacity
  //
  //--------------------------------------------------------------------------

  /// Returns `true` if the string is empty.
  BOOST_STATIC_STRING_NODISCARD
  BOOST_STATIC_STRING_CPP11_CONSTEXPR
  bool
  empty() const noexcept
  {
    return size() == 0;
  }

  /// Returns the number of characters, excluding the null terminator.
  BOOST_STATIC_STRING_CPP11_CONSTEXPR
  size_type
  size() const noexcept
  {
    return this->size_impl();
  }

  /** Returns the number of characters, excluding the null terminator

      Equivalent to calling `size()`.
  */
  BOOST_STATIC_STRING_CPP11_CONSTEXPR
  size_type
  length() const noexcept
  {
    return size();
  }

  /// Returns the maximum number of characters that can be stored, excluding the null terminator.
  BOOST_STATIC_STRING_CPP11_CONSTEXPR
  size_type
  max_size() const noexcept
  {
    return N;
  }

  /** Reserve space for `n` characters, excluding the null terminator

      This function has no effect when `n <= max_size()`.

      @throw std::length_error if `n > max_size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  void
  reserve(size_type n)
  {
    BOOST_STATIC_STRING_THROW_IF(
      n > max_size(), std::length_error{"n > max_size()"});
  }

  /** Returns the number of characters that can be held in currently allocated storage.

      This function always returns `max_size()`.
  */
  BOOST_STATIC_STRING_CPP11_CONSTEXPR
  size_type
  capacity() const noexcept
  {
    return max_size();
  }
    
  /** Reduces memory usage by freeing unused memory.

      This function call has no effect.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  void
  shrink_to_fit() noexcept { }

  //--------------------------------------------------------------------------
  //
  // Operations
  //
  //--------------------------------------------------------------------------

  /// Clears the contents
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  void
  clear() noexcept
  {
    this->set_size(0);
    term();
  }

  /** Insert a character.

      Inserts `count` copies of `ch` at the position `index`.

      @par Exception Safety

      Strong guarantee.

      @note All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @return `*this`

      @param index The index to insert at.
      @param count The number of characters to insert.
      @param ch The character to insert.

      @throw std::length_error `size() + count > max_size()`
      @throw std::out_of_range `index > size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  insert(
    size_type index,
    size_type count,
    value_type ch)
  {
    BOOST_STATIC_STRING_THROW_IF(
      index > size(), std::out_of_range{"index > size()"});
    insert(begin() + index, count, ch);
    return *this;
  }
    
  /** Insert a string.

      Inserts the null-terminated character string pointed to by `s`
      of length `count` at the position `index` where `count`
      is `traits_type::length(s)`.

      @par Exception Safety

      Strong guarantee.

      @note All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @return `*this`

      @param index The index to insert at.
      @param s The string to insert.

      @throw std::length_error `size() + count > max_size()`
      @throw std::out_of_range `index > size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  insert(
    size_type index,
    const_pointer s)
  {
    return insert(index, s, traits_type::length(s));
  }

  /** Insert a string.

      Inserts `count` characters of the string pointed to by `s`
      at the position `index`.

      @par Exception Safety

      Strong guarantee.

      @note All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @return `*this`

      @param index The index to insert at.
      @param s The string to insert.
      @param count The length of the string to insert.

      @throw std::length_error `size() + count > max_size()`
      @throw std::out_of_range `index > size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  insert(
    size_type index,
    const_pointer s,
    size_type count)
  {
    BOOST_STATIC_STRING_THROW_IF(
      index > size(), std::out_of_range{"index > size()"});
    insert(data() + index, s, s + count);
    return *this;
  }

  /** Insert a string.

      Inserts the string `str`
      at the position `index`.

      @par Exception Safety

      Strong guarantee.

      @note The insertion is done unchecked when
      the capacity of `str` differs from that of the
      string the function is called on.

      @note All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @tparam M The size of the input string.

      @return `*this`

      @param index The index to insert at.
      @param str The string to insert.

      @throw std::length_error `size() + str.size() > max_size()`
      @throw std::out_of_range `index > size()`
  */
  template<std::size_t M>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  insert(
    size_type index,
    const basic_static_string<M, CharT, Traits>& str)
  {
    return insert_unchecked(index, str.data(), str.size());
  }

#ifndef GENERATING_DOCUMENTATION
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  insert(
    size_type index,
    const basic_static_string& str)
  {
    return insert(index, str.data(), str.size());
  }
#endif

  /** Insert a string.

      Inserts a string, obtained by `str.substr(index_str, count)`
      at the position `index`.

      @par Exception Safety

      Strong guarantee.

      @note The insertion is done unchecked when
      the capacity of `str` differs from that of the
      string the function is called on.

      @note All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @tparam M The size of the input string.

      @return `*this`

      @param index The index to insert at.
      @param str The string from which to insert.
      @param index_str The index in `str` to start inserting from.
      @param count The number of characters to insert.
      The default argument for this parameter is @ref npos.

      @throw std::length_error `size() + str.substr(index_str, count).size() > max_size()`
      @throw std::out_of_range `index > size()`
      @throw std::out_of_range `index_str > str.size()`
  */
  template<std::size_t M>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  insert(
    size_type index,
    const basic_static_string<M, CharT, Traits>& str,
    size_type index_str,
    size_type count = npos)
  {
    BOOST_STATIC_STRING_THROW_IF(
      index_str > str.size(), std::out_of_range{"index_str > str.size()"});
    return insert_unchecked(index, str.data() + index_str, (std::min)(count, str.size() - index_str));
  }

#ifndef GENERATING_DOCUMENTATION
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  insert(
    size_type index,
    const basic_static_string& str,
    size_type index_str,
    size_type count = npos)
  {
    BOOST_STATIC_STRING_THROW_IF(
      index_str > str.size(), std::out_of_range{"index_str > str.size()"});
    return insert(index, str.data() + index_str, (std::min)(count, str.size() - index_str));
  }
#endif

  /** Insert a character.

      Inserts the character `ch` before the character pointed by `pos`.

      @par Precondition

      `pos` shall be vaild within `[data(), data() + size()]`

      @par Exception Safety

      Strong guarantee.

      @note All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @return An iterator which refers to the first inserted character
      or `pos` if no characters were inserted

      @param pos The index to insert at.
      @param ch The character to insert.

      @throw std::length_error `size() + 1 > max_size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  iterator
  insert(
    const_iterator pos,
    value_type ch)
  {
    return insert(pos, 1, ch);
  }

  /** Insert characters.

      Inserts `count` copies of `ch` before the character pointed by `pos`.

      @par Precondition

      `pos` shall be valid within `[data(), data() + size()]`

      @par Exception Safety

      Strong guarantee.

      @note All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @return An iterator which refers to the first inserted character
      or `pos` if no characters were inserted

      @param pos The position to insert at.
      @param count The number of characters to insert.
      @param ch The character to insert.

      @throw std::length_error `size() + count > max_size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  iterator
  insert(
    const_iterator pos,
    size_type count,
    value_type ch);

  /** Insert a range of characters.

      Inserts characters from the range `[first, last)` before the
      character pointed to by `pos`.

      @par Precondition

      `pos` shall be valid within `[data(), data() + size()]`,

      `[first, last)` shall be a valid range

      @par Exception Safety

      Strong guarantee.

      @note All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @tparam InputIterator The type of the iterators.

      @par Constraints

      `InputIterator` satisfies __InputIterator__ and does not 
      satisfy __ForwardIterator__.

      @return An iterator which refers to the first inserted character
      or `pos` if no characters were inserted

      @param pos The position to insert at.
      @param first An iterator representing the first character to insert.
      @param last An iterator representing one past the last character to insert.

      @throw std::length_error `size() + insert_count > max_size()`
  */
  template<typename InputIterator>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
  iterator
#else
  typename std::enable_if<
    detail::is_input_iterator<
      InputIterator>::value && 
        !detail::is_forward_iterator<
          InputIterator>::value, iterator>::type
#endif
  insert(
    const_iterator pos,
    InputIterator first,
    InputIterator last);

#ifndef GENERATING_DOCUMENTATION
  template<typename ForwardIterator>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  typename std::enable_if<
    detail::is_forward_iterator<
      ForwardIterator>::value, 
        iterator>::type
  insert(
    const_iterator pos,
    ForwardIterator first,
    ForwardIterator last);
#endif

  /** Insert characters from an initializer list.

      Inserts characters from `ilist` before `pos`.

      @par Precondition

      `pos` shall be valid within `[data(), data() + size()]`

      @par Exception Safety

      Strong guarantee.

      @note All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @return An iterator which refers to the first inserted character
      or `pos` if no characters were inserted

      @param pos The position to insert at.
      @param ilist The initializer list from which to insert.

      @throw std::length_error `size() + ilist.size() > max_size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  iterator
  insert(
    const_iterator pos,
    std::initializer_list<value_type> ilist)
  {
    return insert_unchecked(pos, ilist.begin(), ilist.size());
  }

  /** Insert characters from an object convertible to `string_view_type`.

      Constructs a temporary `string_view_type` object `sv` from `t` and
      inserts `[sv.begin(), sv.end())` at `index`.

      @par Precondition

      `index` shall be valid within `[data(), data() + size()]`

      @par Exception Safety

      Strong guarantee.

      @note All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @return `*this`

      @tparam T The type of the object to convert.

      @par Constraints

      `std::is_convertible<const T&, string_view>::value &&
      !std::is_convertible<const T&, const CharT*>::value`.

      @param index The index to insert at.
      @param t The string to insert from.

      @throw std::length_error `size() + sv.size() > max_size()`
      @throw std::out_of_range `index > size()`
  */
  template<typename T>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
  basic_static_string&
#else
  typename std::enable_if<
    std::is_convertible<
      const T&, string_view_type>::value &&
        !std::is_convertible<
          const T&, const_pointer>::value,
            basic_static_string&>::type
#endif
  insert(
    size_type index,
    const T& t)
  {
    return insert(index, t, 0, npos);
  }

  /** Insert characters from an object convertible to `string_view_type`.

      Constructs a temporary `string_view_type` object `sv` from `t`
      and inserts `sv.substr(index_str, count)` at `index`.

      @par Exception Safety

      Strong guarantee.

      @note All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @tparam T The type of the object to convert.

      @par Constraints

      `std::is_convertible<const T&, string_view>::value &&
      !std::is_convertible<const T&, const_pointer>::value`.

      @return `*this`

      @param index The index to insert at.
      @param t The string to insert from.
      @param index_str The index in the temporary `string_view_type` object
      to start the substring from.
      @param count The number of characters to insert.

      @throw std::length_error `size() + sv.size() > max_size()`
      @throw std::out_of_range `index > size()`
      @throw std::out_of_range `index_str > sv.size()`
  */
  template<typename T>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
  basic_static_string&
#else
  typename std::enable_if<
    std::is_convertible<const T&, string_view_type>::value &&
      !std::is_convertible<const T&, const_pointer>::value,
        basic_static_string&>::type
#endif
  insert(
    size_type index,
    const T& t,
    size_type index_str,
    size_type count = npos)
  {
    const auto s = string_view_type(t).substr(index_str, count);
    return insert(index, s.data(), s.size());
  }

  /** Removes `min(count, size() - index)` characters starting at `index`

      @throw std::out_of_range if `index > size()`
      @return `*this`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  erase(
    size_type index = 0,
    size_type count = npos);

  /** Removes the character at `pos`

      @return iterator pointing to the character immediately following the character erased, or `end()` if no such character exists
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  iterator
  erase(const_iterator pos)
  {
    BOOST_STATIC_STRING_ASSERT(!empty());
    return erase(pos, pos + 1);
  }

  /** Removes the characters in the range `(first, last)`

      @return iterator pointing to the character last pointed to before the erase, or `end()` if no such character exists
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  iterator
  erase(
    const_iterator first,
    const_iterator last);

  /** Appends the given character `ch` to the end of the string.

      @throw std::length_error if `1 > max_size() - size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  void
  push_back(value_type ch);

  /** Removes the last character from the string

      The behavior is undefined if the string is empty.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  void
  pop_back() noexcept
  {
    BOOST_STATIC_STRING_ASSERT(!empty());
    this->set_size(size() - 1);
    term();
  }

  /** Appends `count` copies of character `ch`

      The appended characters may be null.

      @throw std::length_error if `count > max_size() - size()`
      @return `*this`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  append(
    size_type count,
    value_type ch)
  {
    return insert(size(), count, ch);
  }

  /** Append to the string.
    
      Appends the contents of string view `sv`

      The appended string can contain null characters.

      @throw std::length_error if `sv.size() > max_size() - size()`
      @return `*this`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  append(
    string_view_type sv)
  {
    return append(sv.data(), sv.size());
  }

  /** Append to the string.
    
      Appends the contents of `sv.substr(pos, count)`

      The appended string can contain null characters.

      @throw std::out_of_range if `pos > sv.size()`
      @throw std::length_error if `sv.substr(pos, count).size() > max_size() - size()`
      @return `*this`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  append(
    string_view_type sv,
    size_type pos,
    size_type count = npos)
  {
    return append(sv.substr(pos, count));
  }

  /** Append to the string.
    
      Appends characters in the range `(s, s + count)`
    
      The appended string can contain null characters.

      @throw std::length_error if `count > max_size() - size()`
      @return `*this`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  append(
    const_pointer s,
    size_type count);

  /** Append to the string.
    
      Appends the null-terminated character string pointed to by `s`
    
      The length of the string is determined by the first
      null character using `traits_type::length(s)`.

      @throw std::length_error if `traits_type::length(s) > max_size() - size()`
      @return `*this`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  append(
    const_pointer s)
  {
    return append(s, traits_type::length(s));
  }

  /** Append to the string.
    
      Appends characters from the range `(first, last)`

      The inserted string can contain null characters.
      This function does not participate in overload resolution if
      `InputIterator` does not satisfy <em>LegacyInputIterator</em>

      @throw std::length_error if `std::distance(first, last) > max_size() - size()`
      @return `*this`
  */
  template<typename InputIterator>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
  basic_static_string&
#else
  typename std::enable_if<
    detail::is_input_iterator<InputIterator>::value,
      basic_static_string&>::type
#endif
  append(
    InputIterator first,
    InputIterator last)
  {
    insert(end(), first, last);
    return *this;
  }

  /** Append to the string.
    
      Appends characters from initializer list `ilist`

      The appended string can contain null characters.

      @throw std::length_error if `ilist.size() > max_size() - size()`
      @return `*this`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  append(
    std::initializer_list<value_type> ilist)
  {
    insert(end(), ilist);
    return *this;
  }

  /** Append to the string.
    
      Appends characters from `string_view_type{t}`

      The appended string can contain null characters.
      This function participates in overload resolution if
      `T` is convertible to `string_view_type` and `T` is not
      convertible to `const_pointer`.

      @throw std::length_error if `string_view_type{t}.size() > max_size() - size()`
      @return `*this`
  */
  template<typename T>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
  basic_static_string&
#else
  typename std::enable_if<
    std::is_convertible<
      const T&, string_view_type>::value &&
        !std::is_convertible<
          const T&, const_pointer>::value,
            basic_static_string&>::type
#endif
  append(
    const T& t)
  {
    return append(string_view_type{t});
  }

  /** Append to the string.
    
      Appends characters from `string_view_type{t}.substr{pos, count}`

      The appended string can contain null characters.
      This function participates in overload resolution if
      `T` is convertible to `string_view_type` and `T` is not
      convertible to `const_pointer`.

      @throw std::out_of_range if `pos > string_view_type{t}.size()`
      @throw std::length_error if `string_view_type{t}.substr(pos, count).size() > max_size() - size()`
      @return `*this`
  */
  template<typename T>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
  basic_static_string&
#else
  typename std::enable_if<
    std::is_convertible<
      const T&, string_view_type>::value &&
        !std::is_convertible<
          const T&, const_pointer>::value,
            basic_static_string&>::type
#endif
  append(
    const T& t,
    size_type pos,
    size_type count = npos)
  {
    return append(string_view_type{t}.substr(pos, count));
  }

  /** Append to the string.

      @throw std::length_error if `s.size() > max_size() - size()`
  */
  template<std::size_t M>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  operator+=(
    const basic_static_string<M, CharT, Traits>& s)
  {
    return append(s.data(), s.size());
  }

  /** Append to the string.
    
      Appends the given character `ch` to the end of the string.

      @throw std::length_error if `1 > max_size() - size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  operator+=(
    value_type ch)
  {
    push_back(ch);
    return *this;
  }

  /** Append to the string.
    
      Appends the null-terminated character string pointed to by `s`
    
      The length of the string is determined by the first
      null character using `traits_type::length(s)`.

      @throw std::length_error if `traits_type::length(s) > max_size() - size()`
      @return `*this`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  operator+=(
    const_pointer s)
  {
    return append(s);
  }

  /** Append to the string.
    
      Appends characters from initializer list `ilist`

      The appended string can contain null characters.

      @throw std::length_error if `ilist.size() > max_size() - size()`
      @return `*this`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  operator+=(
    std::initializer_list<value_type> ilist)
  {
    return append(ilist);
  }

  /** Append to the string.
    
      Appends a copy of the characters from `string_view_type{t}`

      The appended string can contain null characters.
      This function participates in overload resolution if
      `T` is convertible to `string_view_type` and `T` is not
      convertible to `const_pointer`.

      @throw std::length_error if `string_view_type{t}.size() > max_size()`
  */
  template<typename T>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
  basic_static_string&
#else
  typename std::enable_if<
    std::is_convertible<T, string_view_type>::value &&
      !std::is_convertible<T, const_pointer>::value,
        basic_static_string&>::type
#endif
  operator+=(
    const T& t)
  {
    return append(t);
  }

  /** Compare the string with another.
        
      Compares this string to `s`.
  */
  template<std::size_t M>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  int
  compare(
    const basic_static_string<M, CharT, Traits>& s) const noexcept
  {
    return detail::lexicographical_compare<CharT, Traits>(
      data(), size(), s.data(), s.size());
  }

  /** Compare the string with another.
        
      Compares a `[pos1, pos1+count1)` substring of this string to `s`. If `count1 > size() - pos1` the substring is `[pos1, size())`.
  */
  template<std::size_t M>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  int
  compare(
    size_type pos1,
    size_type count1,
    const basic_static_string<M, CharT, Traits>& s) const
  {
    return detail::lexicographical_compare<CharT, Traits>(
      subview(pos1, count1), s.data(), s.size());
  }

  /** Compare the string with another.
        
      Compares a `[pos1, pos1+count1)` substring of this string to a substring `[pos2, pos2+count2)` of `s`. 
      If `count1 > size() - pos1` the first substring is `[pos1, size())`. Likewise, if `count2 > s.size() - pos2` the
      second substring is `[pos2, s.size())`.
  */
  template<std::size_t M>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  int
  compare(
    size_type pos1,
    size_type count1,
    const basic_static_string<M, CharT, Traits>& s,
    size_type pos2,
    size_type count2 = npos) const
  {
    return detail::lexicographical_compare(
      subview(pos1, count1), s.subview(pos2, count2));
  }

  /** Compare the string with another.
        
      Compares this string to the null-terminated character sequence beginning at the character pointed to by `s` with length `traits_type::length(s)`.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  int
  compare(
    const_pointer s) const noexcept
  {
    return detail::lexicographical_compare<CharT, Traits>(
      data(), size(), s, traits_type::length(s));
  }

  /** Compare the string with another.
        
      Compares a `[pos1, pos1+count1)` substring of this string to the null-terminated character sequence beginning at the character pointed to by `s` with
      length `traits_type::length(s)`. If `count1 > size() - pos1` the substring is `[pos1, size())`.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  int
  compare(
    size_type pos1,
    size_type count1,
    const_pointer s) const
  {
    return detail::lexicographical_compare<CharT, Traits>(
      subview(pos1, count1), s, traits_type::length(s));
  }

  /** Compare the string with another.
        
      Compares a `[pos1, pos1+count1)` substring of this string to the characters in the range `[s, s + count2)`. If `count1 > size() - pos1` the substring is `[pos1, size())`.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  int
  compare(
    size_type pos1,
    size_type count1,
    const_pointer s,
    size_type count2) const
  {
    return detail::lexicographical_compare<CharT, Traits>(
      subview(pos1, count1), s, count2);
  }

  /** Compare the string with another.
      
      Compares this string to `s`.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  int
  compare(
    string_view_type s) const noexcept
  {
    return detail::lexicographical_compare<CharT, Traits>(
      data(), size(), s.data(), s.size());
  }

  /** Compare the string with another.

      Compares a `[pos1, pos1+count1)` substring of this string to `s`. If `count1 > size() - pos1` the substring is `[pos1, size())`.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  int
  compare(
    size_type pos1,
    size_type count1,
    string_view_type s) const
  {
    return detail::lexicographical_compare<CharT, Traits>(
      subview(pos1, count1), s);
  }

  /** Compare the string with another.
        
      Replaces the part of the string indicated by `[pos1, pos1 + count1)` with a substring `[pos2, pos2 + count2)` of `t` after converting to `string_view_type`.

      This function participates in overload resolution if
      `T` is convertible to `string_view_type` and `T` is not
      convertible to `const_pointer`.
  */
  template<typename T>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
  int
#else
  typename std::enable_if<
    std::is_convertible<const T&, string_view_type>::value &&
      !std::is_convertible<const T&, const_pointer>::value,
        int>::type
#endif
  compare(
    size_type pos1,
    size_type count1,
    const T& t,
    size_type pos2,
    size_type count2 = npos) const
  {
    return compare(pos1, count1,
      string_view_type(t).substr(pos2, count2));
  }

  /** Return a substring.

      Returns a substring of the string.

      @par Exception Safety

      Strong guarantee.

      @return A string object containing the characters
      `[data() + pos, std::min(count, size() - pos))`.

      @param pos The index to being the substring at. The
      default arugment for this parameter is `0`.
      @param count The length of the substring. The default arugment
      for this parameter is @ref npos.

      @throw std::out_of_range `pos > size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string
  substr(
    size_type pos = 0,
    size_type count = npos) const
  {
    BOOST_STATIC_STRING_THROW_IF(
      pos > size(), std::out_of_range{"pos > size()"});
    return basic_static_string(&data()[pos], (std::min)(count, size() - pos));
  }

  /** Return a view of a substring.

      Returns a view of a substring.

      @par Exception Safety

      Strong guarantee.

      @return A `string_view_type` object referring 
      to `[data() + pos, std::min(count, size() - pos))`.

      @param pos The index to being the substring at. The
      default arugment for this parameter is `0`.
      @param count The length of the substring. The default arugment
      for this parameter is @ref npos.

      @throw std::out_of_range `pos > size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  string_view_type
  subview(
    size_type pos = 0,
    size_type count = npos) const
  {
    BOOST_STATIC_STRING_THROW_IF(
      pos > size(), std::out_of_range{"pos > size()"});
    return string_view_type(&data()[pos], (std::min)(count, size() - pos));
  }

  /** Copy a substring to another string.

      Copies `std::min(count, size() - pos)` characters starting at
      index `pos` to the string pointed to by `dest`.

      @note The resulting string is not null terminated.

      @return The number of characters copied.

      @param count The number of characters to copy.
      @param dest The string to copy to.
      @param pos The index to begin copying from. The
      default argument for this parameter is `0`.

      @throw std::out_of_range `pos > max_size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  copy(
    CharT* dest,
    size_type count,
    size_type pos = 0) const;

  /** Change the size of the string.

      Resizes the string to contain `n` characters. If
      `n > size()`, characters with the value `CharT()` are
      appended. Otherwise, `size()` is reduced to `n`.

      @param n The size to resize the string to.

      @throw std::out_of_range `n > max_size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  void
  resize(size_type n);

  /** Change the size of the string.

        Resizes the string to contain `n` characters. If
        `n > size()`, copies of `c` are
        appended. Otherwise, `size()` is reduced to `n`.

        @param n The size to resize the string to.
        @param c The characters to append if the size
        increases.

        @throw std::out_of_range `n > max_size()`
    */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  void
  resize(
    size_type n,
    value_type c);

  /// Exchange the contents of this string with another.
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  void
  swap(basic_static_string& s) noexcept;

  /// Exchange the contents of this string with another.
  template<std::size_t M>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  void
  swap(basic_static_string<M, CharT, Traits>& s); 

  /** Replace a substring with a string.

      Replaces `rcount` characters starting at index `pos1` with those
      of `str`, where `rcount` is `std::min(n1, size() - pos1)`.

      @par Exception Safety

      Strong guarantee.

      @note The replacement is done unchecked when
      the capacity of `str` differs from that of the
      string the function is called on.
        
      All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @tparam M The size of the input string.

      @return `*this`

      @param pos1 The index to replace at.
      @param n1 The number of characters to replace.
      @param str The string to replace with.

      @throw std::length_error `size() + (str.size() - rcount) > max_size()`
      @throw std::out_of_range `pos1 > size()`
  */
  template<size_t M>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  replace(
    size_type pos1,
    size_type n1,
    const basic_static_string<M, CharT, Traits>& str)
  {
    return replace_unchecked(pos1, n1, str.data(), str.size());
  }

#ifndef GENERATING_DOCUMENTATION
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  replace(
    size_type pos1,
    size_type n1,
    const basic_static_string& str)
  {
    return replace(pos1, n1, str.data(), str.size());
  }
#endif

  /** Replace a substring with a substring.

      Replaces `rcount` characters starting at index `pos1` with those of
      `str.subview(pos2, n2)`, where `rcount` is `std::min(n1, size() - pos1)`.

      @par Exception Safety

      Strong guarantee.

      @note The replacement is done unchecked when
      the capacity of `str` differs from that of the
      string the function is called on.
        
      All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @return `*this`

      @param pos1 The index to replace at.
      @param n1 The number of characters to replace.
      @param str The string to replace with.
      @param pos2 The index to begin the substring.
      @param n2 The length of the substring.
      The default argument for this parameter is @ref npos.

      @throw std::length_error `size() + (std::min(str.size(), n2) - rcount) > max_size()`
      @throw std::out_of_range `pos1 > size()`
      @throw std::out_of_range `pos2 > str.size()`
  */
  template<std::size_t M>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  replace(
    size_type pos1,
    size_type n1,
    const basic_static_string<M, CharT, Traits>& str,
    size_type pos2,
    size_type n2 = npos)
  {
    // this check is not redundant, its for the substr operation performed on str
    BOOST_STATIC_STRING_THROW_IF(
      pos2 > str.size(), std::out_of_range{"pos2 > str.size()"});
    return replace_unchecked(pos1, n1, str.data() + pos2, (std::min)(n2, str.size() - pos2));
  }

#ifndef GENERATING_DOCUMENTATION
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  replace(
    size_type pos1,
    size_type n1,
    const basic_static_string& str,
    size_type pos2,
    size_type n2 = npos)
  {
    BOOST_STATIC_STRING_THROW_IF(
      pos2 > str.size(), std::out_of_range{"pos2 > str.size()"});
    return replace(pos1, n1, str.data() + pos2, (std::min)(n2, str.size() - pos2));
  }
#endif

  /** Replace a substring with an object convertible to `string_view_type`.

      Constructs a temporary `string_view_type` object `sv` from `t`, and
      replaces `rcount` characters starting at index `pos1` with those
      of `sv`, where `rcount` is `std::min(n1, size() - pos1)`.

      @par Exception Safety

      Strong guarantee.

      @note All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @tparam T The type of the object to convert.

      @par Constraints

      `std::is_convertible<const T&, string_view>::value &&
      !std::is_convertible<const T&, const CharT*>::value`.

      @return `*this`

      @param pos1 The index to replace at.
      @param n1 The number of characters to replace.
      @param t The object to replace with.

      @throw std::length_error `size() + (sv.size() - rcount) > max_size()`
      @throw std::out_of_range `pos1 > size()`
  */
  template<typename T>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
  basic_static_string&
#else
  typename std::enable_if<
    std::is_convertible<T, string_view_type>::value &&
      !std::is_convertible<T, const_pointer>::value,
        basic_static_string&>::type
#endif
  replace(
    size_type pos1,
    size_type n1,
    const T& t)
  {
    string_view_type sv = t;
    return replace(pos1, n1, sv.data(), sv.size());
  }

  /** Replace a substring with a substring of an object convertible to `string_view_type`.

      Constructs a temporary `string_view_type` object `sv` from `t`, and
      replaces `rcount` characters starting at index `pos1` with those
      of `sv.substr(pos2, n2)`, where `rcount` is `std::min(n1, size() - pos)`.

      @par Exception Safety

      Strong guarantee.

      @note All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @tparam T The type of the object to convert.

      @par Constraints

      `std::is_convertible<const T&, string_view>::value &&
      !std::is_convertible<const T&, const CharT*>::value`.

      @return `*this`

      @param pos1 The index to replace at.
      @param n1 The number of characters to replace.
      @param t The object to replace with.
      @param pos2 The index to begin the substring.
      @param n2 The length of the substring.
      The default argument for this parameter is @ref npos.

      @throw std::length_error `size() + (std::min(n2, sv.size()) - rcount) > max_size()`
      @throw std::out_of_range `pos1 > size()`
      @throw std::out_of_range `pos2 > sv.size()`
  */
  template<typename T>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
  basic_static_string&
#else
  typename std::enable_if<
    std::is_convertible<T, string_view_type>::value &&
      !std::is_convertible<T, const_pointer>::value,
        basic_static_string&>::type
#endif
  replace(
    size_type pos1,
    size_type n1,
    const T& t,
    size_type pos2,
    size_type n2 = npos)
  {
    string_view_type sv = t;
    return replace(pos1, n1, sv.substr(pos2, n2));
  }

  /** Replace a substring with a string.

      Replaces `rcount` characters starting at index `pos` with those of
      `[s, s + n2)`, where `rcount` is `std::min(n1, size() - pos)`.

      @par Exception Safety

      Strong guarantee.

      @note All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @return `*this`

      @param pos The index to replace at.
      @param n1 The number of characters to replace.
      @param s The string to replace with.
      @param n2 The length of the string to replace with.

      @throw std::length_error `size() + (n2 - rcount) > max_size()`
      @throw std::out_of_range `pos > size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  replace(
    size_type pos,
    size_type n1,
    const_pointer s,
    size_type n2)
  {
    BOOST_STATIC_STRING_THROW_IF(
      pos > size(), std::out_of_range{"pos > size()"});
    return replace(data() + pos, data() + pos + (std::min)(n1, size() - pos), s, n2);
  }

  /** Replace a substring with a string.

      Replaces `rcount` characters starting at index `pos` with those of
      `[s, s + len)`, where the length of the string `len` is `traits_type::length(s)` and `rcount`
      is `std::min(n1, size() - pos)`.

      @par Exception Safety

      Strong guarantee.

      @note All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @return `*this`

      @param pos The index to replace at.
      @param n1 The number of characters to replace.
      @param s The string to replace with.

      @throw std::length_error `size() + (len - rcount) > max_size()`
      @throw std::out_of_range `pos > size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  replace(
    size_type pos,
    size_type n1,
    const_pointer s)
  {
    return replace(pos, n1, s, traits_type::length(s));
  }

  /** Replace a substring with copies of a character.

      Replaces `rcount` characters starting at index `pos` with `n2` copies
      of `c`, where `rcount` is `std::min(n1, size() - pos)`.

      @par Exception Safety

      Strong guarantee.

      @note All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @return `*this`

      @param pos The index to replace at.
      @param n1 The number of characters to replace.
      @param n2 The number of characters to replace with.
      @param c The character to replace with.

      @throw std::length_error `size() + (n2 - rcount) > max_size()`
      @throw std::out_of_range `pos > size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  replace(
    size_type pos,
    size_type n1,
    size_type n2,
    value_type c)
  {
    BOOST_STATIC_STRING_THROW_IF(
      pos > size(), std::out_of_range{"pos > size()"});
    return replace(data() + pos, data() + pos + (std::min)(n1, size() - pos), n2, c);
  }

  /** Replace a range with a string.

      Replaces the characters in the range `[i1, i2)`
      with those of `str`.

      @par Precondition

      `[i1, i2)` is a valid range.

      @par Exception Safety

      Strong guarantee.

      @note The replacement is done unchecked when
      the capacity of `str` differs from that of the
      string the function is called on.
      
      All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @tparam M The size of the input string.

      @return `*this`

      @param i1 An iterator referring to the first character to replace.
      @param i2 An iterator referring past the end of
      the last character to replace.
      @param str The string to replace with.

      @throw std::length_error `size() + (str.size() - std::distance(i1, i2)) > max_size()`
  */
  template<std::size_t M>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  replace(
    const_iterator i1,
    const_iterator i2,
    const basic_static_string<M, CharT, Traits>& str)
  {
    return replace_unchecked(i1, i2, str.data(), str.size());
  }

#ifndef GENERATING_DOCUMENTATION
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  replace(
    const_iterator i1,
    const_iterator i2,
    const basic_static_string& str)
  {
    return replace(i1, i2, str.data(), str.size());
  }
#endif

  /** Replace a range with an object convertible to `string_view_type`.

      Constructs a temporary `string_view_type` object `sv` from `t`, and
      replaces the characters in the range `[i1, i2)` with those
      of `sv`.

      @par Precondition

      `[i1, i2)` is a valid range.

      @par Exception Safety

      Strong guarantee.

      @note All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @tparam T The type of the object to convert.

      @par Constraints

      `std::is_convertible<const T&, string_view>::value &&
      !std::is_convertible<const T&, const CharT*>::value`.

      @return `*this`

      @param i1 An iterator referring to the first character to replace.
      @param i2 An iterator referring past the end of
      the last character to replace.
      @param t The object to replace with.

      @throw std::length_error `size() + (sv.size() - std::distance(i1, i2)) > max_size()`
  */
  template<typename T>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
  basic_static_string&
#else
  typename std::enable_if<
    std::is_convertible<T, string_view_type>::value &&
      !std::is_convertible<T, const_pointer>::value,
        basic_static_string&>::type
#endif
  replace(
    const_iterator i1,
    const_iterator i2,
    const T& t)
  {
    string_view_type sv = t;
    return replace(i1, i2, sv.begin(), sv.end());
  }

  /** Replace a range with a string.

      Replaces the characters in the range `[i1, i2)` with those of
      `[s, s + n)`.

      @par Precondition

      `[i1, i2)` is a valid range.

      @par Exception Safety

      Strong guarantee.

      @note All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @return `*this`

      @param i1 An iterator referring to the first character to replace.
      @param i2 An iterator referring past the end of
      the last character to replace.
      @param s The string to replace with.
      @param n The length of the string to replace with.

      @throw std::length_error `size() + (n - std::distance(i1, i2)) > max_size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  replace(
    const_iterator i1,
    const_iterator i2,
    const_pointer s,
    size_type n)
  {
    return replace(i1, i2, s, s + n);
  }

  /** Replace a range with a string.

      Replaces the characters in the range `[i1, i2)` with those of
      `[s, s + len)`, where the length of the string `len` is `traits_type::length(s)`.

      @par Precondition

      `[i1, i2)` shall be a valid range.

      @par Exception Safety

      Strong guarantee.

      @note All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @return `*this`

      @param i1 An iterator referring to the first character to replace.
      @param i2 An iterator referring past the end of
      the last character to replace.
      @param s The string to replace with.

      @throw std::length_error `size() + (len - std::distance(i1, i2)) > max_size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  replace(
    const_iterator i1,
    const_iterator i2,
    const_pointer s)
  {
    return replace(i1, i2, s, traits_type::length(s));
  }

  /** Replace a range with copies of a character.

      Replaces the characters in the range `[i1, i2)` with
      `n` copies of `c`.

      @par Precondition

      `[i1, i2)` is a valid range.

      @par Exception Safety

      Strong guarantee.

      @note All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @return `*this`

      @param i1 An iterator referring to the first character to replace.
      @param i2 An iterator past the end of
      the last character to replace.
      @param n The number of characters to replace with.
      @param c The character to replace with.

      @throw std::length_error `size() + (n - std::distance(i1, i2)) > max_size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  replace(
    const_iterator i1,
    const_iterator i2,
    size_type n,
    value_type c);

  /** Replace a range with a range.

      Replaces the characters in the range `[i1, i2)`
      with those of `[j1, j2)`.

      @par Precondition

      `[i1, i2)` is a valid range.

      `[j1, j2)` is a valid range.

      @par Exception Safety

      Strong guarantee.

      @note All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @tparam InputIterator The type of the iterators.

      @par Constraints

      `InputIterator` satisfies __InputIterator__ and does not 
      satisfy __ForwardIterator__.

      @return `*this`

      @param i1 An iterator referring to the first character to replace.
      @param i2 An iterator referring past the end of
      the last character to replace.
      @param j1 An iterator referring to the first character to replace with.
      @param j2 An iterator referring past the end of
      the last character to replace with.

      @throw std::length_error `size() + (inserted - std::distance(i1, i2)) > max_size()`
  */
  template<typename InputIterator>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
  basic_static_string&
#else
  typename std::enable_if<
    detail::is_input_iterator<
      InputIterator>::value && 
        !detail::is_forward_iterator<
          InputIterator>::value, 
            basic_static_string<N, CharT, Traits>&>::type
#endif
  replace(
    const_iterator i1,
    const_iterator i2,
    InputIterator j1,
    InputIterator j2);

#ifndef GENERATING_DOCUMENTATION
  template<typename ForwardIterator>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  typename std::enable_if<
    detail::is_forward_iterator<
      ForwardIterator>::value,
        basic_static_string<N, CharT, Traits>&>::type
  replace(
    const_iterator i1,
    const_iterator i2,
    ForwardIterator j1,
    ForwardIterator j2);
#endif

  /** Replace a range with an initializer list.

      Replaces the characters in the range `[i1, i2)`
      with those of contained in the initializer list `il`.

      @par Precondition

      `[i1, i2)` is a valid range.

      @par Exception Safety

      Strong guarantee.

      @note All references, pointers, or iterators
      referring to contained elements are invalidated. Any
      past-the-end iterators are also invalidated.

      @return `*this`

      @param i1 An iterator referring to the first character to replace.
      @param i2 An iterator past the end of
      the last character to replace.
      @param il The initializer list to replace with.

      @throw std::length_error `size() + (il.size() - std::distance(i1, i2)) > max_size()`
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  replace(
    const_iterator i1,
    const_iterator i2,
    std::initializer_list<value_type> il)
  {
    return replace_unchecked(i1, i2, il.begin(), il.size());
  }

  //--------------------------------------------------------------------------
  //
  // Search
  //
  //--------------------------------------------------------------------------

  /** Find the first occurrence of a string within the string.

      Constructs a temporary `string_view_type` object `sv` from `t`, and finds
      the first occurrence of `sv` within the string starting at the index `pos`.

      @par Complexity

      Linear.

      @note An empty string is always found.

      @tparam T The type of the object to convert.

      @par Constraints

      `std::is_convertible<const T&, string_view>::value &&
      !std::is_convertible<const T&, const CharT*>::value`.

      @return The lowest index `idx` greater than or equal to `pos`
      where each element of `[sv.begin(), sv.end())` is equal to
      that of `[begin() + idx, begin() + idx + count)` if one exists,
      and @ref npos otherwise.

      @param t The string to search for.
      @param pos The index to start searching at. The default argument
      for this parameter is `0`.
  */
  template<class T>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
  size_type
#else
  typename std::enable_if<
    std::is_convertible<T, string_view_type>::value &&
      !std::is_convertible<T, const_pointer>::value,
        size_type>::type
#endif
  find(
    const T& t,
    size_type pos = 0) const
      noexcept(detail::is_nothrow_convertible<const T&, string_view_type>::value)
  {
    string_view_type sv = t;
    return find(sv.data(), pos, sv.size());
  }
    
    /** Find the first occurrence of a string within the string.
        
      Finds the first occurrence of `str` within the
      string starting at the index `pos`.

      @par Complexity

      Linear.

      @return The lowest index `idx` greater than or equal to `pos` 
      where each element of `str` is equal to that of 
      `[begin() + idx, begin() + idx + str.size())` 
      if one exists, and @ref npos otherwise.

      @param str The string to search for.
      @param pos The index to start searching at. The default argument for
      this parameter is `0`.
  */
  template<std::size_t M>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  find(
    const basic_static_string<M, CharT, Traits>& str,
    size_type pos = 0) const noexcept
  {
    return find(str.data(), pos, str.size());
  }

  /** Find the first occurrence of a string within the string.

      Finds the first occurrence of the string pointed to
      by `s` within the string starting at the index `pos`.

      @par Complexity

      Linear.

      @note An empty string is always found.

      @return The lowest index `idx` greater than or equal to `pos`
      where each element of `[s, s + n)` is equal to that of
      `[begin() + idx, begin() + idx + n)` if one exists,
      and @ref npos otherwise.

      @param s The string to search for.
      @param pos The index to start searching at.
      @param n The length of the string to search for.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  find(
    const_pointer s, 
    size_type pos,
    size_type n) const noexcept;

  /** Find the first occurrence of a string within the string.

      Finds the first occurrence of the string pointed to by `s`
      of length `count` within the string starting at the index `pos`,
      where `count` is `traits_type::length(s)`.

      @par Complexity

      Linear.

      @note An empty string is always found.

      @return The lowest index `idx` greater than or equal to `pos`
      where each element of `[s, s + count)` is equal to that of
      `[begin() + idx, begin() + idx + count)` if one exists,
      and @ref npos otherwise.

      @param s The string to search for.
      @param pos The index to start searching at. The default argument
      for this parameter is `0`.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  find(
    const_pointer s,
    size_type pos = 0) const noexcept
  {
    return find(s, pos, traits_type::length(s));
  }

  /** Find the first occurrence of a character within the string.

      Finds the first occurrence of `c` within the string
      starting at the index `pos`.

      @par Complexity

      Linear.

      @return The index corrosponding to the first occurrence of `c` within
      `[begin() + pos, end())` if it exists, and @ref npos otherwise.

      @param c The character to search for.
      @param pos The index to start searching at. The default argument
      for this parameter is `0`.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  find(
    value_type c,
    size_type pos = 0) const noexcept
  {
    return find(&c, pos, 1);
  }


  /** Find the last occurrence of a string within the string.

      Constructs a temporary `string_view_type` object `sv` from `t`, and finds
      the last occurrence of `sv` within the string starting before or at
      the index `pos`.

      @par Complexity

      Linear.

      @tparam T The type of the object to convert.

      @par Constraints

      `std::is_convertible<const T&, string_view>::value &&
      !std::is_convertible<const T&, const CharT*>::value`.

      @return The highest index `idx` less than or equal to `pos`
      where each element of `[sv.begin(), sv.end())` is equal to
      that of `[begin() + idx, begin() + idx + count)` if one exists,
      and @ref npos otherwise.

      @param t The string to search for.
      @param pos The index to start searching at. The default argument
      for this parameter is @ref npos.
  */
  template<class T>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
  size_type
#else
  typename std::enable_if<
    std::is_convertible<T, string_view_type>::value &&
      !std::is_convertible<T, const_pointer>::value,
        size_type>::type
#endif
  rfind(
    const T& t,
    size_type pos = npos) const 
      noexcept(detail::is_nothrow_convertible<const T&, string_view_type>::value)
  {
    string_view_type sv = t;
    return rfind(sv.data(), pos, sv.size());
  }

  /** Find the last occurrence of a string within the string.

      Finds the last occurrence of `str` within the string
      starting before or at the index `pos`.

      @par Complexity

      Linear.

      @return The highest index `idx` less than or equal to `pos`
      where each element of `str` is equal to that
      of `[begin() + idx, begin() + idx + str.size())`
      if one exists, and @ref npos otherwise.

      @param str The string to search for.
      @param pos The index to start searching at. The default argument for
      this parameter is @ref npos.
  */
  template<std::size_t M>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  rfind(
    const basic_static_string<M, CharT, Traits>& str,
    size_type pos = npos) const noexcept
  {
    return rfind(str.data(), pos, str.size());
  }

  /** Find the last occurrence of a string within the string.

      Finds the last occurrence of the string pointed to
      by `s` within the string starting before or at
      the index `pos`.

      @par Complexity

      Linear.

      @return The highest index `idx` less than or equal to `pos`
      where each element of `[s, s + n)` is equal to that of
      `[begin() + idx, begin() + idx + n)` if one exists,
      and @ref npos otherwise.

      @param s The string to search for.
      @param pos The index to start searching at.
      @param n The length of the string to search for.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  rfind(
    const_pointer s, 
    size_type pos,
    size_type n) const noexcept;

  /** Find the last occurrence of a string within the string.

      Finds the last occurrence of the string pointed to by `s`
      of length `count` within the string starting before or at the
      index `pos`, where `count` is `traits_type::length(s)`.

      @par Complexity

      Linear.

      @return The highest index `idx` less than or equal to `pos`
      where each element of `[s, s + count)` is equal to that of
      `[begin() + idx, begin() + idx + count)` if one exists,
      and @ref npos otherwise.

      @param s The string to search for.
      @param pos The index to stop searching at. The default argument
      for this parameter is @ref npos.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  rfind(
    const_pointer s,
    size_type pos = npos) const noexcept
  {
    return rfind(s, pos, traits_type::length(s));
  }

  /** Find the last occurrence of a character within the string.

      Finds the last occurrence of `c` within the string
      starting before or at the index `pos`.

      @par Complexity

      Linear.

      @return The index corrosponding to the last occurrence of `c` within
      `[begin(), begin() + pos]` if it exists, and @ref npos otherwise.

      @param c The character to search for.
      @param pos The index to stop searching at. The default argument
      for this parameter is @ref npos.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  rfind(
    value_type c,
    size_type pos = npos) const noexcept
  {
    return rfind(&c, pos, 1);
  }

  /** Find the first occurrence of any of the characters within the string.

      Constructs a temporary `string_view_type` object `sv` from `t`, and finds
      the first occurrence of any of the characters in `sv`
      within the string starting at the index `pos`.

      @par Complexity

      Linear.

      @tparam T The type of the object to convert.

      @par Constraints

      `std::is_convertible<const T&, string_view>::value &&
      !std::is_convertible<const T&, const CharT*>::value`.

      @return The index corrosponding to the first occurrence of
      any of the characters in `[sv.begin(), sv.end())` within
      `[begin() + pos, end())` if it exists, and @ref npos otherwise.

      @param t The characters to search for.
      @param pos The index to start searching at. The default argument
      for this parameter is `0`.
  */
  template<class T>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
  size_type
#else
  typename std::enable_if<
    std::is_convertible<T, string_view_type>::value &&
      !std::is_convertible<T, const_pointer>::value,
        size_type>::type
#endif
  find_first_of(
    const T& t,
    size_type pos = 0) const
      noexcept(detail::is_nothrow_convertible<const T&, string_view_type>::value)
  {
    string_view_type sv = t;
    return find_first_of(sv.data(), pos, sv.size());
  }

  /** Find the first occurrence of any of the characters within the string.

      Finds the first occurrence of any of the characters within `str` within the
      string starting at the index `pos`.

      @par Complexity

      Linear.

      @return The index corrosponding to the first occurrence of any of the characters
      of `str` within `[begin() + pos, end())` if it exists, and @ref npos otherwise.

      @param str The characters to search for.
      @param pos The index to start searching at. The default argument for
      this parameter is `0`.
  */
  template<std::size_t M>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  find_first_of(
    const basic_static_string<M, CharT, Traits>& str,
    size_type pos = 0) const noexcept
  {
    return find_first_of(str.data(), pos, str.size());
  }

  /** Find the first occurrence of any of the characters within the string.

      Finds the first occurrence of any of the characters within the string pointed to
      by `s` within the string starting at the index `pos`.

      @par Complexity

      Linear.

      @return The index corrosponding to the first occurrence
      of any of the characters in `[s, s + n)` within `[begin() + pos, end())`
      if it exists, and @ref npos otherwise.

      @param s The characters to search for.
      @param pos The index to start searching at.
      @param n The length of the string to search for.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  find_first_of(
    const_pointer s,
    size_type pos,
    size_type n) const noexcept;

  /** Find the first occurrence of any of the characters within the string.

      Finds the first occurrence of the any of the characters within string
      pointed to by `s` of length `count` within the string starting at the
      index `pos`, where `count` is `traits_type::length(s)`.

      @par Complexity

      Linear.

      @return The index corrosponding to the first occurrence of any of
      the characters in `[s, s + count)` within
      `[begin() + pos, end())` if it exists, and @ref npos otherwise.

      @param s The characters to search for.
      @param pos The index to start searching at. The default argument
      for this parameter is `0`.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  find_first_of(
    const_pointer s,
    size_type pos = 0) const noexcept
  {
    return find_first_of(s, pos, traits_type::length(s));
  }

  /** Find the first occurrence of a character within the string.

      Finds the first occurrence of `c` within the string
      starting at the index `pos`.

      @par Complexity

      Linear.

      @return The index corrosponding to the first occurrence of `c` within
      `[begin() + pos, end())` if it exists, and @ref npos otherwise.

      @param c The character to search for.
      @param pos The index to start searching at. The default argument
      for this parameter is `0`.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  find_first_of(
    value_type c,
    size_type pos = 0) const noexcept
  {
    return find_first_of(&c, pos, 1);
  }

  /** Find the last occurrence of any of the characters within the string.

      Constructs a temporary `string_view_type` object `sv` from `t`, and finds
      the last occurrence of any of the characters in `sv`
      within the string before or at the index `pos`.

      @par Complexity

      Linear.

      @tparam T The type of the object to convert.

      @par Constraints

      `std::is_convertible<const T&, string_view>::value &&
      !std::is_convertible<const T&, const CharT*>::value`.

      @return The index corrosponding to the last occurrence of
      any of the characters in `[sv.begin(), sv.end())` within
      `[begin(), begin() + pos]` if it exists, and @ref npos otherwise.

      @param t The characters to search for.
      @param pos The index to stop searching at. The default argument
      for this parameter is @ref npos.
  */
  template<class T>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
  size_type
#else
  typename std::enable_if<
    std::is_convertible<T, string_view_type>::value &&
      !std::is_convertible<T, const_pointer>::value,
        size_type>::type
#endif
  find_last_of(
    const T& t,
    size_type pos = npos) const
      noexcept(detail::is_nothrow_convertible<const T&, string_view_type>::value)
  {
    string_view_type sv = t;
    return find_last_of(sv.data(), pos, sv.size());
  }
     
  /** Find the last occurrence of any of the characters within the string.

      Finds the last occurrence of any of the characters within `str` within the
      string starting before or at the index `pos`.

      @par Complexity

      Linear.

      @return The index corrosponding to the last occurrence of any of the characters
      of `str` within `[begin(), begin() + pos]` if it exists, and @ref npos otherwise.

      @param str The characters to search for.
      @param pos The index to stop searching at. The default argument for
      this parameter is @ref npos.
  */
  template<std::size_t M>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  find_last_of(
    const basic_static_string<M, CharT, Traits>& str,
    size_type pos = npos) const noexcept
  {
    return find_last_of(str.data(), pos, str.size());
  }

  /** Find the last occurrence of any of the characters within the string.

      Finds the last occurrence of any of the characters within the string pointed to
      by `s` within the string before or at the index `pos`.

      @par Complexity

      Linear.

      @return The index corrosponding to the last occurrence
      of any of the characters in `[s, s + n)` within `[begin(), begin() + pos]`
      if it exists, and @ref npos otherwise.

      @param s The characters to search for.
      @param pos The index to stop searching at.
      @param n The length of the string to search for.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  find_last_of(
    const_pointer s,
    size_type pos,
    size_type n) const noexcept;

  /** Find the last occurrence of any of the characters within the string.

      Finds the last occurrence of any of the characters within the string pointed to
      by `s` of length `count` within the string before or at the index `pos`,
      where `count` is `traits_type::length(s)`.

      @par Complexity

      Linear.

      @return The index corrosponding to the last occurrence
      of any of the characters in `[s, s + count)` within `[begin(), begin() + pos]`
      if it exists, and @ref npos otherwise.

      @param s The characters to search for.
      @param pos The index to stop searching at. The default argument for
      this parameter is @ref npos.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  find_last_of(
    const_pointer s,
    size_type pos = npos) const noexcept
  {
    return find_last_of(s, pos, traits_type::length(s));
  }

  /** Find the last occurrence of a character within the string.

      Finds the last occurrence of `c` within the string
      before or at the index `pos`.

      @par Complexity

      Linear.

      @return The index corrosponding to the last occurrence of `c` within
      `[begin(), begin() + pos]` if it exists, and @ref npos otherwise.

      @param c The character to search for.
      @param pos The index to stop searching at. The default argument
      for this parameter is @ref npos.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  find_last_of(
    value_type c,
    size_type pos = npos) const noexcept
  {
    return find_last_of(&c, pos, 1);
  }

  /** Find the first occurrence of a character not within the string.

      Constructs a temporary `string_view_type` object `sv` from `t`, and finds
      the first character that is not within `sv`, starting at the index `pos`.

      @par Complexity

      Linear.

      @tparam T The type of the object to convert.

      @par Constraints

      `std::is_convertible<const T&, string_view>::value &&
      !std::is_convertible<const T&, const CharT*>::value`.

      @return The index corrosponding to the first occurrence of
      a character that is not in `[sv.begin(), sv.end())` within
      `[begin() + pos, end())` if it exists, and @ref npos otherwise.

      @param t The characters to ignore.
      @param pos The index to start searching at. The default argument
      for this parameter is `0`.
  */
  template<class T>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
  size_type
#else
  typename std::enable_if<
    std::is_convertible<T, string_view_type>::value &&
      !std::is_convertible<T, const_pointer>::value,
        size_type>::type
#endif
  find_first_not_of(
    const T& t,
    size_type pos = 0) const
      noexcept(detail::is_nothrow_convertible<const T&, string_view_type>::value)
  {
    string_view_type sv = t;
    return find_first_not_of(sv.data(), pos, sv.size());
  }

  /** Find the first occurrence of any of the characters not within the string.

      Finds the first occurrence of a character that is not within `str`
      within the string starting at the index `pos`.

      @par Complexity

      Linear.

      @return The index corrosponding to the first character of `[begin() + pos, end())`
      that is not within `str` if it exists, and @ref npos otherwise.

      @param str The characters to ignore.
      @param pos The index to start searching at. The default argument for
      this parameter is `0`.
  */
  template<std::size_t M>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  find_first_not_of(
    const basic_static_string<M, CharT, Traits>& str,
    size_type pos = 0) const noexcept
  {
    return find_first_not_of(str.data(), pos, str.size());
  }

  /** Find the first occurrence of any of the characters not within the string.

      Finds the first occurrence of a character that is not within the string
      pointed to by `s` within the string starting at the index `pos`.

      @par Complexity

      Linear.

      @return The index corrosponding to the first character of `[begin() + pos, end())`
      that is not within `[s, s + n)` if it exists, and @ref npos otherwise.

      @param s The characters to ignore.
      @param pos The index to start searching at. The default argument for
      this parameter is `0`.
      @param n The length of the characters to ignore.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  find_first_not_of(
    const_pointer s,
    size_type pos, 
    size_type n) const noexcept;

  /** Find the first occurrence of any of the characters not within the string.

      Finds the first occurrence of a character that is not within the string
      pointed to by `s` of length `count` within the string starting
      at the index `pos`, where `count` is `traits_type::length(s)`.

      @par Complexity

      Linear.

      @return The index corrosponding to the first character of `[begin() + pos, end())`
      that is not within `[s, s + count)` if it exists, and @ref npos otherwise.

      @param s The characters to ignore.
      @param pos The index to start searching at. The default argument for
      this parameter is `0`.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  find_first_not_of(
    const_pointer s,
    size_type pos = 0) const noexcept
  {
    return find_first_not_of(s, pos, traits_type::length(s));
  }

  /** Find the first occurrence of a character not equal to `c`.

      Finds the first occurrence of a character that is not equal
      to `c`.

      @par Complexity

      Linear.

      @return The index corrosponding to the first character of `[begin() + pos, end())`
      that is not equal to `c` if it exists, and @ref npos otherwise.

      @param c The character to ignore.
      @param pos The index to start searching at. The default argument for
      this parameter is `0`.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  find_first_not_of(
    value_type c,
    size_type pos = 0) const noexcept
  {
    return find_first_not_of(&c, pos, 1);
  }

  /** Find the last occurrence of a character not within the string.

      Constructs a temporary `string_view_type` object `sv` from `t`, and finds
      the last character that is not within `sv`, starting at the index `pos`.

      @par Complexity

      Linear.

      @tparam T The type of the object to convert.

      @par Constraints

      `std::is_convertible<const T&, string_view>::value &&
      !std::is_convertible<const T&, const CharT*>::value`.

      @return The index corrosponding to the last occurrence of
      a character that is not in `[sv.begin(), sv.end())` within
      `[begin(), begin() + pos]` if it exists, and @ref npos otherwise.

      @param t The characters to ignore.
      @param pos The index to start searching at. The default argument
      for this parameter is @ref npos.
  */
  template<class T>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
  size_type
#else
  typename std::enable_if<
    std::is_convertible<T, string_view_type>::value &&
      !std::is_convertible<T, const_pointer>::value,
        size_type>::type
#endif
  find_last_not_of(
    const T& t,
    size_type pos = npos) const
      noexcept(detail::is_nothrow_convertible<const T&, string_view_type>::value)
  {
    string_view_type sv = t;
    return find_last_not_of(sv.data(), pos, sv.size());
  }

  /** Find the last occurrence of a character not within the string.

      Finds the last occurrence of a character that is not within `str`
      within the string before or at the index `pos`.

      @par Complexity

      Linear.

      @return The index corrosponding to the last character of `[begin(), begin() + pos]`
      that is not within `str` if it exists, and @ref npos otherwise.

      @param str The characters to ignore.
      @param pos The index to stop searching at. The default argument for
      this parameter is @ref npos.
  */
  template<size_t M>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  find_last_not_of(
    const basic_static_string<M, CharT, Traits>& str,
    size_type pos = npos) const noexcept
  {
    return find_last_not_of(str.data(), pos, str.size());
  }

  /** Find the last occurrence of a character not within the string.

      Finds the last occurrence of a character that is not within the
      string pointed to by `s` within the string before or at the index `pos`.

      @par Complexity

      Linear.

      @return The index corrosponding to the last character of `[begin(), begin() + pos]`
      that is not within `[s, s + n)` if it exists, and @ref npos otherwise.

      @param s The characters to ignore.
      @param pos The index to stop searching at. The default argument for
      this parameter is @ref npos.
      @param n The length of the characters to ignore.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  find_last_not_of(
    const_pointer s,
    size_type pos,
    size_type n) const noexcept;


  /** Find the last occurrence of a character not within the string.

      Finds the last occurrence of a character that is not within the
      string pointed to by `s` of length `count` within the string
      before or at the index `pos`, where `count` is `traits_type::length(s)`.

      @par Complexity

      Linear.

      @return The index corrosponding to the last character of `[begin(), begin() + pos]`
      that is not within `[s, s + count)` if it exists, and @ref npos otherwise.

      @param s The characters to ignore.
      @param pos The index to stop searching at. The default argument for
      this parameter is @ref npos.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  find_last_not_of(
    const_pointer s,
    size_type pos = npos) const noexcept
  {
    return find_last_not_of(s, pos, traits_type::length(s));
  }

  /** Find the last occurrence of a character not equal to `c`.

      Finds the last occurrence of a character that is not equal
      to `c` before or at the index `pos`.

      @par Complexity

      Linear.

      @return The index corrosponding to the last character of `[begin(), begin() + pos]`
      that is not equal to `c` if it exists, and @ref npos otherwise.

      @param c The character to ignore.
      @param pos The index to start searching at. The default argument for
      this parameter is @ref npos.
  */
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  find_last_not_of(
    value_type c,
    size_type pos = npos) const noexcept
  {
    return find_last_not_of(&c, pos, 1);
  }

  /// Returns whether the string begins with `s` 
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  bool 
  starts_with(
    string_view_type s) const noexcept
  {
    const size_type len = s.size();
    return size() >= len && !traits_type::compare(data(), s.data(), len);
  }
    
  /// Returns whether the string begins with `c`
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  bool 
  starts_with(
    value_type c) const noexcept
  {
    return !empty() && traits_type::eq(front(), c);
  }
    
  /// Returns whether the string begins with `s`
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  bool 
  starts_with(
    const_pointer s) const noexcept
  {
    const size_type len = traits_type::length(s);
    return size() >= len && !traits_type::compare(data(), s, len);
  }
    
  /// Returns whether the string ends with `s`
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  bool 
  ends_with(
    string_view_type s) const noexcept
  {
    const size_type len = s.size();
    return size() >= len && !traits_type::compare(data() + (size() - len), s.data(), len);
  }
    
  /// Returns whether the string ends with `c`
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  bool
  ends_with(
    value_type c) const noexcept
  {
    return !empty() && traits_type::eq(back(), c);
  }
    
  /// Returns whether the string begins with `s`
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  bool 
  ends_with(
    const_pointer s) const noexcept
  {
    const size_type len = traits_type::length(s);
    return size() >= len && !traits_type::compare(data() + (size() - len), s, len);
  }

private:
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  term() noexcept
  {
    this->term_impl();
    return *this;
  }

  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  assign_char(value_type ch, std::true_type) noexcept
  {
    this->set_size(1);
    traits_type::assign(data()[0], ch);
    return term();
  }

  basic_static_string&
  assign_char(value_type ch, std::false_type)
  {
    BOOST_STATIC_STRING_THROW(
      std::length_error{"max_size() == 0"});
    return *this;
  }

  // Returns the size of data read from input iterator. Read data begins at data() + size() + 1.
  template<typename InputIterator>
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  size_type
  read_back(
    InputIterator first, 
    InputIterator last);

  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  replace_unchecked(
    size_type pos,
    size_type n1,
    const_pointer s,
    size_type n2)
  {
    BOOST_STATIC_STRING_THROW_IF(
      pos > size(), std::out_of_range{"pos > size()"});
    return replace_unchecked(data() + pos, data() + pos + n1, s, n2);
  }

  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  replace_unchecked(
    const_iterator i1,
    const_iterator i2,
    const_pointer s,
    size_type n2);

  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  basic_static_string&
  insert_unchecked(
    size_type index,
    const_pointer s,
    size_type count)
  {
    BOOST_STATIC_STRING_THROW_IF(
      index > size(), std::out_of_range{"index > size()"});
    insert_unchecked(data() + index, s, count);
    return *this;
  }
  
  BOOST_STATIC_STRING_CPP14_CONSTEXPR
  iterator
  insert_unchecked(
    const_iterator pos,
    const_pointer s,
    size_type count);
};

//------------------------------------------------------------------------------
//
// Non-member functions
//
//------------------------------------------------------------------------------

template<
  std::size_t N, std::size_t M,
  typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
bool
operator==(
  const basic_static_string<N, CharT, Traits>& lhs,
  const basic_static_string<M, CharT, Traits>& rhs)
{
  return lhs.compare(rhs) == 0;
}

template<
  std::size_t N, std::size_t M,
  typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
bool
operator!=(
  const basic_static_string<N, CharT, Traits>& lhs,
  const basic_static_string<M, CharT, Traits>& rhs)
{
  return lhs.compare(rhs) != 0;
}

template<
  std::size_t N, std::size_t M,
  typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
bool
operator<(
  const basic_static_string<N, CharT, Traits>& lhs,
  const basic_static_string<M, CharT, Traits>& rhs)
{
  return lhs.compare(rhs) < 0;
}

template<
  std::size_t N, std::size_t M,
  typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
bool
operator<=(
  const basic_static_string<N, CharT, Traits>& lhs,
  const basic_static_string<M, CharT, Traits>& rhs)
{
  return lhs.compare(rhs) <= 0;
}

template<
  std::size_t N, std::size_t M,
  typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
bool
operator>(
  const basic_static_string<N, CharT, Traits>& lhs,
  const basic_static_string<M, CharT, Traits>& rhs)
{
  return lhs.compare(rhs) > 0;
}

template<
  std::size_t N, std::size_t M,
  typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
bool
operator>=(
  const basic_static_string<N, CharT, Traits>& lhs,
  const basic_static_string<M, CharT, Traits>& rhs)
{
  return lhs.compare(rhs) >= 0;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
bool
operator==(
  const CharT* lhs,
  const basic_static_string<N, CharT, Traits>& rhs)
{
  return detail::lexicographical_compare<CharT, Traits>(
    lhs, Traits::length(lhs),
    rhs.data(), rhs.size()) == 0;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
bool
operator==(
  const basic_static_string<N, CharT, Traits>& lhs,
  const CharT* rhs)
{
  return detail::lexicographical_compare<CharT, Traits>(
    lhs.data(), lhs.size(),
    rhs, Traits::length(rhs)) == 0;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
bool
operator!=(
  const CharT* lhs,
  const basic_static_string<N, CharT, Traits>& rhs)
{
  return detail::lexicographical_compare<CharT, Traits>(
    lhs, Traits::length(lhs),
    rhs.data(), rhs.size()) != 0;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
bool
operator!=(
  const basic_static_string<N, CharT, Traits>& lhs,
  const CharT* rhs)
{
  return detail::lexicographical_compare<CharT, Traits>(
    lhs.data(), lhs.size(),
    rhs, Traits::length(rhs)) != 0;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
bool
operator<(
  const CharT* lhs,
  const basic_static_string<N, CharT, Traits>& rhs)
{
  return detail::lexicographical_compare<CharT, Traits>(
    lhs, Traits::length(lhs),
    rhs.data(), rhs.size()) < 0;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
bool
operator<(
  const basic_static_string<N, CharT, Traits>& lhs,
  const CharT* rhs)
{
  return detail::lexicographical_compare<CharT, Traits>(
    lhs.data(), lhs.size(),
    rhs, Traits::length(rhs)) < 0;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
bool
operator<=(
  const CharT* lhs,
  const basic_static_string<N, CharT, Traits>& rhs)
{
  return detail::lexicographical_compare<CharT, Traits>(
    lhs, Traits::length(lhs),
    rhs.data(), rhs.size()) <= 0;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
bool
operator<=(
  const basic_static_string<N, CharT, Traits>& lhs,
  const CharT* rhs)
{
  return detail::lexicographical_compare<CharT, Traits>(
    lhs.data(), lhs.size(),
    rhs, Traits::length(rhs)) <= 0;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
bool
operator>(
  const CharT* lhs,
  const basic_static_string<N, CharT, Traits>& rhs)
{
  return detail::lexicographical_compare<CharT, Traits>(
    lhs, Traits::length(lhs),
    rhs.data(), rhs.size()) > 0;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
bool
operator>(
  const basic_static_string<N, CharT, Traits>& lhs,
  const CharT* rhs)
{
  return detail::lexicographical_compare<CharT, Traits>(
    lhs.data(), lhs.size(),
    rhs, Traits::length(rhs)) > 0;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
bool
operator>=(
  const CharT* lhs,
  const basic_static_string<N, CharT, Traits>& rhs)
{
  return detail::lexicographical_compare<CharT, Traits>(
    lhs, Traits::length(lhs),
    rhs.data(), rhs.size()) >= 0;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
bool
operator>=(
  const basic_static_string<N, CharT, Traits>& lhs,
  const CharT* rhs)
{
  return detail::lexicographical_compare<CharT, Traits>(
    lhs.data(), lhs.size(),
    rhs, Traits::length(rhs)) >= 0;
}

template<
  std::size_t N, std::size_t M,
  typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
basic_static_string<N + M, CharT, Traits>
operator+(
  const basic_static_string<N, CharT, Traits>& lhs,
  const basic_static_string<M, CharT, Traits>& rhs)
{
  return basic_static_string<N + M, CharT, Traits>(lhs) += rhs;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
basic_static_string<N + 1, CharT, Traits>
operator+(
  const basic_static_string<N, CharT, Traits>& lhs,
  CharT rhs)
{
  return basic_static_string<N + 1, CharT, Traits>(lhs) += rhs;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
basic_static_string<N + 1, CharT, Traits>
operator+(
  CharT lhs,
  const basic_static_string<N, CharT, Traits>& rhs)
{
  // The cast to std::size_t is needed here since 0 is a null pointer constant
  return basic_static_string<N + 1, CharT, Traits>(rhs).insert(
    std::size_t(0), 1, lhs);
}

// Add a null-terminated character array to a string.
template<
  std::size_t N, std::size_t M,
  typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
basic_static_string<(N + M) - 1, CharT, Traits>
operator+(
  const basic_static_string<N, CharT, Traits>& lhs,
  const CharT(&rhs)[M])
{
  // Subtract 1 to account for the null terminator, as "hello" is a char[6]
  return basic_static_string<(N + M) - 1, CharT, Traits>(lhs).append(+rhs, M - 1);
}

// Add a string to a null-terminated character array.
template<
  std::size_t N, std::size_t M,
  typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
basic_static_string<(N + M) - 1, CharT, Traits>
operator+(
  const CharT(&lhs)[N],
  const basic_static_string<M, CharT, Traits>& rhs)
{
  // Subtract 1 to account for the null terminator, as "hello" is a char[6]
  // The cast to std::size_t is needed here since 0 is a null pointer constant
  return basic_static_string<(N + M) - 1, CharT, Traits>(rhs).insert(
    std::size_t(0), +lhs, N - 1);
}

//------------------------------------------------------------------------------
//
// swap
//
//------------------------------------------------------------------------------

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
void
swap(
  basic_static_string<N, CharT, Traits>& lhs,
  basic_static_string<N, CharT, Traits>& rhs)
{
  lhs.swap(rhs);
}

template<
  std::size_t N, std::size_t M,
  typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
void
swap(
  basic_static_string<N, CharT, Traits>& lhs,
  basic_static_string<M, CharT, Traits>& rhs)
{
  lhs.swap(rhs);
}

//------------------------------------------------------------------------------
//
// Input/Output
//
//------------------------------------------------------------------------------

template<std::size_t N, typename CharT, typename Traits>
inline
std::basic_ostream<CharT, Traits>& 
operator<<(
  std::basic_ostream<CharT, Traits>& os, 
  const basic_static_string<N, CharT, Traits>& s)
{
  return os << typename basic_static_string<N, CharT, Traits>::string_view_type(s.data(), s.size());
}

//------------------------------------------------------------------------------
//
// Numeric conversions
//
//------------------------------------------------------------------------------

/// Converts `value` to a `static_string` 
static_string<std::numeric_limits<int>::digits10 + 1>
inline
to_static_string(int value) noexcept
{
  return detail::to_static_string_int_impl<
    std::numeric_limits<int>::digits10 + 1>(value);
}

/// Converts `value` to a `static_string` 
static_string<std::numeric_limits<long>::digits10 + 1>
inline
to_static_string(long value) noexcept
{
  return detail::to_static_string_int_impl<
    std::numeric_limits<long>::digits10 + 1>(value);
}

/// Converts `value` to a `static_string` 
static_string<std::numeric_limits<long long>::digits10 + 1>
inline
to_static_string(long long value) noexcept
{
  return detail::to_static_string_int_impl<
    std::numeric_limits<long long>::digits10 + 1>(value);
}

/// Converts `value` to a `static_string` 
static_string<std::numeric_limits<unsigned int>::digits10 + 1>
inline
to_static_string(unsigned int value) noexcept
{
  return detail::to_static_string_int_impl<
    std::numeric_limits<unsigned int>::digits10 + 1>(value);
}

/// Converts `value` to a `static_string` 
static_string<std::numeric_limits<unsigned long>::digits10 + 1>
inline
to_static_string(unsigned long value) noexcept
{
  return detail::to_static_string_int_impl<
    std::numeric_limits<unsigned long>::digits10 + 1>(value);
}

/// Converts `value` to a `static_string` 
static_string<std::numeric_limits<unsigned long long>::digits10 + 1>
inline
to_static_string(unsigned long long value) noexcept
{
  return detail::to_static_string_int_impl<
    std::numeric_limits<unsigned long long>::digits10 + 1>(value);
}

/// Converts `value` to a `static_string` 
static_string<std::numeric_limits<float>::max_digits10 + 1>
inline
to_static_string(float value) noexcept
{
  return detail::to_static_string_float_impl<
    std::numeric_limits<float>::max_digits10 + 1>(value);
}

/// Converts `value` to a `static_string` 
static_string<std::numeric_limits<double>::max_digits10 + 1>
inline
to_static_string(double value) noexcept
{
  return detail::to_static_string_float_impl<
    std::numeric_limits<double>::max_digits10 + 1>(value);
}

/// Converts `value` to a `static_string` 
static_string<std::numeric_limits<long double>::max_digits10 + 1>
inline
to_static_string(long double value) noexcept
{
  return detail::to_static_string_float_impl<
    std::numeric_limits<long double>::max_digits10 + 1>(value);
}

/// Converts `value` to a `static_wstring` 
static_wstring<std::numeric_limits<int>::digits10 + 1>
inline
to_static_wstring(int value) noexcept
{
  return detail::to_static_wstring_int_impl<
    std::numeric_limits<int>::digits10 + 1>(value);
}

/// Converts `value` to a `static_wstring` 
static_wstring<std::numeric_limits<long>::digits10 + 1>
inline
to_static_wstring(long value) noexcept
{
  return detail::to_static_wstring_int_impl<
    std::numeric_limits<long>::digits10 + 1>(value);
}

/// Converts `value` to a `static_wstring` 
static_wstring<std::numeric_limits<long long>::digits10 + 1>
inline
to_static_wstring(long long value) noexcept
{
  return detail::to_static_wstring_int_impl<
    std::numeric_limits<long long>::digits10 + 1>(value);
}

/// Converts `value` to a `static_wstring` 
static_wstring<std::numeric_limits<unsigned int>::digits10 + 1>
inline
to_static_wstring(unsigned int value) noexcept
{
  return detail::to_static_wstring_int_impl<
    std::numeric_limits<unsigned int>::digits10 + 1>(value);
}

/// Converts `value` to a `static_wstring` 
static_wstring<std::numeric_limits<unsigned long>::digits10 + 1>
inline
to_static_wstring(unsigned long value) noexcept
{
  return detail::to_static_wstring_int_impl<
    std::numeric_limits<unsigned long>::digits10 + 1>(value);
}

/// Converts `value` to a `static_wstring` 
static_wstring<std::numeric_limits<unsigned long long>::digits10 + 1>
inline
to_static_wstring(unsigned long long value) noexcept
{
  return detail::to_static_wstring_int_impl<
    std::numeric_limits<unsigned long long>::digits10 + 1>(value);
}

/// Converts `value` to a `static_wstring` 
static_wstring<std::numeric_limits<float>::max_digits10 + 1>
inline
to_static_wstring(float value) noexcept
{
  return detail::to_static_wstring_float_impl<
    std::numeric_limits<float>::max_digits10 + 1>(value);
}

/// Converts `value` to a `static_wstring` 
static_wstring<std::numeric_limits<double>::max_digits10 + 1>
inline
to_static_wstring(double value) noexcept
{
  return detail::to_static_wstring_float_impl<
    std::numeric_limits<double>::max_digits10 + 1>(value);
}

/// Converts `value` to a `static_wstring` 
static_wstring<std::numeric_limits<long double>::max_digits10 + 1>
inline
to_static_wstring(long double value) noexcept
{
  return detail::to_static_wstring_float_impl<
    std::numeric_limits<long double>::max_digits10 + 1>(value);
}

//------------------------------------------------------------------------------
//
// Deduction Guides
//
//------------------------------------------------------------------------------

#ifdef BOOST_STATIC_STRING_USE_DEDUCT
template<std::size_t N, typename CharT>
basic_static_string(CharT(&)[N]) -> 
  basic_static_string<N, CharT, std::char_traits<CharT>>;
#endif

//------------------------------------------------------------------------------
//
// Hashing
//
//------------------------------------------------------------------------------

#ifndef BOOST_STATIC_STRING_STANDALONE
/// hash_value overload for Boost.Container_Hash
template <std::size_t N, 
  typename CharT, 
  typename Traits>
std::size_t 
hash_value(
  const basic_static_string<N, CharT, Traits>& str)
{
  return boost::hash_range(str.begin(), str.end());
}
#endif

} // static_string
} // boost

/// std::hash partial specialization for basic_static_string
namespace std {

template<std::size_t N, typename CharT, typename Traits>
struct hash<
#if GENERATING_DOCUMENTATION
  basic_static_string
#else
  boost::static_string::basic_static_string<N, CharT, Traits>
#endif
  >
{
  std::size_t 
  operator()(
    const boost::static_string::basic_static_string<N, CharT, Traits>& str) const noexcept
  {
#ifndef BOOST_STATIC_STRING_STANDALONE
    return boost::hash_range(str.begin(), str.end());
#else
    using view_type = typename boost::static_string::
      basic_static_string<N, CharT, Traits>::string_view_type;
    return std::hash<view_type>()(view_type(str.data(), str.size()));
#endif
  }
};
} // std

//--------------------------------------------------------------------------
//
// Implementation
//
//--------------------------------------------------------------------------

#ifndef GENERATING_DOCUMENTATION
namespace boost {
namespace static_string {

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
basic_static_string<N, CharT, Traits>::
basic_static_string(const_pointer s)
{
  const auto count = traits_type::length(s);
  BOOST_STATIC_STRING_THROW_IF(count > max_size(), 
    std::length_error{"count > max_size()"});
  traits_type::copy(data(), s, count + 1);
  this->set_size(count);
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
assign(
  size_type count,
  value_type ch) ->
    basic_static_string&
{
  BOOST_STATIC_STRING_THROW_IF(count > max_size(), 
    std::length_error{"count > max_size()"});
  this->set_size(count);
  traits_type::assign(data(), size(), ch);
  return term();
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
assign(
    const basic_static_string& s) noexcept ->
        basic_static_string&
{
  if(this == &s)
    return *this;
  this->set_size(s.size());
  const auto n = size() + 1;
  traits_type::copy(data(), &s.data()[0], n);
  return *this;
}

template<std::size_t N, typename CharT, typename Traits>
template<std::size_t M>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
assign(
  const basic_static_string<M, CharT, Traits>& s,
  size_type pos,
  size_type count) ->
    basic_static_string&
{
  const auto ss = s.subview(pos, count);
  return assign(ss.data(), ss.size());
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
assign(
  const_pointer s,
  size_type count) ->
    basic_static_string&
{
  BOOST_STATIC_STRING_THROW_IF(count > max_size(), 
    std::length_error{"count > max_size()"});
  this->set_size(count);
  traits_type::move(data(), s, size());
  return term();
}

template<std::size_t N, typename CharT, typename Traits>
template<class InputIterator>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
assign(
  InputIterator first,
  InputIterator last) ->
    typename std::enable_if<
      detail::is_input_iterator<InputIterator>::value,
        basic_static_string&>::type
{
  auto ptr = data();
  for (std::size_t i = 0; first != last; ++first, ++ptr, ++i)
  {
    if (i >= max_size())
    {
      this->set_size(i);
      term();
      BOOST_STATIC_STRING_THROW(std::length_error{"n > max_size()"});
    }
    traits_type::assign(*ptr, *first);
  }
  this->set_size(ptr - data());
  return term();
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
insert(
  const_iterator pos,
  size_type count,
  value_type ch) ->
    iterator
{
  const auto curr_size = size();
  const auto curr_data = data();
  BOOST_STATIC_STRING_THROW_IF(
    count > max_size() - curr_size, std::length_error{"count() > max_size() - size()"});
  const auto index = pos - curr_data;
  traits_type::move(&curr_data[index + count], &curr_data[index], curr_size - index + 1);
  traits_type::assign(&curr_data[index], count, ch);
  this->set_size(curr_size + count);
  return &curr_data[index];
}

template<std::size_t N, typename CharT, typename Traits>
template<class ForwardIterator>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
insert(
  const_iterator pos,
  ForwardIterator first,
  ForwardIterator last) ->
    typename std::enable_if<
      detail::is_forward_iterator<
        ForwardIterator>::value, iterator>::type
{
  const auto curr_size = size();
  const auto curr_data = data();
  const std::size_t count = detail::distance(first, last);
  const std::size_t index = pos - curr_data;
  const auto s = &*first;
  BOOST_STATIC_STRING_THROW_IF(
    count > max_size() - curr_size, std::length_error{"count > max_size() - size()"});
  const bool inside = detail::ptr_in_range(curr_data, curr_data + curr_size, s);
  if (!inside || (inside && ((s - curr_data) + count <= index)))
  {
    traits_type::move(&curr_data[index + count], &curr_data[index], curr_size - index + 1);
    detail::copy_with_traits<Traits>(first, last, &curr_data[index]);
  }
  else
  {
    const size_type offset = s - curr_data;
    traits_type::move(&curr_data[index + count], &curr_data[index], curr_size - index + 1);
    if (offset < index)
    {
      const size_type diff = index - offset;
      traits_type::copy(&curr_data[index], &curr_data[offset], diff);
      traits_type::copy(&curr_data[index + diff], &curr_data[index + count], count - diff);
    }
    else
    {
      traits_type::copy(&curr_data[index], &curr_data[offset + count], count);
    }
  }
  this->set_size(curr_size + count);
  return curr_data + index;
}

template<std::size_t N, typename CharT, typename Traits>
template<class InputIterator>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
insert(
  const_iterator pos,
  InputIterator first,
  InputIterator last) ->
    typename std::enable_if<
      detail::is_input_iterator<
        InputIterator>::value && 
          !detail::is_forward_iterator<
            InputIterator>::value, iterator>::type
{
  const auto curr_size = size();
  const auto curr_data = data();
  const auto count = read_back(first, last);
  const std::size_t index = pos - curr_data;
  std::rotate(&curr_data[index], &curr_data[curr_size + 1], &curr_data[curr_size + count + 1]);
  this->set_size(curr_size + count);
  return curr_data + index;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
erase(
  size_type index,
  size_type count) ->
    basic_static_string&
{
  BOOST_STATIC_STRING_THROW_IF(
    index > size(), std::out_of_range{"index > size()"});
  erase(data() + index, data() + index + (std::min)(count, size() - index));
  return *this;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
erase(
  const_iterator first,
  const_iterator last) ->
    iterator
{
  const auto curr_data = data();
  const std::size_t index = first - curr_data;
  traits_type::move(&curr_data[index], last, (end() - last) + 1);
  this->set_size(size() - std::size_t(last - first));
  return curr_data + index;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
void
basic_static_string<N, CharT, Traits>::
push_back(
  value_type ch)
{
  const auto curr_size = size();
  BOOST_STATIC_STRING_THROW_IF(
    curr_size >= max_size(), std::length_error{"size() >= max_size()"});
  traits_type::assign(data()[curr_size], ch);
  this->set_size(curr_size + 1);
  term();
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
append(
  const_pointer s,
  size_type count) ->
    basic_static_string&
{
  const auto curr_size = size();
  BOOST_STATIC_STRING_THROW_IF(
    count > max_size() - curr_size, std::length_error{"count > max_size() - size()"});
  traits_type::copy(&data()[curr_size], s, count);
  this->set_size(curr_size + count);
  return term();
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
copy(CharT* dest, size_type count, size_type pos) const ->
  size_type
{
  const auto s = subview(pos, count);
  traits_type::copy(dest, s.data(), s.size());
  return s.size();
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
void
basic_static_string<N, CharT, Traits>::
resize(size_type n)
{
  BOOST_STATIC_STRING_THROW_IF(
    n > max_size(), std::length_error{"n > max_size()"});
  const auto curr_size = size();
  if(n > curr_size)
    traits_type::assign(&data()[curr_size], n - curr_size, value_type());
  this->set_size(n);
  term();
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
void
basic_static_string<N, CharT, Traits>::
resize(size_type n, value_type c)
{
  BOOST_STATIC_STRING_THROW_IF(
    n > max_size(), std::length_error{"n > max_size()"});
  const auto curr_size = size();
  if(n > curr_size)
    traits_type::assign(&data()[curr_size], n - curr_size, c);
  this->set_size(n);
  term();
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
void
basic_static_string<N, CharT, Traits>::
swap(basic_static_string& s) noexcept
{
  const auto curr_size = size();
  basic_static_string tmp(s);
  s.set_size(curr_size);
  traits_type::copy(&s.data()[0], data(), curr_size + 1);
  this->set_size(tmp.size());
  traits_type::copy(data(), tmp.data(), size() + 1);
}

template<std::size_t N, typename CharT, typename Traits>
template<std::size_t M>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
void
basic_static_string<N, CharT, Traits>::
swap(basic_static_string<M, CharT, Traits>& s)
{
  const auto curr_size = size();
  BOOST_STATIC_STRING_THROW_IF(
    curr_size > s.max_size(), std::length_error{"size() > s.max_size()"});
  BOOST_STATIC_STRING_THROW_IF(
    s.size() > max_size(), std::length_error{"s.size() > max_size()"});
  basic_static_string tmp(s);
  s.set_size(curr_size);
  traits_type::copy(&s.data()[0], data(), curr_size + 1);
  this->set_size(tmp.size());
  traits_type::copy(data(), &tmp.data()[0], size() + 1);
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
replace(
  const_iterator i1,
  const_iterator i2,
  size_type n,
  value_type c) ->
    basic_static_string<N, CharT, Traits>&
{
  const auto curr_size = size();
  const auto curr_data = data();
  const std::size_t n1 = i2 - i1;
  BOOST_STATIC_STRING_THROW_IF(
    n > max_size() ||
    curr_size - n1 >= max_size() - n,
    std::length_error{"replaced string exceeds max_size()"});
  const auto pos = i1 - curr_data;
  traits_type::move(&curr_data[pos + n], i2, (end() - i2) + 1);
  traits_type::assign(&curr_data[pos], n, c);
  this->set_size((curr_size - n1) + n);
  return *this;
}

template<std::size_t N, typename CharT, typename Traits>
template<typename ForwardIterator>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
replace(
  const_iterator i1,
  const_iterator i2,
  ForwardIterator j1,
  ForwardIterator j2) ->
    typename std::enable_if<
      detail::is_forward_iterator<ForwardIterator>::value,
        basic_static_string<N, CharT, Traits>&>::type
{
  const auto curr_size = size();
  const auto curr_data = data();
  const auto first_addr = &*j1;
  const std::size_t n1 = i2 - i1;
  const std::size_t n2 = detail::distance(j1, j2);
  const std::size_t pos = i1 - curr_data;
  BOOST_STATIC_STRING_THROW_IF(
    n2 > max_size() ||
    curr_size - (std::min)(n1, curr_size - pos) >= max_size() - n2,
    std::length_error{"replaced string exceeds max_size()"});
  const bool inside = detail::ptr_in_range(curr_data, curr_data + curr_size, first_addr);
  // due to short circuit evaluation, the second operand of the logical
  // AND expression will only be evaluated if s is within the string,
  // which means that operand of the cast will never be negative.
  if (inside && std::size_t(first_addr - curr_data) == pos && n1 == n2)
    return *this;
  if (!inside || (inside && ((first_addr - curr_data) + n2 <= pos)))
  {
    // source outside
    traits_type::move(&curr_data[pos + n2], &curr_data[pos + n1], curr_size - pos - n1 + 1);
    detail::copy_with_traits<Traits>(j1, j2, &curr_data[pos]);
  }
  else
  {
    // source inside
    const size_type offset = first_addr - curr_data;
    if (n2 >= n1)
    {
      // grow/unchanged
      const size_type diff = offset <= pos + n1 ? (std::min)((pos + n1) - offset, n2) : 0;
      // shift all right of splice point by n2 - n1 to the right
      traits_type::move(&curr_data[pos + n2], &curr_data[pos + n1], curr_size - pos - n1 + 1);
      // copy all before splice point
      traits_type::move(&curr_data[pos], &curr_data[offset], diff);
      // copy all after splice point
      traits_type::move(&curr_data[pos + diff], &curr_data[(offset - n1) + n2 + diff], n2 - diff);
    }
    else
    {
      // shrink
      // copy all elements into place
      traits_type::move(&curr_data[pos], &curr_data[offset], n2);
      // shift all elements after splice point left
      traits_type::move(&curr_data[pos + n2], &curr_data[pos + n1], curr_size - pos - n1 + 1);
    }
  }
  this->set_size((curr_size - n1) + n2);
  return *this;
}

template<std::size_t N, typename CharT, typename Traits>
template<typename InputIterator>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
replace(
  const_iterator i1,
  const_iterator i2,
  InputIterator j1,
  InputIterator j2) ->
    typename std::enable_if<
      detail::is_input_iterator<
        InputIterator>::value &&
          !detail::is_forward_iterator<
            InputIterator>::value, 
              basic_static_string<N, CharT, Traits>&>::type
{
  const auto curr_size = size();
  const auto curr_data = data();
  const std::size_t n1 = detail::distance(i1, i2);
  const std::size_t n2 = read_back(j1, j2);
  const std::size_t pos = i1 - curr_data;
  BOOST_STATIC_STRING_THROW_IF(
    n2 > max_size() ||
    curr_size - (std::min)(n1, curr_size - pos) >= max_size() - n2,
    std::length_error{"replaced string exceeds max_size()"});
  // Rotate to the correct order. [i2, end] will now start with the replaced string, 
  // continue to the existing string not being replaced, and end with a null terminator
  std::rotate(&curr_data[pos], &curr_data[curr_size + 1], &curr_data[curr_size + n2 + 1]);
  // Move everything from the end of the splice point to the end of the rotated string to
  // the begining of the splice point
  traits_type::move(&curr_data[pos + n2], &curr_data[pos + n2 + n1], ((curr_size - n1) + n2) - pos);
  this->set_size((curr_size - n1) + n2);
  return *this;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
find(
  const_pointer s,
  size_type pos,
  size_type n) const noexcept ->
    size_type
{
  const auto curr_size = size();
  if (pos > curr_size || n > curr_size - pos)
    return npos;
  if (!n)
    return pos;
  const auto res = detail::search(&data()[pos], &data()[curr_size], s, &s[n], traits_type::eq);
  return res == end() ? npos : detail::distance(data(), res);
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
rfind(
  const_pointer s,
  size_type pos,
  size_type n) const noexcept ->
    size_type
{
  const auto curr_size = size();
  const auto curr_data = data();
  if (curr_size < n)
    return npos;
  if (pos > curr_size - n)
    pos = curr_size - n;
  if (!n)
    return pos;
  for (auto sub = &curr_data[pos]; sub >= curr_data; --sub)
    if (!traits_type::compare(sub, s, n))
      return detail::distance(curr_data, sub);
  return npos;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
find_first_of(
  const_pointer s,
  size_type pos,
  size_type n) const noexcept ->
    size_type
{
  const auto curr_data = data();
  if (pos >= size() || !n)
    return npos;
  const auto res = detail::find_first_of(&curr_data[pos], &curr_data[size()], s, &s[n], traits_type::eq);
  return res == end() ? npos : detail::distance(curr_data, res);
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
find_last_of(
  const_pointer s,
  size_type pos,
  size_type n) const noexcept ->
    size_type
{
  const auto curr_size = size();
  if (!n)
    return npos;
  if (pos >= curr_size)
    pos = 0;
  else
    pos = curr_size - (pos + 1);
  const auto res = detail::find_first_of(rbegin() + pos, rend(), s, &s[n], traits_type::eq);
  return res == rend() ? npos : curr_size - 1 - detail::distance(rbegin(), res);
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
find_first_not_of(
  const_pointer s,
  size_type pos,
  size_type n) const noexcept ->
    size_type
{
  if (pos >= size())
    return npos;
  if (!n)
    return pos;
  const auto res = detail::find_not_of<Traits>(&data()[pos], &data()[size()], s, n);
  return res == end() ? npos : detail::distance(data(), res);
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
find_last_not_of(
  const_pointer s,
  size_type pos,
  size_type n) const noexcept ->
    size_type
{
  const auto curr_size = size();
  if (pos >= curr_size)
    pos = curr_size - 1;
  if (!n)
    return pos;
  pos = curr_size - (pos + 1);
  const auto res = detail::find_not_of<Traits>(rbegin() + pos, rend(), s, n);
  return res == rend() ? npos : curr_size - 1 - detail::distance(rbegin(), res);
}

template<std::size_t N, typename CharT, typename Traits>
template<typename InputIterator>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
read_back(
  InputIterator first,
  InputIterator last) -> 
    size_type
{
  const auto curr_data = data();
  auto new_size = size();
  for (; first != last; ++first)
  {
    BOOST_STATIC_STRING_THROW_IF(
      new_size >= max_size(), std::length_error{"count > max_size() - size()"});
    traits_type::assign(curr_data[++new_size], *first);
  }
  return new_size - size();
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
replace_unchecked(
  const_iterator i1,
  const_iterator i2,
  const_pointer s,
  size_type n2) ->
    basic_static_string&
{
  const auto curr_data = data();
  const auto curr_size = size();
  const std::size_t pos = i1 - curr_data;
  const std::size_t n1 = i2 - i1;
  BOOST_STATIC_STRING_THROW_IF(
    n2 > max_size() ||
    curr_size - (std::min)(n1, curr_size - pos) >= max_size() - n2,
    std::length_error{"replaced string exceeds max_size()"});
  traits_type::move(&curr_data[pos + n2], i2, (end() - i2) + 1);
  traits_type::copy(&curr_data[pos], s, n2);
  this->set_size((curr_size - n1) + n2);
  return *this;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
insert_unchecked(
  const_iterator pos,
  const_pointer s,
  size_type count) ->
    iterator
{
  const auto curr_data = data();
  const auto curr_size = size();
  BOOST_STATIC_STRING_THROW_IF(
    count > max_size() - curr_size,
    std::length_error{"count > max_size() - size()"});
  const std::size_t index = pos - curr_data;
  traits_type::move(&curr_data[index + count], pos, (end() - pos) + 1);
  traits_type::copy(&curr_data[index], s, count);
  this->set_size(curr_size + count);
  return curr_data + index;
}
} // static_string
} // boost
#endif
#endif