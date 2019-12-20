//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
// Copyright (c) 2019 Krystian Stasiowski (sdkrystian at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/static_string
//

#ifndef BOOST_STATIC_STRING_STATIC_STRING_HPP
#define BOOST_STATIC_STRING_STATIC_STRING_HPP

#include <boost/static_string/config.hpp>
#include <boost/static_string/detail/static_string.hpp>
#include <algorithm>
#include <cstdint>
#include <initializer_list>
#include <iosfwd>
#include <type_traits>

namespace boost {
namespace static_string {

/** A fixed-capacity string.

    These objects behave like `std::string` except that the storage
    is not dynamically allocated but rather fixed in size, and
    stored in the object itself.

    These strings offer performance advantages when an algorithm
    can execute with a reasonable upper limit on the size of a value.

    @see to_static_string 
*/
template<
    std::size_t N,
    typename CharT,
    typename Traits = std::char_traits<CharT>>
class basic_static_string 
#ifndef GENERATING_DOCUMENTATION
  : detail::optimization_base<N, CharT, Traits>
#endif
{
private:
    template<std::size_t, class, class>
    friend class basic_static_string;

    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    void
    term() noexcept
    {
        this->term_impl();
    }

    using base = detail::optimization_base<N, CharT, Traits>;

public:
    //--------------------------------------------------------------------------
    //
    // Member types
    //
    //--------------------------------------------------------------------------

    using traits_type       = Traits;
    using value_type        = typename Traits::char_type;
    using size_type         = std::size_t;
    using difference_type   = std::ptrdiff_t;
    using pointer           = value_type*;
    using reference         = value_type&;
    using const_pointer     = value_type const*;
    using const_reference   = value_type const&;
    using iterator          = value_type*;
    using const_iterator    = value_type const*;
    
    using reverse_iterator =
        std::reverse_iterator<iterator>;
    
    using const_reverse_iterator =
        std::reverse_iterator<const_iterator>;

    /// The type of `string_view` returned by the interface
    using string_view_type =
        basic_string_view<CharT, Traits>;

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
    basic_static_string() noexcept;

    /** Construct a `basic_static_string`.
    
        Construct the string with `count` copies of character `ch`.
    
        The behavior is undefined if `count >= npos`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string(
        size_type count,
        CharT ch);

    /** Construct a `basic_static_string`.
        
        Construct with a substring (pos, other.size()) of `other`.
    */
    template<std::size_t M>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string(
        basic_static_string<M, CharT, Traits> const& other,
        size_type pos);

    /** Construct a `basic_static_string`.
    
        Construct with a substring (pos, count) of `other`.
    */
    template<std::size_t M>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string(
        basic_static_string<M, CharT, Traits> const& other,
        size_type pos,
        size_type count);

    /** Construct a `basic_static_string`.
        
        Construct with the first `count` characters of `s`, including nulls.
     */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string(
        CharT const* s,
        size_type count);

    /** Construct a `basic_static_string`.
        
        Construct from a null terminated string.
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string(
        CharT const* s);

    /** Construct a `basic_static_string`.
    
        Construct from a range of characters
    */
    template<class InputIterator>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string(
        InputIterator first,
        InputIterator last
    #ifndef GENERATING_DOCUMENTATION
        , typename std::enable_if<
            detail::is_input_iterator<InputIterator>::value,
                iterator>::type* = 0
    #endif
    );

    /** Construct a `basic_static_string`.
        
        Copy constructor.
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string(
        basic_static_string const& other) noexcept;

    /** Construct a `basic_static_string`.
        
        Copy constructor.
    */
    template<std::size_t M>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string(
        basic_static_string<M, CharT, Traits> const& other);

    /** Construct a `basic_static_string`.
        
        Construct from an initializer list
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string(
        std::initializer_list<CharT> init);

    /** Construct a `basic_static_string`.
    
        Construct from a `string_view`
    */
    explicit
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string(
        string_view_type sv);

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
        T const& t,
        size_type pos,
        size_type n);

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
    operator=(
        basic_static_string const& s) noexcept
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
    operator=(
        basic_static_string<M, CharT, Traits> const& s)
    {
        return assign(s);
    }

    /** Assign to the string.
    
        Replace the contents with those of the null-terminated string `s`

        @throw std::length_error if `Traits::length(s) > max_size()`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    operator=(
        CharT const* s)
    {
        return assign(s);
    }

    /** Assign to the string.
    
        Assign from single character.
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    operator=(
        CharT ch)
    {
        return assign_char(ch,
            std::integral_constant<bool, (N > 0)>{});
    }

    /** Assign to the string.

        Assign from initializer list.
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    operator=(
        std::initializer_list<CharT> ilist)
    {
        return assign(ilist);
    }

    /** Assign to the string.
    
        Assign from `string_view_type`.
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    operator=(
        string_view_type sv)
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
        CharT ch);

    /** Replace the contents.
    
        Replace the contents with a copy of another `basic_static_string`

        @return `*this`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    assign(
        basic_static_string const& s) noexcept;

    /** Replace the contents.
    
        Replace the contents with a copy of another `basic_static_string`

        @throw std::length_error if `s.size() > max_size()`
        @return `*this`
    */
    template<std::size_t M>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    assign(
        basic_static_string<M, CharT, Traits> const& s)
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
        basic_static_string<M, CharT, Traits> const& s,
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
        CharT const* s,
        size_type count);

