/**
 * Cpp11 - [c] Steffen Dettmer 2012, 2014 <Steffen.Dettmer@gmail.com>
 *
 * Examples in form of test code demonstrating C++ 2011.
 *
 * \file cpp11/myvector.h A simple own vector class, demonstrating
 *       std::vector ideas.
 */

#ifndef CPP11_MYVECTOR_H
#define CPP11_MYVECTOR_H 1

#include <list>
#include <string>
#include <stdexcept>

template<typename T>
class MyVector
{
public:
    MyVector()
        : elem{nullptr} { }

    explicit MyVector(int size)
        : sz{static_cast<size_t>(size)}
    {
        if (size<0) {
            throw std::length_error("constructing MyVector");
        }
        elem=new T[size];
    }

    MyVector(std::initializer_list<T> list)
        : elem{new T[list.size()]}, sz{list.size()}
    {
        std::copy(list.begin(), list.end(), elem);
        // from Stroustrup FAQ: for std::vector<>:
        // reserve(list.size());
        // std::uninitialized_copy(list.begin(), list.end(), elem);
        // sz=list.size();
    }

    MyVector(const MyVector<T> &v)
        : elem{new T[v.s]}, sz{v.sz}
    {
        std::copy(v.begin(), v.end(), elem);
    }

    MyVector(MyVector<T> &&v)
        : elem{v.elem}, sz{v.sz} { v.elem=NULL; v.sz=0; }

    virtual ~MyVector() { if (elem) delete[] elem; }

    virtual MyVector<T> &operator=(const MyVector<T> &v);

    virtual MyVector<T> &operator=(MyVector<T> &&v);

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
inline MyVector<T> &MyVector<T>::operator=(const MyVector<T> &v)
{
    if (elem) delete[] elem;
    elem=new T[v.sz];
    sz=v.sz;
    std::copy(v.begin(), v.end(), elem);
    return *this;
}


template <typename T>
inline MyVector<T> &MyVector<T>::operator=(MyVector<T> &&v)
{
    elem=v.elem;
    v.elem=nullptr;
    sz=v.sz;
    v.sz=0;
    return *this;
}

template <typename T>
inline T& MyVector<T>::operator[](int i)
{
    if (i>=0 && i<size()) {
        return elem[i];
    } else {
        throw std::out_of_range("index operator[]");
    }
}

template <typename T>
inline const T& MyVector<T>::operator[](int i) const
{
    if (i>=0 && i<size()) {
        return elem[i];
    } else {
        throw std::out_of_range("index operator[]");
    }
}


#endif // CPP11_MYVECTOR_H

/* vim: set ts=4 sw=4 tw=76: */
