//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/fixed_string
//

#ifndef BOOST_FIXED_STRING_FIXED_STRING_HPP
#define BOOST_FIXED_STRING_FIXED_STRING_HPP

#include <boost/fixed_string/config.hpp>
#include <boost/fixed_string/detail/fixed_string.hpp>
#include <algorithm>
#include <cstdint>
#include <initializer_list>
#include <iosfwd>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace boost {
namespace fixed_string {

/** A fixed-capacity string.

    These objects behave like `std::string` except that the storage
    is not dynamically allocated but rather fixed in size, and
    stored in the object itself.

    These strings offer performance advantages when an algorithm
    can execute with a reasonable upper limit on the size of a value.

    @see to_fixed_string 
*/
template<
    std::size_t N,
    typename CharT = char,
    typename Traits = std::char_traits<CharT>>
class fixed_string
{
    template<std::size_t, class, class>
    friend class fixed_string;

    void
    term()
    {
        Traits::assign(s_[n_], 0);
    }

    std::size_t n_;
    CharT s_[N + 1];

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
        string_view;

    //--------------------------------------------------------------------------
    //
    // Constants
    //
    //--------------------------------------------------------------------------

    /// Maximum size of the string excluding any null terminator
    static std::size_t constexpr static_capacity = N;

    /// A special index
    static constexpr size_type npos = size_type(-1);

    //--------------------------------------------------------------------------
    //
    // Construction
    //
    //--------------------------------------------------------------------------

    /// Construct an empty string
    fixed_string();

    /** Construct the string with `count` copies of character `ch`.
    
        The behavior is undefined if `count >= npos`
    */
    fixed_string(
        size_type count,
        CharT ch);

    /// Construct with a substring (pos, other.size()) of `other`.
    template<std::size_t M>
    fixed_string(
        fixed_string<M, CharT, Traits> const& other,
        size_type pos);

    /// Construct with a substring (pos, count) of `other`.
    template<std::size_t M>
    fixed_string(
        fixed_string<M, CharT, Traits> const& other,
        size_type pos,
        size_type count);

    /// Construct with the first `count` characters of `s`, including nulls.
    fixed_string(
        CharT const* s,
        size_type count);

    /// Construct from a null terminated string.
    fixed_string(
        CharT const* s);

    /// Construct from a range of characters
    template<class InputIt>
    fixed_string(
        InputIt first,
        InputIt last
    #ifndef GENERATING_DOCUMENTATION
        , typename std::enable_if<
            detail::is_input_iterator<InputIt>::value,
                iterator>::type* = 0
    #endif
    );

    /// Copy constructor.
    fixed_string(
        fixed_string const& other);

    /// Copy constructor.
    template<std::size_t M>
    fixed_string(
        fixed_string<M, CharT, Traits> const& other);

    /// Construct from an initializer list
    fixed_string(
        std::initializer_list<CharT> init);

    /// Construct from a `string_view`

    explicit
    fixed_string(
        string_view_type sv);

    /** Construct from any object convertible to `string_view_type`.

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
    fixed_string(
        T const& t,
        size_type pos,
        size_type n);

    //--------------------------------------------------------------------------
    //
    // Assignment
    //
    //--------------------------------------------------------------------------

    /** Copy assignment

        If `*this` and `s` are the same object,
        this function has no effect.
    */
    fixed_string&
    operator=(
        fixed_string const& s) noexcept
    {
        return assign(s);
    }

    /** Replace the contents with a copy of `s`

        @throw std::length_error if `s.size() > max_size()`
    */
    template<std::size_t M>
    fixed_string&
    operator=(
        fixed_string<M, CharT, Traits> const& s)
    {
        return assign(s);
    }

    /** Replace the contents with those of the null-terminated string `s`

        @throw std::length_error if `Traits::length(s) > max_size()`
    */
    fixed_string&
    operator=(
        CharT const* s)
    {
        return assign(s);
    }

    /// Assign from single character.
    fixed_string&
    operator=(
        CharT ch)
    {
        return assign_char(ch,
            std::integral_constant<bool, (N>0)>{});
    }

    /// Assign from initializer list.
    fixed_string&
    operator=(
        std::initializer_list<CharT> ilist)
    {
        return assign(ilist);
    }

    /// Assign from `string_view_type`.
    fixed_string&
    operator=(
        string_view_type sv)
    {
        return assign(sv);
    }

    /** Replace the contents with `count` copies of character `ch`

        @throw std::length_error if `count > max_size()`
        @return `*this`
    */
    fixed_string&
    assign(
        size_type count,
        CharT ch);

    /** Replace the contents with a copy of another `fixed_string`

        @return `*this`
    */
    fixed_string&
    assign(
        fixed_string const& s) noexcept;

    /** Replace the contents with a copy of another `fixed_string`

        @throw std::length_error if `s.size() > max_size()`
        @return `*this`
    */
    template<std::size_t M>
    fixed_string&
    assign(
        fixed_string<M, CharT, Traits> const& s)
    {
        // VFALCO this could come in two flavors,
        //        N>M and N<M, and skip the exception
        //        check when N>M
        return assign(s.data(), s.size());
    }

    /** Replace the contents with a copy of `count` characters starting at `npos` from `s`.

        @throw std::length_error if `count > max_size()`
        @return `*this`
    */
    template<std::size_t M>
    fixed_string&
    assign(
        fixed_string<M, CharT, Traits> const& s,
        size_type pos,
        size_type count = npos);

    /** Replace the contents with the first `count` characters of `s`, including nulls.

        @throw std::length_error if `count > max_size()`
        @return `*this`
    */
    fixed_string&
    assign(
        CharT const* s,
        size_type count);

