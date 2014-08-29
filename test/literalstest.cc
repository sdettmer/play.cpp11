/**
 * Cpp11 - [c] Steffen Dettmer 2012, 2014 <Steffen.Dettmer@gmail.com>
 *
 * Examples in form of test code demonstrating C++ 2011.
 *
 * \file test/literalstest.cc Tests src/cpp11/literals.h.
 */

#include "cpp11/literals.h"

#include <cppunit/extensions/HelperMacros.h>

class LiteralsTest : public CppUnit::TestCase {
    CPPUNIT_TEST_SUITE(LiteralsTest);
    CPPUNIT_TEST(testComplex);
    CPPUNIT_TEST(testColor);
    CPPUNIT_TEST_SUITE_END();
  public:
    void testComplex() {
        using Complex = std::complex<long double>;
        {
            Complex c1 { 10.0, 5.0 };
            Complex c2 = 10.0 + 5.0_i;
            CPPUNIT_ASSERT(c1==c2);
        }
        {
            Complex c1 { -10.0, -5.0 };
            Complex c2 = -10.0 + -5.0_i;
            CPPUNIT_ASSERT(c1==c2);
        }
        {
            Complex c1 { 10, 5 };
            Complex c2 = 10 + 5_i;
            CPPUNIT_ASSERT(c1==c2);
        }
        {
            Complex c1 { -10, -5 };
            Complex c2 = -10 + -5_i;
            CPPUNIT_ASSERT(c1==c2);
        }
    }
    void testColor() {
        {
            const Color purple = 0x00ff00ff_col;
            Color ref{ 0, 0xff, 0, 0xff};
            CPPUNIT_ASSERT(purple==ref);
        }
        {
            const Color color = 0x01021044_col;
            Color ref{ 1, 2, 16, 0x44};
            CPPUNIT_ASSERT(color==ref);
        }
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(LiteralsTest);

/* vim: set ts=4 sw=4 tw=76: */
