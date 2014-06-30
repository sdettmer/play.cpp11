/**
 * Cpp11 - [c] Steffen Dettmer 2012, 2014 <Steffen.Dettmer@gmail.com>
 *
 * Examples in form of test code demonstrating C++ 2011.
 *
 * \file testrunner.c Runs all tests that have been registered by
 *                    CPPUNIT_TEST.
 */

/* Implement simple CppUnit test runner
 * (see http://sourceforge.net/projects/cppunit/) */
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <iostream>

int main()
{
    // Get the top level suite from the registry to access
    //   the tests registered by CPPUNIT_TEST
    CppUnit::Test *suite
        = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    // Adds the test to the list of test to run
    CppUnit::TextUi::TestRunner runner;
    runner.addTest(suite);

    // Change the default outputter to a compiler error format outputter
    runner.setOutputter(
        new CppUnit::CompilerOutputter(&runner.result(), std::cerr)
    );

    // Run the tests.
    const bool wasSucessful = runner.run();

    // Return error code 1 if the one of test failed.
    return wasSucessful ? 0 : 1;
}

/* vim: set ts=4 sw=4 tw=76: */
