/**
 * Cpp11 - [c] Steffen Dettmer 2012, 2014 <Steffen.Dettmer@gmail.com>
 *
 * Examples in form of test code demonstrating C++ 2011.
 *
 * \file dummy.cc Test if all headers compile and if something is linkable.
 */

#include <string>
#include <iostream>
#include "cpp11/cpp11.h"

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

int main()
{
    Vector<std::string> v{"Hello,"," world!"};
    for (auto &e: v) {
        std::cout << e;
    }
    std::cout << std::endl;
    return v.size()==2 ? 0 : 1;
}