    /** Replace the contents.
    
        Replace the contents with a copy of a null terminated string `s`

        @throw std::length_error if `Traits::length(s) > max_size()`
        @return `*this`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    assign(
        CharT const* s)
    {
        return assign(s, Traits::length(s));
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
    basic_static_string&
    assign(
        std::initializer_list<CharT> ilist)
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
        ! std::is_convertible<T, CharT const*>::value,
            basic_static_string&>::type
#endif
    assign(T const& t)
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
        ! std::is_convertible<T, CharT const*>::value,
            basic_static_string&>::type
#endif
    assign(
        T const& t,
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
    at(size_type pos);

    /** Access specified character with bounds checking.

        @throw std::out_of_range if `pos >= size()`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    const_reference
    at(size_type pos) const;

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
    CharT&
    front() noexcept
    {
        return data()[0];
    }

    /** Accesses the first character.

        Undefined behavior if `empty() == true`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    CharT const&
    front() const noexcept
    {
        return data()[0];
    }

    /** Accesses the last character.

        Undefined behavior if `empty() == true`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    CharT&
    back() noexcept
    {
        return data()[size()-1];
    }

    /** Accesses the last character.

        Undefined behavior if `empty() == true`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    CharT const&
    back() const noexcept
    {
        return data()[size()-1];
    }

    /// Returns a pointer to the first character of the string.
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    CharT*
    data() noexcept
    {
        return this->data_impl();
    }

    /// Returns a pointer to the first character of a string.
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    CharT const*
    data() const noexcept
    {
        return this->data_impl();
    }

    /// Returns a non-modifiable standard C character array version of the string.
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    CharT const*
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
    reserve(std::size_t n);

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
    shrink_to_fit() noexcept
    {
    }

    //--------------------------------------------------------------------------
    //
    // Operations
    //
    //--------------------------------------------------------------------------

    /// Clears the contents
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    void
    clear() noexcept;

    /** Insert into the string.
    
        Inserts `count` copies of character `ch` at the position `index`

        @throw std::out_of_range if `index > size()`
        @throw std::length_error if `count > max_size() - size()`
        @return `*this`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    insert(
        size_type index,
        size_type count,
        CharT ch);
    
    /** Insert into the string.
    
        Inserts null-terminated string pointed to by `s` at the position `index`

        @throw std::out_of_range if `index > size()`
        @throw std::length_error if `count > max_size() - size()`
        @return `*this`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    insert(
        size_type index,
        CharT const* s)
    {
        return insert(index, s, Traits::length(s));
    }

    /** Insert into the string.
    
        Inserts the characters in the range `(s, s+count)` at the position `index`

        The inserted string can contain null characters.

        @throw std::out_of_range if `index > size()`
        @throw std::length_error if `count > max_size() - size()`
        @return `*this`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    insert(
        size_type index,
        CharT const* s,
        size_type count);

    /** Insert into the string.
    
        Inserts the contents of string view `sv` at the position `index`

        The inserted string can contain null characters.

        @throw std::out_of_range if `index > size()`
        @throw std::length_error if `sv.size() > max_size() - size()`
        @return `*this`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    insert(
        size_type index,
        string_view_type sv)
    {
        return insert(index, sv.data(), sv.size());
    }

    /** Insert into the string.
    
        Inserts the string `sv.substr(index_str, count)` at the position `index`

        The inserted string can contain null characters.

        @throw std::out_of_range if `index > size() || index_str > sv.size()`
        @throw std::length_error if `sv.substr(index_str, count).size() > max_size() - size()`
        @return `*this`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    insert(
        size_type index,
        string_view_type sv,
        size_type index_str,
        size_type count = npos)
    {
        return insert(index, sv.substr(index_str, count));
    }

    /** Insert into the string.
    
        Inserts character `ch` before the character (if any) pointed by `pos`

        The inserted character can be null.

        @throw std::length_error if `1 > max_size() - size()`
        @return An iterator to the first inserted character or pos if no insertion took place
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    iterator
    insert(
        const_iterator pos,
        CharT ch)
    {
        return insert(pos, 1, ch);
    }

    /** Insert into the string.

        Inserts `count` copies of character `ch` before the character (if any) pointed by `pos`

        The inserted characters can be null.

        @throw std::length_error if `count > max_size() - size()`
        @return An iterator to the first inserted character or pos if no insertion took place
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    iterator
    insert(
        const_iterator pos,
        size_type count,
        CharT ch);

    /** Insert into the string.
    
        Inserts characters from the range `(first, last)` before the element (if any) pointed by `pos`

        The inserted string can contain null characters.
        This function does not participate in overload resolution if
        `InputIterator` does not satisfy <em>LegacyInputIterator</em>

        @throw std::length_error if `std::distance(first, last) > max_size() - size()`
        @return An iterator to the first inserted character or pos if no insertion took place
    */
    template<typename InputIterator>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
    iterator
#else
    typename std::enable_if<
        detail::is_input_iterator<InputIterator>::value,
            iterator>::type
#endif
    insert(
        const_iterator pos,
        InputIterator first,
        InputIterator last);

