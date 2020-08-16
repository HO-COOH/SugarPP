#pragma once
#include <type_traits>
#include <random>
#include <iostream>
#include <algorithm>
#include <variant>
#include <array>

class RangeRandomEngineBase
{
protected:
    static inline std::mt19937 rdEngine{ std::random_device{}() };
};

//forward declaration for Range because it is needed in MultiRange::operator|()
template <typename T, typename T2 = int, typename T3 = std::conditional_t<std::is_integral_v<T>&& std::is_floating_point_v<T2>, T2, T>>
class Range;

template<typename... Ranges>
class MultiRange
{
    std::tuple<Ranges...> ranges;

    template<size_t I=std::tuple_size_v<decltype(ranges)>-1>
    void incRange()
    {
        if(auto& range=std::get<I>(ranges); (++range).current == range.max)
        {
            if constexpr(I!=0)
            {
                range.rewind();
                incRange<I - 1>();
            }
        }
    }
public:
    MultiRange(Ranges...ranges) :ranges{ ranges... } {}
    MultiRange(std::tuple<Ranges...> ranges) :ranges{ ranges } {}

    auto begin()
    {
        return *this;
    }
    auto end()
    {
        return std::apply([](auto&... ranges) { return std::make_tuple(ranges.end()...); }, ranges);
    }
    MultiRange& operator++()
    {
        incRange();
        return *this;
    }
    template<typename EndValueTuple>
    bool operator!=(EndValueTuple const& rhs)
    {
        return std::get<0>(ranges) != std::get<0>(rhs);
    }
    auto operator*() const
    {
        return std::apply([](auto&... ranges) { return std::make_tuple(*ranges...); }, ranges);
    }

    template<typename Range>
    auto operator|(Range rhs)
    {
        return MultiRange{ std::tuple_cat(ranges, rhs) };
    }
};


template <typename T, typename T2, typename T3>
class Range : RangeRandomEngineBase
{
    T3 const start;
    T3 current;
    T3 const max;
    T2 const step;
public:
    Range(T start, T end, T2 step = 1) : start(static_cast<T3>(start)), current(static_cast<T3>(start)), max(static_cast<T3>(end)), step(step) {}
    auto operator*() const { return current; }
    auto begin() { return *this; }
    auto end() const { return max; }
    bool operator!=(Range rhs) const
    { 
        if constexpr (std::is_arithmetic_v<T3>)
            return current < rhs.current;
        else
            return current != rhs.current;
    }
    bool operator!=(T3 value) const
    {
        if constexpr (std::is_arithmetic_v<T3>)
            return current < value;
        else
            return current != value;
    }
    Range& operator++() { ++current; return *this; }
    void rewind() { current = start; }

    /*Random number functions*/
    [[nodiscard]] auto rand()
    {
        if constexpr (std::is_integral_v<T3>)
        {
            static std::uniform_int_distribution<std::conditional_t<std::is_same_v<T, char>, int, T3>> rdInt{ current, max };
            return rdInt(rdEngine);
        }
        if constexpr (std::is_floating_point_v<T3>)
        {
            static std::uniform_real_distribution<T3> rdDouble{ current, max };
            return rdDouble(rdEngine);
        }
    }
    [[nodiscard]] T3 randFast() const
    {
        return (static_cast<double>(::rand()) / RAND_MAX) * (max - current) + current;
    }

    template<typename Container>
    void fillRand(Container& container)
    {
        std::generate(std::begin(container), std::end(container), [this] {return rand(); });
    }
    template<typename InputIt>
    void fillRand(InputIt begin, InputIt end)
    {
        std::generate(begin, end, [this] {return rand(); });
    }
    template<typename Container>
    void fillRandFast(Container& container) const
    {
        std::generate(std::begin(container), std::end(container), [this] {return randFast(); });
    }
    template<typename InputIt>
    void fillRandFast(InputIt begin, InputIt end)
    {
        std::generate(begin, end, [this] {return randFast(); });
    }


    /*Added from my [HavingFun] repo to support range matching*/
    /**
     * @return true if number is within range, false otherwise
    */
    template <typename Num, typename = std::enable_if_t<std::is_arithmetic_v<Num>>>
    friend bool operator==(Num number, Range const& rhs)
    {
        return (number >= rhs.current) && (number <= rhs.max);
    }

    template<typename Num, typename = std::enable_if_t<std::is_arithmetic_v<Num>>>
    bool operator==(Num number) const
    {
        return number == (*this);
    }

    friend std::ostream& operator<<(std::ostream& os, Range const& range)
    {
        os << '[' << range.current << ',' << range.max << ']';
        return os;
    }

    template<typename...U>
    friend class MultiRange;

    template<typename Range>
    auto operator|(Range rhs)
    {
        return MultiRange{ *this, rhs };
    }
};

