/*****************************************************************//**
 * \file   Enumerate.hpp
 * \brief  Python's enumerate port
 * 
 * \author Peter
 * \date   September 2020
 *********************************************************************/
#pragma once

#include <tuple>
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
    Enumerate(Iterable& iterable, CounterType start=0) :iterable(iterable), index(start){}
    template<typename Iterator>
    class EnumerateIterator
    {
        Iterator iter;
        CounterType index;
    public:
        EnumerateIterator(Iterator iter, CounterType index):iter(std::move(iter)), index(index){}
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
     * @return Return an EnumerateIterator object, that points to the start of the `iterable`
     */
    auto begin() { return EnumerateIterator{ std::begin(iterable), index }; }

    /**
     * @return Return an EnumerateIterator object, that points to the start of the `iterable`
     */
    auto end() { return EnumerateIterator{ std::end(iterable), index }; }
};