    /** Insert into the string.
    
        Inserts elements from initializer list `ilist` before the element (if any) pointed by `pos`

        The inserted string can contain null characters.

        @throw std::length_error if `ilist.size() > max_size() - size()`
        @return An iterator to the first inserted character or pos if no insertion took place
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    iterator
    insert(
        const_iterator pos,
        std::initializer_list<CharT> ilist)
    {
        return insert(pos, ilist.begin(), ilist.end());
    }

    /** Insert into the string.
    
        Inserts elements from `string_view_type{t}` at the position `index`

        The inserted string can contain null characters.
        This function participates in overload resolution if
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.

        @throw std::length_error if `string_view_type{t}.substr(index_str, count).size() > max_size() - size()`
        @return `*this`
    */
    template<typename T>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
    basic_static_string&
#else
    typename std::enable_if<
        std::is_convertible<
            T const&, string_view_type>::value &&
        ! std::is_convertible<
            T const&, CharT const*>::value,
                basic_static_string&>::type
#endif
    insert(
        size_type index,
        T const& t);

    /** Insert into the string.

        Inserts elements from `string_view_type{t}.substr(index_str, count)` at the position `index`

        The inserted string can contain null characters.
        This function participates in overload resolution if
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.

        @throw std::out_of_range if `index_str > string_view_type{t}.size()`
        @throw std::length_error if `string_view_type{t}.substr(index_str, count).size() > max_size() - size()`
        @return `*this`
    */
    template<typename T>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
    basic_static_string&
#else
    typename std::enable_if<
        std::is_convertible<T const&, string_view_type>::value &&
        ! std::is_convertible<T const&, CharT const*>::value,
            basic_static_string&>::type
#endif
    insert(
        size_type index,
        T const& t,
        size_type index_str,
        size_type count = npos);

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
    erase(
        const_iterator pos);

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
    push_back(
        CharT ch);

    /** Removes the last character from the string

        The behavior is undefined if the string is empty.
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    void
    pop_back() noexcept
    {
      BOOST_STATIC_STRING_ASSERT(size() > 0);
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
        CharT ch)
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
        CharT const* s,
        size_type count);

    /** Append to the string.
    
        Appends the null-terminated character string pointed to by `s`
    
        The length of the string is determined by the first
        null character using `Traits::length(s)`.

        @throw std::length_error if `Traits::length(s) > max_size() - size()`
        @return `*this`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    append(
        CharT const* s)
    {
        return append(s, Traits::length(s));
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
        std::initializer_list<CharT> ilist)
    {
        insert(end(), ilist);
        return *this;
    }

    /** Append to the string.
    
        Appends characters from `string_view_type{t}`

        The appended string can contain null characters.
        This function participates in overload resolution if
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.

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
            T const&, string_view_type>::value &&
        ! std::is_convertible<
            T const&, CharT const*>::value,
                basic_static_string&>::type
#endif
    append(
        T const& t)
    {
        return append(string_view_type{t});
    }

    /** Append to the string.
    
        Appends characters from `string_view_type{t}.substr{pos, count}`

        The appended string can contain null characters.
        This function participates in overload resolution if
        `T` is convertible to `string_view_type` and `T` is not
        convertible to `CharT const*`.

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
            T const&, string_view_type>::value &&
        ! std::is_convertible<
            T const&, CharT const*>::value,
                basic_static_string&>::type
#endif
    append(
        T const& t,
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
        basic_static_string<M, CharT, Traits> const& s)
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
        CharT ch)
    {
        push_back(ch);
        return *this;
    }

    /** Append to the string.
    
        Appends the null-terminated character string pointed to by `s`
    
        The length of the string is determined by the first
        null character using `Traits::length(s)`.

        @throw std::length_error if `Traits::length(s) > max_size() - size()`
        @return `*this`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    operator+=(
        CharT const* s)
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
        std::initializer_list<CharT> ilist)
    {
        return append(ilist);
    }

    /** Append to the string.
    
        Appends a copy of the characters from `string_view_type{t}`

        The appended string can contain null characters.
        This function participates in overload resolution if
        `T` is convertible to `string_view_type` and `T` is not
        convertible to `CharT const*`.

        @throw std::length_error if `string_view_type{t}.size() > max_size()`
    */
    template<typename T>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
    basic_static_string&
#else
    typename std::enable_if<
        std::is_convertible<T, string_view_type>::value &&
        ! std::is_convertible<T, CharT const*>::value,
            basic_static_string&>::type
#endif
    operator+=(
        T const& t)
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
        basic_static_string<M, CharT, Traits> const& s) const noexcept
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
        basic_static_string<M, CharT, Traits> const& s) const
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
        basic_static_string<M, CharT, Traits> const& s,
        size_type pos2,
        size_type count2 = npos) const
    {
        return detail::lexicographical_compare(
            subview(pos1, count1), s.subview(pos2, count2));
    }

    /** Compare the string with another.
        
        Compares this string to the null-terminated character sequence beginning at the character pointed to by `s` with length `Traits::length(s)`.
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    int
    compare(
        CharT const* s) const noexcept
    {
        return detail::lexicographical_compare<CharT, Traits>(
            data(), size(), s, Traits::length(s));
    }

    /** Compare the string with another.
        
        Compares a `[pos1, pos1+count1)` substring of this string to the null-terminated character sequence beginning at the character pointed to by `s` with
        length `Traits::length(s)`. If `count1 > size() - pos1` the substring is `[pos1, size())`.
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    int
    compare(
        size_type pos1,
        size_type count1,
        CharT const* s) const
    {
        return detail::lexicographical_compare<CharT, Traits>(
            subview(pos1, count1), s, Traits::length(s));
    }

    /** Compare the string with another.
        
        Compares a `[pos1, pos1+count1)` substring of this string to the characters in the range `[s, s + count2)`. If `count1 > size() - pos1` the substring is `[pos1, size())`.
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    int
    compare(
        size_type pos1,
        size_type count1,
        CharT const* s,
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
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.
    */
    template<typename T>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
    int
