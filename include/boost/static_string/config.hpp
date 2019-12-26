//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
// Copyright (c) 2019 Krystian Stasiowski (sdkrystian at gmail dot com)
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
#ifdef __cpp_deduction_guides
#define BOOST_STATIC_STRING_USE_DEDUCT
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

#if __cplusplus > 201703L
#define BOOST_STATIC_STRING_CPP20_CONSTEXPR constexpr
#define BOOST_STATIC_STRING_CPP17_CONSTEXPR constexpr
#define BOOST_STATIC_STRING_CPP14_CONSTEXPR constexpr
#define BOOST_STATIC_STRING_CPP11_CONSTEXPR constexpr
#define BOOST_STATIC_STRING_CPP20_CONSTEXPR_USED
#define BOOST_STATIC_STRING_CPP17_CONSTEXPR_USED
#define BOOST_STATIC_STRING_CPP14_CONSTEXPR_USED
#define BOOST_STATIC_STRING_CPP11_CONSTEXPR_USED
#define BOOST_STATIC_STRING_ALLOW_UNINIT_MEM
#elif __cplusplus >= 201703L
#define BOOST_STATIC_STRING_CPP20_CONSTEXPR
#define BOOST_STATIC_STRING_CPP17_CONSTEXPR constexpr
#define BOOST_STATIC_STRING_CPP14_CONSTEXPR constexpr
#define BOOST_STATIC_STRING_CPP11_CONSTEXPR constexpr
#define BOOST_STATIC_STRING_CPP17_CONSTEXPR_USED
#define BOOST_STATIC_STRING_CPP14_CONSTEXPR_USED
#define BOOST_STATIC_STRING_CPP11_CONSTEXPR_USED
#elif __cplusplus >= 201402L
#define BOOST_STATIC_STRING_CPP20_CONSTEXPR
#define BOOST_STATIC_STRING_CPP17_CONSTEXPR
#define BOOST_STATIC_STRING_CPP14_CONSTEXPR constexpr
#define BOOST_STATIC_STRING_CPP11_CONSTEXPR constexpr
#define BOOST_STATIC_STRING_CPP14_CONSTEXPR_USED
#define BOOST_STATIC_STRING_CPP11_CONSTEXPR_USED
#else
#define BOOST_STATIC_STRING_CPP20_CONSTEXPR
#define BOOST_STATIC_STRING_CPP17_CONSTEXPR
#define BOOST_STATIC_STRING_CPP14_CONSTEXPR
#define BOOST_STATIC_STRING_CPP11_CONSTEXPR constexpr
#define BOOST_STATIC_STRING_CPP11_CONSTEXPR_USED
#endif

#ifndef BOOST_STATIC_STRING_STANDALONE
#include <boost/utility/string_view.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>
#include <boost/throw_exception.hpp>
#else
#include <cassert>
#include <string_view>
#endif

#ifdef BOOST_STATIC_STRING_NO_EXCEPTIONS
#define BOOST_STATIC_STRING_COND_NOEXCEPT noexcept
#define BOOST_STATIC_STRING_THROW_IF(cond, ex)
#define BOOST_STATIC_STRING_THROW(ex)
#else
#define BOOST_STATIC_STRING_COND_NOEXCEPT
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

#if (__cplusplus >= 201402L) && \
(__cplusplus < 201703L) && \
defined(BOOST_STATIC_STRING_NULL_OPTIMIZATION) && \
defined(__clang__) && \
((__clang_major__ == 4) || (__clang_major__ == 5))
#error The null terminator optimization is not supported for clang 4.x and clang 5.x
#undef BOOST_STATIC_STRING_NULL_OPTIMIZATION
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
