//
// Copyright (c) 2019-2020 Krystian Stasiowski (sdkrystian at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/static_string
//

#include <string>
#include <boost/static_string/static_string.hpp>

// char_traits aren't fully constexpr until c++20
#if BOOST_STATIC_STRING_STANDARD_VERSION <= 201703L && BOOST_STATIC_STRING_STANDARD_VERSION >= 201402L
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
#ifdef BOOST_STATIC_STRING_CPP20
  // c++20 constexpr tests
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

  // reverse iterators
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
  }

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
  a.insert(0, a.begin());
  a.insert(0, a.begin(), 1);
  a.insert(a.begin(), 'a');
  a.insert(a.begin(), {'a'});

  // erase
  a.erase(0, 1);
  a.erase(a.begin());
  a.erase(a.begin(), a.end());

  // push
  a.push_back('a');
  a.pop_back();

  // append
  a.append(1, 'a');
  a.append("a", 1);
  a.append("a");
  a.append(a.begin(), a.end());
  a.append({'a'});

  // append operator
  a += 'a';
  a += "a";
  a += {'a'};

  // compare
  a.compare(b);
  a.compare(0, 1, b);
  a.compare(0, 1, b, 0, 1);
  a.compare("a");
  a.compare(0, 1, "a");
  a.compare(0, 1, "a", 1);

  // substr
  a.substr(0);

  // subview
  a.subview(0);

  // copy
  char k[20]{};
  a.copy(k, 1, 0);

  // resize
  a.resize(1);
  a.resize(1, 'a');

  // swap
  a.swap(b);

  // replace
  a.replace(0, 1, a);
  a.replace(0, 1, a, 0, 1);
  a.replace(0, 1, a.data(), 1);
  a.replace(0, 1, a.data());
  a.replace(0, 1, 1, 'a');
  a.replace(a.begin(), a.end(), a);
  a.replace(a.begin(), a.end(), a.data(), 1);
  a.replace(a.begin(), a.end(), a.data());
  a.replace(a.begin(), a.end(), 1, 'a');
  a.replace(a.begin(), a.end(), a.begin(), a.end());
  a.replace(a.begin(), a.end(), {'a'});

#ifdef BOOST_STATIC_STRING_IS_CONST_EVAL
  a.clear();
  a.replace(a.begin(), a.end(), "a");
  a.replace(a.begin(), a.end(), "a", 1);
#endif

  // find
  a.find(a);
  a.find("a", 0, 1);
  a.find("a", 0);
  a.find('a', 0);

  // rfind
  a.rfind(a);
  a.rfind("a", 0, 1);
  a.rfind("a", 0);
  a.rfind('a', 0);

  // find_first_of
  a.find_first_of(a);
  a.find_first_of("a", 0, 1);
  a.find_first_of("a", 0);
  a.find_first_of('a', 0);

  // find_first_not_of
  a.find_first_not_of(a);
  a.find_first_not_of("a", 0, 1);
  a.find_first_not_of("a", 0);
  a.find_first_not_of('a', 0);

  // starts_with
  a.starts_with('a');
  a.starts_with("a");

  // ends_with
  a.ends_with('a');
  a.ends_with("a");

  return true;
#elif defined(BOOST_STATIC_STRING_CPP17)
  //c++17 constexpr tests

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

  // reverse iterators
  //{
  //  auto j = a.rbegin();
  //}
  //{
  //  auto j = a.crbegin();
  //}
  //{
  //  auto j = a.rend();
  //}
  //{
  //  auto j = a.crend();
  //}

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
  a.insert(0, a.begin());
  a.insert(0, a.begin(), 1);
  a.insert(a.begin(), 'a');
  a.insert(a.begin(), {'a'});

  // erase
  a.erase(0, 1);
  a.erase(a.begin());
  a.erase(a.begin(), a.end());

  // push
  a.push_back('a');
  a.pop_back();

  // append
  a.append(1, 'a');
  a.append("a", 1);
  a.append("a");
  a.append(a.begin(), a.end());
  a.append({'a'});

  // append operator
  a += 'a';
  a += "a";
  a += {'a'};

  // compare
  a.compare(b);
  a.compare(0, 1, b);
  a.compare(0, 1, b, 0, 1);
  a.compare("a");
  a.compare(0, 1, "a");
  a.compare(0, 1, "a", 1);

  // substr
  a.substr(0);

  // subview
  a.subview(0);

  // copy
  char k[20]{};
  a.copy(k, 1, 0);

  // resize
  a.resize(1);
  a.resize(1, 'a');

  // swap
  a.swap(b);

  // replace
  a.replace(0, 1, a);
  a.replace(0, 1, a, 0, 1);
  a.replace(0, 1, a.data(), 1);
  a.replace(0, 1, a.data());
  a.replace(0, 1, 1, 'a');
  a.replace(a.begin(), a.end(), a);
  a.replace(a.begin(), a.end(), a.data(), 1);
  a.replace(a.begin(), a.end(), a.data());
  a.replace(a.begin(), a.end(), 1, 'a');
  a.replace(a.begin(), a.end(), a.begin(), a.end());
  a.replace(a.begin(), a.end(), {'a'});