#else
    typename std::enable_if<
        std::is_convertible<T const&, string_view_type>::value &&
        ! std::is_convertible<T const&, CharT const*>::value,
            int>::type
#endif
    compare(
        size_type pos1,
        size_type count1,
        T const& t,
        size_type pos2,
        size_type count2 = npos) const
    {
        return compare(pos1, count1,
            string_view_type(t).substr(pos2, count2));
    }

    /** Returns a substring.
    
        Returns a substring `(pos, pos + count)`. If the requested substring is greater than the size of the string, the returned substring is `[pos, size())`.

        @throw std::out_of_range if `pos > size()`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string
    substr(
        size_type pos = 0,
        size_type count = npos) const;

    /** Returns a view of a substring.

        Returns a view of a substring `(pos, pos + count)`. If the requested view is greater than the size of the string, the returned view is `[pos, size())`.

        @throw std::out_of_range if `pos > size()`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    string_view_type
    subview(
        size_type pos = 0,
        size_type count = npos) const;

    /** Copy a substring.

        Copy a substring `(pos, pos+count)` to character string pointed to by `dest`.
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    size_type
    copy(
        CharT* dest,
        size_type count,
        size_type pos = 0) const noexcept;

    /** Changes the number of characters stored.

        If the resulting string is larger, the new
        characters are uninitialized.
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    void
    resize(
        std::size_t n);

    /** Changes the number of characters stored.

        If the resulting string is larger, the new
        characters are initialized to the value of `c`.
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    void
    resize(
        std::size_t n,
        CharT c);

    /// Exchange the contents of this string with another.
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    void
    swap(
        basic_static_string& s) noexcept;

    /// Exchange the contents of this string with another.
    template<std::size_t M>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    void
    swap(
        basic_static_string<M, CharT, Traits>& s);

    /** Replace a subset of the string.
    
        Replaces the part of the string indicated by `[pos1, pos1 + n1)` with `s`
        
        @throw std::out_of_range if `pos1 > size()`
        @throw std::length_error if the resulting string exceeds `max_size()`
        @return `*this`
    */
    template<size_t M>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    replace(
        size_type pos1,
        size_type n1,
        const basic_static_string<M, CharT, Traits>& str)
    {
      return replace(pos1, n1, str.data(), str.size());
    }

    /** Replace a subset of the string.
    
        Replaces the part of the string indicated by `[pos1, pos1 + n1)` with substring `[pos2, pos2 + n2)` of `str`
        except that if `n2` is greater than `str.size()`, `[pos2, pos2 + str.size())` is used.

        @throw std::out_of_range if `pos1 > size()`
        @throw std::length_error if the resulting string exceeds `max_size()`
        @return `*this`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    replace(
        size_type pos1,
        size_type n1,
        const basic_static_string& str,
        size_type pos2,
        size_type n2 = npos)
    {
      return replace(pos1, n1, str.subview(pos2, n2));
    }

    /** Replace a subset of the string.
    
        Replaces the part of the string indicated by `[pos1, pos1 + n1)` with `t` after converting to `string_view_type`.

        This function participates in overload resolution if
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.

        @throw std::out_of_range if `pos1 > size()`
        @throw std::length_error if the resulting string exceeds `max_size()`
        @return `*this`
    */
    template<typename T>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
    basic_static_string&
#else
    typename std::enable_if<
        std::is_convertible<T, string_view_type>::value &&
        ! std::is_convertible<T, CharT const*>::value,
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

    /** Replace a subset of the string.
    
        Replaces the part of the string indicated by `[pos1, pos1 + n1)` with substring `[pos2, pos2 + n2)` of `t`
        after converting to `string_view_type`, except that if `n2` is greater than `t.size()`, `[pos2, pos2 + t.size())` is used.

        This function participates in overload resolution if
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.

        @throw std::out_of_range if `pos1 > size()`
        @throw std::length_error if the resulting string exceeds `max_size()`
        @return `*this`
    */
    template<typename T>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
    basic_static_string&
