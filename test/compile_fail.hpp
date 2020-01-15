#include <fstream>

#include <streambuf>

#include <boost/static_string/detail/static_string.hpp>

namespace boost
{
namespace static_string
{

static_assert(std::is_base_of<
  detail::static_string_base_zero<0, char, std::char_traits<char>>,
              static_string<0>>::value,
              "the zero size optimization shall be used for N = 0");

#ifdef BOOST_STATIC_STRING_USE_NULL_OPTIMIZATION
static_assert(std::is_base_of<
  detail::static_string_base_null<1, char, std::char_traits<char>>,
              static_string<1>>::value,
              "the null terminator optimization shall be used for N <= (std::numeric_limits<char>::max)()");

static_assert(std::is_base_of<
  detail::static_string_base_null<(std::numeric_limits<char>::max)(), char, std::char_traits<char>>,
              static_string<(std::numeric_limits<char>::max)()>>::value,
              "the null terminator optimization shall be used for N <= std::numeric_limits<char>::max()");

static_assert(std::is_base_of<
  detail::static_string_base_zero<(std::numeric_limits<char>::max)() + 1, char, std::char_traits<char>>,
              static_string<(std::numeric_limits<char>::max)() + 1>>::value,
              "the minimum size type optimization shall be used for N > std::numeric_limits<char>::max()");
#else
static_assert(std::is_base_of<
  detail::static_string_base_zero<(std::numeric_limits<char>::max)() + 1, char, std::char_traits<char>>,
              static_string<(std::numeric_limits<char>::max)() + 1>>::value,
              "the minimum size type optimization shall be used for N > 0");
#endif

static_assert(!detail::is_input_iterator<int>::value, "is_input_iterator is incorrect");
static_assert(!detail::is_input_iterator<double>::value, "is_input_iterator is incorrect");
static_assert(detail::is_input_iterator<int*>::value, "is_input_iterator is incorrect");
static_assert(detail::is_input_iterator<std::istreambuf_iterator<char>>::value, "is_input_iterator is incorrect");

static_assert(!detail::is_forward_iterator<int>::value, "is_forward_iterator is incorrect");
static_assert(!detail::is_forward_iterator<double>::value, "is_forward_iterator is incorrect");
static_assert(detail::is_forward_iterator<int*>::value, "is_forward_iterator is incorrect");
static_assert(!detail::is_forward_iterator<std::istreambuf_iterator<char>>::value, "is_forward_iterator is incorrect");
}
}