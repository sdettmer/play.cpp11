/**
 * Cpp11 - [c] Steffen Dettmer 2012, 2014 <Steffen.Dettmer@gmail.com>
 *
 * Examples in form of test code demonstrating C++ 2011.
 *
 * \file cpp11/cpp11.h A simple Vector class.
 */

#ifndef CPP11_CPP11_H
#define CPP11_CPP11_H 1

#include <string>
#include <exception>
#include <stdexcept>

template<typename T>
class Vector
{
public:
    Vector()
        : elem{nullptr} { }

    explicit Vector(int size)
        : sz{static_cast<size_t>(size)}
    {
        if (size<0) {
            throw std::length_error("constructing Vector");
        }
        elem=new T[size];
    }

    Vector(std::initializer_list<T> list)
        : elem{new T[list.size()]}, sz{list.size()}
    {
        std::copy(list.begin(), list.end(), elem);
        // from Stroustrup FAQ: for std::vector<>:
        // reserve(list.size());
        // std::uninitialized_copy(list.begin(), list.end(), elem);
        // sz=list.size();
    }

    Vector(const Vector<T> &v)
        : elem{new T[v.s]}, sz{v.sz}
    {
        std::copy(v.begin(), v.end(), elem);
    }

    Vector(Vector<T> &&v)
        : elem{v.elem}, sz{v.sz} { v.elem=NULL; v.sz=0; }

    virtual ~Vector() { if (elem) delete[] elem; }

    virtual Vector<T> &operator=(const Vector<T> &v);

    virtual Vector<T> &operator=(Vector<T> &&v);

    virtual T& operator[](int i);
    virtual const T& operator[](int i) const;

    virtual size_t size() const { return sz; }

    virtual T* begin() { return elem; }
    virtual const T* begin() const { return elem; }

    virtual T* end() { return elem+size(); }
    virtual const T* end() const { return elem+size(); }

private:
    T *elem;
    size_t sz;
};

template <typename T>
Vector<T> &Vector<T>::operator=(const Vector<T> &v)
{
    if (elem) delete[] elem;
    elem=new T[v.sz];
    sz=v.sz;
    std::copy(v.begin(), v.end(), elem);
    return *this;
}


template <typename T>
Vector<T> &Vector<T>::operator=(Vector<T> &&v)
{
    elem=v.elem;
    v.elem=nullptr;
    sz=v.sz;
    v.sz=0;
    return *this;
}

template <typename T>
T& Vector<T>::operator[](int i)
{
    if (i>=0 && i<size()) {
        return elem[i];
    } else {
        throw std::out_of_range("index operator[]");
    }
}

template <typename T>
const T& Vector<T>::operator[](int i) const
{
    if (i>=0 && i<size()) {
        return elem[i];
    } else {
        throw std::out_of_range("index operator[]");
    }
}

#endif // CPP11_CPP11_

/* vim: set ts=4 sw=4 tw=76: */
