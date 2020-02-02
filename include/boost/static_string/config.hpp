//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
// Copyright (c) 2019-2020 Krystian Stasiowski (sdkrystian at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/static_string
//

#ifndef BOOST_STATIC_STRING_CONFIG_HPP
#define BOOST_STATIC_STRING_CONFIG_HPP

// Are we dependent on Boost?
// #define BOOST_STATIC_STRING_STANDALONE

// Disable exceptions and their associated checks
// #define BOOST_STATIC_STRING_NO_EXCEPTIONS

// Opt-in to the null terminator optimization
// #define BOOST_STATIC_STRING_NULL_OPTIMIZATION

// Can we have deduction guides?
#if __cpp_deduction_guides >= 201703
#define BOOST_STATIC_STRING_USE_DEDUCT
#endif

// Include <version> if we can
#ifdef __has_include 
#if __has_include(<version>)
#include <version>
#endif
#endif

// Can we use is_constant_evaluated?
// Standard version
#if __cpp_lib_is_constant_evaluated >= 201811L
#define BOOST_STATIC_STRING_USE_IS_CONST_EVAL
#define BOOST_STATIC_STRING_IS_CONST_EVAL std::is_constant_evaluated()
#elif defined(__clang__) && (__clang_major__ > 8)
// If we have clang version 9+, we can use the intrinsic
// While gcc also has this, we don't need it
#define BOOST_STATIC_STRING_USE_IS_CONST_EVAL
#define BOOST_STATIC_STRING_IS_CONST_EVAL __builtin_is_constant_evaluated()
#endif

// Can we use [[nodiscard]]?
#ifdef __has_attribute
#if __has_attribute(nodiscard)
#define BOOST_STATIC_STRING_NODISCARD [[nodiscard]]
#else
#define BOOST_STATIC_STRING_NODISCARD
#endif
#else
#define BOOST_STATIC_STRING_NODISCARD
#endif

// MSVC doesn't define __cplusplus by default
#ifdef _MSVC_LANG
#define BOOST_STATIC_STRING_STANDARD_VERSION _MSVC_LANG
#else
#define BOOST_STATIC_STRING_STANDARD_VERSION __cplusplus
#endif

#if BOOST_STATIC_STRING_STANDARD_VERSION > 201703L
#define BOOST_STATIC_STRING_CPP20
#define BOOST_STATIC_STRING_CPP17
#define BOOST_STATIC_STRING_CPP14
#define BOOST_STATIC_STRING_CPP11
#define BOOST_STATIC_STRING_CPP20_CONSTEXPR constexpr
#define BOOST_STATIC_STRING_CPP17_CONSTEXPR constexpr
#define BOOST_STATIC_STRING_CPP14_CONSTEXPR constexpr
#define BOOST_STATIC_STRING_CPP11_CONSTEXPR constexpr
#elif BOOST_STATIC_STRING_STANDARD_VERSION >= 201703L
#define BOOST_STATIC_STRING_CPP17
#define BOOST_STATIC_STRING_CPP14
#define BOOST_STATIC_STRING_CPP11
#define BOOST_STATIC_STRING_CPP20_CONSTEXPR
#define BOOST_STATIC_STRING_CPP17_CONSTEXPR constexpr
#define BOOST_STATIC_STRING_CPP14_CONSTEXPR constexpr
#define BOOST_STATIC_STRING_CPP11_CONSTEXPR constexpr
#elif BOOST_STATIC_STRING_STANDARD_VERSION >= 201402L
#define BOOST_STATIC_STRING_CPP14
#define BOOST_STATIC_STRING_CPP11
#define BOOST_STATIC_STRING_CPP20_CONSTEXPR
#define BOOST_STATIC_STRING_CPP17_CONSTEXPR
#define BOOST_STATIC_STRING_CPP14_CONSTEXPR constexpr
#define BOOST_STATIC_STRING_CPP11_CONSTEXPR constexpr
#else
#define BOOST_STATIC_STRING_CPP11
#define BOOST_STATIC_STRING_CPP20_CONSTEXPR
#define BOOST_STATIC_STRING_CPP17_CONSTEXPR
#define BOOST_STATIC_STRING_CPP14_CONSTEXPR
#define BOOST_STATIC_STRING_CPP11_CONSTEXPR constexpr
#endif

