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

    /// The type of `string_view_type` returned by the interface
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
        CharT ch) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

    /** Construct a `basic_static_string`.
        
        Construct with a substring (pos, other.size()) of `other`.
    */
    template<std::size_t M>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string(
        basic_static_string<M, CharT, Traits> const& other,
        size_type pos) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

    /** Construct a `basic_static_string`.
    
        Construct with a substring (pos, count) of `other`.
    */
    template<std::size_t M>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string(
        basic_static_string<M, CharT, Traits> const& other,
        size_type pos,
        size_type count) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

    /** Construct a `basic_static_string`.
        
        Construct with the first `count` characters of `s`, including nulls.
     */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string(
        CharT const* s,
        size_type count) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

    /** Construct a `basic_static_string`.
        
        Construct from a null terminated string.
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string(
        CharT const* s) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

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
        InputIterator last) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

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
        basic_static_string<M, CharT, Traits> const& other) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

    /** Construct a `basic_static_string`.
        
        Construct from an initializer list
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string(
        std::initializer_list<CharT> init) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

    /** Construct a `basic_static_string`.
    
        Construct from a `string_view_type`
    */
    explicit
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string(
        string_view_type sv) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

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
        size_type n) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

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
        basic_static_string<M, CharT, Traits> const& s) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        CharT const* s) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
    {
        return assign(s);
    }

    /** Assign to the string.
    
        Assign from single character.
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    operator=(
        CharT ch) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        std::initializer_list<CharT> ilist) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
    {
        return assign(ilist);
    }

    /** Assign to the string.
    
        Assign from `string_view_type`.
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    operator=(
        string_view_type sv) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        CharT ch) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

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
        basic_static_string<M, CharT, Traits> const& s) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        size_type count = npos) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

    /** Replace the contents.
    
        Replace the contents with the first `count` characters of `s`, including nulls.

        @throw std::length_error if `count > max_size()`
        @return `*this`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    assign(
        CharT const* s,
        size_type count) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

    /** Replace the contents.
    
        Replace the contents with a copy of a null terminated string `s`

        @throw std::length_error if `Traits::length(s) > max_size()`
        @return `*this`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    assign(
        CharT const* s) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        InputIterator last) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

    /** Replace the contents.
    
        Replace the contents with the characters in an initializer list

        @throw std::length_error if `ilist.size() > max_size()`
        @return `*this`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    assign(
        std::initializer_list<CharT> ilist) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
    assign(T const& t) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        size_type count = npos) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
    at(size_type pos) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

    /** Access specified character with bounds checking.

        @throw std::out_of_range if `pos >= size()`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    const_reference
    at(size_type pos) const BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

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
        return data()[size() - 1];
    }

    /** Accesses the last character.

        Undefined behavior if `empty() == true`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    CharT const&
    back() const noexcept
    {
        return data()[size() - 1];
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
    reserve(std::size_t n) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

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
        CharT ch) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
    {
      BOOST_STATIC_STRING_THROW_IF(
        index > size(), std::out_of_range{"index > size()"});
      insert(begin() + index, count, ch);
      return *this;
    }
    
    /** Insert a string.

        Inserts the null-terminated character string pointed to by `s`
        of length `count` at the position `index` where `count`
        is `Traits::length(s)`.

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
        CharT const* s) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
    {
        return insert(index, s, Traits::length(s));
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
        CharT const* s,
        size_type count) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

    /** Insert a string.

        Inserts the string `str`
        at the position `index`.

        @par Exception Safety

        Strong guarantee.

        @note
        The insertion is done unchecked, as the source cannot be
        within the destination.

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
        const basic_static_string<M, CharT, Traits>& str) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
    {
        return insert_unchecked(index, str.data(), str.size());
    }

    /** Insert a string.

        Inserts the string `str`
        at the position `index`.

        @par Exception Safety

        Strong guarantee.

        @note All references, pointers, or iterators
        referring to contained elements are invalidated. Any
        past-the-end iterators are also invalidated.

        @return `*this`

        @param index The index to insert at.
        @param str The string to insert.

        @throw std::length_error `size() + str.size() > max_size()`
        @throw std::out_of_range `index > size()`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    insert(
        size_type index,
        const basic_static_string& str) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
    {
        return insert(index, str.data(), str.size());
    }

    /** Insert a string.

        Inserts a string, obtained by `str.substr(index_str, count)`
        at the position `index`.

        @par Exception Safety

        Strong guarantee.

        @note
        The insertion is done unchecked, as the source cannot be
        within the destination.

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
        size_type count = npos) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
    {
        BOOST_STATIC_STRING_THROW_IF(
          index_str > str.size(), std::out_of_range{"index_str > str.size()"}
        );
        return insert_unchecked(index, str.data() + index_str, (std::min)(count, str.size() - index_str));
    }

    /** Insert a string.

        Inserts a string, obtained by `str.substr(index_str, count)`
        at the position `index`.

        @par Exception Safety

        Strong guarantee.

        @note All references, pointers, or iterators
        referring to contained elements are invalidated. Any
        past-the-end iterators are also invalidated.

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
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    insert(
        size_type index,
        const basic_static_string& str,
        size_type index_str,
        size_type count = npos) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
    {
        BOOST_STATIC_STRING_THROW_IF(
          index_str > str.size(), std::out_of_range{"index_str > str.size()"}
        );
        return insert(index, str.data() + index_str, (std::min)(count, str.size() - index_str));
    }

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
        CharT ch) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        CharT ch) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

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
              ! detail::is_forward_iterator<
                    InputIterator>::value, iterator>::type
#endif
    insert(
        const_iterator pos,
        InputIterator first,
        InputIterator last) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

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

       @tparam ForwardIterator The type of the iterators.

       @par Constraints

       `InputIterator` satisfies __ForwardIterator__.

       @return An iterator which refers to the first inserted character
       or `pos` if no characters were inserted

       @param pos The position to insert at.
       @param first An iterator representing the first character to insert.
       @param last An iterator representing one past the last character to insert.

       @throw std::length_error `size() + insert_count > max_size()`
   */
    template<typename ForwardIterator>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
    iterator
