/**
 * Cpp11 - [c] Steffen Dettmer 2012, 2014 <Steffen.Dettmer@gmail.com>
 *
 * Examples in form of test code demonstrating C++ 2011.
 *
 * \file cpp11/literals.cc Some simple literals.
 */

#include "cpp11/literals.h"
#include <sstream>
#include <iostream>

Color operator"" _col (const char *literal, size_t length) {
    int a=0, r=0, g=0, b=0;
    // parse literals like "0x000000ff_col".
    {
        std::stringstream str{literal};
        uint32_t rgb;
        str >> rgb;
        if (!str.fail()) {
            a = (rgb >> 24) & 0xff;
            r = (rgb >> 16) & 0xff;
            g = (rgb >>  8) & 0xff;
            b = (rgb      ) & 0xff;
        }
    }
    return Color(a, r, g, b);
}

Color operator"" _col (const unsigned long long value) {
    int a=0, r=0, g=0, b=0;
    {
        uint32_t rgb { static_cast<uint32_t>(value) };
        a = (rgb >> 24) & 0xff;
        r = (rgb >> 16) & 0xff;
        g = (rgb >>  8) & 0xff;
        b = (rgb      ) & 0xff;
    }
    return Color(a, r, g, b);
}

/* vim: set ts=4 sw=4 tw=76: */