    /** Replace the contents with a copy of a null terminated string `s`

        @throw std::length_error if `Traits::length(s) > max_size()`
        @return `*this`
    */
    fixed_string&
    assign(
        CharT const* s)
    {
        return assign(s, Traits::length(s));
    }

    /** Replace the contents with a copy of characters from the range `(first, last)`
      
        @throw std::length_error if `std::distance(first, last) > max_size()`
        @return `*this`
    */
    template<typename InputIt>
#ifdef GENERATING_DOCUMENTATION
    fixed_string&
#else
    typename std::enable_if<
        detail::is_input_iterator<InputIt>::value,
            fixed_string&>::type
#endif
    assign(
        InputIt first,
        InputIt last);

    /** Replace the contents with the characters in an initializer list

        @throw std::length_error if `ilist.size() > max_size()`
        @return `*this`
    */
    fixed_string&
    assign(
        std::initializer_list<CharT> ilist)
    {
        return assign(ilist.begin(), ilist.end());
    }

    /** Replace the contents with a copy of the characters from `string_view_type{t}`

        @throw std::length_error if `string_view_type{t}.size() > max_size()`
    */
    template<typename T>
#if GENERATING_DOCUMENTATION
    fixed_string&
#else
    typename std::enable_if<
        std::is_convertible<T, string_view_type>::value &&
        ! std::is_convertible<T, CharT const*>::value,
            fixed_string&>::type
#endif
    assign(T const& t)
    {
        string_view_type ss{t};
        return assign(ss.data(), ss.size());
    }

    /** Replace the contents with a copy of the characters from `string_view_type{t}.substr(pos, count)`

        The range `[pos, count)` is extracted from the value
        obtained by converting `t` to `string_view_type`,
        and used to assign the string.

        @throw std::out_of_range if `pos > string_view_type{t}.size()`
        @throw std::length_error if `string_view_type{t}.substr(pos, count).size() > max_size()`
    */

    template<typename T>
#if GENERATING_DOCUMENTATION
    fixed_string&
#else
    typename std::enable_if<
        std::is_convertible<T, string_view_type>::value &&
        ! std::is_convertible<T, CharT const*>::value,
            fixed_string&>::type
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
    reference
    at(size_type pos);

    /** Access specified character with bounds checking.

        @throw std::out_of_range if `pos >= size()`
    */
    const_reference
    at(size_type pos) const;

    /** Access specified character

        Undefined behavior if `pos > size()`
    */
    reference
    operator[](size_type pos)
    {
        return s_[pos];
    }

    /** Access specified character.

        Undefined behavior if `pos > size()`
    */
    const_reference
    operator[](size_type pos) const
    {
        return s_[pos];
    }

    /** Accesses the first character.

        Undefined behavior if `empty() == true`
    */
    CharT&
    front()
    {
        return s_[0];
    }

    /** Accesses the first character.

        Undefined behavior if `empty() == true`
    */
    CharT const&
    front() const
    {
        return s_[0];
    }

    /** Accesses the last character.

        Undefined behavior if `empty() == true`
    */
    CharT&
    back()
    {
        return s_[n_-1];
    }

    /** Accesses the last character.

        Undefined behavior if `empty() == true`
    */
    CharT const&
    back() const 
    {
        return s_[n_-1];
    }

    /// Returns a pointer to the first character of the string.
    CharT*
    data() noexcept
    {
        return &s_[0];
    }

    /// Returns a pointer to the first character of a string.
    CharT const*
    data() const noexcept
    {
        return &s_[0];
    }

    /// Returns a non-modifiable standard C character array version of the string.
    CharT const*
    c_str() const noexcept
    {
        return data();
    }

    /// Convert a static string to a `string_view_type`
    operator string_view_type() const noexcept
    {
        return basic_string_view<
            CharT, Traits>{data(), size()};
    }

    //--------------------------------------------------------------------------
    //
    // Iterators
    //
    //--------------------------------------------------------------------------

    /// Returns an iterator to the beginning.
    iterator
    begin() noexcept
    {
        return &s_[0];
    }

    /// Returns an iterator to the beginning.
    const_iterator
    begin() const noexcept
    {
        return &s_[0];
    }

    /// Returns an iterator to the beginning.
    const_iterator
    cbegin() const noexcept
    {
        return &s_[0];
    }

    /// Returns an iterator to the end.
    iterator
    end() noexcept
    {
        return &s_[n_];
    }

    /// Returns an iterator to the end.
    const_iterator
    end() const noexcept
    {
        return &s_[n_];
    }

    /// Returns an iterator to the end.
    const_iterator
    cend() const noexcept
    {
        return &s_[n_];
    }

    /// Returns a reverse iterator to the beginning.
    reverse_iterator
    rbegin() noexcept
    {
        return reverse_iterator{end()};
    }

    /// Returns a reverse iterator to the beginning.
    const_reverse_iterator
    rbegin() const noexcept
    {
        return const_reverse_iterator{cend()};
    }

    /// Returns a reverse iterator to the beginning.
    const_reverse_iterator
    crbegin() const noexcept
    {
        return const_reverse_iterator{cend()};
    }

    /// Returns a reverse iterator to the end.
    reverse_iterator
    rend() noexcept
    {
        return reverse_iterator{begin()};
    }

    /// Returns a reverse iterator to the end.
    const_reverse_iterator
    rend() const noexcept
    {
        return const_reverse_iterator{cbegin()};
    }

    /// Returns a reverse iterator to the end.
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
    BOOST_FIXED_STRING_NODISCARD
    bool
    empty() const
    {
        return n_ == 0;
    }

    /// Returns the number of characters, excluding the null terminator.
    size_type
    size() const
    {
        return n_;
    }

