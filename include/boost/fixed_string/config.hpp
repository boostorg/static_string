//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/fixed_string
//

#ifndef BOOST_FIXED_STRING_CONFIG_HPP
#define BOOST_FIXED_STRING_CONFIG_HPP

#define BOOST_FIXED_STRING_USE_BOOST

#if __cplusplus >= 201703L
#define BOOST_FIXED_STRING_CPP17
#elif __cplusplus >= 201402L
#define BOOST_FIXED_STRING_CPP14
#else
#define BOOST_FIXED_STRING_CPP11
#endif

#if defined(BOOST_FIXED_STRING_CPP17) || defined(BOOST_FIXED_STRING_USE_BOOST)
#define BOOST_FIXED_STRING_STRING_VIEW
#endif

#ifdef BOOST_FIXED_STRING_USE_BOOST
#include <boost/utility/string_view.hpp>
#elif defined(BOOST_FIXED_STRING_CPP17)
#include <string_view>
#endif


namespace boost {
namespace fixed_string {

/// The type of `string_view` used by the library
#ifdef BOOST_FIXED_STRING_STRING_VIEW
using string_view = 
#ifdef BOOST_FIXED_STRING_USE_BOOST
    boost::string_view;
#else
    std::string_view;
#endif
#endif

/// The type of `basic_string_view` used by the library
#ifdef BOOST_FIXED_STRING_STRING_VIEW
template<typename CharT, typename Traits>
using basic_string_view =
#ifdef BOOST_FIXED_STRING_USE_BOOST
    boost::basic_string_view<CharT, Traits>;
#else
    std::basic_string_view<CharT, Traits>;
#endif
#endif

#ifndef BOOST_NODISCARD
#define BOOST_NODISCARD
#endif

#ifdef BOOST_FIXED_STRING_USE_BOOST
#define BOOST_FIXED_STRING_THROW(e) (BOOST_THROW_EXCEPTION(e))
#else
#define BOOST_FIXED_STRING_THROW(e) (throw e)
#endif

} // fixed_string
} // boost

#endif