#else
    typename std::enable_if<
        std::is_convertible<T, string_view_type>::value &&
        ! std::is_convertible<T, CharT const*>::value,
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

    /** Replace a subset of the string.
    
        Replaces the part of the string indicated by `[pos1, pos1 + n1)` with the characters in the range `[s, s + n2)`.

        @throw std::out_of_range if `pos1 > size()`
        @throw std::length_error if the resulting string exceeds `max_size()`
        @return `*this`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    replace(
        size_type pos,
        size_type n1,
        const CharT* s,
        size_type n2);


    /** Replace a subset of the string.
    
        Replaces the part of the string indicated by `[pos1, pos1 + n1)` with the characters in the range `[s, s + Traits::length(s))`.

        @throw std::out_of_range if `pos1 > size()`
        @throw std::length_error if the resulting string exceeds `max_size()`
        @return `*this`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    replace(
        size_type pos,
        size_type n1,
        const CharT* s)
    {
      return replace(pos, n1, s, Traits::length(s));
    }

    /** Replace a subset of the string.
    
        Replaces the part of the string indicated by `[pos1, pos1 + n1)` with `n2` copies of `c`.

        @throw std::out_of_range if `pos1 > size()`
        @throw std::length_error if the resulting string exceeds `max_size()`
        @return `*this`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    replace(
        size_type pos,
        size_type n1,
        size_type n2,
        CharT c);

    /** Replace a subset of the string.
    
        Replaces the part of the string indicated by `[i1, i2)` with `s`

        @throw std::out_of_range if `pos1 > size()`
        @throw std::length_error if the resulting string exceeds `max_size()`
        @return `*this`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    replace(
        const_iterator i1,
        const_iterator i2,
        const basic_static_string& str)
    {
      return replace(i1, i2, str.data(), str.size());
    }

    /** Replace a subset of the string.
    
        Replaces the part of the string indicated by `[i1, i2)` with `t` after converting to `string_view_type`.

        This function participates in overload resolution if
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.

        @throw std::out_of_range if `i1` and `i2` do not refer to elements within the range `[0, size())`
        @throw std::length_error if the resulting string exceeds `max_size()`
        @return `*this`
    */
    template<typename T>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
    basic_static_string&
#else
    typename std::enable_if<
        std::is_convertible<T, string_view_type>::value &&
        ! std::is_convertible<T, CharT const*>::value,
            basic_static_string&>::type
#endif
    replace(
        const_iterator i1,
        const_iterator i2,
        const T& t)
    {
      string_view_type sv = t;
      return replace(i1 - begin(), i2 - i1, sv.data(), sv.size());
    }

    /** Replace a subset of the string.
    
        Replaces the part of the string indicated by `[i1, i2)` with the characters in the range `[s, s + n)`.

        @throw std::out_of_range if `i1` and `i2` do not refer to elements within the range `[0, size())`
        @throw std::length_error if the resulting string exceeds `max_size()`
        @return `*this`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    replace(
        const_iterator i1,
        const_iterator i2,
        const CharT* s,
        size_type n)
    {
      return replace(i1 - begin(), i2 - i1, s, n);
    }

    /** Replace a subset of the string.
    
        Replaces the part of the string indicated by `[i1, i2)` with the characters in the range `[s, s + Traits::length(s))`.

        @throw std::out_of_range if `i1` and `i2` do not refer to elements within the range `[0, size())`
        @throw std::length_error if the resulting string exceeds `max_size()`
        @return `*this`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    replace(
        const_iterator i1,
        const_iterator i2,
        const CharT* s)
    {
      return replace(i1, i2, s, Traits::length(s));
    }

    /** Replace a subset of the string.
    
        Replaces the part of the string indicated by `[i1, i2)` with `n` copies of `c`.

        @throw std::out_of_range if `i1` and `i2` do not refer to elements within the range `[0, size())`
        @throw std::length_error if the resulting string exceeds `max_size()`
        @return `*this`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    replace(
        const_iterator i1,
        const_iterator i2,
        size_type n,
        CharT c)
    {
      return replace(i1 - begin(), i2 - i1, n, c);
    }

    /** Replace a subset of the string.
    
        Replaces the part of the string indicated by `[i1, i2)` with the characters in the range `[j1, j2)`.

        @throw std::out_of_range if `i1` and `i2` do not refer to elements within the range `[0, size())`
        @throw std::length_error if the resulting string exceeds `max_size()`
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
    replace(
        const_iterator i1,
        const_iterator i2,
        InputIterator j1,
        InputIterator j2)
    {
      return replace(i1, i2, basic_static_string(j1, j2));
    }

    /** Replace a subset of the string.
    
        Replaces the part of the string indicated by `[i1, i2)` with the characters in the initializer list `il`.

        @throw std::out_of_range if `i1` and `i2` do not refer to elements within the range `[0, size())`
        @throw std::length_error if the resulting string exceeds `max_size()`
        @return `*this`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    replace(
        const_iterator i1,
        const_iterator i2,
        std::initializer_list<CharT> il)
    {
      return replace(i1, i2, il.begin(), il.size());
    }

    //--------------------------------------------------------------------------
    //
    // Search
    //
    //--------------------------------------------------------------------------

    /** Finds the first substring.
    
        Finds the first substring equal to `t`.

        This function participates in overload resolution if
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.
    */
    template<class T>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
    size_type
#else
    typename std::enable_if<
        std::is_convertible<T, string_view_type>::value &&
        ! std::is_convertible<T, CharT const*>::value,
            size_type>::type
#endif
    find(
        const T& t,
        size_type pos = 0) const noexcept(detail::is_nothrow_convertible<const T&,
                                          string_view_type>::value)
    {
      string_view_type sv = t;
      return find(sv.data(), pos, sv.size());
    }
    
    /** Finds the first substring.

        Finds the first substring equal to `str`.
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

    /** Finds the first substring.
    
        Finds the first substring equal to the range `(s, s + count)`. This range may contain null characters.
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    size_type
    find(
        const CharT* s, 
        size_type pos,
        size_type n) const noexcept;

    /** Finds the first substring.
    
        Finds the first substring equal to the character
        string pointed to by `s`.
    
        The length of the string is determined by the first
        null character using `Traits::length(s)`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    size_type
    find(
        const CharT* s,
        size_type pos = 0) const noexcept
    {
      return find(s, pos, Traits::length(s));
    }

    /// Finds the first character `c`.
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    size_type
    find(
        CharT c,
        size_type pos = 0) const noexcept
    {
      return find(&c, pos, 1);
    }


    /** Finds the last substring.
    
        Finds the last substring equal to `t`.

        This function participates in overload resolution if
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.
    */
    template<class T>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
    size_type
