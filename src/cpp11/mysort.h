/**
 * Cpp11 - [c] Steffen Dettmer 2012, 2014 <Steffen.Dettmer@gmail.com>
 *
 * Examples in form of test code demonstrating C++ 2011.
 *
 * \file cpp11/mysort.h Demonstrating the use of "tag types" to select
 *       template specializations.
 *
 * STL uses this a lot for containers (iterators) and numeric
 * functions. Main idea is to define types that allow distinguishing
 * different template instances. The types could be simple empty
 * structs (inheritance allows generalization, as usual).
 * For example, an iterator could be random-access or forward-only. To
 * be able to know this at compile time (for all classes of future
 * container iterators), the template uses an internal typedef to
 * define "iterator_category". This roughly (and a bit simplified) could
 * look this way:
 *
 * \code
 * struct my_input_iterator_tag { };
 * struct my_forward_iterator_tag : public my_input_iterator_tag { };
 * struct my_random_access_iterator_tag : public my_forward_iterator_tag { };
 *
 *
 * struct my_forward_list_iterator {
 *   typedef my_forward_iterator_tag iterator_category;
 * };
 * struct my_random_access_iterator {
 *   typedef my_random_access_iterator_tag iterator_category;
 * };
 * \endcode
 *
 * Even if having just soe container::iterator i, it can be determined at
 * compile time whether it is a random_access_iterator or a
 * forward_iterator, for example using a small helper:
 *
 * \code
 * template <typename T>
 * struct is_random { enum { value = 0 }; };
 * template <>
 * struct is_random<my_forward_iterator_tag> { enum { value = 0 }; };
 * template <>
 * struct is_random<my_random_access_iterator_tag> { enum { value = 1 }; };
 * \endcode
 *
 * With these tools, it can be used easily:
 *
 * \code
 * typedef my_vector_iterator myiterator;
 * static_assert(is_random<myiterator::iterator_category>{}() == 1, "never");
 * \endcode
 *
 * Please note, the old way was to write:
 *
 * \code
 * static_assert(is_random<myiterator::iterator_category>()() == 1, "never");
 * \endcode
 *
 * See namespace oversimplified_iterator_tags below for working example code.
 *
 * The STL uses this approach, but (of course as always) more sophisticated
 * using "traits".
 */

#ifndef CPP11_MYSORT_H
#define CPP11_MYSORT_H 1

#include <forward_list>
#include <algorithm>

// Sorting using a randomly accessible iterator can simply be delegated to
// standard sort function.
// Please note that we overload the function my_sort_helper using a third
// parameter, which does not even have a name, because not used otherwise.
template <typename RandomAccessIterator>
void my_sort_helper(
    RandomAccessIterator begin,
    RandomAccessIterator end,
    std::random_access_iterator_tag)
{
    std::sort(begin,end);
}

// Sorting a forward-only iteratable can be done using a temporary copy
// which is randomly accessible, such as a vector.
// The type of the third parameter forward_iterator_tag, so this overloaded
// version can be used for all forward-only-iterators.
template <typename ForwardAccessIterator>
void my_sort_helper(
    ForwardAccessIterator begin,
    ForwardAccessIterator end,
    std::forward_iterator_tag)
{
    std::vector<typename ForwardAccessIterator::value_type> v{begin, end};

    // or:
    // template <typename ContainerOrIterator>
    // using Value_type = typename ContainerOrIterator::value_type;
    // std::vector<Value_type<ForwardAccessIterator>> v{begin, end};

    std::sort(v.begin(), v.end());
    std::copy(v.begin(), v.end(), begin);
}

// All STL Container Classes define a type "iterator". We can define a
// short-hard such as "Iterator_type" allowing to write:
// "Iterator_type<Container>" instead of "Container::iterator".
template <typename Container>
using Iterator_type = typename Container::iterator;

