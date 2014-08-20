/**
 * Cpp11 - [c] Steffen Dettmer 2012, 2014 <Steffen.Dettmer@gmail.com>
 *
 * Examples in form of test code demonstrating C++ 2011.
 *
 * \file cpp11/factorial.cc - Compile time tests of Factorial.
 */
#include "cpp11/factorial.h"


/**
 * Implementation tests implementation of compile time factorial functions.
 * No call needed, because it is tested via static_assert at compilation.
 * (no "static" to avoid a potential warning).
 */
void factorial_meta_test()
{
    static_assert(Factorial<0>::value==1,   "<0>");
    static_assert(Factorial<1>::value==1,   "<1>");
    static_assert(Factorial<2>::value==2,   "<2>");
    static_assert(Factorial<3>::value==2*3, "<3>");
    static_assert(Factorial<5>::value==120, "<5>");

    static_assert(factorial_func(0)==1,     "(0)");
    static_assert(factorial_func(1)==1,     "(1)");
    static_assert(factorial_func(2)==2,     "(2)");
    static_assert(factorial_func(3)==2*3,   "(3)");
    static_assert(factorial_func(5)==120,   "(5)");

    static_assert(Factorial<10>::value==factorial_func(10),"10");
    static_assert(FactorialNew<10>()()==factorial_func(10),"10");
    static_assert(FactorialNew<10>::value==factorial_func(10),"10");

    char ref[6*8];
    char oldway[Factorial<3>::value*8];
    char newway[factorial_func(3)*8];
    char newwaytempl[FactorialNew<3>()()*8];

    static_assert(sizeof(oldway)==sizeof(ref),"oldway");
    static_assert(sizeof(newway)==sizeof(ref),"newway");
    static_assert(sizeof(newwaytempl)==sizeof(ref),"newwaytempl");
}

/* vim: set ts=4 sw=4 tw=76: */
