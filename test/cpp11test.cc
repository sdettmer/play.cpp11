/**
 * Cpp11 - [c] Steffen Dettmer 2012, 2014 <Steffen.Dettmer@gmail.com>
 *
 * Examples in form of test code demonstrating C++ 2011.
 *
 * \file test/cpp11test.cc Implements and registers C++-11 examples.
 */

#include "cpp11/cpp11.h"

#include <string>
#include <map>
#include <exception>
#include <stdexcept>
#include <cppunit/extensions/HelperMacros.h>

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>{new T {std::forward<Args>(args)...}};
}

class Cpp11Test : public CppUnit::TestCase
{
    // Define the test suite (simply one suite with all tests)
    CPPUNIT_TEST_SUITE(Cpp11Test);
    CPPUNIT_TEST(testStaticAssert);
    CPPUNIT_TEST(testConstruct);
    CPPUNIT_TEST_EXCEPTION(testConstructLength,std::length_error);
    CPPUNIT_TEST_EXCEPTION(testUnderflow,std::out_of_range);
    CPPUNIT_TEST_EXCEPTION(testOverflow,std::out_of_range);
    CPPUNIT_TEST(testRangeFor);
    CPPUNIT_TEST(testUniquePtr);
    CPPUNIT_TEST(testSharedPtr);
    CPPUNIT_TEST_SUITE_END();

  public:
    Cpp11Test()=default;

    /// Configure precondition for each test.
    void setUp() override
    { }

    /// Cleanup a test.
    void tearDown() override
    { }

    /// Only to ensure that CppUnit is executing something
    void testConstructor()
    { }

    void testStaticAssert();

    void testConstruct()
    {
        {
            Vector<int> v(1);
            v[0]=-1;
        }
        {
            Vector<std::string> v(1);
            v[0]="hello";
        }
        {
            Vector<char> v1{'a','b','c','d'};
            Vector<char> v2{'A','B','C','D'};
            Vector<Vector<char>> m(3);
            m[0]=v1;
            m[1]=v2;
            m[2]=v1;
            CPPUNIT_ASSERT(m[1][2]=='C');
        }
    }

    void testConstructLength() {
        Vector<int> v(-1);
    }

    void testUnderflow() {
        Vector<int> v(1);
        v[-1]=0;
    }
    void testOverflow() {
        Vector<int> v(1);
        v[1]=0;
    }

    void testRangeFor();

    void testUniquePtr() {
        {
            auto p = new Vector<int>(1);
            (*p)[0]=7;
            {
              std::unique_ptr<Vector<int> > up { p };
              (*up)[0]++;
            }
            // should be deleted
        }
        {
            auto up1 = make_unique<Vector<int> > (
                std::initializer_list<int> {7});
            {
              {
                  std::unique_ptr<Vector<int> > up2;
                  up2.swap(up1);
                  (*up2)[0]++;
              }
              // should be deleted
            }
        }
    }

    void testSharedPtr() {
        {
            auto p = new Vector<int>(1);
            (*p)[0]=7;
            {
              std::shared_ptr<Vector<int> > sp1 { p };
              (*sp1)[0]++;
              {
                  std::shared_ptr<Vector<int> > sp2 { sp1 };
                  (*sp2)[0]++;
              }
              {
                  std::shared_ptr<Vector<int> > sp3;
                  sp3 = sp1;
                  (*sp3)[0]++;
              }
              (*sp1)[0]++;
              CPPUNIT_ASSERT((*p)[0]==11);
            }
        }
        {
            auto sp1 = std::make_shared<Vector<int> > (
                std::initializer_list<int> {7});
            {
              {
                  std::shared_ptr<Vector<int> > sp2 { sp1 };
                  (*sp2)[0]++;
              }
              {
                  std::shared_ptr<Vector<int> > sp3;
                  sp3 = sp1;
                  (*sp3)[0]++;
              }
              (*sp1)[0]++;
              CPPUNIT_ASSERT((*sp1)[0]==10);
            }
        }
    }

  private:
    Cpp11Test(const Cpp11Test &a)=default;
    Cpp11Test(Cpp11Test &&a)=default;
    Cpp11Test &operator=(const Cpp11Test &a)=delete;
    Cpp11Test &operator=(Cpp11Test &&a)=delete;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(Cpp11Test);

void Cpp11Test::testStaticAssert()
{
    static_assert(sizeof(char)==1,"sizeof(char)==1");
}

void Cpp11Test::testRangeFor()
{
    Vector<std::string> v(3);
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

/* vim: set et ts=4 sw=4 tw=76: */