#else
    typename std::enable_if<
        std::is_convertible<T, string_view_type>::value &&
        ! std::is_convertible<T, CharT const*>::value,
            size_type>::type
#endif
    rfind(
        const T& t,
        size_type pos = npos) const noexcept(detail::is_nothrow_convertible<const T&,
                                             string_view_type>::value)
    {
      string_view_type sv = t;
      return rfind(sv.data(), pos, sv.size());
    }

    /** Finds the last substring.
    
        Finds the last substring equal to `str`.
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

    /** Finds the last substring.
    
        Finds the last substring equal to the range `(s, s + count)`. This range may contain null characters.
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    size_type
    rfind(
        const CharT* s, 
        size_type pos,
        size_type n) const noexcept;

    /** Finds the last substring.
    
        Finds the last substring equal to the character
        string pointed to by `s`.

        The length of the string is determined by the first
        null character using `Traits::length(s)`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    size_type
    rfind(
        const CharT* s,
        size_type pos = npos) const noexcept
    {
      return rfind(s, pos, Traits::length(s));
    }

    /// Finds the last character `c`.
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    size_type
    rfind(
        CharT c,
        size_type pos = npos) const noexcept
    {
      return rfind(&c, pos, 1);
    }

    /** Finds the first character equal to any character in the string.
    
        Finds the first character equal to one of the characters in `t`.

        This function participates in overload resolution if
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.
    */
    template<class T>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
    size_type
#else
    typename std::enable_if<
        std::is_convertible<T, string_view_type>::value &&
        ! std::is_convertible<T, CharT const*>::value,
            size_type>::type
#endif
    find_first_of(
        const T& t,
        size_type pos = 0) const noexcept(detail::is_nothrow_convertible<const T&,
                                          string_view_type>::value)
    {
      string_view_type sv = t;
      return find_first_of(sv.data(), pos, sv.size());
    }

    /** Finds the first character equal to any character in the string.
    
        Finds the first character equal to one of the characters in `str`.
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

    /** Finds the first character equal to any character in the string.

        Finds the first character equal to one of the characters in the range `(s, s + count)`. This range can include null characters.
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    size_type
    find_first_of(
        const CharT* s,
        size_type pos,
        size_type n) const noexcept;

    /** Finds the first character equal to any character in the string.
    
        Finds the first character equal to one of the characters
        in character string pointed to by `s`. 
     
        The length of the string is determined by the first
        null character using `Traits::length(s)`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    size_type
    find_first_of(
        const CharT* s,
        size_type pos = 0) const noexcept
    {
      return find_first_of(s, pos, Traits::length(s));
    }

    /// Finds the first character equal to `c`.
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    size_type
    find_first_of(
        CharT c,
        size_type pos = 0) const noexcept
    {
      return find_first_of(&c, pos, 1);
    }

    /** Finds the last character equal to any character in the string.
    
        Finds the last character equal to one of the characters in `t`.

        This function participates in overload resolution if
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.
    */
    template<class T>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
    size_type
#else
    typename std::enable_if<
        std::is_convertible<T, string_view_type>::value &&
        ! std::is_convertible<T, CharT const*>::value,
            size_type>::type
#endif
    find_last_of(
        const T& t,
        size_type pos = npos) const noexcept(detail::is_nothrow_convertible<const T&,
                                             string_view_type>::value)
    {
      string_view_type sv = t;
      return find_last_of(sv.data(), pos, sv.size());
    }
     
    /** Finds the last character equal to any character in the string.
    
        Finds the last character equal to one of the characters in `str`.
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

    /** Finds the last character equal to any character in the string.
    
        Finds the last character equal to one of the characters in the range `(s, s + count)`. This range can include null characters.
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    size_type
    find_last_of(
        const CharT* s,
        size_type pos,
        size_type n) const noexcept;

    /** Finds the last character equal to any character in the string.
    
        Finds the last character equal to one of the characters
        in character string pointed to by `s`.

        The length of the string is determined by the first
        null character using `Traits::length(s)`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    size_type
    find_last_of(
        const CharT* s,
        size_type pos = npos) const noexcept
    {
      return find_last_of(s, pos, Traits::length(s));
    }

    /// Finds the last character equal to `c`.
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    size_type
    find_last_of(
        CharT c,
        size_type pos = npos) const noexcept
    {
      return find_last_of(&c, pos, 1);
    }

    /** Finds the first character equal to none the characters in the string.
    
        Finds the first character equal to none of the characters in `t`.

        This function participates in overload resolution if
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.
    */
    template<class T>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
    size_type
#else
    typename std::enable_if<
        std::is_convertible<T, string_view_type>::value &&
        ! std::is_convertible<T, CharT const*>::value,
            size_type>::type
