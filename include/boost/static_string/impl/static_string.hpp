//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
// Copyright (c) 2019 Krystian Stasiowski (sdkrystian at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/static_string
//

#ifndef BOOST_STATIC_STRING_IMPL_STATIC_STRING_HPP
#define BOOST_STATIC_STRING_IMPL_STATIC_STRING_HPP

#include <boost/static_string/config.hpp>
#include <boost/static_string/detail/static_string.hpp>

namespace boost {
namespace static_string {

//------------------------------------------------------------------------------
//
// Construction
//
//------------------------------------------------------------------------------

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP11_CONSTEXPR
basic_static_string<N, CharT, Traits>::
basic_static_string() noexcept
{
#ifdef BOOST_STATIC_STRING_CPP20_CONSTEXPR_USED
  term();
#endif
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
basic_static_string<N, CharT, Traits>::
basic_static_string(size_type count, CharT ch) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  assign(count, ch);
}

template<std::size_t N, typename CharT, typename Traits>
template<std::size_t M>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
basic_static_string<N, CharT, Traits>::
basic_static_string(basic_static_string<M, CharT, Traits> const& other,
    size_type pos) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  assign(other, pos);
}

template<std::size_t N, typename CharT, typename Traits>
template<std::size_t M>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
basic_static_string<N, CharT, Traits>::
basic_static_string(
    basic_static_string<M, CharT, Traits> const& other,
    size_type pos,
    size_type count) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  assign(other, pos, count);
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
basic_static_string<N, CharT, Traits>::
basic_static_string(CharT const* s, size_type count) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  assign(s, count);
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
basic_static_string<N, CharT, Traits>::
basic_static_string(CharT const* s) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  auto const count = Traits::length(s);
  BOOST_STATIC_STRING_THROW_IF(count > max_size(), 
                                std::length_error{"count > max_size()"});
  this->set_size(count);
  Traits::copy(data(), s, size() + 1);
}

template<std::size_t N, typename CharT, typename Traits>
template<class InputIterator>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
basic_static_string<N, CharT, Traits>::
basic_static_string(
    InputIterator first,
    InputIterator last,
    typename std::enable_if<
        detail::is_input_iterator<InputIterator>::value,
            iterator>::type*) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  assign(first, last);
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
basic_static_string<N, CharT, Traits>::
basic_static_string(
    basic_static_string const& s) noexcept
{
  assign(s);
}

template<std::size_t N, typename CharT, typename Traits>
template<std::size_t M>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
basic_static_string<N, CharT, Traits>::
basic_static_string(
    basic_static_string<M, CharT, Traits> const& s) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  assign(s);
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
basic_static_string<N, CharT, Traits>::
basic_static_string(std::initializer_list<CharT> init) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  assign(init.begin(), init.end());
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
basic_static_string<N, CharT, Traits>::
basic_static_string(string_view_type sv) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  assign(sv);
}

template<std::size_t N, typename CharT, typename Traits>
template<class T, class>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
basic_static_string<N, CharT, Traits>::
basic_static_string(T const& t, size_type pos, size_type n) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  assign(t, pos, n);
}