    /** Returns the number of characters, excluding the null terminator

        Equivalent to calling `size()`.
    */
    size_type
    length() const
    {
        return size();
    }

    /// Returns the maximum number of characters that can be stored, excluding the null terminator.
    size_type constexpr
    max_size() const
    {
        return N;
    }

    /** Reserve space for `n` characters, excluding the null terminator

        This function has no effect when `n <= max_size()`.

        @throw std::length_error if `n > max_size()`
    */
    void
    reserve(std::size_t n);

    /** Returns the number of characters that can be held in currently allocated storage.

        This function always returns `max_size()`.
    */
    size_type constexpr
    capacity() const
    {
        return max_size();
    }
    
    /** Reduces memory usage by freeing unused memory.

        This function call has no effect.
    */
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
    void
    clear();

    /** Inserts `count` copies of character `ch` at the position `index`

        @throw std::out_of_range if `index > size()`
        @throw std::length_error if `size() + count > max_size()`
        @return `*this`
    */
    fixed_string&
    insert(
        size_type index,
        size_type count,
        CharT ch);
    
    /** Inserts null-terminated string pointed to by `s` at the position `index`

        @throw std::out_of_range if `index > size()`
        @throw std::length_error if `size() + count > max_size()`
        @return `*this`
    */
    fixed_string&
    insert(
        size_type index,
        CharT const* s)
    {
        return insert(index, s, Traits::length(s));
    }

    /** Inserts the characters in the range `[s, s+count]` at the position `index`

        The inserted string can contain null characters.

        @throw std::out_of_range if `index > size()`
        @throw std::length_error if ` size() + count> max_size()`
        @return `*this`
    */
    fixed_string&
    insert(
        size_type index,
        CharT const* s,
        size_type count);

    /** Inserts the contents of string view `sv` at the position `index`

        The inserted string can contain null characters.

        @throw std::out_of_range if `index > size()`
        @throw std::length_error if `size() + sv.size() > max_size()`
        @return `*this`
    */
    fixed_string&
    insert(
        size_type index,
        string_view_type sv)
    {
        return insert(index, sv.data(), sv.size());
    }

    /** Inserts the string `sv.substr(index_str, count)` at the position `index`

        The inserted string can contain null characters.

        @throw std::out_of_range if `index > size() || index_str > sv.size()`
        @throw std::length_error if `size() + sv.substr(index_str, count).size() > max_size()`
        @return `*this`
    */
    fixed_string&
    insert(
        size_type index,
        string_view_type sv,
        size_type index_str,
        size_type count = npos)
    {
        return insert(index, sv.substr(index_str, count));
    }

    /** Inserts character `ch` before the character (if any) pointed by `pos`

        The inserted character can be null.

        @throw std::length_error if `size() + 1> max_size()`
        @return An iterator to the first inserted character or pos if no insertion took place
    */
    iterator
    insert(
        const_iterator pos,
        CharT ch)
    {
        return insert(pos, 1, ch);
    }

    /** Inserts `count` copies of character `ch` before the character (if any) pointed by `pos`

        The inserted characters can be null.

        @throw std::length_error if `size() + count > max_size()`
        @return An iterator to the first inserted character or pos if no insertion took place
    */
    iterator
    insert(
        const_iterator pos,
        size_type count,
        CharT ch);

    /** Inserts characters from the range `(first, last)` before the element (if any) pointed by `pos`

        The inserted string can contain null characters.
        This function does not participate in overload resolution if
        `InputIt` does not satisfy <em>LegacyInputIterator</em>

        @throw std::length_error if `size() + std::distance(first, last) > max_size()`
        @return An iterator to the first inserted character or pos if no insertion took place
    */
    template<typename InputIt>
#if GENERATING_DOCUMENTATION
    iterator
#else
    typename std::enable_if<
        detail::is_input_iterator<InputIt>::value,
            iterator>::type
#endif
    insert(
        const_iterator pos,
        InputIt first,
        InputIt last);

    /** Inserts elements from initializer list `ilist` before the element (if any) pointed by `pos`

        The inserted string can contain null characters.

        @throw std::length_error if `size() + ilist.size() > max_size()`
        @return An iterator to the first inserted character or pos if no insertion took place
    */
    iterator
    insert(
        const_iterator pos,
        std::initializer_list<CharT> ilist)
    {
        return insert(pos, ilist.begin(), ilist.end());
    }

    /** Inserts elements from `string_view{t}` at the position `index`

        The inserted string can contain null characters.
        This function participates in overload resolution if
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.

        @throw std::length_error if `size() + string_view{t}.substr(index_str, count).size() > max_size()`
        @return `*this`
    */
    template<typename T>
#if GENERATING_DOCUMENTATION
    fixed_string&
#else
    typename std::enable_if<
        std::is_convertible<
            T const&, string_view_type>::value &&
        ! std::is_convertible<
            T const&, CharT const*>::value,
                fixed_string&>::type
#endif
    insert(
        size_type index,
        T const& t);

    /** Inserts elements from `string_view{t}.substr(index_str, count)` at the position `index`

        The inserted string can contain null characters.
        This function participates in overload resolution if
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.

        @throw std::out_of_range if `index_str > string_view{t}.size()`
        @throw std::length_error if `size() + string_view{t}.substr(index_str, count).size() > max_size()`
        @return `*this`
    */
    template<typename T>
#if GENERATING_DOCUMENTATION
    fixed_string&
#else
    typename std::enable_if<
        std::is_convertible<T const&, string_view_type>::value &&
        ! std::is_convertible<T const&, CharT const*>::value,
            fixed_string&>::type
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
    fixed_string&
    erase(
        size_type index = 0,
        size_type count = npos);

    /** Removes the character at `pos`

        @return iterator pointing to the character immediately following the character erased, or `end()` if no such character exists
    */
    iterator
    erase(
        const_iterator pos);