// All STL Iterator Classes define an "iterator_category". We can define a
// short-hard such as "Iterator_category" allowing to write:
// "Iterator_category<Iterator>" instead of
// "typename <std::iterator_traits<Iterator>::iterator_category>".
template <typename Iterator>
using Iterator_category
    = typename std::iterator_traits<Iterator>::iterator_category;

// Now we can can call the appropriate overloaded sort function
// (of course only one call should be used, it makes not much sense to sort
// it three times, surely only the first would have any visible effect):
template <typename Container>
void my_sort(Container &c)
{
    my_sort_helper(
        c.begin(),
        c.end(),
        typename std::iterator_traits<
            typename Container::iterator
        >::iterator_category{}
    );

    // or using the short-hand helpers:
    my_sort_helper(
        c.begin(),
        c.end(),
        Iterator_category<Iterator_type<Container>>{}
    );

    // Stroustroup uses an additional type name "Iter" in his example
    // (which, or at least its name, seems to be a bit artificial to me):
    using Iter=Iterator_type<Container>;
    my_sort_helper(c.begin(), c.end(), Iterator_category<Iter>{});
}




// For better illustration, here the simplified example code:
namespace oversimplified_iterator_tags {

// Define some different (empty) types to distinguish.
struct my_input_iterator_tag { };
struct my_forward_iterator_tag : public my_input_iterator_tag { };
struct my_random_access_iterator_tag : public my_forward_iterator_tag { };

// Different iterators have different iterator categories tagged,
// Some my_forward_list could use a my_forward_list_iterator, which is a
// forward-only iterator, shown by iterator_category being a
// my_forward_iterator_tag type.
struct my_iterator {
  typedef my_input_iterator_tag iterator_category;
};
struct my_forward_list_iterator : public my_iterator {
  typedef my_forward_iterator_tag iterator_category;
};
struct my_random_access_iterator  : public my_iterator {
  typedef my_random_access_iterator_tag iterator_category;
};
// Some my_vector could use a my_vector_iterator, which is a
// my_random_access_iterator, shown by iterator_category being a
// my_random_access_iterator_tag type, of course it can be "indirect" by
// using specialization (inheritance).
struct my_vector_iterator : public my_random_access_iterator { };

// Now let's define a simple functionality to determine the iterator type at
// compile time. We make a template that is specialized for
// random_access_iterator_tag.
template <typename T>
struct is_random_access {
    enum { value = 0 };
    constexpr int operator()() { return value; }
};
template <>
struct is_random_access<my_forward_iterator_tag> {
    enum { value = 0 };
    constexpr int operator()() { return value; }
};
template <>
struct is_random_access<my_random_access_iterator_tag> {
    enum { value = 1 };
    constexpr int operator()() { return value; }
};

// We can check the value directly:
static_assert(is_random_access<int>::value == 0, "int is random access");
// or via default aka function-call operator on a (unname) instance:
static_assert(is_random_access<int>{}() == 0, "int is random access");

// When someone passes an iterator type only, for example myiterator:
typedef my_vector_iterator myiterator;
// It can be determined whether it is random access or not at compile time:
static_assert(
    is_random_access<myiterator::iterator_category>{}() == 1,
    "myiterator is no random access iterator, but in this test it should be."
);

// Alternatively, we could write this a constexpr function:
constexpr bool is_random_access_function(my_forward_iterator_tag tag) {
    return false;
}
constexpr bool is_random_access_function(my_random_access_iterator_tag tag) {
    return true;
}
// Maybe it is easier to read:
static_assert(
    is_random_access_function(myiterator::iterator_category{}),
    "myiterator is no random access iterator, but in this test it should be."
);
static_assert(
    !is_random_access_function(my_forward_list_iterator::iterator_category{}),
    "my_forward_list_iterator is random access, but it shouldn't be."
);
} // namespace oversimplified_iterator_tags


#endif // CPP11_MYSORT_H

/* vim: set ts=4 sw=4 tw=76: */

