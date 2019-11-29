//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
// Copyright (c) 2019 Krystian Stasiowski (sdkrystian at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/fixed_string
//

#ifndef BOOST_FIXED_STRING_IMPL_FIXED_STRING_HPP
#define BOOST_FIXED_STRING_IMPL_FIXED_STRING_HPP

#include <boost/fixed_string/config.hpp>
#include <boost/fixed_string/detail/fixed_string.hpp>

namespace boost {
namespace fixed_string {

//------------------------------------------------------------------------------
//
// Construction
//
//------------------------------------------------------------------------------

template<std::size_t N, typename CharT, typename Traits>
fixed_string<N, CharT, Traits>::
fixed_string()
{
    this->set_size(0);
    term();
}

template<std::size_t N, typename CharT, typename Traits>
fixed_string<N, CharT, Traits>::
fixed_string(size_type count, CharT ch)
{
    assign(count, ch);
}

template<std::size_t N, typename CharT, typename Traits>
template<std::size_t M>
fixed_string<N, CharT, Traits>::
fixed_string(fixed_string<M, CharT, Traits> const& other,
    size_type pos)
{
    assign(other, pos);
}

template<std::size_t N, typename CharT, typename Traits>
template<std::size_t M>
fixed_string<N, CharT, Traits>::
fixed_string(
    fixed_string<M, CharT, Traits> const& other,
    size_type pos,
    size_type count)
{
    assign(other, pos, count);
}

template<std::size_t N, typename CharT, typename Traits>
fixed_string<N, CharT, Traits>::
fixed_string(CharT const* s, size_type count)
{
    assign(s, count);
}

template<std::size_t N, typename CharT, typename Traits>
fixed_string<N, CharT, Traits>::
fixed_string(CharT const* s)
{
    auto const count = Traits::length(s);
    if(count > max_size())
        BOOST_FIXED_STRING_THROW(std::length_error{
            "count > max_size()"});
    this->set_size(count);
    Traits::copy(data(), s, size() + 1);
}

template<std::size_t N, typename CharT, typename Traits>
template<class InputIterator>
fixed_string<N, CharT, Traits>::
fixed_string(
    InputIterator first,
    InputIterator last,
    typename std::enable_if<
        detail::is_input_iterator<InputIterator>::value,
            iterator>::type*)
{
    assign(first, last);
}

template<std::size_t N, typename CharT, typename Traits>
fixed_string<N, CharT, Traits>::
fixed_string(
    fixed_string const& s)
{
    assign(s);
}

template<std::size_t N, typename CharT, typename Traits>
template<std::size_t M>
fixed_string<N, CharT, Traits>::
fixed_string(
    fixed_string<M, CharT, Traits> const& s)
{
    assign(s);
}

template<std::size_t N, typename CharT, typename Traits>
fixed_string<N, CharT, Traits>::
fixed_string(std::initializer_list<CharT> init)
{
    assign(init.begin(), init.end());
}

template<std::size_t N, typename CharT, typename Traits>
fixed_string<N, CharT, Traits>::
fixed_string(string_view_type sv)
{
    assign(sv);
}

template<std::size_t N, typename CharT, typename Traits>
template<class T, class>
fixed_string<N, CharT, Traits>::
fixed_string(T const& t, size_type pos, size_type n)
{
    assign(t, pos, n);
}

//------------------------------------------------------------------------------
//
// Assignment
//
//------------------------------------------------------------------------------

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
assign(
    size_type count,
    CharT ch) ->
        fixed_string&
{
    if(count > max_size())
        BOOST_FIXED_STRING_THROW(std::length_error{
            "count > max_size()"});
    this->set_size(count);
    Traits::assign(data(), size(), ch);
    term();
    return *this;
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
assign(
    fixed_string const& s) noexcept ->
        fixed_string&
{
    if(this == &s)
        return *this;
    this->set_size(s.size());
    auto const n = size() + 1;
    // VFALCO This informs the static analyzer
    //BOOST_BEAST_ASSUME(n != 0);
    Traits::copy(data(), &s.data()[0], n);
    return *this;
}

template<std::size_t N, typename CharT, typename Traits>
template<std::size_t M>
auto
fixed_string<N, CharT, Traits>::
assign(
    fixed_string<M, CharT, Traits> const& s,
    size_type pos,
    size_type count) ->
    fixed_string&
{
    auto const ss = s.substr(pos, count);
    return assign(ss.data(), ss.size());
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
assign(
    CharT const* s,
    size_type count) ->
        fixed_string&
{
    if(count > max_size())
        BOOST_FIXED_STRING_THROW(std::length_error{
            "count > max_size()"});
    this->set_size(count);
    // check for overlap, then move if needed
    if (s <= &data()[size()] && s >= data())
      Traits::move(data(), s, size());
    else
      Traits::copy(data(), s, size());
    term();
    return *this;
}

template<std::size_t N, typename CharT, typename Traits>
template<class InputIterator>
auto
fixed_string<N, CharT, Traits>::
assign(
    InputIterator first,
    InputIterator last) ->
        typename std::enable_if<
            detail::is_input_iterator<InputIterator>::value,
                fixed_string&>::type
{
    std::size_t const n = std::distance(first, last);
    if(n > max_size())
        BOOST_FIXED_STRING_THROW(std::length_error{
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
auto
fixed_string<N, CharT, Traits>::
at(size_type pos) ->
    reference
{
    if(pos >= size())
        BOOST_FIXED_STRING_THROW(std::out_of_range{
            "pos >= size()"});
    return data()[pos];
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
at(size_type pos) const ->
    const_reference
{
    if(pos >= size())
        BOOST_FIXED_STRING_THROW(std::out_of_range{
            "pos >= size()"});
    return data()[pos];
}

//------------------------------------------------------------------------------
//
// Capacity
//
//------------------------------------------------------------------------------

template<std::size_t N, typename CharT, typename Traits>
void
fixed_string<N, CharT, Traits>::
reserve(std::size_t n)
{
    if(n > max_size())
        BOOST_FIXED_STRING_THROW(std::length_error{
            "n > max_size()"});
}

//------------------------------------------------------------------------------
//
// Operations
//
//------------------------------------------------------------------------------

template<std::size_t N, typename CharT, typename Traits>
void
fixed_string<N, CharT, Traits>::
clear()
{
    this->set_size(0);
    term();
}

//------------------------------------------------------------------------------

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
insert(
    size_type index,
    size_type count,
    CharT ch) ->
        fixed_string&
{
    if(index > size())
        BOOST_FIXED_STRING_THROW(std::out_of_range{
            "index > size()"});
    insert(begin() + index, count, ch);
    return *this;
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
insert(
    size_type index,
    CharT const* s,
    size_type count) ->
        fixed_string&
{
    if(index > size())
        BOOST_FIXED_STRING_THROW(std::out_of_range{
            "index > size()"});
    if(size() + count > max_size())
        BOOST_FIXED_STRING_THROW(std::length_error{
            "size() + count > max_size()"});
    const bool inside = s <= &data()[size()] && s >= data();
    if (!inside || (inside && ((s - data()) + count <= index)))
    {
      Traits::move(&data()[index + count], &data()[index], size() - index + 1);
      Traits::copy(&data()[index], s, count);
    }
    else
    {
      const size_type offset = s - data();
      Traits::move(&data()[index + count], &data()[index], size() - index + 1);
      if (offset < index)
      {
        const size_type diff = index - offset;
        Traits::copy(&data()[index], &data()[offset], diff);
        Traits::copy(&data()[index + diff], &data()[index + count], count - diff);
      }
      else
      {
        Traits::copy(&data()[index], &data()[offset + count], count);
      }
    }
    this->set_size(size() + count);
    return *this;
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
insert(
    const_iterator pos,
    size_type count,
    CharT ch) ->
        iterator
{
    if(size() + count > max_size())
        BOOST_FIXED_STRING_THROW(std::length_error{
            "size() + count() > max_size()"});
    auto const index = pos - data();
    Traits::move(&data()[index + count], &data()[index], size() - index);
    this->set_size(size() + count);
    Traits::assign(&data()[index], count, ch);
    term();
    return &data()[index];
}

template<std::size_t N, typename CharT, typename Traits>
template<class InputIterator>
auto
fixed_string<N, CharT, Traits>::
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
auto
fixed_string<N, CharT, Traits>::
insert(
    size_type index,
    T const & t) ->
        typename std::enable_if<
            std::is_convertible<
                T const&, string_view_type>::value &&
            ! std::is_convertible<
                T const&, CharT const*>::value, fixed_string&
                    >::type
{
    return insert(index, t, 0, npos);
}

template<std::size_t N, typename CharT, typename Traits>
template<class T>
auto
fixed_string<N, CharT, Traits>::
insert(
    size_type index,
    T const & t,
    size_type index_str,
    size_type count) ->
        typename std::enable_if<
            std::is_convertible<
                T const&, string_view_type>::value &&
            ! std::is_convertible<
                T const&, CharT const*>::value, fixed_string&
                    >::type
{
    auto const s =
        string_view_type(t).substr(index_str, count);
    return insert(index, s.data(), s.size());
}

//------------------------------------------------------------------------------

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
erase(
    size_type index,
    size_type count) ->
        fixed_string&
{
    if(index > size())
        BOOST_FIXED_STRING_THROW(std::out_of_range{
            "index > size()"});
    auto const n = (std::min)(count, size() - index);
    Traits::move(&data()[index], &data()[index + n], size() - (index + n) + 1);
    this->set_size(size() - n);
    return *this;
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
erase(
    const_iterator pos) ->
        iterator
{
    erase(pos - begin(), 1);
    return begin() + (pos - begin());
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
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
void
fixed_string<N, CharT, Traits>::
push_back(
    CharT ch)
{
    if(size() >= max_size())
        BOOST_FIXED_STRING_THROW(std::length_error{
            "size() >= max_size()"});
    Traits::assign(data()[size()], ch);
    this->set_size(size() + 1);
    term();
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
append(
    CharT const* s,
    size_type count) ->
        fixed_string&
{
    if(size() + count > max_size())
        BOOST_FIXED_STRING_THROW(std::length_error{
            "size() + count > max_size()"});
    Traits::move(&data()[size() + count], &data()[size()], size() - size());
    Traits::copy(&data()[size()], s, count);
    this->set_size(size() + count);
    term();
    return *this;
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
substr(size_type pos, size_type count) const ->
    string_view
{
    if(pos > size())
        BOOST_FIXED_STRING_THROW(std::out_of_range{
            "pos > size()"});
    return {&data()[pos], (std::min)(count, size() - pos)};
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
copy(CharT* dest, size_type count, size_type pos) const ->
    size_type
{
    auto const s = substr(pos, count);
    Traits::copy(dest, s.data(), s.size());
    return s.size();
}

template<std::size_t N, typename CharT, typename Traits>
void
fixed_string<N, CharT, Traits>::
resize(std::size_t n)
{
    if(n > max_size())
        BOOST_FIXED_STRING_THROW(std::length_error{
            "n > max_size()"});
    if(n > size())
        Traits::assign(&data()[size()], n - size(), CharT{});
    this->set_size(n);
    term();
}

template<std::size_t N, typename CharT, typename Traits>
void
fixed_string<N, CharT, Traits>::
resize(std::size_t n, CharT c)
{
    if(n > max_size())
        BOOST_FIXED_STRING_THROW(std::length_error{
            "n > max_size()"});
    if(n > size())
        Traits::assign(&data()[size()], n - size(), c);
    this->set_size(n);
    term();
}

template<std::size_t N, typename CharT, typename Traits>
void
fixed_string<N, CharT, Traits>::
swap(fixed_string& s)
{
    fixed_string tmp(s);
    s.set_size(size());
    Traits::copy(&s.data()[0], data(), size() + 1);
    this->set_size(tmp.size());
    Traits::copy(data(), &tmp.data()[0], size() + 1);
}

template<std::size_t N, typename CharT, typename Traits>
template<std::size_t M>
void
fixed_string<N, CharT, Traits>::
swap(fixed_string<M, CharT, Traits>& s)
{
    if(size() > s.max_size())
        BOOST_FIXED_STRING_THROW(std::length_error{
            "size() > s.max_size()"});
    if(s.size() > max_size())
        BOOST_FIXED_STRING_THROW(std::length_error{
            "s.size() > max_size()"});
    fixed_string tmp(s);
    s.set_size(size());
    Traits::copy(&s.data()[0], data(), size() + 1);
    this->set_size(tmp.size());
    Traits::copy(data(), &tmp.data()[0], size() + 1);
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
replace(
    size_type pos,
    size_type n1,
    const CharT* s,
    size_type n2) -> fixed_string<N, CharT, Traits>&
{
  if (pos > size())
    BOOST_FIXED_STRING_THROW(std::out_of_range{
           "pos > size()"});
  if (size() - (std::min)(n1, size() - pos) >= max_size() - n2)
    BOOST_FIXED_STRING_THROW(std::length_error{
           "replaced string exceeds max_size()"});
  if (pos + n1 >= size())
    n1 = size() - pos;
  const bool inside = s <= &data()[size()] && s >= data();
  if (inside && size_type(s - data()) == pos && n1 == n2)
    return *this;
  if (!inside || (inside && ((s - data()) + n2 <= pos)))
  {
    // source outside
    Traits::move(&data()[pos + n2], &data()[pos + n1], size() - pos - n1 + 1);
    Traits::copy(&data()[pos], s, n2);
  }
  else
  {
    // source inside
    const size_type offset = s - data();
    if (n2 >= n1)
    {
      // grow/unchanged
      // shift all right of splice point by n2 - n1 to the right
      Traits::move(&data()[pos + n2], &data()[pos + n1], size() - pos - n1 + 1);
      const size_type diff = offset <= pos + n1 ? (std::min)((pos + n1) - offset, n2) : 0;
      // copy all before splice point
      Traits::move(&data()[pos], &data()[offset], diff);
      // copy all after splice point
      Traits::move(&data()[pos + diff], &data()[offset + (n2 - n1) + diff], n2 - diff);
    }
    else
    {
      // shrink
      // copy all elements into place
      Traits::move(&data()[pos], &data()[offset], n2);
      // shift all elements after splice point left
      Traits::move(&data()[pos + n2], &data()[pos + n1], size() - pos - n1 + 1);
    }
  }
  this->set_size(size() + (n2 - n1));
  return *this;
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
replace(
    size_type pos,
    size_type n1,
    size_type n2,
    CharT c) -> fixed_string<N, CharT, Traits> &
{
  if (pos > size())
    BOOST_FIXED_STRING_THROW(std::out_of_range{
           "pos > size()"});
  if (size() - (std::min)(n1, size() - pos) >= max_size() - n2)
    BOOST_FIXED_STRING_THROW(std::length_error{
           "replaced string exceeds max_size()"});
  if (pos + n1 >= size())
    n1 = size() - pos;
  Traits::move(&data()[pos + n2], &data()[pos + n1], size() - pos - n1 + 1);
  Traits::assign(&data()[pos], n2, c);
  this->set_size(size() + (n2 - n1));
  return *this;
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
find(
    const CharT* s,
    size_type pos,
    size_type n) const -> 
        size_type
{
  if (pos > size() || n > size() - pos)
    return npos;
  if (!n)
    return pos;
  const auto res = std::search(&data()[pos], &data()[size()], s, &s[n], Traits::eq);
  return res == end() ? npos : std::distance(data(), res);
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
rfind(
    const CharT* s,
    size_type pos,
    size_type n) const -> 
        size_type
{
  if (size() < n)
    return npos;
  if (pos > size() - n)
    pos = size() - n;
  if (!n)
    return pos;
  for (auto sub = &data()[pos]; sub >= data(); --sub)
    if (!Traits::compare(sub, s, n))
      return std::distance(data(), sub);
  return npos;
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
find_first_of(
    const CharT* s,
    size_type pos,
    size_type n) const -> 
        size_type
{
  if (pos >= size() || !n)
    return npos;
  const auto res = std::find_first_of(&data()[pos], &data()[size()], s, &s[n], Traits::eq);
  return res == end() ? npos : std::distance(data(), res);
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
find_last_of(
    const CharT* s,
    size_type pos,
    size_type n) const -> 
        size_type
{
  if (!n)
    return npos;
  if (pos >= size())
    pos = 0;
  else
    pos = size() - (pos + 1);
  const auto res = std::find_first_of(rbegin() + pos, rend(), s, &s[n], Traits::eq);
  return res == rend() ? npos : size() - 1 - std::distance(rbegin(), res);
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
find_first_not_of(
    const CharT* s,
    size_type pos,
    size_type n) const -> 
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
auto
fixed_string<N, CharT, Traits>::
find_last_not_of(
    const CharT* s,
    size_type pos,
    size_type n) const -> 
        size_type
{
  if (pos >= size())
    pos = size() - 1;
  if (!n)
    return pos;
  pos = size() - (pos + 1);
  const auto res = detail::find_not_of<Traits>(rbegin() + pos, rend(), s, n);
  return res == rend() ? npos : size() - 1 - std::distance(rbegin(), res);
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
assign_char(CharT ch, std::true_type) ->
    fixed_string&
{
    this->set_size(1);
    Traits::assign(data()[0], ch);
    term();
    return *this;
}

template<std::size_t N, typename CharT, typename Traits>
auto
fixed_string<N, CharT, Traits>::
assign_char(CharT, std::false_type) ->
    fixed_string&
{
    BOOST_FIXED_STRING_THROW(std::length_error{
        "max_size() == 0"});
}

template<class Integer, class>
inline
fixed_string<detail::max_digits(sizeof(Integer))>
to_fixed_string(Integer x)
{
    using CharT = char;
    using Traits = std::char_traits<CharT>;
    BOOST_FIXED_STRING_STATIC_ASSERT(std::is_integral<Integer>::value, "Integer must be an integral type");
    char buf[detail::max_digits(sizeof(Integer))];
    auto last = buf + sizeof(buf);
    auto it = detail::raw_to_string<
        CharT, Integer, Traits>(last, sizeof(buf), x);
    fixed_string<detail::max_digits(sizeof(Integer))> s;
    s.resize(static_cast<std::size_t>(last - it));
    auto p = s.data();
    while(it < last)
        Traits::assign(*p++, *it++);
    return s;
}

} // fixed_string
} // boost

#endif
