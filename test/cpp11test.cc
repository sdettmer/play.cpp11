/**
 * Cpp11 - [c] Steffen Dettmer 2012, 2014 <Steffen.Dettmer@gmail.com>
 *
 * Examples in form of test code demonstrating C++ 2011.
 *
 * \file test/cpp11test.cc Implements and registers C++-11 examples.
 */

#include "cpp11/cpp11.h"
#include "cpp11/mysort.h"

#include <string>
#include <locale>
#include <map>
#include <exception>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <iterator>
#include <thread>
#include <mutex>
#include <chrono>
#include <queue>
#include <condition_variable>

#include <cassert> // calls abort and allows backtrace in gdb.
#include <cppunit/extensions/HelperMacros.h>

// New return type declaration with "->"
struct Outer {
    struct Inner { };
    Outer::Inner f1(Inner inner);  // Old style, see below.
    auto f2(Inner inner) -> Inner; // New style, see below.
};
// Compiles with g++, but non-stanard:
//   Inner Outer::f1(Inner inner) { return inner; }
// Old style:
Outer::Inner Outer::f1(Inner inner) { return inner; }
// New style:
auto Outer::f2(Inner inner) -> Inner { return inner; }

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

class Cpp11Test : public CppUnit::TestCase
{
    // Define the test suite (simply one suite with all tests)
    CPPUNIT_TEST_SUITE(Cpp11Test);
    CPPUNIT_TEST(testStaticAssert);
    CPPUNIT_TEST(testEnumStruct);
    CPPUNIT_TEST(testString);
    CPPUNIT_TEST(testNoExcept);
    CPPUNIT_TEST(testConstruct);
    CPPUNIT_TEST_EXCEPTION(testConstructLength,std::length_error);
    CPPUNIT_TEST_EXCEPTION(testUnderflow,std::out_of_range);
    CPPUNIT_TEST_EXCEPTION(testOverflow,std::out_of_range);
    CPPUNIT_TEST(testRangeFor);
    CPPUNIT_TEST(testEqualRange);
    CPPUNIT_TEST(testUniquePtr);
    CPPUNIT_TEST(testSharedPtr);
    CPPUNIT_TEST(testBind);
    CPPUNIT_TEST(testFunction);
    CPPUNIT_TEST(testThread);
    CPPUNIT_TEST(testConsumer);
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
        enum old_enum_e { old_none, old_one, old_two, old_three };
        enum old_enum_e old_enum1 = old_one;
        old_enum_e old_enum2 = old_two;
        CPPUNIT_ASSERT(static_cast<int>(old_enum1) == 1);
        CPPUNIT_ASSERT(static_cast<int>(old_enum2) == 2);

        enum struct new_enum_e { none, one, two, three };
        new_enum_e new_enum { new_enum_e::one };

        CPPUNIT_ASSERT(static_cast<int>(new_enum) == 1);

        enum struct new_enum_char_e : char { none, one, two, three };
        new_enum_char_e new_enum_char { new_enum_char_e::two };
        CPPUNIT_ASSERT(static_cast<int>(new_enum_char) == 2);

        enum struct new_enum_uint8_t_e : uint8_t { none, one, two, three };
        new_enum_uint8_t_e new_enum_uint8_t { new_enum_uint8_t_e::three };
        CPPUNIT_ASSERT(static_cast<int>(new_enum_uint8_t) == 3);

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

    void testString() {
        const char     *utf8  = u8"UTF-8";
        const char16_t *utf16 = u"UTF-16";
        const char32_t *utf32 = U"UTF-32";
        const char     *raw   = R"(RAW without specials \n)";

        CPPUNIT_ASSERT(std::string(utf8)=="UTF-8");
        std::u16string u16str(utf16);
        {
            std::u16string u16str(u"UTF-16");
            std::string    str;
            // Not implemented in libdst++ I use with gcc-4.8:
            // std::wstring_convert<
            //    std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
            // str = converter.to_bytes(u16str.c_str());
        }
        // CPPUNIT_ASSERT(u16str==std::string("UTF-16"));
        //CPPUNIT_ASSERT(std::u32string(utf32)=="UTF-32");

        std::string rawstr(raw);
        CPPUNIT_ASSERT(rawstr=="RAW without specials \\n");
        CPPUNIT_ASSERT(utf16!=nullptr);
        CPPUNIT_ASSERT(utf32!=nullptr);
    }