#endif
    find_first_not_of(
        const T& t,
        size_type pos = 0) const noexcept(detail::is_nothrow_convertible<const T&,
                                          string_view_type>::value)
    {
      string_view_type sv = t;
      return find_first_not_of(sv.data(), pos, sv.size());
    }

    /** Finds the first character equal to none the characters in the string.
    
        Finds the first character equal to none of the characters in `str`.
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

    /** Finds the first character equal to none the characters in the string.
        
        Finds the first character equal to none of characters in range `(s, s + count)`. This range can include null characters.
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    size_type
    find_first_not_of(
        const CharT* s,
        size_type pos, 
        size_type n) const noexcept;

    /** Finds the first character equal to none the characters in the string.
    
        Finds the first character equal to none of the characters in
        character string pointed to by `s`.

        The length of the string is determined by the first
        null character using `Traits::length(s)`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    size_type
    find_first_not_of(
        const CharT* s,
        size_type pos = 0) const noexcept
    {
      return find_first_not_of(s, pos, Traits::length(s));
    }

    /// Finds the first character not equal to `c`.
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    size_type
    find_first_not_of(
        CharT c,
        size_type pos = 0) const noexcept
    {
      return find_first_not_of(&c, pos, 1);
    }

    /** Finds the last character equal to none the characters in the string.
    
        Finds the last character equal to none of the characters in `t`.

        This function participates in overload resolution if
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.
    */
    template<class T>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
    size_type
#else
    typename std::enable_if<
        std::is_convertible<T, string_view_type>::value &&
        ! std::is_convertible<T, CharT const*>::value,
            size_type>::type
