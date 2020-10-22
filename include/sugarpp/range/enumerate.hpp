/*****************************************************************//**
 * \file   Enumerate.hpp
 * \brief  Python's enumerate port
 * 
 * \author Peter
 * \date   September 2020
 *********************************************************************/
#pragma once

#include <tuple>

#ifdef SugarPPNamespace
namespace SugarPP
{
#endif

    /**
     * @brief The Enumerate iterator object which is used in range-based for loop with Enumerate class
     * @details I tried to nest this class inside Enumerate, but it won't successfully deduce class template and seems to be considered as a defect in standard.
     * This happens in nested template class (both the outter class and the inner class are templates).
     * Futhermore, adding a deduction guide inside will cause GCC error: "deduction guide 'A<T>::B(U, T) -> A<T>::B<U>' must be declared at namespace scope", while Clang accepts it.
     * @see https://godbolt.org/z/1Ej7bd
     * @see https://stackoverflow.com/questions/46103102/how-to-provide-deduction-guide-for-nested-template-class
     */
    template<typename Iterator, typename CounterType = size_t>
    class EnumerateIterator
    {
        Iterator iter;
        CounterType index;
    public:
        EnumerateIterator(Iterator iter, CounterType index) :iter(std::move(iter)), index(index) {}
        EnumerateIterator& operator++()
        {
            ++iter;
            ++index;
            return *this;
        }
        auto operator*()
        {
            return std::forward_as_tuple(index, *iter);
        }
        bool operator!=(EnumerateIterator const& rhs) const
        {
            return iter != rhs.iter;
        }
    };
    /**
     * @brief A Python-like Enumerate object, when dereference, returns <index, content>
     * @tparam Iterable The type of the `iterable` object
     * @tparam CounterType The type of the counter, default to `size_t`
     */
    template<typename Iterable, typename CounterType = size_t>
    class Enumerate
    {
        Iterable& iterable;
        CounterType index;
    public:
        /**
         * @brief Construct an Enumerate object by an iterable and an optional index
         * @param iterable Any type of object that supports iteration, eg. returns a iterator when calling `std::begin(iterable)`
         * @param start A counter which defaults to 0
         */
        Enumerate(Iterable& iterable, CounterType start = 0) :iterable(iterable), index(start) {}
        /**
         * @return Return an EnumerateIterator object, that points to the start of the `iterable`
         */
        auto begin() { return EnumerateIterator{ std::begin(iterable), index }; }

        /**
         * @return Return an EnumerateIterator object, that points to the end of the `iterable`
         */
        auto end() { return EnumerateIterator{ std::end(iterable), index }; }
    };
#ifdef SugarPPNamespace
}
#endif