#ifdef BOOST_STATIC_STRING_NO_EXCEPTIONS
#define BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT noexcept
#define BOOST_STATIC_STRING_THROW_IF(cond, ex)
#define BOOST_STATIC_STRING_THROW(ex)
#else
#define BOOST_STATIC_STRING_NO_EXCEPTIONS_NOEXCEPT
#endif

// Boost and non-Boost versions of utilities
#ifndef BOOST_STATIC_STRING_STANDALONE
#ifndef BOOST_STATIC_STRING_THROW_IF
#define BOOST_STATIC_STRING_THROW_IF(cond, ex) if (cond) BOOST_THROW_EXCEPTION(ex)
#endif
#ifndef BOOST_STATIC_STRING_THROW
#define BOOST_STATIC_STRING_THROW(ex) BOOST_THROW_EXCEPTION(ex)
#endif
#ifndef BOOST_STATIC_STRING_STATIC_ASSERT
#define BOOST_STATIC_STRING_STATIC_ASSERT(cond, msg) BOOST_STATIC_ASSERT_MSG(cond, msg)
#endif
#ifndef BOOST_STATIC_STRING_ASSERT
#define BOOST_STATIC_STRING_ASSERT(cond) BOOST_ASSERT(cond)
#endif
#else
#ifndef BOOST_STATIC_STRING_THROW_IF
#define BOOST_STATIC_STRING_THROW_IF(cond, ex) if (cond) throw ex
#endif
#ifndef BOOST_STATIC_STRING_THROW
#define BOOST_STATIC_STRING_THROW(ex) throw ex
#endif
#ifndef BOOST_STATIC_STRING_STATIC_ASSERT
#define BOOST_STATIC_STRING_STATIC_ASSERT(cond, msg) static_assert(cond, msg)
#endif
#ifndef BOOST_STATIC_STRING_ASSERT
#define BOOST_STATIC_STRING_ASSERT(cond) assert(cond)
#endif
#endif

// Compiler bug prevents constexpr from working with clang 4.x and 5.x
// if it is detected, we disable constexpr
#if (BOOST_STATIC_STRING_STANDARD_VERSION >= 201402L && \
BOOST_STATIC_STRING_STANDARD_VERSION < 201703L) && \
defined(__clang__) && \
((__clang_major__ == 4) || (__clang_major__ == 5))
// This directive works on clang
#warning "C++14 constexpr is not supported in clang 4.x and 5.x due to a compiler bug."
#ifdef BOOST_STATIC_STRING_CPP14
#undef BOOST_STATIC_STRING_CPP14
#endif
#undef BOOST_STATIC_STRING_CPP14_CONSTEXPR
#define BOOST_STATIC_STRING_CPP14_CONSTEXPR
#endif

// For clang and msvc, constexpr does not work with library
// comparison function objects.
#if (defined(__clang__) && (__clang_major__ < 9)) || \
(defined(_MSVC_LANG) && !defined(BOOST_STATIC_STRING_CPP20))
#define BOOST_STATIC_STRING_NO_PTR_COMP_FUNCTIONS
#endif

#ifndef BOOST_STATIC_STRING_STANDALONE
#include <boost/assert.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/static_assert.hpp>
#include <boost/utility/string_view.hpp>
#include <boost/throw_exception.hpp>
#else
#include <cassert>
#include <stdexcept>
#include <string_view>
#endif

#ifdef __clang__
#include <new>
#define STRING_VALUE(...) STRING_VALUE__(__VA_ARGS__)
#define STRING_VALUE__(...) #__VA_ARGS__
#pragma message("STATIC STRING CLANG DEBUG INFO")
#pragma message("LIBSTDCXX OLD: " STRING_VALUE(__GLIBCPP__)) 
#pragma message("LIBSTDCXX NEW: " STRING_VALUE(__GLIBCXX__)) 
#pragma message("LIBCXX: " STRING_VALUE(_LIBCPP_VERSION)) 
#endif

namespace boost {
namespace static_string {

/// The type of `basic_string_view` used by the library
template<typename CharT, typename Traits>
using basic_string_view =
#ifndef BOOST_STATIC_STRING_STANDALONE
    boost::basic_string_view<CharT, Traits>;
#else
    std::basic_string_view<CharT, Traits>;
#endif
} // static_string
} // boost

#endif
