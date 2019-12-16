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
basic_static_string(size_type count, CharT ch)
{
    assign(count, ch);
}

template<std::size_t N, typename CharT, typename Traits>
template<std::size_t M>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
basic_static_string<N, CharT, Traits>::
basic_static_string(basic_static_string<M, CharT, Traits> const& other,
    size_type pos)
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
    size_type count)
{
    assign(other, pos, count);
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
basic_static_string<N, CharT, Traits>::
basic_static_string(CharT const* s, size_type count)
{
    assign(s, count);
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
basic_static_string<N, CharT, Traits>::
basic_static_string(CharT const* s)
{
    auto const count = Traits::length(s);
    if(count > max_size())
        BOOST_STATIC_STRING_THROW(std::length_error{
            "count > max_size()"});
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
            iterator>::type*)
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
    basic_static_string<M, CharT, Traits> const& s)
{
    assign(s);
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
basic_static_string<N, CharT, Traits>::
basic_static_string(std::initializer_list<CharT> init)
{
    assign(init.begin(), init.end());
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
basic_static_string<N, CharT, Traits>::
basic_static_string(string_view_type sv)
{
    assign(sv);
}

template<std::size_t N, typename CharT, typename Traits>
template<class T, class>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
basic_static_string<N, CharT, Traits>::
basic_static_string(T const& t, size_type pos, size_type n)
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
    CharT ch) ->
        basic_static_string&
{
    if(count > max_size())
        BOOST_STATIC_STRING_THROW(std::length_error{
            "count > max_size()"});
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
    size_type count) ->
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
    size_type count) ->
        basic_static_string&
{
    if(count > max_size())
        BOOST_STATIC_STRING_THROW(std::length_error{
            "count > max_size()"});
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
    InputIterator last) ->
        typename std::enable_if<
            detail::is_input_iterator<InputIterator>::value,
                basic_static_string&>::type
{
    std::size_t const n = std::distance(first, last);
    if(n > max_size())
        BOOST_STATIC_STRING_THROW(std::length_error{
            "n > max_size()"});
    this->set_size(n);
    for(auto it = data(); first != last; ++it, ++first)
        Traits::assign(*it, *first);
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
at(size_type pos) ->
    reference
{
    if(pos >= size())
        BOOST_STATIC_STRING_THROW(std::out_of_range{
            "pos >= size()"});
    return data()[pos];
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
at(size_type pos) const ->
    const_reference
{
    if(pos >= size())
        BOOST_STATIC_STRING_THROW(std::out_of_range{
            "pos >= size()"});
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
reserve(std::size_t n)
{
    if(n > max_size())
        BOOST_STATIC_STRING_THROW(std::length_error{
            "n > max_size()"});
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
    size_type count,
    CharT ch) ->
        basic_static_string&
{
    if(index > size())
        BOOST_STATIC_STRING_THROW(std::out_of_range{
            "index > size()"});
    insert(begin() + index, count, ch);
    return *this;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
insert(
    size_type index,
    CharT const* s,
    size_type count) ->
        basic_static_string&
{
    const auto curr_size = size();
    const auto curr_data = data();
    if(index > curr_size)
        BOOST_STATIC_STRING_THROW(std::out_of_range{
            "index > size()"});
    if(count > max_size() - curr_size)
        BOOST_STATIC_STRING_THROW(std::length_error{
            "count > max_size() - size()"});
    const bool inside = s <= &curr_data[curr_size] && s >= curr_data;
    if (!inside || (inside && ((s - curr_data) + count <= index)))
    {
      Traits::move(&curr_data[index + count], &curr_data[index], curr_size - index + 1);
      Traits::copy(&curr_data[index], s, count);
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
    return *this;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
insert(
    const_iterator pos,
    size_type count,
    CharT ch) ->
        iterator
{
    const auto curr_size = size();
    const auto curr_data = data();
    if(count > max_size() - curr_size)
        BOOST_STATIC_STRING_THROW(std::length_error{
            "count() > max_size() - size()"});
    auto const index = pos - curr_data;
    Traits::move(&curr_data[index + count], &curr_data[index], curr_size - index);
    this->set_size(curr_size + count);
    Traits::assign(&curr_data[index], count, ch);
    term();
    return &curr_data[index];
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
                InputIterator>::value, iterator>::type
{
  const size_type index = pos - begin();
  return insert(index, &*first, std::distance(first, last)).begin() + index;
}

template<std::size_t N, typename CharT, typename Traits>
template<class T>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
insert(
    size_type index,
    T const & t) ->
        typename std::enable_if<
            std::is_convertible<
                T const&, string_view_type>::value &&
            ! std::is_convertible<
                T const&, CharT const*>::value, basic_static_string&
                    >::type
{
    return insert(index, t, 0, npos);
}

template<std::size_t N, typename CharT, typename Traits>
template<class T>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
insert(
    size_type index,
    T const & t,
    size_type index_str,
    size_type count) ->
        typename std::enable_if<
            std::is_convertible<
                T const&, string_view_type>::value &&
            ! std::is_convertible<
                T const&, CharT const*>::value, basic_static_string&
                    >::type
{
    auto const s =
        string_view_type(t).substr(index_str, count);
    return insert(index, s.data(), s.size());
}

//------------------------------------------------------------------------------

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
erase(
    size_type index,
    size_type count) ->
        basic_static_string&
{
    const auto curr_size = size();
    const auto curr_data = data();
    if(index > curr_size)
        BOOST_STATIC_STRING_THROW(std::out_of_range{
            "index > size()"});
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
    const_iterator pos) ->
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
    const_iterator last) ->
        iterator
{
    erase(first - begin(),
        std::distance(first, last));
    return begin() + (first - begin());
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
void
basic_static_string<N, CharT, Traits>::
push_back(
    CharT ch)
{
    const auto curr_size = size();
    if(curr_size >= max_size())
        BOOST_STATIC_STRING_THROW(std::length_error{
            "size() >= max_size()"});
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
    size_type count) ->
        basic_static_string&
{
    const auto curr_size = size();
    if(count > max_size() - curr_size)
        BOOST_STATIC_STRING_THROW(std::length_error{
            "count > max_size() - size()"});
    Traits::copy(&data()[curr_size], s, count);
    this->set_size(curr_size + count);
    term();
    return *this;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
substr(size_type pos, size_type count) const ->
    basic_static_string
{
    if(pos > size())
        BOOST_STATIC_STRING_THROW(std::out_of_range{
            "pos > size()"});
    return {&data()[pos], (std::min)(count, size() - pos)};
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
subview(size_type pos, size_type count) const ->
    string_view_type
{
  if (pos > size())
    BOOST_STATIC_STRING_THROW(std::out_of_range{
        "pos > size()"});
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
resize(std::size_t n)
{
    const auto curr_size = size();
    if(n > max_size())
        BOOST_STATIC_STRING_THROW(std::length_error{
            "n > max_size()"});
    if(n > curr_size)
        Traits::assign(&data()[curr_size], n - curr_size, CharT{});
    this->set_size(n);
    term();
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
void
basic_static_string<N, CharT, Traits>::
resize(std::size_t n, CharT c)
{
    const auto curr_size = size();
    if(n > max_size())
        BOOST_STATIC_STRING_THROW(std::length_error{
            "n > max_size()"});
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
swap(basic_static_string<M, CharT, Traits>& s)
{
    const auto curr_size = size();
    if(curr_size > s.max_size())
        BOOST_STATIC_STRING_THROW(std::length_error{
            "size() > s.max_size()"});
    if(s.size() > max_size())
        BOOST_STATIC_STRING_THROW(std::length_error{
            "s.size() > max_size()"});
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
    size_type n2) -> basic_static_string<N, CharT, Traits>&
{
    const auto curr_size = size();
    const auto curr_data = data();
    if (pos > curr_size)
      BOOST_STATIC_STRING_THROW(std::out_of_range{
             "pos > size()"});
    if (curr_size - (std::min)(n1, curr_size - pos) >= max_size() - n2)
      BOOST_STATIC_STRING_THROW(std::length_error{
             "replaced string exceeds max_size()"});
    if (pos + n1 >= curr_size)
      n1 = curr_size - pos;
    const bool inside = s <= &curr_data[curr_size] && s >= curr_data;
    if (inside && size_type(s - curr_data) == pos && n1 == n2)
      return *this;
    if (!inside || (inside && ((s - curr_data) + n2 <= pos)))
    {
      // source outside
      Traits::move(&curr_data[pos + n2], &curr_data[pos + n1], curr_size - pos - n1 + 1);
      Traits::copy(&curr_data[pos], s, n2);
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
BOOST_STATIC_STRING_CPP14_CONSTEXPR
auto
basic_static_string<N, CharT, Traits>::
replace(
    size_type pos,
    size_type n1,
    size_type n2,
    CharT c) -> basic_static_string<N, CharT, Traits> &
{
    const auto curr_size = size();
    const auto curr_data = data();
    if (pos > curr_size)
      BOOST_STATIC_STRING_THROW(std::out_of_range{
             "pos > size()"});
    if (curr_size - (std::min)(n1, curr_size - pos) >= max_size() - n2)
      BOOST_STATIC_STRING_THROW(std::length_error{
             "replaced string exceeds max_size()"});
    if (pos + n1 >= curr_size)
      n1 = curr_size - pos;
    Traits::move(&curr_data[pos + n2], &curr_data[pos + n1], curr_size - pos - n1 + 1);
    Traits::assign(&curr_data[pos], n2, c);
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
    const auto res = std::search(&data()[pos], &data()[curr_size], s, &s[n], Traits::eq);
    return res == end() ? npos : std::distance(data(), res);
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
        return std::distance(curr_data, sub);
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
    const auto res = std::find_first_of(&curr_data[pos], &curr_data[size()], s, &s[n], Traits::eq);
    return res == end() ? npos : std::distance(curr_data, res);
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
    const auto res = std::find_first_of(rbegin() + pos, rend(), s, &s[n], Traits::eq);
    return res == rend() ? npos : curr_size - 1 - std::distance(rbegin(), res);
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
    return res == end() ? npos : std::distance(data(), res);
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
    return res == rend() ? npos : curr_size - 1 - std::distance(rbegin(), res);
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
assign_char(CharT, std::false_type) ->
    basic_static_string&
{
    BOOST_STATIC_STRING_THROW(std::length_error{
        "max_size() == 0"});
}

template<class Integer, class>
inline
basic_static_string<detail::max_digits(sizeof(Integer)), char>
to_static_string(Integer x)
{
    using CharT = char;
    using Traits = std::char_traits<CharT>;
    BOOST_STATIC_STRING_STATIC_ASSERT(std::is_integral<Integer>::value, "Integer must be an integral type");
    char buf[detail::max_digits(sizeof(Integer))];
    auto last = buf + sizeof(buf);
    auto it = detail::raw_to_string<
        CharT, Integer, Traits>(last, sizeof(buf), x);
    basic_static_string<detail::max_digits(sizeof(Integer)), char> s;
    s.resize(static_cast<std::size_t>(last - it));
    auto p = s.data();
    while(it < last)
        Traits::assign(*p++, *it++);
    return s;
}

} // static_string
} // boost

#endif