    /** Removes the characters in the range `(first, last)`

        @return iterator pointing to the character last pointed to before the erase, or `end()` if no such character exists
    */
    iterator
    erase(
        const_iterator first,
        const_iterator last);

    /** Appends the given character `ch` to the end of the string.

        @throw std::length_error if `size() + 1 > max_size()`
    */
    void
    push_back(
        CharT ch);

    /** Removes the last character from the string

        The behavior is undefined if the string is empty.
    */
    void
    pop_back()
    {
      BOOST_FIXED_STRING_ASSERT(n_ > 0);
      Traits::assign(s_[--n_], 0);
    }

    /** Appends `count` copies of character `ch`

        The appended characters may be null.

        @throw std::length_error if `size() + count > max_size()`
        @return `*this`
    */
    fixed_string&
    append(
        size_type count,
        CharT ch)
    {
        return insert(n_, count, ch);
    }

    /** Appends the contents of string view `sv`

        The appended string can contain null characters.

        @throw std::length_error if `size() + sv.size() > max_size()`
        @return `*this`
    */
    fixed_string&
    append(
        string_view_type sv)
    {
        return append(sv.data(), sv.size());
    }

    /** Appends the contents of `sv.substr(pos, count)`

        The appended string can contain null characters.

        @throw std::out_of_range if `pos > sv.size()`
        @throw std::length_error if `size() + sv.substr(pos, count).size() > max_size()`
        @return `*this`
    */
    fixed_string&
    append(
        string_view_type sv,
        size_type pos,
        size_type count = npos)
    {
        return append(sv.substr(pos, count));
    }

    /** Appends characters in the range `(s, s + count)`
    
        The appended string can contain null characters.

        @throw std::length_error if `size() + count > max_size()`
        @return `*this`
    */
    fixed_string&
    append(
        CharT const* s,
        size_type count);

    /** Appends the null-terminated character string pointed to by `s`
    
        The length of the string is determined by the first
        null character using `Traits::length(s)`.

        @throw std::length_error if `size() + Traits::length(s) > max_size()`
        @return `*this`
    */
    fixed_string&
    append(
        CharT const* s)
    {
        return append(s, Traits::length(s));
    }

    /** Appends characters from the range `(first, last)`

        The inserted string can contain null characters.
        This function does not participate in overload resolution if
        `InputIt` does not satisfy <em>LegacyInputIterator</em>

        @throw std::length_error if `size() + std::distance(first, last) > max_size()`
        @return `*this`
    */
    template<typename InputIt>
#if GENERATING_DOCUMENTATION
    fixed_string&
#else
    typename std::enable_if<
        detail::is_input_iterator<InputIt>::value,
            fixed_string&>::type
#endif
    append(
        InputIt first,
        InputIt last)
    {
        insert(end(), first, last);
        return *this;
    }

    /** Appends characters from initializer list `ilist`

        The appended string can contain null characters.

        @throw std::length_error if `size() + ilist.size() > max_size()`
        @return `*this`
    */
    fixed_string&
    append(
        std::initializer_list<CharT> ilist)
    {
        insert(end(), ilist);
        return *this;
    }

    /** Appends characters from `string_view{t}`

        The appended string can contain null characters.
        This function participates in overload resolution if
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.

        @throw std::length_error if `size() + string_view{t} > max_size()`
        @return `*this`
    */
    template<typename T>
#if GENERATING_DOCUMENTATION
    fixed_string&
#else
    typename std::enable_if<
        std::is_convertible<
            T const&, string_view_type>::value &&
        ! std::is_convertible<
            T const&, CharT const*>::value,
                fixed_string&>::type
#endif
    append(
        T const& t)
    {
        return append(string_view{t});
    }

    /** Appends characters from `string_view{t}.substr{pos, count}`

        The appended string can contain null characters.
        This function participates in overload resolution if
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.

        @throw std::out_of_range if `pos > string_view{t}.size()`
        @throw std::length_error if `size() + string_view{t}.substr(pos, count).size() > max_size()`
        @return `*this`
    */
    template<typename T>
#if GENERATING_DOCUMENTATION
    fixed_string&
#else
    typename std::enable_if<
        std::is_convertible<
            T const&, string_view_type>::value &&
        ! std::is_convertible<
            T const&, CharT const*>::value,
                fixed_string&>::type
#endif
    append(
        T const& t,
        size_type pos,
        size_type count = npos)
    {
        return append(string_view{t}.substr(pos, count));
    }

    template<std::size_t M>
    fixed_string&
    operator+=(
        fixed_string<M, CharT, Traits> const& s)
    {
        return append(s.data(), s.size());
    }

    /** Appends the given character `ch` to the end of the string.

        @throw std::length_error if `size() + 1 > max_size()`
    */
    fixed_string&
    operator+=(
        CharT ch)
    {
        push_back(ch);
        return *this;
    }

    /** Appends the null-terminated character string pointed to by `s`
    
        The length of the string is determined by the first
        null character using `Traits::length(s)`.

        @throw std::length_error if `size() + Traits::length(s) > max_size()`
        @return `*this`
    */
    fixed_string&
    operator+=(
        CharT const* s)
    {
        return append(s);
    }

    /** Appends characters from initializer list `ilist`

        The appended string can contain null characters.

        @throw std::length_error if `size() + ilist.size() > max_size()`
        @return `*this`
    */
    fixed_string&
    operator+=(
        std::initializer_list<CharT> ilist)
    {
        return append(ilist);
    }

