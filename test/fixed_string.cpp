//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/fixed_string
//

// Test that header file is self-contained.
#include <boost/fixed_string/fixed_string.hpp>

#include <boost/core/lightweight_test.hpp>

namespace boost {
namespace fixed_string {

static
void
testConstruct()
{
    {
        fixed_string<1> s;
        BOOST_TEST(s.empty());
        BOOST_TEST(s.size() == 0);
        BOOST_TEST(s == "");
        BOOST_TEST(*s.end() == 0);
    }
    {
        fixed_string<4> s1(3, 'x');
        BOOST_TEST(! s1.empty());
        BOOST_TEST(s1.size() == 3);
        BOOST_TEST(s1 == "xxx");
        BOOST_TEST(*s1.end() == 0);
        BOOST_TEST_THROWS(
            (fixed_string<2>(3, 'x')),
            std::length_error);
    }
    {
        fixed_string<5> s1("12345");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<3> s2(s1, 2);
        BOOST_TEST(s2 == "345");
        BOOST_TEST(*s2.end() == 0);
        fixed_string<0> s3(s1, 5);
        BOOST_TEST(s3.empty());
        BOOST_TEST(s3.front() == 0);
        BOOST_TEST(*s3.end() == 0);
    }
    {
        fixed_string<5> s1("12345");
        fixed_string<2> s2(s1, 1, 2);
        BOOST_TEST(s2 == "23");
        BOOST_TEST(*s2.end() == 0);
        fixed_string<0> s3(s1, 5, 1);
        BOOST_TEST(s3.empty());
        BOOST_TEST(s3.front() == 0);
        BOOST_TEST(*s3.end() == 0);
        BOOST_TEST_THROWS(
            (fixed_string<5>(s1, 6)),
            std::out_of_range);
    }
    {
        fixed_string<5> s1("UVXYZ", 3);
        BOOST_TEST(s1 == "UVX");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<5> s2("X\0""Y\0""Z", 3);
        BOOST_TEST(std::memcmp(
            s2.data(), "X\0""Y", 3) == 0);
        BOOST_TEST(*s2.end() == 0);
    }
    {
        fixed_string<5> s1("12345");
        fixed_string<3> s2(
            s1.begin() + 1, s1.begin() + 3);
        BOOST_TEST(s2 == "23");
        BOOST_TEST(*s2.end() == 0);
    }
    {
        fixed_string<5> s1("12345");
        fixed_string<5> s2(s1);
        BOOST_TEST(s2 == "12345");
        BOOST_TEST(*s2.end() == 0);
        fixed_string<6> s3(s1);
        BOOST_TEST(s3 == "12345");
        BOOST_TEST(*s3.end() == 0);
        BOOST_TEST_THROWS(
            (fixed_string<4>(s1)),
            std::length_error);
    }
    {
        fixed_string<3> s1({'1', '2', '3'});
        BOOST_TEST(s1 == "123");
        BOOST_TEST(*s1.end() == 0);
        BOOST_TEST(
            fixed_string<0>({}) == fixed_string<0>());
        BOOST_TEST_THROWS(
            (fixed_string<2>({'1', '2', '3'})),
            std::length_error);
    }
    {
        fixed_string<3> s1(
            string_view("123"));
        BOOST_TEST(s1 == "123");
        BOOST_TEST(*s1.end() == 0);
        BOOST_TEST_THROWS(
            (fixed_string<2>(string_view("123"))),
            std::length_error);
    }
    {
        fixed_string<5> s1(
            std::string("12345"), 2, 2);
        BOOST_TEST(s1 == "34");
        BOOST_TEST(*s1.end() == 0);
        BOOST_TEST_THROWS(
            (fixed_string<2>(std::string("12345"), 1, 3)),
            std::length_error);
    }
}

static
void
testAssignment()
{
    // assign(size_type count, CharT ch)
    BOOST_TEST(fixed_string<3>{}.assign(1, '*') == "*");
    BOOST_TEST(fixed_string<3>{}.assign(3, '*') == "***");
    BOOST_TEST(fixed_string<3>{"abc"}.assign(3, '*') == "***");
    BOOST_TEST_THROWS(fixed_string<1>{"a"}.assign(2, '*'), std::length_error);

    // assign(fixed_string const& s) noexcept
    BOOST_TEST(fixed_string<3>{}.assign(fixed_string<3>{"abc"}) == "abc");
    BOOST_TEST(fixed_string<3>{"*"}.assign(fixed_string<3>{"abc"}) == "abc");
    BOOST_TEST(fixed_string<3>{"***"}.assign(fixed_string<3>{"abc"}) == "abc");

    // assign(fixed_string<M, CharT, Traits> const& s)
    BOOST_TEST(fixed_string<3>{}.assign(fixed_string<5>{"abc"}) == "abc");
    BOOST_TEST(fixed_string<3>{"*"}.assign(fixed_string<5>{"abc"}) == "abc");
    BOOST_TEST(fixed_string<3>{"***"}.assign(fixed_string<5>{"abc"}) == "abc");
    BOOST_TEST_THROWS(fixed_string<3>{}.assign(fixed_string<5>{"abcde"}), std::length_error);

    // assign(fixed_string<M, CharT, Traits> const& s, size_type pos, size_type count = npos)
    BOOST_TEST(fixed_string<4>{}.assign(fixed_string<5>{"abcde"}, 1) == "bcde");
    BOOST_TEST(fixed_string<3>{}.assign(fixed_string<5>{"abcde"}, 1, 3) == "bcd");
    BOOST_TEST(fixed_string<3>{"*"}.assign(fixed_string<5>{"abcde"}, 1, 3) == "bcd");
    BOOST_TEST(fixed_string<3>{"***"}.assign(fixed_string<5>{"abcde"}, 1, 3) == "bcd");
    BOOST_TEST_THROWS(fixed_string<3>{}.assign(fixed_string<5>{"abcde"}, 0), std::length_error);

    // assign(CharT const* s, size_type count)
    BOOST_TEST(fixed_string<3>{}.assign("abc", 3) == "abc");
    BOOST_TEST(fixed_string<3>{"*"}.assign("abc", 3) == "abc");
    BOOST_TEST_THROWS(fixed_string<1>{}.assign("abc", 3), std::length_error);
    
    // assign(CharT const* s)
    BOOST_TEST(fixed_string<3>{}.assign("abc") == "abc");
    BOOST_TEST(fixed_string<3>{"*"}.assign("abc") == "abc");
    BOOST_TEST_THROWS(fixed_string<1>{}.assign("abc"), std::length_error);

    // assign(InputIt first, InputIt last)
    {
        fixed_string<4> const cs{"abcd"};
        fixed_string<4> s{"ad"};
        BOOST_TEST(fixed_string<4>{}.assign(cs.begin(), cs.end()) == "abcd");
        BOOST_TEST(fixed_string<4>{"*"}.assign(cs.begin(), cs.end()) == "abcd");
        BOOST_TEST_THROWS(fixed_string<2>{"*"}.assign(cs.begin(), cs.end()), std::length_error);
    }
    
    // assign(std::initializer_list<CharT> ilist)
    BOOST_TEST(fixed_string<3>{}.assign({'a', 'b', 'c'}) == "abc");
    BOOST_TEST(fixed_string<3>{"*"}.assign({'a', 'b', 'c'}) == "abc");
    BOOST_TEST(fixed_string<3>{"***"}.assign({'a', 'b', 'c'}) == "abc");
    BOOST_TEST_THROWS(fixed_string<1>{}.assign({'a', 'b', 'c'}), std::length_error);

    // assign(T const& t)
    {
        struct T
        {
            operator string_view() const noexcept
            {
                return "abc";
            }
        };
        BOOST_TEST(fixed_string<3>{}.assign(T{}) == "abc");
        BOOST_TEST(fixed_string<3>{"*"}.assign(T{}) == "abc");
        BOOST_TEST(fixed_string<3>{"***"}.assign(T{}) == "abc");
        BOOST_TEST_THROWS(fixed_string<2>{"**"}.assign(T{}), std::length_error);
    }

    // assign(T const& t, size_type pos, size_type count = npos)
    {
        struct T
        {
            operator string_view() const noexcept
            {
                return "abcde";
            }
        };
        BOOST_TEST(fixed_string<5>{}.assign(T{}, 0) == "abcde");
        BOOST_TEST(fixed_string<5>{}.assign(T{}, 0, 5) == "abcde");
        BOOST_TEST(fixed_string<5>{}.assign(T{}, 1, 3) == "bcd");
        BOOST_TEST(fixed_string<5>{"*"}.assign(T{}, 1) == "bcde");
        BOOST_TEST_THROWS(fixed_string<2>{"**"}.assign(T{}, 6, 3), std::out_of_range);
        BOOST_TEST_THROWS(fixed_string<2>{"**"}.assign(T{}, 1, 3), std::length_error);
    }

    //---

    {
        fixed_string<3> s1("123");
        fixed_string<3> s2;
        s2 = s1;
        BOOST_TEST(s2 == "123");
        BOOST_TEST(*s2.end() == 0);
    }
    {
        fixed_string<3> s1("123");
        fixed_string<5> s2;
        s2 = s1;
        BOOST_TEST(s2 == "123");
        BOOST_TEST(*s2.end() == 0);
        fixed_string<1> s3;
        BOOST_TEST_THROWS(
            s3 = s1,
            std::length_error);
    }
    {
        fixed_string<3> s1;
        s1 = "123";
        BOOST_TEST(s1 == "123");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<1> s2;
        BOOST_TEST_THROWS(
            s2 = "123",
            std::length_error);
    }
    {
        fixed_string<1> s1;
        s1 = 'x';
        BOOST_TEST(s1 == "x");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<0> s2;
        BOOST_TEST_THROWS(
            s2 = 'x',
            std::length_error);
    }
    {
        fixed_string<3> s1;
        s1 = {'1', '2', '3'};
        BOOST_TEST(s1 == "123");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<1> s2;
        BOOST_TEST_THROWS(
            (s2 = {'1', '2', '3'}),
            std::length_error);
    }
    {
        fixed_string<3> s1;
        s1 = string_view("123");
        BOOST_TEST(s1 == "123");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<1> s2;
        BOOST_TEST_THROWS(
            s2 = string_view("123"),
            std::length_error);
    }

    {
        fixed_string<4> s1;
        s1.assign(3, 'x');
        BOOST_TEST(s1 == "xxx");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<2> s2;
        BOOST_TEST_THROWS(
            s2.assign(3, 'x'),
            std::length_error);
    }
    {
        fixed_string<5> s1("12345");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<5> s2;
        s2.assign(s1);
        BOOST_TEST(s2 == "12345");
        BOOST_TEST(*s2.end() == 0);
    }
    {
        fixed_string<5> s1("12345");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<7> s2;
        s2.assign(s1);
        BOOST_TEST(s2 == "12345");
        BOOST_TEST(*s2.end() == 0);
        fixed_string<3> s3;
        BOOST_TEST_THROWS(
            s3.assign(s1),
            std::length_error);
    }
    {
        fixed_string<5> s1("12345");
        fixed_string<5> s2;
        s2.assign(s1, 1);
        BOOST_TEST(s2 == "2345");
        BOOST_TEST(*s2.end() == 0);
        s2.assign(s1, 1, 2);
        BOOST_TEST(s2 == "23");
        BOOST_TEST(*s2.end() == 0);
        s2.assign(s1, 1, 100);
        BOOST_TEST(s2 == "2345");
        BOOST_TEST(*s2.end() == 0);
        BOOST_TEST_THROWS(
            (s2.assign(s1, 6)),
            std::out_of_range);
        fixed_string<3> s3;
        BOOST_TEST_THROWS(
            s3.assign(s1, 1),
            std::length_error);
    }
    {
        fixed_string<5> s1;
        s1.assign("12");
        BOOST_TEST(s1 == "12");
        BOOST_TEST(*s1.end() == 0);
        s1.assign("12345");
        BOOST_TEST(s1 == "12345");
        BOOST_TEST(*s1.end() == 0);
    }
    {
        fixed_string<5> s1;
        s1.assign("12345", 3);
        BOOST_TEST(s1 == "123");
        BOOST_TEST(*s1.end() == 0);
    }
    {
        fixed_string<5> s1("12345");
        fixed_string<3> s2;
        s2.assign(s1.begin(), s1.begin() + 2);
        BOOST_TEST(s2 == "12");
        BOOST_TEST(*s2.end() == 0);
        BOOST_TEST_THROWS(
            (s2.assign(s1.begin(), s1.end())),
            std::length_error);
    }
    {
        fixed_string<5> s1;
        s1.assign({'1', '2', '3'});
        BOOST_TEST(s1 == "123");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<1> s2;
        BOOST_TEST_THROWS(
            (s2.assign({'1', '2', '3'})),
            std::length_error);
    }
    {
        fixed_string<5> s1;
        s1.assign(string_view("123"));
        BOOST_TEST(s1 == "123");
        BOOST_TEST(*s1.end() == 0);
        s1.assign(string_view("12345"));
        BOOST_TEST(s1 == "12345");
        BOOST_TEST(*s1.end() == 0);
        BOOST_TEST_THROWS(
            s1.assign(string_view("1234567")),
            std::length_error);
    }
    {
        fixed_string<5> s1;
        s1.assign(std::string("12345"), 2, 2);
        BOOST_TEST(s1 == "34");
        BOOST_TEST(*s1.end() == 0);
        s1.assign(std::string("12345"), 3);
        BOOST_TEST(s1 == "45");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<2> s2;
        BOOST_TEST_THROWS(
            (s2.assign(std::string("12345"), 1, 3)),
            std::length_error);
    }
}

static
void
testElements()
{
    using cfs3 = fixed_string<3> const;

    // at(size_type pos)
    BOOST_TEST(fixed_string<3>{"abc"}.at(0) == 'a');
    BOOST_TEST(fixed_string<3>{"abc"}.at(2) == 'c');
    BOOST_TEST_THROWS(fixed_string<3>{""}.at(0), std::out_of_range);
    BOOST_TEST_THROWS(fixed_string<3>{"abc"}.at(4), std::out_of_range);
    
    // at(size_type pos) const
    BOOST_TEST(cfs3{"abc"}.at(0) == 'a');
    BOOST_TEST(cfs3{"abc"}.at(2) == 'c');
    BOOST_TEST_THROWS(cfs3{""}.at(0), std::out_of_range);
    BOOST_TEST_THROWS(cfs3{"abc"}.at(4), std::out_of_range);

    // operator[](size_type pos)
    BOOST_TEST(fixed_string<3>{"abc"}[0] == 'a');
    BOOST_TEST(fixed_string<3>{"abc"}[2] == 'c');
    BOOST_TEST(fixed_string<3>{"abc"}[3] == 0);
    BOOST_TEST(fixed_string<3>{""}[0] == 0);

    // operator[](size_type pos) const
    BOOST_TEST(cfs3{"abc"}[0] == 'a');
    BOOST_TEST(cfs3{"abc"}[2] == 'c');
    BOOST_TEST(cfs3{"abc"}[3] == 0);
    BOOST_TEST(cfs3{""}[0] == 0);

    // front()
    BOOST_TEST(fixed_string<3>{"a"}.front() == 'a');
    BOOST_TEST(fixed_string<3>{"abc"}.front() == 'a');

    // front() const
    BOOST_TEST(cfs3{"a"}.front() == 'a');
    BOOST_TEST(cfs3{"abc"}.front() == 'a');

    // back()
    BOOST_TEST(fixed_string<3>{"a"}.back() == 'a');
    BOOST_TEST(fixed_string<3>{"abc"}.back() == 'c');

    // back() const 
    BOOST_TEST(cfs3{"a"}.back() == 'a');
    BOOST_TEST(cfs3{"abc"}.back() == 'c');

    // data() noexcept
    // data() const noexcept
    // c_str() const noexcept
    // operator string_view_type() const noexcept

    //---

    {
        fixed_string<5> s("12345");
        BOOST_TEST(s.at(1) == '2');
        BOOST_TEST(s.at(4) == '5');
        BOOST_TEST_THROWS(
            s.at(5) = 0,
            std::out_of_range);
    }
    {
        fixed_string<5> const s("12345");
        BOOST_TEST(s.at(1) == '2');
        BOOST_TEST(s.at(4) == '5');
        BOOST_TEST_THROWS(
            s.at(5),
            std::out_of_range);
    }
    {
        fixed_string<5> s("12345");
        BOOST_TEST(s[1] == '2');
        BOOST_TEST(s[4] == '5');
        s[1] = '_';
        BOOST_TEST(s == "1_345");
    }
    {
        fixed_string<5> const s("12345");
        BOOST_TEST(s[1] == '2');
        BOOST_TEST(s[4] == '5');
        BOOST_TEST(s[5] == 0);
    }
    {
        fixed_string<3> s("123");
        BOOST_TEST(s.front() == '1');
        BOOST_TEST(s.back() == '3');
        s.front() = '_';
        BOOST_TEST(s == "_23");
        s.back() = '_';
        BOOST_TEST(s == "_2_");
    }
    {
        fixed_string<3> const s("123");
        BOOST_TEST(s.front() == '1');
        BOOST_TEST(s.back() == '3');
    }
    {
        fixed_string<3> s("123");
        BOOST_TEST(std::memcmp(
            s.data(), "123", 3) == 0);
    }
    {
        fixed_string<3> const s("123");
        BOOST_TEST(std::memcmp(
            s.data(), "123", 3) == 0);
    }
    {
        fixed_string<3> s("123");
        BOOST_TEST(std::memcmp(
            s.c_str(), "123\0", 4) == 0);
    }
    {
        fixed_string<3> s("123");
        string_view sv = s;
        BOOST_TEST(fixed_string<5>(sv) == "123");
    }
}

static
void
testIterators()
{
    {
        fixed_string<3> s;
        BOOST_TEST(std::distance(s.begin(), s.end()) == 0);
        BOOST_TEST(std::distance(s.rbegin(), s.rend()) == 0);
        s = "123";
        BOOST_TEST(std::distance(s.begin(), s.end()) == 3);
        BOOST_TEST(std::distance(s.rbegin(), s.rend()) == 3);
    }
    {
        fixed_string<3> const s("123");
        BOOST_TEST(std::distance(s.begin(), s.end()) == 3);
        BOOST_TEST(std::distance(s.cbegin(), s.cend()) == 3);
        BOOST_TEST(std::distance(s.rbegin(), s.rend()) == 3);
        BOOST_TEST(std::distance(s.crbegin(), s.crend()) == 3);
    }
}

static
void
testCapacity()
{
    // empty()
    BOOST_TEST(fixed_string<0>{}.empty());
    BOOST_TEST(fixed_string<1>{}.empty());
    BOOST_TEST(! fixed_string<1>{"a"}.empty());
    BOOST_TEST(! fixed_string<3>{"abc"}.empty());

    // size()
    BOOST_TEST(fixed_string<0>{}.size() == 0);
    BOOST_TEST(fixed_string<1>{}.size() == 0);
    BOOST_TEST(fixed_string<1>{"a"}.size() == 1);
    BOOST_TEST(fixed_string<3>{"abc"}.size() == 3);
    BOOST_TEST(fixed_string<5>{"abc"}.size() == 3);

    // length()
    BOOST_TEST(fixed_string<0>{}.length() == 0);
    BOOST_TEST(fixed_string<1>{}.length() == 0);
    BOOST_TEST(fixed_string<1>{"a"}.length() == 1);
    BOOST_TEST(fixed_string<3>{"abc"}.length() == 3);
    BOOST_TEST(fixed_string<5>{"abc"}.length() == 3);

    // max_size()
    BOOST_TEST(fixed_string<0>{}.max_size() == 0);
    BOOST_TEST(fixed_string<1>{}.max_size() == 1);
    BOOST_TEST(fixed_string<1>{"a"}.max_size() == 1);
    BOOST_TEST(fixed_string<3>{"abc"}.max_size() == 3);
    BOOST_TEST(fixed_string<5>{"abc"}.max_size() == 5);

    // reserve(std::size_t n)
    fixed_string<3>{}.reserve(0);
    fixed_string<3>{}.reserve(1);
    fixed_string<3>{}.reserve(3);
    BOOST_TEST_THROWS(fixed_string<0>{}.reserve(1), std::length_error);
    BOOST_TEST_THROWS(fixed_string<3>{}.reserve(4), std::length_error);

    // capacity()
    BOOST_TEST(fixed_string<0>{}.capacity() == 0);
    BOOST_TEST(fixed_string<1>{}.capacity() == 1);
    BOOST_TEST(fixed_string<1>{"a"}.capacity() == 1);
    BOOST_TEST(fixed_string<3>{"abc"}.capacity() == 3);
    BOOST_TEST(fixed_string<5>{"abc"}.capacity() == 5);

    //---

    fixed_string<3> s;
    BOOST_TEST(s.empty());
    BOOST_TEST(s.size() == 0);
    BOOST_TEST(s.length() == 0);
    BOOST_TEST(s.max_size() == 3);
    BOOST_TEST(s.capacity() == 3);
    s = "123";
    BOOST_TEST(! s.empty());
    BOOST_TEST(s.size() == 3);
    BOOST_TEST(s.length() == 3);
    s.reserve(0);
    s.reserve(3);
    BOOST_TEST_THROWS(
        s.reserve(4),
        std::length_error);
    s.shrink_to_fit();
    BOOST_TEST(! s.empty());
    BOOST_TEST(s.size() == 3);
    BOOST_TEST(s.length() == 3);
    BOOST_TEST(*s.end() == 0);
}

static
void
testClear()
{
    // clear()
    fixed_string<3> s("123");
    s.clear();
    BOOST_TEST(s.empty());
    BOOST_TEST(*s.end() == 0);
}

static
void
testInsert()
{
    using sv = string_view;

    // insert(size_type index, size_type count, CharT ch)
    // The overload resolution is ambiguous
    // here because 0 is also a pointer type
    //BOOST_TEST(fixed_string<3>{"bc"}.insert(0, 1, 'a') == "abc");
    BOOST_TEST(fixed_string<3>{"bc"}.insert(std::size_t(0), 1, 'a') == "abc");
    BOOST_TEST(fixed_string<3>{"ac"}.insert(1, 1, 'b') == "abc");
    BOOST_TEST(fixed_string<3>{"ab"}.insert(2, 1, 'c') == "abc");
    BOOST_TEST_THROWS(fixed_string<4>{"abc"}.insert(4, 1, '*'), std::out_of_range);
    BOOST_TEST_THROWS(fixed_string<3>{"abc"}.insert(1, 1, '*'), std::length_error);

    // insert(size_type index, CharT const* s)
    BOOST_TEST(fixed_string<3>{"bc"}.insert(0, "a") == "abc");
    BOOST_TEST_THROWS(fixed_string<4>{"abc"}.insert(4, "*"), std::out_of_range);
    BOOST_TEST_THROWS(fixed_string<3>{"abc"}.insert(1, "*"), std::length_error);

    // insert(size_type index, CharT const* s, size_type count)
    BOOST_TEST(fixed_string<4>{"ad"}.insert(1, "bcd", 2) == "abcd");
    BOOST_TEST_THROWS(fixed_string<4>{"abc"}.insert(4, "*"), std::out_of_range);
    BOOST_TEST_THROWS(fixed_string<3>{"abc"}.insert(1, "*"), std::length_error);
    
    // insert(size_type index, string_view_type sv)
    BOOST_TEST(fixed_string<3>{"ac"}.insert(1, sv{"b"}) == "abc");
    BOOST_TEST_THROWS(fixed_string<4>{"abc"}.insert(4, sv{"*"}), std::out_of_range);
    BOOST_TEST_THROWS(fixed_string<3>{"abc"}.insert(1, sv{"*"}), std::length_error);

    // insert(size_type index, string_view_type sv, size_type index_str, size_type count = npos)
    BOOST_TEST(fixed_string<4>{"ad"}.insert(1, sv{"abcd"}, 1, 2) == "abcd");
    BOOST_TEST(fixed_string<4>{"ad"}.insert(1, sv{"abc"}, 1) == "abcd");
    BOOST_TEST_THROWS((fixed_string<4>{"ad"}.insert(1, sv{"bc"}, 3, 0)), std::out_of_range);
    BOOST_TEST_THROWS((fixed_string<3>{"ad"}.insert(1, sv{"bc"}, 0, 2)), std::length_error);

    // insert(const_iterator pos, CharT ch)
    {
        fixed_string<3> s{"ac"};
        BOOST_TEST(s.insert(s.begin() + 1, 'b') == s.begin() + 1);
        BOOST_TEST(s == "abc");
        BOOST_TEST_THROWS(s.insert(s.begin() + 1, '*'), std::length_error);
    }

    // insert(const_iterator pos, size_type count, CharT ch)
    {
        fixed_string<4> s{"ac"};
        BOOST_TEST(s.insert(s.begin() + 1, 2, 'b') == s.begin() + 1);
        BOOST_TEST(s == "abbc");
        BOOST_TEST_THROWS(s.insert(s.begin() + 1, 2, '*'), std::length_error);
    }

    // insert(const_iterator pos, InputIt first, InputIt last)
    {
        fixed_string<4> const cs{"abcd"};
        fixed_string<4> s{"ad"};
        BOOST_TEST(s.insert(s.begin() + 1, cs.begin() + 1, cs.begin() + 3) == s.begin() + 1);
        BOOST_TEST(s == "abcd");
    }

    // insert(const_iterator pos, std::initializer_list<CharT> ilist)
    {
        fixed_string<4> s{"ad"};
        BOOST_TEST(s.insert(s.begin() + 1, {'b', 'c'}) == s.begin() + 1);
        BOOST_TEST(s == "abcd");
    }

    // insert(size_type index, T const& t)
    {
        struct T
        {
            operator string_view() const noexcept
            {
                return "b";
            }
        };
        BOOST_TEST(fixed_string<3>{"ac"}.insert(1, T{}) == "abc");
        BOOST_TEST_THROWS(fixed_string<4>{"abc"}.insert(4, T{}), std::out_of_range);
        BOOST_TEST_THROWS(fixed_string<3>{"abc"}.insert(1, T{}), std::length_error);
    }

    // insert(size_type index, T const& t, size_type index_str, size_type count = npos)
    {
        struct T
        {
            operator string_view() const noexcept
            {
                return "abcd";
            }
        };
        BOOST_TEST(fixed_string<6>{"ae"}.insert(1, T{}, 1) == "abcde");
        BOOST_TEST(fixed_string<6>{"abe"}.insert(2, T{}, 2) == "abcde");
        BOOST_TEST(fixed_string<4>{"ac"}.insert(1, T{}, 1, 1) == "abc");
        BOOST_TEST(fixed_string<4>{"ad"}.insert(1, T{}, 1, 2) == "abcd");
        BOOST_TEST_THROWS(fixed_string<4>{"abc"}.insert(4, T{}), std::out_of_range);
        BOOST_TEST_THROWS(fixed_string<3>{"abc"}.insert(1, T{}), std::length_error);
    }

    //---

    {
        // Using 7 as the size causes a miscompile in MSVC14.2 x64 Release
        fixed_string<8> s1("12345");
        s1.insert(2, 2, '_');
        BOOST_TEST(s1 == "12__345");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<6> s2("12345");
        BOOST_TEST_THROWS(
            (s2.insert(2, 2, '_')),
            std::length_error);
        fixed_string<6> s3("12345");
        BOOST_TEST_THROWS(
            (s3.insert(6, 2, '_')),
            std::out_of_range);
    }
    {
        fixed_string<7> s1("12345");
        s1.insert(2, "__");
        BOOST_TEST(s1 == "12__345");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<6> s2("12345");
        BOOST_TEST_THROWS(
            (s2.insert(2, "__")),
            std::length_error);
        fixed_string<6> s3("12345");
        BOOST_TEST_THROWS(
            (s2.insert(6, "__")),
            std::out_of_range);
    }
    {
        fixed_string<7> s1("12345");
        s1.insert(2, "TUV", 2);
        BOOST_TEST(s1 == "12TU345");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<6> s2("12345");
        BOOST_TEST_THROWS(
            (s2.insert(2, "TUV", 2)),
            std::length_error);
        fixed_string<6> s3("12345");
        BOOST_TEST_THROWS(
            (s3.insert(6, "TUV", 2)),
            std::out_of_range);
    }
    {
        fixed_string<7> s1("12345");
        s1.insert(2, fixed_string<3>("TU"));
        BOOST_TEST(s1 == "12TU345");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<6> s2("12345");
        BOOST_TEST_THROWS(
            (s2.insert(2, fixed_string<3>("TUV"))),
            std::length_error);
        fixed_string<6> s3("12345");
        BOOST_TEST_THROWS(
            (s3.insert(6, fixed_string<3>("TUV"))),
            std::out_of_range);
    }
    {
        fixed_string<7> s1("12345");
        s1.insert(2, fixed_string<3>("TUV"), 1);
        BOOST_TEST(s1 == "12UV345");
        BOOST_TEST(*s1.end() == 0);
        s1 = "12345";
        s1.insert(2, fixed_string<3>("TUV"), 1, 1);
        BOOST_TEST(s1 == "12U345");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<6> s2("12345");
        BOOST_TEST_THROWS(
            (s2.insert(2, fixed_string<3>("TUV"), 1, 2)),
            std::length_error);
        fixed_string<6> s3("12345");
        BOOST_TEST_THROWS(
            (s3.insert(6, fixed_string<3>("TUV"), 1, 2)),
            std::out_of_range);
    }
    {
        fixed_string<4> s1("123");
        s1.insert(s1.begin() + 1, '_');
        BOOST_TEST(s1 == "1_23");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<3> s2("123");
        BOOST_TEST_THROWS(
            (s2.insert(s2.begin() + 1, '_')),
            std::length_error);
    }
    {
        fixed_string<4> s1("12");
        s1.insert(s1.begin() + 1, 2, '_');
        BOOST_TEST(s1 == "1__2");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<4> s2("123");
        BOOST_TEST_THROWS(
            (s2.insert(s2.begin() + 1, 2, ' ')),
            std::length_error);
    }
    {
        fixed_string<3> s1("123");
        fixed_string<5> s2("UV");
        s2.insert(s2.begin() + 1, s1.begin(), s1.end());
        BOOST_TEST(s2 == "U123V");
        BOOST_TEST(*s2.end() == 0);
        fixed_string<4> s3("UV");
        BOOST_TEST_THROWS(
            (s3.insert(s3.begin() + 1, s1.begin(), s1.end())),
            std::length_error);
    }
    {
        fixed_string<5> s1("123");
        s1.insert(1, string_view("UV"));
        BOOST_TEST(s1 == "1UV23");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<4> s2("123");
        BOOST_TEST_THROWS(
            (s2.insert(1, string_view("UV"))),
            std::length_error);
        fixed_string<5> s3("123");
        BOOST_TEST_THROWS(
            (s3.insert(5, string_view("UV"))),
            std::out_of_range);
    }
    {
        fixed_string<5> s1("123");
        s1.insert(1, std::string("UV"));
        BOOST_TEST(s1 == "1UV23");
        BOOST_TEST(*s1.end() == 0);
        BOOST_TEST_THROWS(
            (s1.insert(1, std::string("UV"))),
            std::length_error);
    }
    {
        fixed_string<6> s1("123");
        s1.insert(1, std::string("UVX"), 1);
        BOOST_TEST(s1 == "1VX23");
        BOOST_TEST(*s1.end() == 0);
        s1.insert(4, std::string("PQR"), 1, 1);
        BOOST_TEST(s1 == "1VX2Q3");
        BOOST_TEST(*s1.end() == 0);
        BOOST_TEST_THROWS(
            (s1.insert(4, std::string("PQR"), 1, 1)),
            std::length_error);
    }
}

static
void
testErase()
{
    // erase(size_type index = 0, size_type count = npos)
    BOOST_TEST(fixed_string<3>{"abc"}.erase() == "");
    BOOST_TEST(fixed_string<3>{"abc"}.erase(1) == "a");
    BOOST_TEST(fixed_string<3>{"abc"}.erase(2) == "ab");
    BOOST_TEST(fixed_string<3>{"abc"}.erase(1, 1) == "ac");
    BOOST_TEST(fixed_string<3>{"abc"}.erase(0, 2) == "c");
    BOOST_TEST(fixed_string<3>{"abc"}.erase(3, 0) == "abc");
    BOOST_TEST(fixed_string<3>{"abc"}.erase(3, 4) == "abc");
    BOOST_TEST_THROWS(fixed_string<3>{"abc"}.erase(4, 0), std::out_of_range);

    // erase(const_iterator pos)
    {
        fixed_string<3> s{"abc"};
        BOOST_TEST(s.erase(s.begin() + 1) == s.begin() + 1);
        BOOST_TEST(s == "ac");
    }
    {
        fixed_string<3> s{"abc"};
        BOOST_TEST(s.erase(s.begin() + 3) == s.end());
        BOOST_TEST(s == "abc");
    }

    // erase(const_iterator first, const_iterator last)
    {
        fixed_string<4> s{"abcd"};
        BOOST_TEST(s.erase(s.begin() + 1, s.begin() + 3) == s.begin() + 1);
        BOOST_TEST(s == "ad");
    }

    //---

    {
        fixed_string<9> s1("123456789");
        BOOST_TEST(s1.erase(1, 1) == "13456789");
        BOOST_TEST(s1 == "13456789");
        BOOST_TEST(*s1.end() == 0);
        BOOST_TEST(s1.erase(5) == "13456");
        BOOST_TEST(s1 == "13456");
        BOOST_TEST(*s1.end() == 0);
        BOOST_TEST_THROWS(
            s1.erase(7),
            std::out_of_range);
    }
    {
        fixed_string<9> s1("123456789");
        BOOST_TEST(*s1.erase(s1.begin() + 5) == '7');
        BOOST_TEST(s1 == "12345789");
        BOOST_TEST(*s1.end() == 0);
    }
    {
        fixed_string<9> s1("123456789");
        BOOST_TEST(*s1.erase(
            s1.begin() + 5, s1.begin() + 7) == '8');
        BOOST_TEST(s1 == "1234589");
        BOOST_TEST(*s1.end() == 0);
    }
}

static
void
testPushBack()
{
    // push_back(CharT ch);
    {
        fixed_string<2> s;
        s.push_back('a');
        BOOST_TEST(s == "a");
        s.push_back('b');
        BOOST_TEST(s == "ab");
        BOOST_TEST_THROWS(s.push_back('c'), std::length_error);
    }

    //---

    {
        fixed_string<3> s1("12");
        s1.push_back('3');
        BOOST_TEST(s1 == "123");
        BOOST_TEST_THROWS(
            s1.push_back('4'),
            std::length_error);
        fixed_string<0> s2;
        BOOST_TEST_THROWS(
            s2.push_back('_'),
            std::length_error);
    }
}

static
void
testPopBack()
{
    // pop_back()
    {
        fixed_string<3> s{"abc"};
        BOOST_TEST(*s.end() == 0);
        s.pop_back();
        BOOST_TEST(s == "ab");
        BOOST_TEST(*s.end() == 0);
        s.pop_back();
        BOOST_TEST(s == "a");
        BOOST_TEST(*s.end() == 0);
        s.pop_back();
        BOOST_TEST(s.empty());
        BOOST_TEST(*s.end() == 0);
    }

    //---

    {
        fixed_string<3> s1("123");
        s1.pop_back();
        BOOST_TEST(s1 == "12");
        BOOST_TEST(*s1.end() == 0);
        s1.pop_back();
        BOOST_TEST(s1 == "1");
        BOOST_TEST(*s1.end() == 0);
        s1.pop_back();
        BOOST_TEST(s1.empty());
        BOOST_TEST(*s1.end() == 0);
    }
}

static
void
testAppend()
{
    using sv = string_view;

    // append(size_type count, CharT ch)
    BOOST_TEST(fixed_string<1>{}.append(1, 'a') == "a");
    BOOST_TEST(fixed_string<2>{}.append(2, 'a') == "aa");
    BOOST_TEST(fixed_string<2>{"a"}.append(1, 'b') == "ab");
    BOOST_TEST_THROWS(fixed_string<2>{"ab"}.append(1, 'c'), std::length_error);

    // append(string_view_type sv)
    BOOST_TEST(fixed_string<3>{"a"}.append(sv{"bc"}) == "abc");
    BOOST_TEST(fixed_string<3>{"ab"}.append(sv{"c"}) == "abc");
    BOOST_TEST_THROWS(fixed_string<3>{"abc"}.append(sv{"*"}), std::length_error);

    // append(string_view_type sv, size_type pos, size_type count = npos)
    BOOST_TEST(fixed_string<3>{"a"}.append(sv{"abc"}, 1) == "abc");
    BOOST_TEST(fixed_string<3>{"a"}.append(sv{"abc"}, 1, 2) == "abc");
    BOOST_TEST_THROWS(fixed_string<3>{"abc"}.append(sv{"a"}, 2, 1), std::out_of_range);
    BOOST_TEST_THROWS(fixed_string<3>{"abc"}.append(sv{"abcd"}, 1, 2), std::length_error);

    // append(CharT const* s, size_type count)
    BOOST_TEST(fixed_string<3>{"a"}.append("bc", 0) == "a");
    BOOST_TEST(fixed_string<3>{"a"}.append("bc", 2) == "abc");
    BOOST_TEST_THROWS(fixed_string<3>{"abc"}.append("bc", 2), std::length_error);

    // append(CharT const* s)
    BOOST_TEST(fixed_string<3>{"a"}.append("bc") == "abc");
    BOOST_TEST_THROWS(fixed_string<3>{"abc"}.append("bc"), std::length_error);

    // append(InputIt first, InputIt last)
    {
        fixed_string<4> const cs{"abcd"};
        fixed_string<4> s{"ad"};
        BOOST_TEST(fixed_string<4>{"ab"}.append(
            cs.begin() + 2, cs.begin() + 4) == "abcd");
        BOOST_TEST_THROWS(fixed_string<2>{"ab"}.append(
            cs.begin() + 2, cs.begin() + 4), std::length_error);
    }

    // append(std::initializer_list<CharT> ilist)
    BOOST_TEST(fixed_string<4>{"ab"}.append({'c', 'd'}) == "abcd");
    BOOST_TEST_THROWS(fixed_string<3>{"ab"}.append({'c', 'd'}), std::length_error);

    // append(T const& t)
    {
        struct T
        {
            operator string_view() const noexcept
            {
                return "c";
            }
        };
        BOOST_TEST(fixed_string<3>{"ab"}.append(T{}) == "abc");
        BOOST_TEST_THROWS(fixed_string<3>{"abc"}.append(T{}), std::length_error);
    }

    // append(T const& t, size_type pos, size_type count = npos)
    {
        struct T
        {
            operator string_view() const noexcept
            {
                return "abcd";
            }
        };
        BOOST_TEST(fixed_string<4>{"ab"}.append(T{}, 2) == "abcd");
        BOOST_TEST(fixed_string<3>{"a"}.append(T{}, 1, 2) == "abc");
        BOOST_TEST_THROWS(fixed_string<4>{"abc"}.append(T{}, 5), std::out_of_range);
        BOOST_TEST_THROWS(fixed_string<3>{"abc"}.append(T{}, 3, 1), std::length_error);
    }

    //---

    {
        fixed_string<3> s1("1");
        s1.append(2, '_');
        BOOST_TEST(s1 == "1__");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<2> s2("1");
        BOOST_TEST_THROWS(
            (s2.append(2, '_')),
            std::length_error);
    }
    {
        fixed_string<2> s1("__");
        fixed_string<3> s2("1");
        s2.append(s1);
        BOOST_TEST(s2 == "1__");
        BOOST_TEST(*s2.end() == 0);
        fixed_string<2> s3("1");
        BOOST_TEST_THROWS(
            s3.append(s1),
            std::length_error);
    }
    {
        fixed_string<3> s1("XYZ");
        fixed_string<4> s2("12");
        s2.append(s1, 1);
        BOOST_TEST(s2 == "12YZ");
        BOOST_TEST(*s2.end() == 0);
        fixed_string<3> s3("12");
        s3.append(s1, 1, 1);
        BOOST_TEST(s3 == "12Y");
        BOOST_TEST(*s3.end() == 0);
        fixed_string<3> s4("12");
        BOOST_TEST_THROWS(
            (s4.append(s1, 4)),
            std::out_of_range);
        fixed_string<3> s5("12");
        BOOST_TEST_THROWS(
            (s5.append(s1, 1)),
            std::length_error);
    }
    {
        fixed_string<4> s1("12");
        s1.append("XYZ", 2);
        BOOST_TEST(s1 == "12XY");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<3> s3("12");
        BOOST_TEST_THROWS(
            (s3.append("XYZ", 2)),
            std::length_error);
    }
    {
        fixed_string<5> s1("12");
        s1.append("XYZ");
        BOOST_TEST(s1 == "12XYZ");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<4> s2("12");
        BOOST_TEST_THROWS(
            s2.append("XYZ"),
            std::length_error);
    }
    {
        fixed_string<3> s1("XYZ");
        fixed_string<5> s2("12");
        s2.append(s1.begin(), s1.end());
        BOOST_TEST(s2 == "12XYZ");
        BOOST_TEST(*s2.end() == 0);
        fixed_string<4> s3("12");
        BOOST_TEST_THROWS(
            s3.append(s1.begin(), s1.end()),
            std::length_error);
    }
    {
        fixed_string<5> s1("123");
        s1.append({'X', 'Y'});
        BOOST_TEST(s1 == "123XY");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<4> s2("123");
        BOOST_TEST_THROWS(
            s2.append({'X', 'Y'}),
            std::length_error);
    }
    {
        string_view s1("XYZ");
        fixed_string<5> s2("12");
        s2.append(s1);
        BOOST_TEST(s2 == "12XYZ");
        BOOST_TEST(*s2.end() == 0);
        fixed_string<4> s3("12");
        BOOST_TEST_THROWS(
            s3.append(s1),
            std::length_error);
    }
    {
        fixed_string<6> s1("123");
        s1.append(std::string("UVX"), 1);
        BOOST_TEST(s1 == "123VX");
        BOOST_TEST(*s1.end() == 0);
        s1.append(std::string("PQR"), 1, 1);
        BOOST_TEST(s1 == "123VXQ");
        BOOST_TEST(*s1.end() == 0);
        fixed_string<3> s2("123");
        BOOST_TEST_THROWS(
            (s2.append(std::string("PQR"), 1, 1)),
            std::length_error);
    }
}

static
void
testPlusEquals()
{
    using sv = string_view;

    // operator+=(CharT ch)
    BOOST_TEST((fixed_string<3>{"ab"} += 'c') == "abc");
    BOOST_TEST_THROWS((fixed_string<3>{"abc"} += '*'), std::length_error);

    // operator+=(CharT const* s)
    BOOST_TEST((fixed_string<3>{"a"} += "bc") == "abc");
    BOOST_TEST_THROWS((fixed_string<3>{"abc"} += "*"), std::length_error);

    // operator+=(std::initializer_list<CharT> init)
    BOOST_TEST((fixed_string<3>{"a"} += {'b', 'c'}) == "abc");
    BOOST_TEST_THROWS((fixed_string<3>{"abc"} += {'*', '*'}), std::length_error);

    // operator+=(string_view_type const& s)
    BOOST_TEST((fixed_string<3>{"a"} += sv{"bc"}) == "abc");
    BOOST_TEST_THROWS((fixed_string<3>{"abc"} += sv{"*"}), std::length_error);

    //---

    {
        fixed_string<2> s1("__");
        fixed_string<3> s2("1");
        s2 += s1;
        BOOST_TEST(s2 == "1__");
        BOOST_TEST(*s2.end() == 0);
        fixed_string<2> s3("1");
        BOOST_TEST_THROWS(
            s3 += s1,
            std::length_error);
    }
    {
        fixed_string<3> s1("12");
        s1 += '3';
        BOOST_TEST(s1 == "123");
        BOOST_TEST_THROWS(
            s1 += '4',
            std::length_error);
    }
    {
        fixed_string<4> s1("12");
        s1 += "34";
        BOOST_TEST(s1 == "1234");
        BOOST_TEST_THROWS(
            s1 += "5",
            std::length_error);
    }
    {
        fixed_string<4> s1("12");
        s1 += {'3', '4'};
        BOOST_TEST(s1 == "1234");
        BOOST_TEST_THROWS(
            (s1 += {'5'}),
            std::length_error);
    }
    {
        string_view s1("34");
        fixed_string<4> s2("12");
        s2 += s1;
        BOOST_TEST(s2 == "1234");
        BOOST_TEST_THROWS(
            s2 += s1,
            std::length_error);
    }
}

void
testCompare()
{
    using str1 = fixed_string<1>;
    using str2 = fixed_string<2>;
    {
        str1 s1;
        str2 s2;
        s1 = "1";
        s2 = "22";
        BOOST_TEST(s1.compare(s2) < 0);
        BOOST_TEST(s2.compare(s1) > 0);
        BOOST_TEST(s1 < "10");
        BOOST_TEST(s2 > "1");
        BOOST_TEST("10" > s1);
        BOOST_TEST("1" < s2);
        BOOST_TEST(s1 < "20");
        BOOST_TEST(s2 > "1");
        BOOST_TEST(s2 > "2");
    }
    {
        str2 s1("x");
        str2 s2("x");
        BOOST_TEST(s1 == s2);
        BOOST_TEST(s1 <= s2);
        BOOST_TEST(s1 >= s2);
        BOOST_TEST(! (s1 < s2));
        BOOST_TEST(! (s1 > s2));
        BOOST_TEST(! (s1 != s2));
    }
    {
        str1 s1("x");
        str2 s2("x");
        BOOST_TEST(s1 == s2);
        BOOST_TEST(s1 <= s2);
        BOOST_TEST(s1 >= s2);
        BOOST_TEST(! (s1 < s2));
        BOOST_TEST(! (s1 > s2));
        BOOST_TEST(! (s1 != s2));
    }
    {
        str2 s("x");
        BOOST_TEST(s == "x");
        BOOST_TEST(s <= "x");
        BOOST_TEST(s >= "x");
        BOOST_TEST(! (s < "x"));
        BOOST_TEST(! (s > "x"));
        BOOST_TEST(! (s != "x"));
        BOOST_TEST("x" == s);
        BOOST_TEST("x" <= s);
        BOOST_TEST("x" >= s);
        BOOST_TEST(! ("x" < s));
        BOOST_TEST(! ("x" > s));
        BOOST_TEST(! ("x" != s));
    }
    {
        str2 s("x");
        BOOST_TEST(s <= "y");
        BOOST_TEST(s < "y");
        BOOST_TEST(s != "y");
        BOOST_TEST(! (s == "y"));
        BOOST_TEST(! (s >= "y"));
        BOOST_TEST(! (s > "x"));
        BOOST_TEST("y" >= s);
        BOOST_TEST("y" > s);
        BOOST_TEST("y" != s);
        BOOST_TEST(! ("y" == s));
        BOOST_TEST(! ("y" <= s));
        BOOST_TEST(! ("y" < s));
    }
    {
        str1 s1("x");
        str2 s2("y");
        BOOST_TEST(s1 <= s2);
        BOOST_TEST(s1 < s2);
        BOOST_TEST(s1 != s2);
        BOOST_TEST(! (s1 == s2));
        BOOST_TEST(! (s1 >= s2));
        BOOST_TEST(! (s1 > s2));
    }
    {
        str1 s1("x");
        str2 s2("xx");
        BOOST_TEST(s1 < s2);
        BOOST_TEST(s2 > s1);
    }
    {
        str1 s1("x");
        str2 s2("yy");
        BOOST_TEST(s1 < s2);
        BOOST_TEST(s2 > s1);
    }
}

void
testSwap()
{
    {
        fixed_string<3> s1("123");
        fixed_string<3> s2("XYZ");
        swap(s1, s2);
        BOOST_TEST(s1 == "XYZ");
        BOOST_TEST(*s1.end() == 0);
        BOOST_TEST(s2 == "123");
        BOOST_TEST(*s2.end() == 0);
        fixed_string<3> s3("UV");
        swap(s2, s3);
        BOOST_TEST(s2 == "UV");
        BOOST_TEST(*s2.end() == 0);
        BOOST_TEST(s3 == "123");
        BOOST_TEST(*s3.end() == 0);
    }
    {
        fixed_string<5> s1("123");
        fixed_string<7> s2("XYZ");
        swap(s1, s2);
        BOOST_TEST(s1 == "XYZ");
        BOOST_TEST(*s1.end() == 0);
        BOOST_TEST(s2 == "123");
        BOOST_TEST(*s2.end() == 0);
        fixed_string<3> s3("UV");
        swap(s2, s3);
        BOOST_TEST(s2 == "UV");
        BOOST_TEST(*s2.end() == 0);
        BOOST_TEST(s3 == "123");
        BOOST_TEST(*s3.end() == 0);
        {
            fixed_string<5> s4("12345");
            fixed_string<3> s5("XYZ");
            BOOST_TEST_THROWS(
                (swap(s4, s5)),
                std::length_error);
        }
        {
            fixed_string<3> s4("XYZ");
            fixed_string<5> s5("12345");
            BOOST_TEST_THROWS(
                (swap(s4, s5)),
                std::length_error);
        }
    }
}

void
testGeneral()
{
    using str1 = fixed_string<1>;
    using str2 = fixed_string<2>;
    {
        str1 s1;
        BOOST_TEST(s1 == "");
        BOOST_TEST(s1.empty());
        BOOST_TEST(s1.size() == 0);
        BOOST_TEST(s1.max_size() == 1);
        BOOST_TEST(s1.capacity() == 1);
        BOOST_TEST(s1.begin() == s1.end());
        BOOST_TEST(s1.cbegin() == s1.cend());
        BOOST_TEST(s1.rbegin() == s1.rend());
        BOOST_TEST(s1.crbegin() == s1.crend());
        BOOST_TEST_THROWS(
            s1.at(0),
            std::out_of_range);
        BOOST_TEST(s1.data()[0] == 0);
        BOOST_TEST(*s1.c_str() == 0);
        BOOST_TEST(std::distance(s1.begin(), s1.end()) == 0);
        BOOST_TEST(std::distance(s1.cbegin(), s1.cend()) == 0);
        BOOST_TEST(std::distance(s1.rbegin(), s1.rend()) == 0);
        BOOST_TEST(std::distance(s1.crbegin(), s1.crend()) == 0);
        BOOST_TEST(s1.compare(s1) == 0);
    }
    {
        str1 const s1;
        BOOST_TEST(s1 == "");
        BOOST_TEST(s1.empty());
        BOOST_TEST(s1.size() == 0);
        BOOST_TEST(s1.max_size() == 1);
        BOOST_TEST(s1.capacity() == 1);
        BOOST_TEST(s1.begin() == s1.end());
        BOOST_TEST(s1.cbegin() == s1.cend());
        BOOST_TEST(s1.rbegin() == s1.rend());
        BOOST_TEST(s1.crbegin() == s1.crend());
        BOOST_TEST_THROWS(
            s1.at(0),
            std::out_of_range);
        BOOST_TEST(s1.data()[0] == 0);
        BOOST_TEST(*s1.c_str() == 0);
        BOOST_TEST(std::distance(s1.begin(), s1.end()) == 0);
        BOOST_TEST(std::distance(s1.cbegin(), s1.cend()) == 0);
        BOOST_TEST(std::distance(s1.rbegin(), s1.rend()) == 0);
        BOOST_TEST(std::distance(s1.crbegin(), s1.crend()) == 0);
        BOOST_TEST(s1.compare(s1) == 0);
    }
    {
        str1 s1;
        str1 s2("x");
        BOOST_TEST(s2 == "x");
        BOOST_TEST(s2[0] == 'x');
        BOOST_TEST(s2.at(0) == 'x');
        BOOST_TEST(s2.front() == 'x');
        BOOST_TEST(s2.back() == 'x');
        str1 const s3(s2);
        BOOST_TEST(s3 == "x");
        BOOST_TEST(s3[0] == 'x');
        BOOST_TEST(s3.at(0) == 'x');
        BOOST_TEST(s3.front() == 'x');
        BOOST_TEST(s3.back() == 'x');
        s2 = "y";
        BOOST_TEST(s2 == "y");
        BOOST_TEST(s3 == "x");
        s1 = s2;
        BOOST_TEST(s1 == "y");
        s1.clear();
        BOOST_TEST(s1.empty());
        BOOST_TEST(s1.size() == 0);
    }
    {
        str2 s1("x");
        str1 s2(s1);
        BOOST_TEST(s2 == "x");
        str1 s3;
        s3 = s2;
        BOOST_TEST(s3 == "x");
        s1 = "xy";
        BOOST_TEST(s1.size() == 2);
        BOOST_TEST(s1[0] == 'x');
        BOOST_TEST(s1[1] == 'y');
        BOOST_TEST(s1.at(0) == 'x');
        BOOST_TEST(s1.at(1) == 'y');
        BOOST_TEST(s1.front() == 'x');
        BOOST_TEST(s1.back() == 'y');
        auto const s4 = s1;
        BOOST_TEST(s4[0] == 'x');
        BOOST_TEST(s4[1] == 'y');
        BOOST_TEST(s4.at(0) == 'x');
        BOOST_TEST(s4.at(1) == 'y');
        BOOST_TEST(s4.front() == 'x');
        BOOST_TEST(s4.back() == 'y');
        BOOST_TEST_THROWS(
            s3 = s1,
            std::length_error);
        BOOST_TEST_THROWS(
            str1{s1},
            std::length_error);
    }
    {
        str1 s1("x");
        str2 s2;
        s2 = s1;
        BOOST_TEST_THROWS(
            s1.resize(2),
            std::length_error);
    }
}

void
testToStaticString()
{
    BOOST_TEST(to_fixed_string<long>(0) == "0");
    BOOST_TEST(to_fixed_string<long>(1) == "1");
    BOOST_TEST(to_fixed_string<long>(0xffff) == "65535");
    BOOST_TEST(to_fixed_string<long>(0x10000) == "65536");
    BOOST_TEST(to_fixed_string<long long>(0xffffffff) == "4294967295");

    BOOST_TEST(to_fixed_string<long>(-1) == "-1");
    BOOST_TEST(to_fixed_string<long>(-65535) == "-65535");
    BOOST_TEST(to_fixed_string<long>(-65536) == "-65536");
    BOOST_TEST(to_fixed_string<long long>(-4294967295ll) == "-4294967295");

    BOOST_TEST(to_fixed_string<unsigned long>(0) == "0");
    BOOST_TEST(to_fixed_string<unsigned long>(1) == "1");
    BOOST_TEST(to_fixed_string<unsigned long>(0xffff) == "65535");
    BOOST_TEST(to_fixed_string<unsigned long>(0x10000) == "65536");
    BOOST_TEST(to_fixed_string<unsigned long>(0xffffffff) == "4294967295");
}

int
runTests()
{
    testConstruct();
    
    testAssignment();
    
    testElements();

    testIterators();
    
    testCapacity();

    testClear();
    testInsert();
    testErase();
    testPushBack();
    testPopBack();
    testAppend();
    testPlusEquals();

    testCompare();
    testSwap();
    testGeneral();
    testToStaticString();

    return report_errors();
}

} // fixed_string
} // boost

int
main()
{
    return boost::fixed_string::runTests();
}
