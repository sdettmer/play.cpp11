/**
 * Cpp11 - [c] Steffen Dettmer 2012, 2014 <Steffen.Dettmer@gmail.com>
 *
 * Examples in form of test code demonstrating C++ 2011.
 *
 * \file cpp11/factorial.h A simple factorialization class.
 */

#ifndef CPP11_FACTORIAL_H
#define CPP11_FACTORIAL_H 1

// Factorial meta "old" style.
// A specialization is used, the value must be extracted explicitely.
template<int N> struct Factorial {
    enum { value = N * Factorial<N-1>::value };
};
template<> struct Factorial<0> {
    enum { value = 1 };
};

// Factorial meta "new" style.
// Thanks to constexpr, it can be written like an ordinary function, much
// simpler to read and understand. It can also be called at runtime.
constexpr int factorial_func(const int value) {
    return (value==0) ? 1 : value*factorial_func(value-1);
}

// Factorial meta "old-with-new" style.
// If a specialization is used, the value can be accessed using the default
// operator aka function call operator. This forms a more uniform logical
// interface, because operator() can be used to access value, without
// needing to know the name of the template member, similar for other
// templates where operator() could do whatever is appropriate without
// needing to disclose internals nor needing to remember specifics.
template<int N> struct FactorialNew {
    enum { value = N * FactorialNew<N-1>::value };
    constexpr int operator()() { return value; };
};
template<> struct FactorialNew<0> {
    enum { value = 1 };
};

#endif // CPP11_FACTORIAL_H

/* vim: set ts=4 sw=4 tw=76: */
