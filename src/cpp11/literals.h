/**
 * Cpp11 - [c] Steffen Dettmer 2012, 2014 <Steffen.Dettmer@gmail.com>
 *
 * Examples in form of test code demonstrating C++ 2011.
 *
 * \file cpp11/literals.h Some simple literal operator tests.
 *
 * For example, allow writing:
 *
 * \code
 * Complex c2 = 10.0 + 5.0_i;
 * \endcode
 */

#ifndef CPP11_LITERALS_H
#define CPP11_LITERALS_H 1

#include <complex>
#include <cstdint> // for uint8_t
#include <cstddef> // for size_t

// Allows Complex i = 5.0_i.
inline std::complex<long double> operator"" _i(const long double value) {
   return std::complex<long double>(0.0, value);
}
// Allows Complex i = 5_i.
inline std::complex<long double> operator"" _i(const unsigned long long value) {
   return std::complex<long double>(0.0, value);
}

// Allows Complex c = 10 + 5.0_i.
inline std::complex<long double> operator+ (
   const long double r, const std::complex<long double> c) {
   return std::complex<long double>(r,0) + c;
}

class Color {
  public:
    constexpr Color(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
       : a_(a), r_(r), g_(g), b_(b)
    { }
    constexpr bool operator==(const Color &color) {
       return (a_ == color.a_) &&
              (r_ == color.r_) &&
              (g_ == color.g_) &&
              (b_ == color.b_);
    }
  private:
    uint8_t a_;
    uint8_t r_;
    uint8_t g_;
    uint8_t b_;
};

Color operator"" _col (const char *literal, size_t length);
Color operator"" _col (const unsigned long long value);

const Color red   = 0x00ff0000_col;
const Color green = 0x0000ff00_col;
const Color blue  = 0x000000ff_col;

#endif // CPP11_LITERALS_H

/* vim: set ts=4 sw=4 tw=76: */
