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

#include <boost/utility/string_view.hpp>

namespace boost {
namespace fixed_string {

/// The type of `string_view` used by the library
using string_view = boost::string_view;

/// The type of `basic_string_view` used by the library
template<typename CharT, typename Traits>
using basic_string_view =
    boost::basic_string_view<CharT, Traits>;

} // fixed_string
} // boost

#endif
