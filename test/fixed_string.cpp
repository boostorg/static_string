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

void
testAssign()
{
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

void
testAccess()
{
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

void
testIterators()
{
    {
        fixed_string<3> s;
        BOOST_TEST(std::distance(
            s.begin(), s.end()) == 0);
        BOOST_TEST(std::distance(
            s.rbegin(), s.rend()) == 0);
        s = "123";
        BOOST_TEST(std::distance(
            s.begin(), s.end()) == 3);
        BOOST_TEST(std::distance(
            s.rbegin(), s.rend()) == 3);
    }
    {
        fixed_string<3> const s("123");
        BOOST_TEST(std::distance(
            s.begin(), s.end()) == 3);
        BOOST_TEST(std::distance(
            s.cbegin(), s.cend()) == 3);
        BOOST_TEST(std::distance(
            s.rbegin(), s.rend()) == 3);
        BOOST_TEST(std::distance(
            s.crbegin(), s.crend()) == 3);
    }
}

void
testCapacity()
{
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

void
testOperations()
{
    //
    // clear
    //

    {
        fixed_string<3> s("123");
        s.clear();
        BOOST_TEST(s.empty());
        BOOST_TEST(*s.end() == 0);
    }

    //
    // insert
    //

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

    //
    // erase
    //

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

    //
    // push_back
    //

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

    //
    // pop_back
    //

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

    //
    // append
    //

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
            (s4.append(s1, 3)),
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

    //
    // operator+=
    //

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
    testAssign();
    testAccess();
    testIterators();
    testCapacity();
    testOperations();
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
