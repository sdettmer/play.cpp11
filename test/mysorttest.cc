/**
 * Cpp11 - [c] Steffen Dettmer 2012, 2014 <Steffen.Dettmer@gmail.com>
 *
 * Examples in form of test code demonstrating C++ 2011.
 *
 * \file test/mysorttest.cc Tests the use of "tag types" to select
 *       template specializations.
 */

#include "cpp11/mysort.h"

#include <cppunit/extensions/HelperMacros.h>

class MySortTest : public CppUnit::TestCase {
    CPPUNIT_TEST_SUITE(MySortTest);
    CPPUNIT_TEST(testMySort);
    CPPUNIT_TEST_SUITE_END();
  public:
    void testMySort()
    {
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
};

CPPUNIT_TEST_SUITE_REGISTRATION(MySortTest);

/* vim: set ts=4 sw=4 tw=76: */