    /** Appends a copy of the characters from `string_view_type{t}`

        The appended string can contain null characters.
        This function participates in overload resolution if
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.

        @throw std::length_error if `string_view_type{t}.size() > max_size()`
    */
    template<typename T>
#if GENERATING_DOCUMENTATION
    fixed_string&
#else
    typename std::enable_if<
        std::is_convertible<T, string_view_type>::value &&
        ! std::is_convertible<T, CharT const*>::value,
            fixed_string&>::type
#endif
    operator+=(
        T const& t)
    {
        return append(t);
    }

    template<std::size_t M>
    int
    compare(
        fixed_string<M, CharT, Traits> const& s) const
    {
        return detail::lexicographical_compare<CharT, Traits>(
            &s_[0], n_, &s.s_[0], s.n_);
    }

    template<std::size_t M>
    int
    compare(
        size_type pos1,
        size_type count1,
        fixed_string<M, CharT, Traits> const& s) const
    {
        return detail::lexicographical_compare<CharT, Traits>(
            substr(pos1, count1), s.data(), s.size());
    }

    template<std::size_t M>
    int
    compare(
        size_type pos1,
        size_type count1,
        fixed_string<M, CharT, Traits> const& s,
        size_type pos2,
        size_type count2 = npos) const
    {
        return detail::lexicographical_compare(
            substr(pos1, count1), s.substr(pos2, count2));
    }

    int
    compare(
        CharT const* s) const
    {
        return detail::lexicographical_compare<CharT, Traits>(
            &s_[0], n_, s, Traits::length(s));
    }

    int
    compare(
        size_type pos1,
        size_type count1,
        CharT const* s) const
    {
        return detail::lexicographical_compare<CharT, Traits>(
            substr(pos1, count1), s, Traits::length(s));
    }

    int
    compare(
        size_type pos1,
        size_type count1,
        CharT const*s,
        size_type count2) const
    {
        return detail::lexicographical_compare<CharT, Traits>(
            substr(pos1, count1), s, count2);
    }

    int
    compare(
        string_view_type s) const
    {
        return detail::lexicographical_compare<CharT, Traits>(
            &s_[0], n_, s.data(), s.size());
    }

    int
    compare(
        size_type pos1,
        size_type count1,
        string_view_type s) const
    {
        return detail::lexicographical_compare<CharT, Traits>(
            substr(pos1, count1), s);
    }

    template<typename T>
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

    string_view_type
    substr(
        size_type pos = 0,
        size_type count = npos) const;

    /// Copy a substring (pos, pos+count) to character string pointed to by `dest`.
    size_type
    copy(
        CharT* dest,
        size_type count,
        size_type pos = 0) const;

    /** Changes the number of characters stored.

        If the resulting string is larger, the new
        characters are uninitialized.
    */
    void
    resize(
        std::size_t n);

    /** Changes the number of characters stored.

        If the resulting string is larger, the new
        characters are initialized to the value of `c`.
    */
    void
    resize(
        std::size_t n,
        CharT c);

    /// Exchange the contents of this string with another.
    void
    swap(
        fixed_string& s);

    /// Exchange the contents of this string with another.
    template<std::size_t M>
    void
    swap(
        fixed_string<M, CharT, Traits>& s);

    template<size_t N>
    fixed_string&
    replace(
        size_type pos1,
        size_type n1,
        const fixed_string<N, CharT, Traits>& str)
    {
      return replace(pos1, n1, str.data(), str.size());
    }

    
    fixed_string&
    replace(
        size_type pos1,
        size_type n1,
        const fixed_string& str,
        size_type pos2,
        size_type n2 = npos)
    {
      return replace(pos1, n1, str.substr(pos2, n2));
    }

    template<typename T>
#if GENERATING_DOCUMENTATION
    fixed_string&
#else
    typename std::enable_if<
        std::is_convertible<T, string_view_type>::value &&
        ! std::is_convertible<T, CharT const*>::value,
            fixed_string&>::type
#endif
    replace(
        size_type pos1,
        size_type n1,
        const T& t)
    {
      string_view_type sv = t;
      return replace(pos1, n1, sv.data(), sv.size());
    }

    template<typename T>
#if GENERATING_DOCUMENTATION
    fixed_string&
#else
    typename std::enable_if<
        std::is_convertible<T, string_view_type>::value &&
        ! std::is_convertible<T, CharT const*>::value,
            fixed_string&>::type
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

    fixed_string&
    replace(
        size_type pos,
        size_type n1,
        const CharT* s,
        size_type n2);

    fixed_string&
    replace(
        size_type pos,
        size_type n1,
        const CharT* s)
    {
      return replace(pos, n1, s, Traits::length(s));
    }

    fixed_string&
    replace(
        size_type pos,
        size_type n1,
        size_type n2,
        CharT c);

    fixed_string&
    replace(
        const_iterator i1,
        const_iterator i2,
        const fixed_string& str)
    {
      return replace(i1, i2, str.data(), str.size());
    }

    template<typename T>
#if GENERATING_DOCUMENTATION
    fixed_string&
#else
    typename std::enable_if<
        std::is_convertible<T, string_view_type>::value &&
        ! std::is_convertible<T, CharT const*>::value,
            fixed_string&>::type
#endif
    replace(
        const_iterator i1,
        const_iterator i2,
        const T& t)
    {
      string_view_type sv = t;
      return replace(i1 - begin(), i2 - i1, sv.data(), sv.size());
    }

    fixed_string&
    replace(
        const_iterator i1,
        const_iterator i2,
        const CharT* s,
        size_type n)
    {
      return replace(i1 - begin(), i2 - i1, s, n);
    }

    fixed_string&
    replace(
        const_iterator i1,
        const_iterator i2,
        const CharT* s)
    {
      return replace(i1, i2, s, Traits::length(s));
    }

