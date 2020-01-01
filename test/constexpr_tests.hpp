#include <string>
#include <boost/static_string/static_string.hpp>

// char_traits aren't constexpr until c++17
#ifndef BOOST_STATIC_STRING_CPP17_CONSTEXPR_USED
struct cxper_char_traits
{
  using char_type = char;
  using int_type = int;
  using state_type = mbstate_t;

  static constexpr void assign(char_type& c1, const char_type& c2) noexcept {}
  static constexpr bool eq(char_type c1, char_type c2) noexcept { return true; }
  static constexpr bool lt(char_type c1, char_type c2) noexcept { return true; }

  static constexpr int compare(const char_type* s1, const char_type* s2, size_t n) { return 0; }
  static constexpr size_t length(const char_type* s) { return 0; }
  static constexpr const char_type* find(const char_type* s, size_t n,
                                         const char_type& a)
  {
    return 0;
  }
  static constexpr char_type* move(char_type* s1, const char_type* s2, size_t n) { return s1; }
  static constexpr char_type* copy(char_type* s1, const char_type* s2, size_t n) { return s1; }
  static constexpr char_type* assign(char_type* s, size_t n, char_type a) { return s; }
};
#else
using cxper_char_traits = std::char_traits<char>;
#endif
using cstatic_string = boost::static_string::basic_static_string<50, char, cxper_char_traits>;

inline
constexpr 
bool 
testConstantEvaluation()
{
#ifdef BOOST_STATIC_STRING_CPP20_CONSTEXPR_USED
  // c++20 constexpr tests
  return true;
#elif defined(BOOST_STATIC_STRING_CPP17_CONSTEXPR_USED)
  // c++17 constexpr tests
  return true;
#elif defined(BOOST_STATIC_STRING_CPP14_CONSTEXPR_USED)
  // c++14 constexpr tests

  // ctors
  cstatic_string a;
  cstatic_string b(1, 'a');
  cstatic_string c(b, 0);
  cstatic_string d(b, 0, 1);
  cstatic_string e("a", 1);
  cstatic_string f("a");
  //cstatic_string g(f.begin(), f.end());
  cstatic_string h(f);
  cstatic_string i({'a'});

  // assignment
  a = b;
  a = "a";
  a = 'a';
  a = {'a'};

  // assign
  a.assign(b);
  a.assign(b, 0, 1);
  a.assign("a", 1);
  a.assign("a");
  a.assign(b.begin(), b.end());
  a.assign({'a'});
  a.assign(1, 'a');

  // element access
  {
    auto j = a.at(0);
  }
  {
    auto j = a[0];
  }
  {
    auto j = a.front();
  }
  {
    auto j = a.back();
  }
  {
    auto j = a.data();
  }
  {
    auto j = a.c_str();
  }
  {
    auto j = a.begin();
  }
  {
    auto j = a.cbegin();
  }
  {
    auto j = a.end();
  }
  {
    auto j = a.cend();
  }
  /*
  {
    auto j = a.rbegin();
  }
  {
    auto j = a.crbegin();
  }
  {
    auto j = a.rend();
  }
  {
    auto j = a.crend();
  }*/

  // capacity and size
  auto j = cstatic_string().size() +
    cstatic_string().empty() +
    cstatic_string().length() +
    cstatic_string().max_size() +
    cstatic_string().capacity();

  // clear
  a.clear();

  // insert
  a.insert(a.begin(), 1, 'a');
  // cannot have a pointer outside the same array due to UB
  a.insert(0, a.begin());
  a.insert(0, a.begin(), 1);
  a.insert(a.begin(), 'a');
  a.insert(a.begin(), {'a'});

  // erase
  a.erase();
  a.erase(a.begin());
  a.erase(a.begin(), a.end());

  // push
  a.push_back('a');
  a.pop_back();

  return true;
#elif defined(BOOST_STATIC_STRING_CPP11_CONSTEXPR_USED)
  // c++11 constexpr tests
  return cstatic_string().size() + 
    cstatic_string().empty() + 
    cstatic_string().length() + 
    cstatic_string().max_size() + 
    cstatic_string().capacity();
#endif
}