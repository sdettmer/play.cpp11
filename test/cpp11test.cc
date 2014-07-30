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
#include <random>

#include <cassert> // calls abort and allows backtrace in gdb.
#include <cppunit/extensions/HelperMacros.h>

// Example: conditionally define noexcept when a type T is trivial
// (i.e. "can be memcopied"). This is useful if do_something() uses
// copy or assignment (which could be implemented and throw for
// non-trivial types)
template<typename T>
struct noexcept_test {
    void do_something(T t) noexcept(std::is_trivial<T>::value) { }
    void do_something(T t, T t2) noexcept(noexcept(t.operator==(t2))) {
        if (t.should_throw) {
            throw std::exception{};
        }
    }
};

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
    CPPUNIT_TEST(testEnumStruct);
    CPPUNIT_TEST(testNoExcept);
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
    CPPUNIT_TEST(testRandom);
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

    void testEnumStruct() {
        enum old_enum_e { old_one, old_two, old_three };
        enum old_enum_e old_enum1 = old_one;
        old_enum_e old_enum2 = old_two;

        enum struct new_enum_e { one, two, three };
        new_enum_e new_enum { new_enum_e::one };

        enum struct new_enum_char_e : char { one, two, three };
        new_enum_char_e new_enum_char { new_enum_char_e::two };

        enum struct new_enum_uint8_t_e : uint8_t { one, two, three };
        new_enum_uint8_t_e new_enum_uint8_t { new_enum_uint8_t_e::three };

        enum struct new_enum_uint8_t_101_e : uint8_t { one=101l, two, three };
        new_enum_uint8_t_101_e new_enum_uint8_t_101;
        new_enum_uint8_t_101 = new_enum_uint8_t_101_e::two;
        CPPUNIT_ASSERT(static_cast<uint8_t>(new_enum_uint8_t_101) == 102);

        // Check is_same<> first.
        static_assert(std::is_same<char, char>::value, "not char 1");
        // Newer style with function call operator:
        static_assert(std::is_same<char, char>(), "not char 1");

        typedef std::underlying_type<new_enum_char_e>::type char_type;
        static_assert(std::is_same<char_type, char>::value, "not char 2");
        // Newer style:
        // Not working:
        //   typedef std::underlying_type<new_enum_char_e>() char_type;
        static_assert(std::is_same<char_type, char>(), "not char 2");

        typedef std::underlying_type<new_enum_uint8_t_101_e>::type uint8_type;
        static_assert(std::is_same<uint8_type, uint8_t>::value, "not uint8_t");
        CPPUNIT_ASSERT(static_cast<uint8_type>(new_enum_uint8_t_101) == 102);
    }

    void testNoExcept() noexcept {
        // Just to check if it compiles.
        // Any exception should lead to terminate() / abort():
        // throw std::exception{};

        struct trivial {
            int i;
            const bool should_throw { false };
            bool operator==(const trivial &t) const noexcept { }
        };
        noexcept_test<trivial> test;
        test.do_something(trivial{});
        try {
            // Our operator== is noexcept, so is do_something(T,T).
            // If do_something would throw, terminate() would be called (we
            // cannot catch noexcept).
            // If do_something would throw, but program would not terminate
            // (because of our catch), the test would have been failed.
            // Unfortunately I'm too lazy to write this via shell scripts,
            // so I tested this interactively only :))
            // test.do_something(trivial{1}, trivial{2});
        } catch(...) {
        }

        struct non_trivial {
            int i;
            const bool should_throw = true;
            non_trivial(int) { }
            bool operator==(const non_trivial &t) const { }
        };
        noexcept_test<non_trivial> test2;
        test2.do_something(non_trivial{1});

        try {
            // Our operator== is NOT noexcept, so do_something(T,T) also is
            // not, so if it throws, we could catch it.
            test2.do_something(non_trivial{1}, non_trivial{2});
        } catch(...) {
            // throw; // <-- uncomment to see if it is working (aborting).
        }

    }

    void testConstruct() {
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

    void testRandom() {

        // Simple but explicit.
        {
            using distribution = std::uniform_int_distribution<>;
            std::default_random_engine engine;
            distribution roll_a_dice {1,6};
            int rand_val = roll_a_dice(engine);
            CPPUNIT_ASSERT(rand_val>=1 && rand_val<=6);
        }

        // Roll a dice.
        {
            using distribution = std::uniform_int_distribution<>;
            std::default_random_engine engine;
            distribution roll_a_dice {1,6};
            int values[6] = {0};
            for (int n=0;n<1000;n++) {
                int rand_val = roll_a_dice(engine);
                CPPUNIT_ASSERT(rand_val>=1 && rand_val<=6);
                ++(values[rand_val-1]);
            }
            CPPUNIT_ASSERT(values[0]>0);
            CPPUNIT_ASSERT(values[0]<1000);
            for (auto v: values) {
                CPPUNIT_ASSERT(v>20);
                CPPUNIT_ASSERT(v<800);
            }
            if (1) {
                std::cout << std::endl;
                for (auto v: values) {
                    std::cout << v << std::endl;
                }
            }
        }

        // Roll a dice with bound call.
        {
            // Note: we cannot directly replace by lambda here, because it
            // would create engine on each call and return a constant value.
            auto roll_a_dice = bind(std::uniform_int_distribution<>{1,6},
                std::default_random_engine{});
            int values[6] = {0};
            for (int n=0;n<1000;n++) {
                int rand_val = roll_a_dice();
                CPPUNIT_ASSERT(rand_val>=1 && rand_val<=6);
                ++(values[rand_val-1]);
            }
            for (auto v: values) {
                CPPUNIT_ASSERT(v>20);
                CPPUNIT_ASSERT(v<800);
            }
            if (0) {
                std::cout << std::endl;
                for (auto v: values) {
                    std::cout << v << std::endl;
                }
            }
        }

        // Helper.
        {
            class Rand_int {
              public:
                Rand_int(int lo, int hi) : dist_{lo, hi} {};
                int operator()() { return dist_(engine_); }
              private:
                std::default_random_engine engine_;
                std::uniform_int_distribution<> dist_;
            };

            // Simple usage:
            Rand_int roll_a_dice{1,6};
            int rand_val = roll_a_dice();
        }

        // Normal distribution example.
        {
            using distribution = std::normal_distribution<>;
            using engine = std::default_random_engine;
            engine my_engine;
            distribution my_distribution {8,2.0}; // mean and deviation
            // Note: values choosen empirically to make output fit terminal.
            // auto pick = std::bind(my_distribution,my_engine);
            auto pick = [&] () { return my_distribution(my_engine); };
            int values[15] = {0};
            for (int n=0;n<10000;n++) {
                int rand_val = pick();
                if(rand_val>=0 && rand_val<=sizeof(values)/sizeof(values[0])) {
                    ++(values[rand_val]);
                }
            }
            if (0) {
                std::cout << std::endl;
                for (auto v: values) {
                    for (int n=0;n<v/25;n++) {
                        if (n<78) {
                            std::cout << '*';
                        }
                        else if (n==78) {
                            std::cout << '>';
                        }
                    }
                    std::cout << std::endl;
                }
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