    fixed_string&
    replace(
        const_iterator i1,
        const_iterator i2,
        size_type n,
        CharT c)
    {
      return replace(i1 - begin(), i2 - i1, n, c);
    }

    template<typename InputIterator>
#if GENERATING_DOCUMENTATION
    fixed_string&
#else
    typename std::enable_if<
        detail::is_input_iterator<InputIterator>::value,
            fixed_string&>::type
#endif
    replace(
        const_iterator i1,
        const_iterator i2,
        InputIterator j1,
        InputIterator j2)
    {
      return replace(i1, i2, fixed_string(j1, j2));
    }

    fixed_string&
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

    /** Finds the first substring equal to `t`.

        This function participates in overload resolution if
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.
    */
    template<class T>
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
        size_type pos = 0) const 
    {
      string_view_type sv = t;
      return find(sv.data(), pos, sv.size());
    }
    
    template<std::size_t N>
    size_type
      find(
        const fixed_string<N, CharT, Traits>& str,
        size_type pos = 0) const noexcept
    {
      return find(str.data(), pos, str.size());
    }

    // Finds the first substring equal to the range `[s, s + count)`. This range may contain null characters.
    size_type
    find(
        const CharT* s, 
        size_type pos,
        size_type n) const;

    /** Finds the first substring equal to the character
        string pointed to by `s`.
    
        The length of the string is determined by the first
        null character using `Traits::length(s)`
    */
    size_type
    find(
        const CharT* s,
        size_type pos = 0) const
    {
      return find(s, pos, Traits::length(s));
    }

    // Finds the first character `c`
    size_type
    find(
        CharT c,
        size_type pos = 0) const noexcept
    {
      return find(&c, pos, 1);
    }


    /** Finds the last substring equal to `t`.

        This function participates in overload resolution if
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.
    */
    template<class T>
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
        size_type pos = npos) const
    {
      string_view_type sv = t;
      return rfind(sv.data(), pos, sv.size());
    }

    template<std::size_t N>
    size_type
      rfind(
        const fixed_string<N, CharT, Traits>& str,
        size_type pos = npos) const noexcept
    {
      return rfind(str.data(), pos, str.size());
    }

    // Finds the last substring equal to the range `[s, s + count)`. This range may contain null characters.
    size_type
    rfind(
        const CharT* s, 
        size_type pos,
        size_type n) const;

    /** Finds the last substring equal to the character
        string pointed to by `s`.

        The length of the string is determined by the first
        null character using `Traits::length(s)`
    */
    size_type
    rfind(
        const CharT* s,
        size_type pos = npos) const
    {
      return rfind(s, pos, Traits::length(s));
    }

    // Finds the last character `c`
    size_type
    rfind(
        CharT c,
        size_type pos = npos) const noexcept
    {
      return rfind(&c, pos, 1);
    }

    /** Finds the first character equal to one of the characters in `t`.

        This function participates in overload resolution if
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.
    */
    template<class T>
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
        size_type pos = 0) const
    {
      string_view_type sv = t;
      return find_first_of(sv.data(), pos, sv.size());
    }

    template<std::size_t N>
    size_type
      find_first_of(
        const fixed_string<N, CharT, Traits>& str,
        size_type pos = 0) const noexcept
    {
      return find_first_of(str.data(), pos, str.size());
    }

    // Finds the first character equal to one of the characters in the range `[s, s + count)`. This range can include null characters.
    size_type
    find_first_of(
        const CharT* s,
        size_type pos,
        size_type n) const;

    /** Finds the first character equal to one of the characters
        in character string pointed to by `s`. 
     
        The length of the string is determined by the first
        null character using `Traits::length(s)`
    */
    size_type
    find_first_of(
        const CharT* s,
        size_type pos = 0) const
    {
      return find_first_of(s, pos, Traits::length(s));
    }

    // Finds the first character equal to `c`.
    size_type
    find_first_of(
        CharT c,
        size_type pos = 0) const noexcept
    {
      return find_first_of(&c, pos, 1);
    }

    /** Finds the last character equal to one of the characters in `t`.

        This function participates in overload resolution if
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.
    */
    template<class T>
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
        size_type pos = npos) const
    {
      string_view_type sv = t;
      return find_last_of(sv.data(), pos, sv.size());
    }
     
    template<std::size_t N>
    size_type
      find_last_of(
        const fixed_string<N, CharT, Traits>& str,
        size_type pos = npos) const noexcept
    {
      return find_last_of(str.data(), pos, str.size());
    }

    // Finds the last character equal to one of the characters in the range `[s, s + count)`. This range can include null characters.
    size_type
    find_last_of(
        const CharT* s,
        size_type pos,
        size_type n) const;

    /** Finds the last character equal to one of the characters
        in character string pointed to by `s`.

        The length of the string is determined by the first
        null character using `Traits::length(s)`
    */
    size_type
    find_last_of(
        const CharT* s,
        size_type pos = npos) const
    {
      return find_last_of(s, pos, Traits::length(s));
    }

    // Finds the last character equal to `c`.
    size_type
    find_last_of(
        CharT c,
        size_type pos = npos) const noexcept
    {
      return find_last_of(&c, pos, 1);
    }

    /** Finds the first character equal to none of characters in `t`.

        This function participates in overload resolution if
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.
    */
    template<class T>
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
        size_type pos = 0) const
    {
      string_view_type sv = t;
      return find_first_not_of(sv.data(), pos, sv.size());
    }

    template<std::size_t N>
    size_type
      find_first_not_of(
        const fixed_string<N, CharT, Traits>& str,
        size_type pos = 0) const noexcept
    {
      return find_first_not_of(str.data(), pos, str.size());
    }

    // Finds the first character equal to none of characters in range `[s, s + count)`. This range can include null characters.
    size_type
    find_first_not_of(
        const CharT* s,
        size_type pos, 
        size_type n) const;

    /** Finds the first character equal to none of characters in
        character string pointed to by `s`.

        The length of the string is determined by the first
        null character using `Traits::length(s)`
    */
    size_type
    find_first_not_of(
        const CharT* s,
        size_type pos = 0) const
    {
      return find_first_not_of(s, pos, Traits::length(s));
    }

    // Finds the first character not equal to `c`.
    size_type
    find_first_not_of(
        CharT c,
        size_type pos = 0) const noexcept
    {
      return find_first_not_of(&c, pos, 1);
    }

    /** Finds the last character equal to none of characters in `t`.

        This function participates in overload resolution if
        `T` is convertible to `string_view` and `T` is not
        convertible to `CharT const*`.
    */
    template<class T>
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
        size_type pos = npos) const
    {
      string_view_type sv = t;
      return find_last_not_of(sv.data(), pos, sv.size());
    }

    template<size_t N>
    size_type
      find_last_not_of(
        const fixed_string<N, CharT, Traits>& str,
        size_type pos = npos) const noexcept
    {
      return find_last_not_of(str.data(), pos, str.size());
    }

    // Finds the last character equal to none of characters in range `[s, s + count)`. This range can include null characters.
    size_type
    find_last_not_of(
        const CharT* s,
        size_type pos,
        size_type n) const;


    /** Finds the last character equal to none of characters in
        character string pointed to by `s`.

        The length of the string is determined by the first
        null character using `Traits::length(s)`
    */
    size_type
    find_last_not_of(
        const CharT* s,
        size_type pos = npos) const
    {
      return find_last_not_of(s, pos, Traits::length(s));
    }

    // Finds the last character not equal to `c`.
    size_type
    find_last_not_of(
        CharT c,
        size_type pos = npos) const noexcept
    {
      return find_last_not_of(&c, pos, 1);
    }

    bool 
    starts_with(
        string_view_type s) const noexcept
    {
      return starts_with(s.data());
    }
    
    bool 
    starts_with(
        CharT c) const noexcept
    {
      return !empty() && Traits::eq(front(), c);
    }
    
    bool 
    starts_with(
        const CharT* s) const
    {
      const size_type len = Traits::length(s);
      return n_ >= len && !Traits::compare(s_, s, len);
    }
    
    bool 
    ends_with(
        string_view_type s) const noexcept
    {
      return ends_with(s.data());
    }
    
    bool
    ends_with(
        CharT c) const noexcept
    {
      return !empty() && Traits::eq(back(), c);
    }
    
    bool 
    ends_with(
        const CharT* s) const
    {
      const size_type len = Traits::length(s);
      return n_ >= len && !Traits::compare(s_ + (n_ - len), s, len);
    }

