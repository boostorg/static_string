//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
// Copyright (c) 2019 Krystian Stasiowski (sdkrystian at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/fixed_string
//

#ifndef BOOST_FIXED_STRING_CONFIG_HPP
#define BOOST_FIXED_STRING_CONFIG_HPP

// Are we dependent on Boost?
// #define BOOST_FIXED_STRING_STANDALONE

// Can we have deduction guides?
#ifdef __cpp_deduction_guides
#define BOOST_FIXED_STRING_USE_DEDUCT
#endif

// Can we use [[nodiscard]]?
#ifdef __has_attribute
#if __has_attribute(nodiscard)
#define BOOST_FIXED_STRING_NODISCARD [[nodiscard]]
#else
#define BOOST_FIXED_STRING_NODISCARD
#endif
#else
#define BOOST_FIXED_STRING_NODISCARD
#endif

#ifndef BOOST_FIXED_STRING_STANDALONE
#include <boost/utility/string_view.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>
#include <boost/throw_exception.hpp>
#else
#include <cassert>
#include <string_view>
#endif

// Boost and non-Boost versions of utilities
#ifndef BOOST_FIXED_STRING_STANDALONE
#ifndef BOOST_FIXED_STRING_THROW
#define BOOST_FIXED_STRING_THROW(ex) BOOST_THROW_EXCEPTION(ex)
#endif
#ifndef BOOST_FIXED_STRING_STATIC_ASSERT
#define BOOST_FIXED_STRING_STATIC_ASSERT(cond, msg) BOOST_STATIC_ASSERT_MSG(cond, msg)
#endif
#ifndef BOOST_FIXED_STRING_ASSERT
#define BOOST_FIXED_STRING_ASSERT(cond) BOOST_ASSERT(cond)
#endif
#else
#ifndef BOOST_FIXED_STRING_THROW
#define BOOST_FIXED_STRING_THROW(ex) throw ex
#endif
#ifndef BOOST_FIXED_STRING_STATIC_ASSERT
#define BOOST_FIXED_STRING_STATIC_ASSERT(cond, msg) static_assert(cond, msg)
#endif
#ifndef BOOST_FIXED_STRING_ASSERT
#define BOOST_FIXED_STRING_ASSERT(cond) assert(cond)
#endif
#endif

namespace boost {
namespace fixed_string {

/// The type of `basic_string_view` used by the library
template<typename CharT, typename Traits>
using basic_string_view =
#ifndef BOOST_FIXED_STRING_STANDALONE
    boost::basic_string_view<CharT, Traits>;
#else
    std::basic_string_view<CharT, Traits>;
#endif
} // fixed_string
} // boost

#endif