//------------------------------------------------------------------------------
//
// Assignment
//
//------------------------------------------------------------------------------

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
assign(
    size_type count,
    CharT ch) BOOST_STATIC_STRING_COND_NOEXCEPT ->
        basic_static_string&
{
  BOOST_STATIC_STRING_THROW_IF(count > max_size(), 
                                std::length_error{"count > max_size()"});
  this->set_size(count);
  Traits::assign(data(), size(), ch);
  term();
  return *this;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
assign(
    basic_static_string const& s) noexcept ->
        basic_static_string&
{
  if(this == &s)
      return *this;
  this->set_size(s.size());
  auto const n = size() + 1;
  Traits::copy(data(), &s.data()[0], n);
  return *this;
}

template<std::size_t N, typename CharT, typename Traits>
template<std::size_t M>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
assign(
    basic_static_string<M, CharT, Traits> const& s,
    size_type pos,
    size_type count) BOOST_STATIC_STRING_COND_NOEXCEPT ->
    basic_static_string&
{
  auto const ss = s.subview(pos, count);
  return assign(ss.data(), ss.size());
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
assign(
    CharT const* s,
    size_type count) BOOST_STATIC_STRING_COND_NOEXCEPT ->
        basic_static_string&
{
  BOOST_STATIC_STRING_THROW_IF(count > max_size(), 
                                std::length_error{"count > max_size()"});
  this->set_size(count);
  Traits::move(data(), s, size());
  term();
  return *this;
}

template<std::size_t N, typename CharT, typename Traits>
template<class InputIterator>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
assign(
    InputIterator first,
    InputIterator last) BOOST_STATIC_STRING_COND_NOEXCEPT ->
        typename std::enable_if<
            detail::is_input_iterator<InputIterator>::value,
                basic_static_string&>::type
{
  auto ptr = data();
  for (std::size_t i = 0; first != last; ++first, ++ptr, ++i)
  {
    if (1 > max_size() - i)
    {
      this->set_size(i);
      BOOST_STATIC_STRING_THROW(std::length_error{"n > max_size()"});
    }
    Traits::assign(*ptr, *first);
  }
  this->set_size(ptr - data());
  term();
  return *this;
}

//------------------------------------------------------------------------------
//
// Element access
//
//------------------------------------------------------------------------------

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
at(size_type pos) BOOST_STATIC_STRING_COND_NOEXCEPT ->
    reference
{
  BOOST_STATIC_STRING_THROW_IF(
      pos >= size(), std::out_of_range{"pos >= size()"});
  return data()[pos];
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
at(size_type pos) const BOOST_STATIC_STRING_COND_NOEXCEPT ->
    const_reference
{
  BOOST_STATIC_STRING_THROW_IF(
      pos >= size(), std::out_of_range{"pos >= size()"});
  return data()[pos];
}

//------------------------------------------------------------------------------
//
// Capacity
//
//------------------------------------------------------------------------------

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
void
basic_static_string<N, CharT, Traits>::
reserve(std::size_t n) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  BOOST_STATIC_STRING_THROW_IF(
      n > max_size(), std::length_error{"n > max_size()"});
}

//------------------------------------------------------------------------------
//
// Operations
//
//------------------------------------------------------------------------------

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
void
basic_static_string<N, CharT, Traits>::
clear() noexcept
{
  this->set_size(0);
  term();
}

//------------------------------------------------------------------------------

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
insert(
    size_type index,
    CharT const* s,
    size_type count) BOOST_STATIC_STRING_COND_NOEXCEPT ->
        basic_static_string&
{
  const auto curr_size = size();
  const auto curr_data = data();
  BOOST_STATIC_STRING_THROW_IF(
      index > curr_size, std::out_of_range{"index > size()"});
  insert(curr_data + index, s, s + count);
  return *this;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
insert(
    const_iterator pos,
    size_type count,
    CharT ch) BOOST_STATIC_STRING_COND_NOEXCEPT ->
        iterator
{
  const auto curr_size = size();
  const auto curr_data = data();
  BOOST_STATIC_STRING_THROW_IF(
      count > max_size() - curr_size, std::length_error{"count() > max_size() - size()"});
  auto const index = pos - curr_data;
  Traits::move(&curr_data[index + count], &curr_data[index], curr_size - index);
  Traits::assign(&curr_data[index], count, ch);
  this->set_size(curr_size + count);
  term();
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
    ForwardIterator last) BOOST_STATIC_STRING_COND_NOEXCEPT ->
        typename std::enable_if<
            detail::is_forward_iterator<
                ForwardIterator>::value, iterator>::type
{
  const auto curr_size = size();
  const auto curr_data = data();
  const auto count = detail::distance(first, last);
  const auto index = pos - curr_data;
  const auto s = &*first;
  BOOST_STATIC_STRING_THROW_IF(
    count > max_size() - curr_size, std::length_error{"count > max_size() - size()"});
  // Makes this more constexpr friendly if we can use is_constant_evaluated
  const bool inside =
#ifdef BOOST_STATIC_STRING_USE_IS_CONST_EVAL
    std::is_constant_evaluated() ?
    detail::is_inside(curr_data, curr_data + curr_size, s) :
    s <= &curr_data[curr_size] && s >= curr_data;
#else
    s <= &curr_data[curr_size] && s >= curr_data;
#endif
  if (!inside || (inside && ((s - curr_data) + count <= index)))
  {
    Traits::move(&curr_data[index + count], &curr_data[index], curr_size - index + 1);
    detail::copy_with_traits<Traits>(first, last, &curr_data[index]);
  }
  else
  {
    const size_type offset = s - curr_data;
    Traits::move(&curr_data[index + count], &curr_data[index], curr_size - index + 1);
    if (offset < index)
    {
      const size_type diff = index - offset;
      Traits::copy(&curr_data[index], &curr_data[offset], diff);
      Traits::copy(&curr_data[index + diff], &curr_data[index + count], count - diff);
    }
    else
    {
      Traits::copy(&curr_data[index], &curr_data[offset + count], count);
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
    InputIterator last) BOOST_STATIC_STRING_COND_NOEXCEPT ->
        typename std::enable_if<
            detail::is_input_iterator<
                InputIterator>::value && 
                    ! detail::is_forward_iterator<
                          InputIterator>::value, iterator>::type
{
  const auto curr_size = size();
  const auto curr_data = data();
  const auto count = read_back(first, last);
  const auto index = pos - curr_data;
  const auto s = curr_data + curr_size + 1;
  BOOST_STATIC_STRING_THROW_IF(
    index > curr_size, std::out_of_range{"index > size()"});
  std::rotate(&curr_data[index], &curr_data[curr_size + 1], &curr_data[curr_size + count + 1]);
  this->set_size(curr_size + count);
  return curr_data + index;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
insert(
    const_iterator pos,
    std::initializer_list<CharT> ilist) BOOST_STATIC_STRING_COND_NOEXCEPT ->
        iterator
{
  const auto curr_size = size();
  const auto curr_data = data();
  const auto index = pos - curr_data;
  BOOST_STATIC_STRING_THROW_IF(
    index > curr_size, std::out_of_range{"index > size()"});
  BOOST_STATIC_STRING_THROW_IF(
    ilist.size() > max_size() - curr_size, std::length_error{"count > max_size() - size()"});
  Traits::move(&curr_data[index + ilist.size()], &curr_data[index], curr_size - index + 1);
  detail::copy_with_traits<Traits>(ilist.begin(), ilist.end(), &curr_data[index]);
  this->set_size(curr_size + ilist.size());
  return curr_data + index;
}

//------------------------------------------------------------------------------

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
erase(
    size_type index,
    size_type count) BOOST_STATIC_STRING_COND_NOEXCEPT ->
        basic_static_string&
{
  const auto curr_size = size();
  const auto curr_data = data();
  BOOST_STATIC_STRING_THROW_IF(
      index > curr_size, std::out_of_range{"index > size()"});
  auto const n = (std::min)(count, curr_size - index);
  Traits::move(&curr_data[index], &curr_data[index + n], curr_size - (index + n) + 1);
  this->set_size(curr_size - n);
  return *this;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
erase(
    const_iterator pos) BOOST_STATIC_STRING_COND_NOEXCEPT ->
        iterator
{
  erase(pos - begin(), 1);
  return begin() + (pos - begin());
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
erase(
    const_iterator first,
    const_iterator last) BOOST_STATIC_STRING_COND_NOEXCEPT ->
        iterator
{
  erase(first - begin(),
      detail::distance(first, last));
  return begin() + (first - begin());
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
void
basic_static_string<N, CharT, Traits>::
push_back(
    CharT ch) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  const auto curr_size = size();
  BOOST_STATIC_STRING_THROW_IF(
      curr_size >= max_size(), std::length_error{"size() >= max_size()"});
  Traits::assign(data()[curr_size], ch);
  this->set_size(curr_size + 1);
  term();
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
append(
    CharT const* s,
    size_type count) BOOST_STATIC_STRING_COND_NOEXCEPT ->
        basic_static_string&
{
  const auto curr_size = size();
  BOOST_STATIC_STRING_THROW_IF(
      count > max_size() - curr_size, std::length_error{"count > max_size() - size()"});
  Traits::copy(&data()[curr_size], s, count);
  this->set_size(curr_size + count);
  term();
  return *this;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
substr(size_type pos, size_type count) const BOOST_STATIC_STRING_COND_NOEXCEPT ->
    basic_static_string
{
  BOOST_STATIC_STRING_THROW_IF(
      pos > size(), std::out_of_range{"pos > size()"});
  return {&data()[pos], (std::min)(count, size() - pos)};
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
subview(size_type pos, size_type count) const BOOST_STATIC_STRING_COND_NOEXCEPT ->
    string_view_type
{
  BOOST_STATIC_STRING_THROW_IF(
    pos > size(), std::out_of_range{"pos > size()"});
  return {&data()[pos], (std::min)(count, size() - pos)};
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
copy(CharT* dest, size_type count, size_type pos) const noexcept ->
    size_type
{
  auto const s = subview(pos, count);
  Traits::copy(dest, s.data(), s.size());
  return s.size();
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
void
basic_static_string<N, CharT, Traits>::
resize(std::size_t n) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  const auto curr_size = size();
  BOOST_STATIC_STRING_THROW_IF(
      n > max_size(), std::length_error{"n > max_size()"});
  if(n > curr_size)
      Traits::assign(&data()[curr_size], n - curr_size, CharT{});
  this->set_size(n);
  term();
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
void
basic_static_string<N, CharT, Traits>::
resize(std::size_t n, CharT c) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  const auto curr_size = size();
  BOOST_STATIC_STRING_THROW_IF(
      n > max_size(), std::length_error{"n > max_size()"});
  if(n > curr_size)
      Traits::assign(&data()[curr_size], n - curr_size, c);
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
  Traits::copy(&s.data()[0], data(), curr_size + 1);
  this->set_size(tmp.size());
  Traits::copy(data(), tmp.data(), size() + 1);
}

template<std::size_t N, typename CharT, typename Traits>
template<std::size_t M>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
void
basic_static_string<N, CharT, Traits>::
swap(basic_static_string<M, CharT, Traits>& s) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  const auto curr_size = size();
  BOOST_STATIC_STRING_THROW_IF(
      curr_size > s.max_size(), std::length_error{"size() > s.max_size()"});
  BOOST_STATIC_STRING_THROW_IF(
      s.size() > max_size(), std::length_error{"s.size() > max_size()"});
  basic_static_string tmp(s);
  s.set_size(curr_size);
  Traits::copy(&s.data()[0], data(), curr_size + 1);
  this->set_size(tmp.size());
  Traits::copy(data(), &tmp.data()[0], size() + 1);
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
replace(
    size_type pos,
    size_type n1,
    const CharT* s,
    size_type n2) BOOST_STATIC_STRING_COND_NOEXCEPT -> 
        basic_static_string<N, CharT, Traits>&
{
  const auto curr_size = size();
  const auto curr_data = data();
  BOOST_STATIC_STRING_THROW_IF(
      pos > curr_size, std::out_of_range{"pos > size()"});
  n1 = (std::min)(n1, curr_size - pos);
  return replace(curr_data + pos, curr_data + pos + n1, s, s + n2);
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
replace(
    size_type pos,
    size_type n1,
    size_type n2,
    CharT c) BOOST_STATIC_STRING_COND_NOEXCEPT -> basic_static_string<N, CharT, Traits> &
{
  const auto curr_size = size();
  const auto curr_data = data();
  BOOST_STATIC_STRING_THROW_IF(
      pos > curr_size, std::out_of_range{"pos > size()"});
  BOOST_STATIC_STRING_THROW_IF(
      curr_size - (std::min)(n1, curr_size - pos) >= max_size() - n2,
      std::length_error{"replaced string exceeds max_size()"});
  n1 = (std::min)(n1, curr_size - pos);
  Traits::move(&curr_data[pos + n2], &curr_data[pos + n1], curr_size - pos - n1 + 1);
  Traits::assign(&curr_data[pos], n2, c);
  this->set_size(curr_size + (n2 - n1));
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
      ForwardIterator j2) BOOST_STATIC_STRING_COND_NOEXCEPT ->
          typename std::enable_if<
              detail::is_forward_iterator<ForwardIterator>::value,
                  basic_static_string<N, CharT, Traits>&>::type
{
  const auto curr_size = size();
  const auto curr_data = data();
  std::size_t n1 = detail::distance(i1, i2);
  const std::size_t n2 = detail::distance(j1, j2);
  const std::size_t pos = i1 - curr_data;
  const auto s = &*j1;
  BOOST_STATIC_STRING_THROW_IF(
    curr_size - (std::min)(n1, curr_size - pos) >= max_size() - n2,
    std::length_error{"replaced string exceeds max_size()"});
  n1 = (std::min)(n1, curr_size - pos);
  // Makes this more constexpr friendly if we can use is_constant_evaluated
  const bool inside =
#ifdef BOOST_STATIC_STRING_USE_IS_CONST_EVAL
    std::is_constant_evaluated() ? 
    detail::is_inside(curr_data, curr_data + curr_size, s) :
    s <= &curr_data[curr_size] && s >= curr_data;
#else
    s <= &curr_data[curr_size] && s >= curr_data;
#endif
  if (inside && size_type(s - curr_data) == pos && n1 == n2)
    return *this;
  if (!inside || (inside && ((s - curr_data) + n2 <= pos)))
  {
    // source outside
    Traits::move(&curr_data[pos + n2], &curr_data[pos + n1], curr_size - pos - n1 + 1);
    detail::copy_with_traits<Traits>(j1, j2, &curr_data[pos]);
  }
  else
  {
    // source inside
    const size_type offset = s - curr_data;
    if (n2 >= n1)
    {
      // grow/unchanged
      // shift all right of splice point by n2 - n1 to the right
      Traits::move(&curr_data[pos + n2], &curr_data[pos + n1], curr_size - pos - n1 + 1);
      const size_type diff = offset <= pos + n1 ? (std::min)((pos + n1) - offset, n2) : 0;
      // copy all before splice point
      Traits::move(&curr_data[pos], &curr_data[offset], diff);
      // copy all after splice point
      Traits::move(&curr_data[pos + diff], &curr_data[offset + (n2 - n1) + diff], n2 - diff);
    }
    else
    {
      // shrink
      // copy all elements into place
      Traits::move(&curr_data[pos], &curr_data[offset], n2);
      // shift all elements after splice point left
      Traits::move(&curr_data[pos + n2], &curr_data[pos + n1], curr_size - pos - n1 + 1);
    }
  }
  this->set_size(curr_size + (n2 - n1));
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
      InputIterator j2) BOOST_STATIC_STRING_COND_NOEXCEPT ->
          typename std::enable_if<
            detail::is_input_iterator<
                InputIterator>::value && 
                    ! detail::is_forward_iterator<
                         InputIterator>::value, 
                             basic_static_string<N, CharT, Traits>&>::type
{
  const auto curr_size = size();
  const auto curr_data = data();
  std::size_t n1 = detail::distance(i1, i2);
  const std::size_t n2 = read_back(j1, j2);
  const std::size_t pos = i1 - curr_data;
  BOOST_STATIC_STRING_THROW_IF(
    pos > curr_size, std::out_of_range{"pos > size()"});
  BOOST_STATIC_STRING_THROW_IF(
    curr_size - (std::min)(n1, curr_size - pos) >= max_size() - n2,
    std::length_error{"replaced string exceeds max_size()"});
  // Rotate to the correct order. [i2, end] will now start with the replaced string, continue to the existing string not being replaced, and end with a null terminator
  std::rotate(&curr_data[pos], &curr_data[curr_size + 1], &curr_data[curr_size + n2 + 1]);
  // Cap the size
  n1 = (std::min)(n1, curr_size - pos);
  // Move everything from the end of the splice point to the end of the rotated string to the begining of the splice point
  Traits::move(&curr_data[pos + n2], &curr_data[pos + n2 + n1], (curr_size + (n2 - n1)) - pos);
  this->set_size(curr_size + (n2 - n1));
  return *this;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
find(
    const CharT* s,
    size_type pos,
    size_type n) const noexcept ->
        size_type
{
  const auto curr_size = size();
  if (pos > curr_size || n > curr_size - pos)
    return npos;
  if (!n)
    return pos;
  const auto res = detail::search(&data()[pos], &data()[curr_size], s, &s[n], Traits::eq);
  return res == end() ? npos : detail::distance(data(), res);
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
rfind(
    const CharT* s,
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
    if (!Traits::compare(sub, s, n))
      return detail::distance(curr_data, sub);
  return npos;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
find_first_of(
    const CharT* s,
    size_type pos,
    size_type n) const noexcept ->
        size_type
{
  const auto curr_data = data();
  if (pos >= size() || !n)
    return npos;
  const auto res = detail::find_first_of(&curr_data[pos], &curr_data[size()], s, &s[n], Traits::eq);
  return res == end() ? npos : detail::distance(curr_data, res);
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
find_last_of(
    const CharT* s,
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
  const auto res = detail::find_first_of(rbegin() + pos, rend(), s, &s[n], Traits::eq);
  return res == rend() ? npos : curr_size - 1 - detail::distance(rbegin(), res);
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
find_first_not_of(
    const CharT* s,
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
    const CharT* s,
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
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
assign_char(CharT ch, std::true_type) noexcept ->
    basic_static_string&
{
  this->set_size(1);
  Traits::assign(data()[0], ch);
  term();
  return *this;
}

template<std::size_t N, typename CharT, typename Traits>
auto
basic_static_string<N, CharT, Traits>::
assign_char(CharT, std::false_type) BOOST_STATIC_STRING_COND_NOEXCEPT ->
    basic_static_string&
{
  BOOST_STATIC_STRING_THROW(std::length_error{"max_size() == 0"});
  return *this;
}

template<std::size_t N, typename CharT, typename Traits>
template<typename InputIterator>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
read_back(
    InputIterator first,
    InputIterator last) -> 
        std::size_t
{
  const auto curr_data = data();
  auto new_size = size();
  for (; first != last; ++first)
  {
    BOOST_STATIC_STRING_THROW_IF(
      1 > max_size() - new_size, std::length_error{"count > max_size() - size()"});
    Traits::assign(curr_data[++new_size], *first);
  }
  return new_size - size();
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
replace_unchecked(
    size_type pos,
    size_type n1,
    const CharT* s,
    size_type n2) BOOST_STATIC_STRING_COND_NOEXCEPT ->
        basic_static_string&
{
  const auto curr_data = data();
  const auto curr_size = size();
  BOOST_STATIC_STRING_THROW_IF(
    pos > curr_size, std::out_of_range{"pos > size()"});
  BOOST_STATIC_STRING_THROW_IF(
    curr_size - (std::min)(n1, curr_size - pos) >= max_size() - n2,
    std::length_error{"replaced string exceeds max_size()"});
  if (pos + n1 >= curr_size)
    n1 = curr_size - pos;
  Traits::move(&curr_data[pos + n2], &curr_data[pos + n1], curr_size - pos - n1 + 1);
  Traits::copy(&curr_data[pos], s, n2);
  this->set_size(curr_size + (n2 - n1));
  return *this;
}

// string

static_string<std::numeric_limits<int>::digits10 + 1>
inline
to_static_string(int value) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  return detail::to_static_string_int_impl<
    std::numeric_limits<int>::digits10 + 1>(value);
}

static_string<std::numeric_limits<long>::digits10 + 1>
inline
to_static_string(long value) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  return detail::to_static_string_int_impl<
    std::numeric_limits<long>::digits10 + 1>(value);
}

static_string<std::numeric_limits<long long>::digits10 + 1>
inline
to_static_string(long long value) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  return detail::to_static_string_int_impl<
    std::numeric_limits<long long>::digits10 + 1>(value);
}

static_string<std::numeric_limits<unsigned int>::digits10 + 1>
inline
to_static_string(unsigned int value) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  return detail::to_static_string_int_impl<
    std::numeric_limits<unsigned int>::digits10 + 1>(value);
}

static_string<std::numeric_limits<unsigned long>::digits10 + 1>
inline
to_static_string(unsigned long value) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  return detail::to_static_string_int_impl<
    std::numeric_limits<unsigned long>::digits10 + 1>(value);
}

static_string<std::numeric_limits<unsigned long long>::digits10 + 1>
inline
to_static_string(unsigned long long value) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  return detail::to_static_string_int_impl<
    std::numeric_limits<unsigned long long>::digits10 + 1>(value);
}

static_string<std::numeric_limits<float>::max_digits10 + 1>
inline
to_static_string(float value) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  return detail::to_static_string_float_impl<
    std::numeric_limits<float>::max_digits10 + 1>(value);
}

static_string<std::numeric_limits<double>::max_digits10 + 1>
inline
to_static_string(double value) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  return detail::to_static_string_float_impl<
    std::numeric_limits<double>::max_digits10 + 1>(value);
}

static_string<std::numeric_limits<long double>::max_digits10 + 1>
inline
to_static_string(long double value) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  return detail::to_static_string_float_impl<
    std::numeric_limits<long double>::max_digits10 + 1>(value);
}

// wstring

static_wstring<std::numeric_limits<int>::digits10 + 1>
inline
to_static_wstring(int value) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  return detail::to_static_wstring_int_impl<
    std::numeric_limits<int>::digits10 + 1>(value);
}

static_wstring<std::numeric_limits<long>::digits10 + 1>
inline
to_static_wstring(long value) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  return detail::to_static_wstring_int_impl<
    std::numeric_limits<long>::digits10 + 1>(value);
}

static_wstring<std::numeric_limits<long long>::digits10 + 1>
inline
to_static_wstring(long long value) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  return detail::to_static_wstring_int_impl<
    std::numeric_limits<long long>::digits10 + 1>(value);
}

static_wstring<std::numeric_limits<unsigned int>::digits10 + 1>
inline
to_static_wstring(unsigned int value) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  return detail::to_static_wstring_int_impl<
    std::numeric_limits<unsigned int>::digits10 + 1>(value);
}

static_wstring<std::numeric_limits<unsigned long>::digits10 + 1>
inline
to_static_wstring(unsigned long value) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  return detail::to_static_wstring_int_impl<
    std::numeric_limits<unsigned long>::digits10 + 1>(value);
}

static_wstring<std::numeric_limits<unsigned long long>::digits10 + 1>
inline
to_static_wstring(unsigned long long value) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  return detail::to_static_wstring_int_impl<
    std::numeric_limits<unsigned long long>::digits10 + 1>(value);
}

static_wstring<std::numeric_limits<float>::max_digits10 + 1>
inline
to_static_wstring(float value) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  return detail::to_static_wstring_float_impl<
    std::numeric_limits<float>::max_digits10 + 1>(value);
}

static_wstring<std::numeric_limits<double>::max_digits10 + 1>
inline
to_static_wstring(double value) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  return detail::to_static_wstring_float_impl<
    std::numeric_limits<double>::max_digits10 + 1>(value);
}

static_wstring<std::numeric_limits<long double>::max_digits10 + 1>
inline
to_static_wstring(long double value) BOOST_STATIC_STRING_COND_NOEXCEPT
{
  return detail::to_static_wstring_float_impl<
    std::numeric_limits<long double>::max_digits10 + 1>(value);
}

} // static_string
} // boost

#endif