private:
    fixed_string&
    assign_char(CharT ch, std::true_type);

    fixed_string&
    assign_char(CharT ch, std::false_type);
};

//------------------------------------------------------------------------------
//
// Disallowed operations
//
//------------------------------------------------------------------------------

// These operations are explicitly deleted since
// there is no reasonable implementation possible.

template<
    std::size_t N, std::size_t M,
    typename CharT, typename Traits>
inline
void
operator+(
    fixed_string<N, CharT, Traits>const& lhs,
    fixed_string<M, CharT, Traits>const& rhs) = delete;

template<std::size_t N, typename CharT, typename Traits>
inline
void
operator+(
    CharT const* lhs,
    fixed_string<N, CharT, Traits>const& rhs) = delete;

template<std::size_t N, typename CharT, typename Traits>
inline
void
operator+(
    CharT lhs,
    fixed_string<N, CharT, Traits> const& rhs) = delete;

template<std::size_t N, typename CharT, typename Traits>
inline
void
operator+(
    fixed_string<N, CharT, Traits> const& lhs,
    CharT const* rhs) = delete;

template<std::size_t N, typename CharT, typename Traits>
inline
void
operator+(
    fixed_string<N, CharT, Traits> const& lhs,
    CharT rhs) = delete;

//------------------------------------------------------------------------------
//
// Non-member functions
//
//------------------------------------------------------------------------------

template<
    std::size_t N, std::size_t M,
    typename CharT, typename Traits>
inline
bool
operator==(
    fixed_string<N, CharT, Traits> const& lhs,
    fixed_string<M, CharT, Traits> const& rhs)
{
    return lhs.compare(rhs) == 0;
}

template<
    std::size_t N, std::size_t M,
    typename CharT, typename Traits>
inline
bool
operator!=(
    fixed_string<N, CharT, Traits> const& lhs,
    fixed_string<M, CharT, Traits> const& rhs)
{
    return lhs.compare(rhs) != 0;
}

template<
    std::size_t N, std::size_t M,
    typename CharT, typename Traits>
inline
bool
operator<(
    fixed_string<N, CharT, Traits> const& lhs,
    fixed_string<M, CharT, Traits> const& rhs)
{
    return lhs.compare(rhs) < 0;
}

template<
    std::size_t N, std::size_t M,
    typename CharT, typename Traits>
inline
bool
operator<=(
    fixed_string<N, CharT, Traits> const& lhs,
    fixed_string<M, CharT, Traits> const& rhs)
{
    return lhs.compare(rhs) <= 0;
}

template<
    std::size_t N, std::size_t M,
    typename CharT, typename Traits>
inline
bool
operator>(
    fixed_string<N, CharT, Traits> const& lhs,
    fixed_string<M, CharT, Traits> const& rhs)
{
    return lhs.compare(rhs) > 0;
}

template<
    std::size_t N, std::size_t M,
    typename CharT, typename Traits>
inline
bool
operator>=(
    fixed_string<N, CharT, Traits> const& lhs,
    fixed_string<M, CharT, Traits> const& rhs)
{
    return lhs.compare(rhs) >= 0;
}