#endif
    find_last_not_of(
        const T& t,
        size_type pos = npos) const noexcept(detail::is_nothrow_convertible<const T&,
                                             string_view_type>::value)
    {
      string_view_type sv = t;
      return find_last_not_of(sv.data(), pos, sv.size());
    }

    /** Finds the last character equal to none the characters in the string.
    
        Finds the last character equal to none of the characters in `str`.
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

    /** Finds the last character equal to none the characters in the string.
        
        Finds the last character equal to none of the characters in range `(s, s + count)`. This range can include null characters.
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    size_type
    find_last_not_of(
        const CharT* s,
        size_type pos,
        size_type n) const noexcept;


    /** Finds the last character equal to none the characters in the string.
    
        Finds the last character equal to none of the characters in
        character string pointed to by `s`.

        The length of the string is determined by the first
        null character using `Traits::length(s)`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    size_type
    find_last_not_of(
        const CharT* s,
        size_type pos = npos) const noexcept
    {
      return find_last_not_of(s, pos, Traits::length(s));
    }

    /// Finds the last character not equal to `c`.
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    size_type
    find_last_not_of(
        CharT c,
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
      return size() >= len && !Traits::compare(data(), s.data(), len);
    }
    
    /// Returns whether the string begins with `c`
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    bool 
    starts_with(
        CharT c) const noexcept
    {
      return !empty() && Traits::eq(front(), c);
    }
    
    /// Returns whether the string begins with `s`
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    bool 
    starts_with(
        const CharT* s) const noexcept
    {
      const size_type len = Traits::length(s);
      return size() >= len && !Traits::compare(data(), s, len);
    }
    
    /// Returns whether the string ends with `s`
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    bool 
    ends_with(
        string_view_type s) const noexcept
    {
      const size_type len = s.size();
      return size() >= len && !Traits::compare(data() + (size() - len), s.data(), len);
    }
    
    /// Returns whether the string ends with `c`
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    bool
    ends_with(
        CharT c) const noexcept
    {
      return !empty() && Traits::eq(back(), c);
    }
    
    /// Returns whether the string begins with `s`
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    bool 
    ends_with(
        const CharT* s) const noexcept
    {
      const size_type len = Traits::length(s);
      return size() >= len && !Traits::compare(data() + (size() - len), s, len);
    }

private:
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    assign_char(CharT ch, std::true_type) noexcept;

    basic_static_string&
    assign_char(CharT ch, std::false_type);
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
    basic_static_string<N, CharT, Traits> const& lhs,
    basic_static_string<M, CharT, Traits> const& rhs)
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
    basic_static_string<N, CharT, Traits> const& lhs,
    basic_static_string<M, CharT, Traits> const& rhs)
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
    basic_static_string<N, CharT, Traits> const& lhs,
    basic_static_string<M, CharT, Traits> const& rhs)
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
    basic_static_string<N, CharT, Traits> const& lhs,
    basic_static_string<M, CharT, Traits> const& rhs)
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
    basic_static_string<N, CharT, Traits> const& lhs,
    basic_static_string<M, CharT, Traits> const& rhs)
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
    basic_static_string<N, CharT, Traits> const& lhs,
    basic_static_string<M, CharT, Traits> const& rhs)
{
    return lhs.compare(rhs) >= 0;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
bool
operator==(
    CharT const* lhs,
    basic_static_string<N, CharT, Traits> const& rhs)
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
    basic_static_string<N, CharT, Traits> const& lhs,
    CharT const* rhs)
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
    CharT const* lhs,
    basic_static_string<N, CharT, Traits> const& rhs)
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
    basic_static_string<N, CharT, Traits> const& lhs,
    CharT const* rhs)
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
    CharT const* lhs,
    basic_static_string<N, CharT, Traits> const& rhs)
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
    basic_static_string<N, CharT, Traits> const& lhs,
    CharT const* rhs)
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
    CharT const* lhs,
    basic_static_string<N, CharT, Traits> const& rhs)
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
    basic_static_string<N, CharT, Traits> const& lhs,
    CharT const* rhs)
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
    CharT const* lhs,
    basic_static_string<N, CharT, Traits> const& rhs)
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
    basic_static_string<N, CharT, Traits> const& lhs,
    CharT const* rhs)
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
    CharT const* lhs,
    basic_static_string<N, CharT, Traits> const& rhs)
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
    basic_static_string<N, CharT, Traits> const& lhs,
    CharT const* rhs)
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
    basic_static_string<N, CharT, Traits>const& lhs,
    basic_static_string<M, CharT, Traits>const& rhs)
{
    return basic_static_string<N + M, CharT, Traits>(lhs) += rhs;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
basic_static_string<N + 1, CharT, Traits>
operator+(
    basic_static_string<N, CharT, Traits> const& lhs,
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
    basic_static_string<N, CharT, Traits> const& rhs)
{
    return basic_static_string<N + 1, CharT, Traits>(rhs).insert(0, lhs);
}

template<
    std::size_t N, std::size_t M,
    typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
basic_static_string<N + M, CharT, Traits>
operator+(
    basic_static_string<N, CharT, Traits>const& lhs,
    const CharT(&rhs)[M])
{
    return basic_static_string<N + M, CharT, Traits>(lhs).append(+rhs, M);
}

template<
    std::size_t N, std::size_t M,
    typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
basic_static_string<N + M, CharT, Traits>
operator+(
    const CharT(&lhs)[N],
    basic_static_string<M, CharT, Traits>const& rhs)
{
    return basic_static_string<N + M, CharT, Traits>(rhs).insert(0, +rhs, N);
}

//------------------------------------------------------------------------------
//
// swap
//
//------------------------------------------------------------------------------

template<
    std::size_t N, 
    typename CharT, 
    typename Traits>
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
operator<<(std::basic_ostream<CharT, Traits>& os, 
    basic_static_string<N, CharT, Traits> const& s)
{
    return os << basic_string_view<CharT, Traits>(s.data(), s.size());
}

//------------------------------------------------------------------------------
//
// Aliases
//
//------------------------------------------------------------------------------

template<std::size_t N>
using static_string = basic_static_string<N, char>;

template<std::size_t N>
using static_wstring = basic_static_string<N, wchar_t>;

template<std::size_t N>
using static_u16string = basic_static_string<N, char16_t>;

template<std::size_t N>
using static_u32string = basic_static_string<N, char32_t>;

//------------------------------------------------------------------------------
//
// Numeric conversions
//
//------------------------------------------------------------------------------

// string

static_string<std::numeric_limits<int>::digits10 + 1>
inline
to_static_string(int value);

static_string<std::numeric_limits<long>::digits10 + 1>
inline
to_static_string(long value);

static_string<std::numeric_limits<long long>::digits10 + 1>
inline
to_static_string(long long value);

static_string<std::numeric_limits<unsigned int>::digits10 + 1>
inline
to_static_string(unsigned int value);

static_string<std::numeric_limits<unsigned long>::digits10 + 1>
inline
to_static_string(unsigned long value);

static_string<std::numeric_limits<unsigned long long>::digits10 + 1>
inline
to_static_string(unsigned long long value);

static_string<std::numeric_limits<float>::max_digits10 + 1>
inline
to_static_string(float value);

static_string<std::numeric_limits<double>::max_digits10 + 1>
inline
to_static_string(double value);

static_string<std::numeric_limits<long double>::max_digits10 + 1>
inline
to_static_string(long double value);

// wstring

static_wstring<std::numeric_limits<int>::digits10 + 1>
inline
to_static_wstring(int value);

static_wstring<std::numeric_limits<long>::digits10 + 1>
inline
to_static_wstring(long value);

static_wstring<std::numeric_limits<long long>::digits10 + 1>
inline
to_static_wstring(long long value);

static_wstring<std::numeric_limits<unsigned int>::digits10 + 1>
inline
to_static_wstring(unsigned int value);

static_wstring<std::numeric_limits<unsigned long>::digits10 + 1>
inline
to_static_wstring(unsigned long value);

static_wstring<std::numeric_limits<unsigned long long>::digits10 + 1>
inline
to_static_wstring(unsigned long long value);

static_wstring<std::numeric_limits<float>::max_digits10 + 1>
inline
to_static_wstring(float value);

static_wstring<std::numeric_limits<double>::max_digits10 + 1>
inline
to_static_wstring(double value);

static_wstring<std::numeric_limits<long double>::max_digits10 + 1>
inline
to_static_wstring(long double value);

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
// hash_value overload for Boost.Container_Hash
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

#ifndef GENERATING_DOCUMENTATION
// std::hash partial specialization for basic_static_string
#endif
namespace std
{
  template <std::size_t N, typename CharT, typename Traits>
  struct hash<boost::static_string::basic_static_string<N, CharT, Traits>> 
  {
    std::size_t 
    operator()(
        const boost::static_string::basic_static_string<N, CharT, Traits>& str) const
    {
#ifndef BOOST_STATIC_STRING_STANDALONE
        return boost::hash_range(str.begin(), str.end());
#else
        using sv = boost::static_string::basic_string_view<CharT, Traits>;
        return std::hash<sv>()(sv(str.data(), str.size()));
#endif
    }
  };
} // std

#include <boost/static_string/impl/static_string.hpp>
#endif
