/**
 * Cpp11 - [c] Steffen Dettmer 2012, 2014 <Steffen.Dettmer@gmail.com>
 *
 * Examples in form of test code demonstrating C++ 2011.
 *
 * \file test/myvectortest.cc Simple tests for simple own vector class.
 */

#include "cpp11/myvector.h"

#include <string>
#include <algorithm>

#include <cppunit/extensions/HelperMacros.h>

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>{new T {std::forward<Args>(args)...}};
}


class MyVectorTest : public CppUnit::TestCase
{
    // Define the test suite (simply one suite with all tests)
    CPPUNIT_TEST_SUITE(MyVectorTest);
    CPPUNIT_TEST(testConstruct);
    CPPUNIT_TEST_EXCEPTION(testConstructLength,std::length_error);
    CPPUNIT_TEST_EXCEPTION(testUnderflow,std::out_of_range);
    CPPUNIT_TEST_EXCEPTION(testOverflow,std::out_of_range);
    CPPUNIT_TEST(testRangeFor);
    CPPUNIT_TEST(testEqualRange);
    CPPUNIT_TEST_SUITE_END();

  public:
    MyVectorTest()=default;

    /// Configure precondition for each test.
    void setUp() override
    { }

    /// Cleanup a test.
    void tearDown() override
    { }

    void testConstruct() {
        {
            MyVector<int> v(1);
            v[0]=-1;
        }
        {
            MyVector<std::string> v(1);
            v[0]="hello";
        }
        {
            MyVector<char> v1{'a','b','c','d'};
            MyVector<char> v2{'A','B','C','D'};
            MyVector<MyVector<char>> m(3);
            m[0]=v1;
            m[1]=v2;
            m[2]=v1;
            CPPUNIT_ASSERT(m[1][2]=='C');
        }
    }

    void testConstructLength() {
        MyVector<int> v(-1);
    }

    void testUnderflow() {
        MyVector<int> v(1);
        v[-1]=0;
    }
    void testOverflow() {
        MyVector<int> v(1);
        v[1]=0;
    }

    void testRangeFor();

    void testEqualRange() {
        struct elem { std::string name; std::string stuff; };
        std::vector<elem> v {
            { "name1", "First name A."  },
            { "name1", "First name B."  },
            { "name2", "Second name A." },
            { "name2", "Second name B." },
            { "name3", "Third name A."  },
            { "name3", "Third name B."  },
        };
        // Compare name part:
        auto cmp_name = [] (const elem &a, const elem &b) {
            return a.name < b.name;
        };
        // Get smart iterator finding name2 (by divinding into halfs)
        elem search { "name2" };
        auto get_name2 = std::equal_range(v.begin(), v.end(), search, cmp_name);

        // Iterator shall give 2nd A and B and nothing more.
        CPPUNIT_ASSERT(get_name2.first->stuff=="Second name A.");
        CPPUNIT_ASSERT((get_name2.first+1)->stuff=="Second name B.");
        CPPUNIT_ASSERT((get_name2.first+2)==get_name2.second);
    }

  private:
    MyVectorTest(const MyVectorTest &a)=default;
    MyVectorTest(MyVectorTest &&a)=default;
    MyVectorTest &operator=(const MyVectorTest &a)=delete;
    MyVectorTest &operator=(MyVectorTest &&a)=delete;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(MyVectorTest);


void MyVectorTest::testRangeFor()
{
    MyVector<std::string> v(3);
    v[0]="one";
    v[1]="two";
    v[2]="three";

    int n=0;
    for (auto &e: v) {
        switch(n++) {
            case 0: CPPUNIT_ASSERT(e=="one"); break;
            case 1: CPPUNIT_ASSERT(e=="two"); break;
            case 2: CPPUNIT_ASSERT(e=="three"); break;
            default: throw std::out_of_range("Cpp11Test::testRangeFor");
        }
    }
}

/* vim: set ts=4 sw=4 tw=76: */