    void testNoExcept() noexcept {
        // Just to check if it compiles.
        // Any exception should lead to terminate() / abort():
        // throw std::exception{};

        struct trivial {
            int i;
            const bool should_throw { false };
            bool operator==(const trivial &t) const noexcept { return false; }
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
            bool operator==(const non_trivial &t) const { return false; }
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

            f1 = [] (int value) -> int { return value + 3; };
            CPPUNIT_ASSERT(f1(4)==7);

            {
                struct add_functor {
                    int add_;
                    add_functor(int add) : add_(add) { }
                    int operator()(int value) { return value + add_; }
                };

                f1=add_functor(10);
                CPPUNIT_ASSERT(f1(5)==15);
            }

        }
    }

    void testThread() {
        struct MyThread {
            const std::string name_;
            MyThread() : name_("") { }
            MyThread(const std::string &name) : name_(name) { }
            void operator()() {
                {
                    if (name_ != "") {
                        std::cout << "start " << name_ << std::endl;
                        std::this_thread::sleep_for(
                            std::chrono::milliseconds{1000});
                        std::cout << "done " << name_ << std::endl;
                    }
                }
            }
        };

        // Note that std::thread constructor copies.
        std::thread t1 { MyThread { "t1" } };
        std::thread t2 { MyThread { "t2" } };

        {
            std::mutex m;
            std::unique_lock<std::mutex> lock { m };
        }
        {
            std::mutex m1;
            std::mutex m2;
            std::unique_lock<std::mutex> lock1 { m1, std::defer_lock };
            std::unique_lock<std::mutex> lock2 { m2, std::defer_lock };

            std::lock(lock1, lock2);
        }

        t1.join();
        t2.join();
    }

    void testConsumer() {

        using Message=std::string;
        class Queue {
            std::queue<Message> mqueue_;
            std::condition_variable mcond_;
            std::mutex mmutex_;

          public:
            void add(const Message &m) {
                std::cout << "add " << m << std::endl;
                std::unique_lock<std::mutex> lock { mmutex_ };
                mqueue_.push(m);
                mcond_.notify_one();
            }
            Message get() {
                std::unique_lock<std::mutex> lock { mmutex_ };
                if (!mqueue_.empty()) {
                    Message m = mqueue_.front();
                    mqueue_.pop();
                    std::cout << "had " << m << std::endl;
                    return m;
                }
                // else: empty --> wait.
                mcond_.wait(lock);
                Message m = mqueue_.front();
                mqueue_.pop();
                std::cout << "waited for " << m << std::endl;
                return m;
            }
        };

        class Producer {
            Queue &queue_;
          public:
            Producer(Queue &queue) : queue_(queue) { }
            void operator()() {
                queue_.add("MSG_0");
                std::this_thread::sleep_for(std::chrono::milliseconds{1000});
                queue_.add("MSG_1");
                std::this_thread::sleep_for(std::chrono::milliseconds{1000});
                queue_.add("MSG_2a");
                queue_.add("MSG_2b");
                std::this_thread::sleep_for(std::chrono::milliseconds{1000});
                queue_.add("MSG_3a");
                queue_.add("MSG_3b");
                std::this_thread::sleep_for(std::chrono::milliseconds{1000});
                queue_.add("STOP");
                std::cout << "Producer done" << std::endl;
            }
        };

        class Consumer {
            Queue &queue_;
          public:
            Consumer(Queue &queue) : queue_(queue) { }
            void operator()() {
                for(;;) {
                    Message m=queue_.get();
                    if (m=="STOP") break;
                    std::cout << "consumed " << m << std::endl;
                }
                std::cout << "Consumer done" << std::endl;
            }
        };

        Queue queue;
        // Consumer consumer(queue);
        // Producer producer(queue);

        std::thread producer { Producer(queue) };
        std::thread consumer { Consumer(queue) };
        producer.join();
        consumer.join();
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
