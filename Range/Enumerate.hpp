#pragma once
#include <utility>
template<typename Iterable, typename CounterType = size_t>
class Enumerate
{
    Iterable& iterable;
    CounterType index;
public:
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
            return std::pair{ index, *iter };
        }
        bool operator!=(EnumerateIterator const& rhs) const
        {
            return iter != rhs.iter;
        }
    };
    auto begin() { return EnumerateIterator{ std::begin(iterable), index }; }
    auto end() { return EnumerateIterator{ std::end(iterable), index }; }
};