#ifdef BOOST_STATIC_STRING_IS_CONST_EVAL
  a.clear();
  a.replace(a.begin(), a.end(), "a");
  a.replace(a.begin(), a.end(), "a", 1);
#endif

  // find
  a.find(a);
  a.find("a", 0, 1);
  a.find("a", 0);
  a.find('a', 0);

  // rfind
  a.rfind(a);
  a.rfind("a", 0, 1);
  a.rfind("a", 0);
  a.rfind('a', 0);

  // find_first_of
  a.find_first_of(a);
  a.find_first_of("a", 0, 1);
  a.find_first_of("a", 0);
  a.find_first_of('a', 0);

  // find_first_not_of
  a.find_first_not_of(a);
  a.find_first_not_of("a", 0, 1);
  a.find_first_not_of("a", 0);
  a.find_first_not_of('a', 0);

  // starts_with
  a.starts_with('a');
  a.starts_with("a");

  // ends_with
  a.ends_with('a');
  a.ends_with("a");
 
  return true;
#elif defined(BOOST_STATIC_STRING_CPP14)
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
  a.insert(0, a.begin());
  a.insert(0, a.begin(), 1);
  a.insert(a.begin(), 'a');
  a.insert(a.begin(), {'a'});

  // erase
  a.erase(0, 1);
  a.erase(a.begin());
  a.erase(a.begin(), a.end());

  // push
  a.push_back('a');
  a.pop_back();

  // append
  a.append(1, 'a');
  a.append("a", 1);
  a.append("a");
  a.append(a.begin(), a.end());
  a.append({'a'});

  // append operator
  a += 'a';
  a += "a";
  a += {'a'};

  // compare
  a.compare(b);
  a.compare(0, 1, b);
  a.compare(0, 1, b, 0, 1);
  a.compare("a");
  a.compare(0, 1, "a");
  a.compare(0, 1, "a", 1);
  
  // substr
  a.substr(0);
  
  // subview
  a.subview(0);
  
  // copy
  char k[20]{};
  a.copy(k, 1, 0);

  // resize
  a.resize(1);
  a.resize(1, 'a');

  // swap
  a.swap(b);

  // replace
  a.replace(0, 1, a);
  a.replace(0, 1, a, 0, 1);
  a.replace(0, 1, a.data(), 1);
  a.replace(0, 1, a.data());
  a.replace(0, 1, 1, 'a');
  a.replace(a.begin(), a.end(), a);
  a.replace(a.begin(), a.end(), a.data(), 1);
  a.replace(a.begin(), a.end(), a.data());
  a.replace(a.begin(), a.end(), 1, 'a');
  a.replace(a.begin(), a.end(), a.begin(), a.end());
  a.replace(a.begin(), a.end(), {'a'});

#ifdef BOOST_STATIC_STRING_IS_CONST_EVAL
  a.clear();
  a.replace(a.begin(), a.end(), "a");
  a.replace(a.begin(), a.end(), "a", 1);
#endif

  // find
  a.find(a);
  a.find("a", 0, 1);
  a.find("a", 0);
  a.find('a', 0);

  // rfind
  a.rfind(a);
  a.rfind("a", 0, 1);
  a.rfind("a", 0);
  a.rfind('a', 0);

  // find_first_of
  a.find_first_of(a);
  a.find_first_of("a", 0, 1);
  a.find_first_of("a", 0);
  a.find_first_of('a', 0);

  // find_first_not_of
  a.find_first_not_of(a);
  a.find_first_not_of("a", 0, 1);
  a.find_first_not_of("a", 0);
  a.find_first_not_of('a', 0);

  // starts_with
  a.starts_with('a');
  a.starts_with("a");

  // ends_with
  a.ends_with('a');
  a.ends_with("a");
  return true;
#elif defined(BOOST_STATIC_STRING_CPP11)
  // c++11 constexpr tests
  return cstatic_string().size() + 
    cstatic_string().empty() + 
    cstatic_string().length() + 
    cstatic_string().max_size() + 
    cstatic_string().capacity();
#endif
}