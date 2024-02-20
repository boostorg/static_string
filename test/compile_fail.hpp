#include <fstream>
#include <streambuf>

namespace boost {
namespace static_strings {

static_assert(!detail::is_input_iterator<int>::value, "is_input_iterator is incorrect");
static_assert(!detail::is_input_iterator<double>::value, "is_input_iterator is incorrect");
static_assert(detail::is_input_iterator<int*>::value, "is_input_iterator is incorrect");
static_assert(detail::is_input_iterator<std::istreambuf_iterator<char>>::value, "is_input_iterator is incorrect");

static_assert(!detail::is_forward_iterator<int>::value, "is_forward_iterator is incorrect");
static_assert(!detail::is_forward_iterator<double>::value, "is_forward_iterator is incorrect");
static_assert(detail::is_forward_iterator<int*>::value, "is_forward_iterator is incorrect");
static_assert(!detail::is_forward_iterator<std::istreambuf_iterator<char>>::value, "is_forward_iterator is incorrect");
} // boost
} // static_strings