#else
    typename std::enable_if<
        detail::is_forward_iterator<
            ForwardIterator>::value, 
                iterator>::type
#endif
    insert(
        const_iterator pos,
        ForwardIterator first,
        ForwardIterator last) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

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
        std::initializer_list<CharT> ilist) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
    {
      const auto offset = pos - begin();
      return insert_unchecked(offset, ilist.begin(), ilist.size()).begin() + offset;
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

        `std::is_convertible<T const&, string_view>::value &&
        !std::is_convertible<T const&, char CharT*>::value`.

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
            T const&, string_view_type>::value &&
        ! std::is_convertible<
            T const&, CharT const*>::value,
                basic_static_string&>::type
#endif
    insert(
        size_type index,
        T const& t) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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

        `std::is_convertible<T const&, string_view>::value &&
        !std::is_convertible<T const&, CharT const*>::value`.

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
        std::is_convertible<T const&, string_view_type>::value &&
        ! std::is_convertible<T const&, CharT const*>::value,
            basic_static_string&>::type
#endif
    insert(
        size_type index,
        T const& t,
        size_type index_str,
        size_type count = npos) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
    {
      auto const s = string_view_type(t).substr(index_str, count);
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
        size_type count = npos) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

    /** Removes the character at `pos`

        @return iterator pointing to the character immediately following the character erased, or `end()` if no such character exists
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    iterator
    erase(
        const_iterator pos) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

    /** Removes the characters in the range `(first, last)`

        @return iterator pointing to the character last pointed to before the erase, or `end()` if no such character exists
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    iterator
    erase(
        const_iterator first,
        const_iterator last) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

    /** Appends the given character `ch` to the end of the string.

        @throw std::length_error if `1 > max_size() - size()`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    void
    push_back(
        CharT ch) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

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
        CharT ch) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        string_view_type sv) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        size_type count = npos) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        size_type count) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

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
        CharT const* s) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        InputIterator last) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        std::initializer_list<CharT> ilist) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
    {
        insert(end(), ilist);
        return *this;
    }

    /** Append to the string.
    
        Appends characters from `string_view_type{t}`

        The appended string can contain null characters.
        This function participates in overload resolution if
        `T` is convertible to `string_view_type` and `T` is not
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
        T const& t) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        size_type count = npos) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        basic_static_string<M, CharT, Traits> const& s) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        CharT ch) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        CharT const* s) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        std::initializer_list<CharT> ilist) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        T const& t) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        basic_static_string<M, CharT, Traits> const& s) const BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        size_type count2 = npos) const BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        CharT const* s) const BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        size_type count2) const BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        string_view_type s) const BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
    {
        return detail::lexicographical_compare<CharT, Traits>(
            subview(pos1, count1), s);
    }

    /** Compare the string with another.
        
        Replaces the part of the string indicated by `[pos1, pos1 + count1)` with a substring `[pos2, pos2 + count2)` of `t` after converting to `string_view_type`.

        This function participates in overload resolution if
        `T` is convertible to `string_view_type` and `T` is not
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
        size_type count2 = npos) const BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        size_type count = npos) const BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

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
        size_type count = npos) const BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

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
        size_type pos = 0) const BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

    /** Change the size of the string.

        Resizes the string to contain `n` characters. If
        `n > size()`, characters with the value `CharT()` are
        appended. Otherwise, `size()` is reduced to `n`.

        @param n The size to resize the string to.

        @throw std::out_of_range `n > max_size()`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    void
    resize(
        std::size_t n) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

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
        std::size_t n,
        CharT c) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

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
        basic_static_string<M, CharT, Traits>& s) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;
        

   /** Replace a substring with a string.

        Replaces `rcount` characters starting at index `pos1` with those
        of `str`, where `rcount` is `std::min(n1, size() - pos1)`.

        @par Exception Safety

        Strong guarantee.

        @note 
        The replacement is done unchecked, as the source cannot be
        within the destination.
        
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
        const basic_static_string<M, CharT, Traits>& str) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
    {
      return replace_unchecked(pos1, n1, str.data(), str.size());
    }

    /** Replace a substring with a string.

        Replaces `rcount` characters starting at index `pos1` with those
        of `str`, where `rcount` is `std::min(n1, size() - pos1)`.

        @par Exception Safety

        Strong guarantee.

        @note All references, pointers, or iterators
        referring to contained elements are invalidated. Any
        past-the-end iterators are also invalidated.

        @return `*this`

        @param pos1 The index to replace at.
        @param n1 The number of characters to replace.
        @param str The string to replace with.

        @throw std::length_error `size() + (str.size() - rcount) > max_size()`
        @throw std::out_of_range `pos1 > size()`
    */
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    replace(
        size_type pos1,
        size_type n1,
        const basic_static_string& str) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
    {
      return replace(pos1, n1, str.data(), str.size());
    }

    /** Replace a substring with a substring.

        Replaces `rcount` characters starting at index `pos1` with those of
        `str.subview(pos2, n2)`, where `rcount` is `std::min(n1, size() - pos1)`.

        @par Exception Safety

        Strong guarantee.

        @note All references, pointers, or iterators
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
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    replace(
        size_type pos1,
        size_type n1,
        const basic_static_string& str,
        size_type pos2,
        size_type n2 = npos) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
    {
      BOOST_STATIC_STRING_THROW_IF(
        pos2 > str.size(), std::out_of_range{"pos2 > str.size()"}
      );
      return replace(pos1, n1, str.data() + pos2, (std::min)(n2, str.size() - pos2));
    }

    /** Replace a substring with a substring.

        Replaces `rcount` characters starting at index `pos1` with those of
        `str.subview(pos2, n2)`, where `rcount` is `std::min(n1, size() - pos1)`.

        @par Exception Safety

        Strong guarantee.

        @note 
        The replacement is done unchecked, as the source cannot be
        within the destination.
        
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
        size_type n2 = npos) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
    {
      BOOST_STATIC_STRING_THROW_IF(
        pos2 > str.size(), std::out_of_range{"pos2 > str.size()"}
      );
      return replace_unchecked(pos1, n1, str.data() + pos2, (std::min)(n2, str.size() - pos2));
    }

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

        `std::is_convertible<T const&, string_view>::value &&
        !std::is_convertible<T const&, char CharT*>::value`.

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
        ! std::is_convertible<T, CharT const*>::value,
            basic_static_string&>::type
#endif
    replace(
        size_type pos1,
        size_type n1,
        const T& t) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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

        `std::is_convertible<T const&, string_view>::value &&
        !std::is_convertible<T const&, char CharT*>::value`.

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
        ! std::is_convertible<T, CharT const*>::value,
            basic_static_string&>::type
#endif
    replace(
        size_type pos1,
        size_type n1,
        const T& t,
        size_type pos2,
        size_type n2 = npos) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
        const CharT* s,
        size_type n2) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;


    /** Replace a substring with a string.

        Replaces `rcount` characters starting at index `pos` with those of
        `[s, s + len)`, where the length of the string `len` is `Traits::length(s)` and `rcount`
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
        const CharT* s) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
    {
      return replace(pos, n1, s, Traits::length(s));
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
        CharT c) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

    /** Replace a range with a string.

        Replaces the characters in the range `[i1, i2)`
        with those of `str`.

        @par Precondition

        `[i1, i2)` is a valid range.

        @par Exception Safety

        Strong guarantee.

        @note All references, pointers, or iterators
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
        const basic_static_string<M, CharT, Traits>& str) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
    {
      return replace(i1, i2, str.data(), str.size());
    }

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

        `std::is_convertible<T const&, string_view>::value &&
        !std::is_convertible<T const&, char CharT*>::value`.

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
        ! std::is_convertible<T, CharT const*>::value,
            basic_static_string&>::type
#endif
    replace(
        const_iterator i1,
        const_iterator i2,
        const T& t) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
    {
      string_view_type sv = t;
      return replace(i1 - begin(), i2 - i1, sv.data(), sv.size());
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
        const CharT* s,
        size_type n) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
    {
      return replace(i1 - begin(), i2 - i1, s, n);
    }

    /** Replace a range with a string.

        Replaces the characters in the range `[i1, i2)` with those of
        `[s, s + len)`, where the length of the string `len` is `Traits::length(s)`.

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
        const CharT* s) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
    {
      return replace(i1, i2, s, Traits::length(s));
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
        CharT c) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
    {
      return replace(i1 - begin(), i2 - i1, n, c);
    }

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
                ! detail::is_forward_iterator<
                      InputIterator>::value, 
                          basic_static_string<N, CharT, Traits>&>::type
#endif
    replace(
        const_iterator i1,
        const_iterator i2,
        InputIterator j1,
        InputIterator j2) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

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

        @tparam ForwardIterator The type of the iterators.

        @par Constraints

        `ForwardIterator` satisfies __ForwardIterator__.

        @return `*this`

        @param i1 An iterator referring to the first character to replace.
        @param i2 An iterator referring past the end of
        the last character to replace.
        @param j1 An iterator referring to the first character to replace with.
        @param j2 An iterator referring past the end of
        the last character to replace with.

        @throw std::length_error `size() + (std::distance(j1, j2) - std::distance(i1, i2)) > max_size()`
    */
    template<typename ForwardIterator>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
#if GENERATING_DOCUMENTATION
    basic_static_string&
#else
    typename std::enable_if<
        detail::is_forward_iterator<
            ForwardIterator>::value,
                basic_static_string<N, CharT, Traits>&>::type
#endif
    replace(
        const_iterator i1,
        const_iterator i2,
        ForwardIterator j1,
        ForwardIterator j2) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

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
        std::initializer_list<CharT> il) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
    {
        return replace_unchecked(i1 - begin(), i2 - i1, il.begin(), il.size());
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

        `std::is_convertible<T const&, string_view>::value &&
        !std::is_convertible<T const&, char CharT*>::value`.

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
        const CharT* s, 
        size_type pos,
        size_type n) const noexcept;

    /** Find the first occurrence of a string within the string.

        Finds the first occurrence of the string pointed to by `s`
        of length `count` within the string starting at the index `pos`,
        where `count` is `Traits::length(s)`.

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
        const CharT* s,
        size_type pos = 0) const noexcept
    {
      return find(s, pos, Traits::length(s));
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
        CharT c,
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

        `std::is_convertible<T const&, string_view>::value &&
        !std::is_convertible<T const&, char CharT*>::value`.

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
        const CharT* s, 
        size_type pos,
        size_type n) const noexcept;

    /** Find the last occurrence of a string within the string.

        Finds the last occurrence of the string pointed to by `s`
        of length `count` within the string starting before or at the
        index `pos`, where `count` is `Traits::length(s)`.

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
        const CharT* s,
        size_type pos = npos) const noexcept
    {
      return rfind(s, pos, Traits::length(s));
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
        CharT c,
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

        `std::is_convertible<T const&, string_view>::value &&
        !std::is_convertible<T const&, char CharT*>::value`.

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
        const CharT* s,
        size_type pos,
        size_type n) const noexcept;

    /** Find the first occurrence of any of the characters within the string.

        Finds the first occurrence of the any of the characters within string
        pointed to by `s` of length `count` within the string starting at the
        index `pos`, where `count` is `Traits::length(s)`.

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
        const CharT* s,
        size_type pos = 0) const noexcept
    {
      return find_first_of(s, pos, Traits::length(s));
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
        CharT c,
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

        `std::is_convertible<T const&, string_view>::value &&
        !std::is_convertible<T const&, char CharT*>::value`.

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
        const CharT* s,
        size_type pos,
        size_type n) const noexcept;

    /** Find the last occurrence of any of the characters within the string.

        Finds the last occurrence of any of the characters within the string pointed to
        by `s` of length `count` within the string before or at the index `pos`,
        where `count` is `Traits::length(s)`.

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
        const CharT* s,
        size_type pos = npos) const noexcept
    {
      return find_last_of(s, pos, Traits::length(s));
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
        CharT c,
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

        `std::is_convertible<T const&, string_view>::value &&
        !std::is_convertible<T const&, char CharT*>::value`.

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
        const CharT* s,
        size_type pos, 
        size_type n) const noexcept;

    /** Find the first occurrence of any of the characters not within the string.

        Finds the first occurrence of a character that is not within the string
        pointed to by `s` of length `count` within the string starting
        at the index `pos`, where `count` is `Traits::length(s)`.

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
        const CharT* s,
        size_type pos = 0) const noexcept
    {
      return find_first_not_of(s, pos, Traits::length(s));
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
        CharT c,
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

        `std::is_convertible<T const&, string_view>::value &&
        !std::is_convertible<T const&, char CharT*>::value`.

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
        const CharT* s,
        size_type pos,
        size_type n) const noexcept;


    /** Find the last occurrence of a character not within the string.

        Finds the last occurrence of a character that is not within the
        string pointed to by `s` of length `count` within the string
        before or at the index `pos`, where `count` is `Traits::length(s)`.

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
        const CharT* s,
        size_type pos = npos) const noexcept
    {
      return find_last_not_of(s, pos, Traits::length(s));
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
    assign_char(CharT ch, std::false_type) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

    // Returns the size of data read from input iterator. Read data begins at data() + size() + 1.
    template<typename InputIterator>
    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    std::size_t
    read_back(
        InputIterator first, 
        InputIterator last) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    replace_unchecked(
        size_type pos,
        size_type n1,
        const CharT* s,
        size_type n2) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;

    BOOST_STATIC_STRING_CPP14_CONSTEXPR
    basic_static_string&
    insert_unchecked(
        size_type index,
        const CharT* s,
        size_type count) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT;
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
    basic_static_string<M, CharT, Traits> const& rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
    basic_static_string<M, CharT, Traits> const& rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
    basic_static_string<M, CharT, Traits> const& rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
    basic_static_string<M, CharT, Traits> const& rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
    basic_static_string<M, CharT, Traits> const& rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
    basic_static_string<M, CharT, Traits> const& rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
{
    return lhs.compare(rhs) >= 0;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
bool
operator==(
    CharT const* lhs,
    basic_static_string<N, CharT, Traits> const& rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
    CharT const* rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
    basic_static_string<N, CharT, Traits> const& rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
    CharT const* rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
    basic_static_string<N, CharT, Traits> const& rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
    CharT const* rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
    basic_static_string<N, CharT, Traits> const& rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
    CharT const* rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
    basic_static_string<N, CharT, Traits> const& rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
    CharT const* rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
    basic_static_string<N, CharT, Traits> const& rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
    CharT const* rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
    basic_static_string<M, CharT, Traits>const& rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
{
    return basic_static_string<N + M, CharT, Traits>(lhs) += rhs;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
basic_static_string<N + 1, CharT, Traits>
operator+(
    basic_static_string<N, CharT, Traits> const& lhs,
    CharT rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
{
    return basic_static_string<N + 1, CharT, Traits>(lhs) += rhs;
}

template<std::size_t N, typename CharT, typename Traits>
BOOST_STATIC_STRING_CPP14_CONSTEXPR
inline
basic_static_string<N + 1, CharT, Traits>
operator+(
    CharT lhs,
    basic_static_string<N, CharT, Traits> const& rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
    const CharT(&rhs)[M]) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
    basic_static_string<M, CharT, Traits>const& rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
    basic_static_string<N, CharT, Traits>& rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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
    basic_static_string<M, CharT, Traits>& rhs) BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
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

/// Converts `value` to a `static_string` 
static_string<std::numeric_limits<int>::digits10 + 1>
inline
to_static_string(int value) noexcept;

/// Converts `value` to a `static_string` 
static_string<std::numeric_limits<long>::digits10 + 1>
inline
to_static_string(long value) noexcept;

/// Converts `value` to a `static_string` 
static_string<std::numeric_limits<long long>::digits10 + 1>
inline
to_static_string(long long value) noexcept;

/// Converts `value` to a `static_string` 
static_string<std::numeric_limits<unsigned int>::digits10 + 1>
inline
to_static_string(unsigned int value) noexcept;

/// Converts `value` to a `static_string` 
static_string<std::numeric_limits<unsigned long>::digits10 + 1>
inline
to_static_string(unsigned long value) noexcept;

/// Converts `value` to a `static_string`
static_string<std::numeric_limits<unsigned long long>::digits10 + 1>
inline
to_static_string(unsigned long long value) noexcept;

/// Converts `value` to a `static_string`
static_string<std::numeric_limits<float>::max_digits10 + 1>
inline
to_static_string(float value) noexcept;

/// Converts `value` to a `static_string`
static_string<std::numeric_limits<double>::max_digits10 + 1>
inline
to_static_string(double value) noexcept;

/// Converts `value` to a `static_string` 
static_string<std::numeric_limits<long double>::max_digits10 + 1>
inline
to_static_string(long double value) noexcept;

/// Converts `value` to a `static_wstring`
static_wstring<std::numeric_limits<int>::digits10 + 1>
inline
to_static_wstring(int value) noexcept;

/// Converts `value` to a `static_wstring`
static_wstring<std::numeric_limits<long>::digits10 + 1>
inline
to_static_wstring(long value) noexcept;

/// Converts `value` to a `static_wstring`
static_wstring<std::numeric_limits<long long>::digits10 + 1>
inline
to_static_wstring(long long value) noexcept;

/// Converts `value` to a `static_wstring`
static_wstring<std::numeric_limits<unsigned int>::digits10 + 1>
inline
to_static_wstring(unsigned int value) noexcept;

/// Converts `value` to a `static_wstring`
static_wstring<std::numeric_limits<unsigned long>::digits10 + 1>
inline
to_static_wstring(unsigned long value) noexcept;

/// Converts `value` to a `static_wstring`
static_wstring<std::numeric_limits<unsigned long long>::digits10 + 1>
inline
to_static_wstring(unsigned long long value) noexcept;

/// Converts `value` to a `static_wstring`
static_wstring<std::numeric_limits<float>::max_digits10 + 1>
inline
to_static_wstring(float value) noexcept;

/// Converts `value` to a `static_wstring`
static_wstring<std::numeric_limits<double>::max_digits10 + 1>
inline
to_static_wstring(double value) noexcept;

/// Converts `value` to a `static_wstring`
static_wstring<std::numeric_limits<long double>::max_digits10 + 1>
inline
to_static_wstring(long double value) noexcept;

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
namespace std
{
  template <std::size_t N, typename CharT, typename Traits>
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
        using sv = boost::static_string::basic_string_view<CharT, Traits>;
        return std::hash<sv>()(sv(str.data(), str.size()));
#endif
    }
  };
} // std

#include <boost/static_string/impl/static_string.hpp>
#endif
