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
#include <algorithm>
#include <vector>
#include <iterator>
#include <forward_list>
#include <cppunit/extensions/HelperMacros.h>

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>{new T {std::forward<Args>(args)...}};
}

int add_one(const int value) { return value + 1; }

int multiply3(const int a, const int b, const int c) { return a * b * c; }

template<typename T>
T mul3(const T a, const T b, const T c) { return a * b * c; }

// Begin my_sort test
template <typename RandomAccessIterator>
void my_sort_helper(
    RandomAccessIterator begin,
    RandomAccessIterator end,
    std::random_access_iterator_tag)
{
    sort(begin,end);
}

template <typename ForwardAccessIterator>
void my_sort_helper(
    ForwardAccessIterator begin,
    ForwardAccessIterator end,
    std::forward_iterator_tag)
{
    std::vector<typename ForwardAccessIterator::value_type> v{begin, end};

    // or:
    // template <typename ContainerOrIterator>
    // using Value_type = typename ContainerOrIterator::value_type;
    // std::vector<Value_type<ForwardAccessIterator>> v{begin, end};

    sort(v.begin(), v.end());
    copy(v.begin(), v.end(), begin);
}

template <typename Container>
using Iterator_type = typename Container::iterator;

template <typename Iterator>
using Iterator_category
    = typename std::iterator_traits<Iterator>::iterator_category;

template <typename Container>
void my_sort(Container &c)
{
    my_sort_helper(
        c.begin(),
        c.end(),
        typename std::iterator_traits<
            typename Container::iterator
        >::iterator_category{});
    // or:
    // using Iter=Iterator_type<Container>;
    // my_sort_helper(c.begin(), c.end(), Iterator_category<Iter>{});
}
// End my_sort test.

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
    CPPUNIT_TEST(testEqualRange);
    CPPUNIT_TEST(testMySort);
    CPPUNIT_TEST(testUniquePtr);
    CPPUNIT_TEST(testSharedPtr);
    CPPUNIT_TEST(testBind);
    CPPUNIT_TEST(testFunction);
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

    void testMySort() {
        {
            std::forward_list<int> list { 7,3,5,1,9 };
            my_sort(list);
            auto i=list.begin();
            CPPUNIT_ASSERT((*i++)==1);
            CPPUNIT_ASSERT((*i++)==3);
            CPPUNIT_ASSERT((*i++)==5);
            CPPUNIT_ASSERT((*i++)==7);
            CPPUNIT_ASSERT((*i++)==9);
            CPPUNIT_ASSERT(i==list.end());
        }
        {
            std::vector<int> v { 8,5,4,2,10 };
            my_sort(v);
            auto i=v.begin();
            CPPUNIT_ASSERT((*i++)==2);
            CPPUNIT_ASSERT((*i++)==4);
            CPPUNIT_ASSERT((*i++)==5);
            CPPUNIT_ASSERT((*i++)==8);
            CPPUNIT_ASSERT((*i++)==10);
            CPPUNIT_ASSERT(i==v.end());
        }
    }

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

    void testBind() {
        {
            auto five = std::bind(add_one,4);
            CPPUNIT_ASSERT(five()==5);
        }
        {
            using namespace std::placeholders;
            auto twice = std::bind(multiply3,1,_1,2);
            CPPUNIT_ASSERT(twice(4)==8);
            auto multiply = std::bind(multiply3,_1,1,_2);
            CPPUNIT_ASSERT(multiply(4,3)==12);
        }
        // Same using a function template.
        {
            using namespace std::placeholders;
            auto multiply = std::bind(mul3<int>,_1,1,_2);
            CPPUNIT_ASSERT(multiply(4,3)==12);
        }
        // Bind a member function via mem_fn().
        {
            std::string x;
            // Because std::string::operator+=() is overloaded, we have to
            // pick the right version by using a cast-to-member-function.
            // prototype is "std::string& operator+=(const char *)",
            // so we cast to a member function pointer to this type
            // We pass a reference to x (otherwise, append() would work with
            // own copy, not the x).
            auto append = std::mem_fn(
                (std::string& (std::string::*) (const char *))
                &std::string::operator+=);
            auto appendx = std::bind(append,std::ref(x),"x");
            CPPUNIT_ASSERT(appendx()=="x");
            CPPUNIT_ASSERT(appendx()=="xx");
            CPPUNIT_ASSERT(x=="xx");

            // Now muuuuch simpler using lambda expressions:
            auto lambda = [&](){ x+="x"; return x; };
            CPPUNIT_ASSERT(lambda()=="xxx");
            CPPUNIT_ASSERT(lambda()=="xxxx");
            CPPUNIT_ASSERT(x=="xxxx");
        }
    }

    void testFunction() {
        {
            // f1 is a function returning an int and getting an int:
            std::function<int(int)> f1 {add_one};
            CPPUNIT_ASSERT(f1(5)==6);

            f1 = [] (int value) { return value + 2; };
            CPPUNIT_ASSERT(f1(5)==7);

            {
                struct add_functor {
                    int add_;
                    add_functor(int add) : add_(add) { }
                    int operator()(int value) { return value + add_; }
                };

                f1=add_functor(3);
                CPPUNIT_ASSERT(f1(5)==8);
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
