/**
 * Cpp11 - [c] Steffen Dettmer 2012, 2014 <Steffen.Dettmer@gmail.com>
 *
 * Examples in form of test code demonstrating C++ 2011.
 *
 * \file test/myrandomtest.h Some tests with <random>.
 */

#include <random>
#include <cppunit/extensions/HelperMacros.h>

class RandomTest : public CppUnit::TestCase {
    CPPUNIT_TEST_SUITE(RandomTest);
    CPPUNIT_TEST(testRandom);
    CPPUNIT_TEST_SUITE_END();
  public:
    void testRandom()
    {

        // Simple but explicit "rolling a dice".
        {
            using distribution = std::uniform_int_distribution<>;
            std::default_random_engine engine;
            distribution roll_a_dice {1,6};
            int rand_val = roll_a_dice(engine);
            CPPUNIT_ASSERT(rand_val>=1 && rand_val<=6);
        }

        // Testing results of rolling a dice.
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
            if (0) {
                std::cout << std::endl;
                for (auto v: values) {
                    std::cout << v << std::endl;
                }
            }
        }

        // Rolling a dice with bound call.
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
            CPPUNIT_ASSERT(rand_val>=1 && rand_val<=6);
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
                size_t rand_val = pick();
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
};

CPPUNIT_TEST_SUITE_REGISTRATION(RandomTest);

/* vim: set ts=4 sw=4 tw=76: */