template<std::size_t N, typename CharT, typename Traits>
inline
bool
operator==(
    CharT const* lhs,
    fixed_string<N, CharT, Traits> const& rhs)
{
    return detail::lexicographical_compare<CharT, Traits>(
        lhs, Traits::length(lhs),
        rhs.data(), rhs.size()) == 0;
}

template<std::size_t N, typename CharT, typename Traits>
inline
bool
operator==(
    fixed_string<N, CharT, Traits> const& lhs,
    CharT const* rhs)
{
    return detail::lexicographical_compare<CharT, Traits>(
        lhs.data(), lhs.size(),
        rhs, Traits::length(rhs)) == 0;
}

template<std::size_t N, typename CharT, typename Traits>
inline
bool
operator!=(
    CharT const* lhs,
    fixed_string<N, CharT, Traits> const& rhs)
{
    return detail::lexicographical_compare<CharT, Traits>(
        lhs, Traits::length(lhs),
        rhs.data(), rhs.size()) != 0;
}

template<std::size_t N, typename CharT, typename Traits>
inline
bool
operator!=(
    fixed_string<N, CharT, Traits> const& lhs,
    CharT const* rhs)
{
    return detail::lexicographical_compare<CharT, Traits>(
        lhs.data(), lhs.size(),
        rhs, Traits::length(rhs)) != 0;
}

template<std::size_t N, typename CharT, typename Traits>
inline
bool
operator<(
    CharT const* lhs,
    fixed_string<N, CharT, Traits> const& rhs)
{
    return detail::lexicographical_compare<CharT, Traits>(
        lhs, Traits::length(lhs),
        rhs.data(), rhs.size()) < 0;
}

template<std::size_t N, typename CharT, typename Traits>
inline
bool
operator<(
    fixed_string<N, CharT, Traits> const& lhs,
    CharT const* rhs)
{
    return detail::lexicographical_compare<CharT, Traits>(
        lhs.data(), lhs.size(),
        rhs, Traits::length(rhs)) < 0;
}

template<std::size_t N, typename CharT, typename Traits>
inline
bool
operator<=(
    CharT const* lhs,
    fixed_string<N, CharT, Traits> const& rhs)
{
    return detail::lexicographical_compare<CharT, Traits>(
        lhs, Traits::length(lhs),
        rhs.data(), rhs.size()) <= 0;
}

template<std::size_t N, typename CharT, typename Traits>
inline
bool
operator<=(
    fixed_string<N, CharT, Traits> const& lhs,
    CharT const* rhs)
{
    return detail::lexicographical_compare<CharT, Traits>(
        lhs.data(), lhs.size(),
        rhs, Traits::length(rhs)) <= 0;
}

template<std::size_t N, typename CharT, typename Traits>
inline
bool
operator>(
    CharT const* lhs,
    fixed_string<N, CharT, Traits> const& rhs)
{
    return detail::lexicographical_compare<CharT, Traits>(
        lhs, Traits::length(lhs),
        rhs.data(), rhs.size()) > 0;
}

template<std::size_t N, typename CharT, typename Traits>
inline
bool
operator>(
    fixed_string<N, CharT, Traits> const& lhs,
    CharT const* rhs)
{
    return detail::lexicographical_compare<CharT, Traits>(
        lhs.data(), lhs.size(),
        rhs, Traits::length(rhs)) > 0;
}

template<std::size_t N, typename CharT, typename Traits>
inline
bool
operator>=(
    CharT const* lhs,
    fixed_string<N, CharT, Traits> const& rhs)
{
    return detail::lexicographical_compare<CharT, Traits>(
        lhs, Traits::length(lhs),
        rhs.data(), rhs.size()) >= 0;
}

template<std::size_t N, typename CharT, typename Traits>
inline
bool
operator>=(
    fixed_string<N, CharT, Traits> const& lhs,
    CharT const* rhs)
{
    return detail::lexicographical_compare<CharT, Traits>(
        lhs.data(), lhs.size(),
        rhs, Traits::length(rhs)) >= 0;
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
inline
void
swap(
    fixed_string<N, CharT, Traits>& lhs,
    fixed_string<N, CharT, Traits>& rhs)
{
    lhs.swap(rhs);
}

template<
    std::size_t N, std::size_t M,
    typename CharT, typename Traits>
inline
void
swap(
    fixed_string<N, CharT, Traits>& lhs,
    fixed_string<M, CharT, Traits>& rhs)
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
    fixed_string<N, CharT, Traits> const& s)
{
  return os << string_view(s);
}

//------------------------------------------------------------------------------
//
// Numeric conversions
//
//------------------------------------------------------------------------------

/** Returns a static string representing an integer as a decimal.

    @param x The signed or unsigned integer to convert.
    This must be an integral type.

    @return A @ref fixed_string with an implementation defined
    maximum size at least as large enough to hold the longest
    possible decimal representation of any integer of the given type.
*/
template<
    class Integer
#ifndef GENERATING_DOCUMENTATION
    , class = typename std::enable_if<
        std::is_integral<Integer>::value>::type
#endif
>
inline
fixed_string<detail::max_digits(sizeof(Integer))>
to_fixed_string(Integer x);

//------------------------------------------------------------------------------
//
// Deduction Guides
//
//------------------------------------------------------------------------------

#ifdef BOOST_FIXED_STRING_USE_DEDUCT
template<std::size_t N, typename CharT>
fixed_string(CharT(&)[N]) -> 
    fixed_string<N, CharT, std::char_traits<CharT>>;
#endif

} // fixed_string
} // boost

#include <boost/fixed_string/impl/fixed_string.hpp>

#endif
