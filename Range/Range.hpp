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
    std::tuple<typename Ranges::value_type...> startValues;
    template<size_t I = std::tuple_size_v<decltype(ranges)>-1>
    void incRange()
    {
        if (auto& range = std::get<I>(ranges); (++range).current == range.max)
        {
            if constexpr (I != 0)
            {
                range.current = std::get<I>(startValues);
                incRange<I - 1>();
            }
        }
    }
public:
    MultiRange(Ranges...ranges) :ranges{ ranges... }, startValues{ ranges.current ... } {}
    MultiRange(std::tuple<Ranges...> ranges) :ranges{ ranges }, startValues{ ranges.current ... } {}

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
    bool operator!=(MultiRange const& rhs) const
    {
        return std::get<0>(ranges) != std::get<0>(rhs.ranges);
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
    T3 current;
    T3 const max;
public:
    T2 const step;
    using value_type = T3;
    Range(T start, T end, T2 step = 1) : current(static_cast<T3>(start)), max(static_cast<T3>(end)), step(step) {}
    auto operator*() const { return current; }
    auto begin() { return *this; }
    auto end() const { return max; }
    auto steps() const { return (max - current) / step + 1; }
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
    Range& operator+=(unsigned i) { current += i * step; return *this; }

    /*Random number functions*/
    [[nodiscard]] auto getDistribution() const
    {
        if constexpr (std::is_integral_v<T3>)
        {
            return std::uniform_int_distribution<std::conditional_t<std::is_same_v<T, char>, int, T3>>{ current, max };
        }
        if constexpr (std::is_floating_point_v<T3>)
        {
            return std::uniform_real_distribution<T3>{ current, max };
        }
    }
    [[nodiscard]] static inline auto& getRandomEngine()
    {
        return rdEngine;
    }
    [[nodiscard]] auto rand()
    {
        if constexpr (std::is_integral_v<T3>)
        {
            return std::uniform_int_distribution<std::conditional_t<std::is_same_v<T, char>, int, T3>>{ current, max }(rdEngine);
        }
        if constexpr (std::is_floating_point_v<T3>)
        {
            return std::uniform_real_distribution<T3>{ current, max }(rdEngine);
        }
    }
    [[nodiscard]] T3 randFast() const
    {
        return (static_cast<double>(::rand()) / RAND_MAX) * (max - current) + current;
    }

    template<typename Container>
    void fillRand(Container& container)
    {
        fillRand(std::begin(container), std::end(container));
    }
    template<typename Container>
    void fillRand(Container& container, size_t count)
    {
        auto begin = std::begin(container);
        if constexpr (std::is_integral_v<T3>)
        {
            //The maximum value of std::uniform_int_distribution is inclusive so need to -1 to exclude the max value edge case
            std::generate_n(std::back_inserter(begin), count, [rdInt = std::uniform_int_distribution<std::conditional_t<std::is_same_v<T, char>, int, T3>>{ current, max - 1 }]() mutable
            {
                return rdInt(rdEngine);
            });
        }
        if constexpr (std::is_floating_point_v<T3>)
        {
            std::generate_n(std::back_inserter(begin), count, [rdFloat = std::uniform_real_distribution<T3>{ current, max }]() mutable
            {
                return rdFloat(rdEngine);
            });
        }
    }
    template<typename InputIt>
    void fillRand(InputIt begin, InputIt end)
    {
        if constexpr (std::is_integral_v<T3>)
        {
            //The maximum value of std::uniform_int_distribution is inclusive so need to -1 to exclude the max value edge case
            std::generate(begin, end, [rdInt = std::uniform_int_distribution<std::conditional_t<std::is_same_v<T, char>, int, T3>>{ current, max - 1 }]() mutable
            {
                return rdInt(rdEngine);
            });
        }
        if constexpr (std::is_floating_point_v<T3>)
        {
            std::generate(begin, end, [rdFloat = std::uniform_real_distribution<T3>{ current, max }]() mutable
            {
                return rdFloat(rdEngine);
            });
        }
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

#include <thread>
#include <vector>


/**
 * @brief A parallel for loop for a specific range
 * @tparam Range The type of [range], which is of the form: Range<value_type, value_type, stepSizeType>
 * @tparam Func The type of [func], which is of the form: Func<ReturnType(Range)>
 * @param range The range loop variable
 * @param func Should be a function that takes a range as parameter and may or may not return stuff
 * @param threadCount The hint of number of threads to launch. The real number of threads depend on the number of steps in [range]
 * @return std::vector<ReturnType> / void if [func] returns void
*/
template<typename RangeType, typename Func>
auto parallel(RangeType range, Func&& func, unsigned threadCount = std::thread::hardware_concurrency())
->std::enable_if_t< std::is_same_v<std::invoke_result_t<std::remove_reference_t<Func>, RangeType>, void>>
{
    /* If there are 7 tasks but 8 threads, we only launch 7 threads
     *
     */
    const auto steps = range.steps();
    const auto threadNum = std::min<std::common_type_t<decltype(steps), decltype(threadCount)>>(steps, threadCount);
    const auto perThread = steps / threadNum;
    std::vector<std::thread> threads;
    threads.reserve(threadNum);


    //for the first (threadNum-1) threads
    for (auto i = 0; i < threadNum - 1; ++i)
        threads.emplace_back([&func, &range, perThread] { func(Range{ *range, *(range += perThread), range.step }); });
    //the last thread
    threads.emplace_back([&func, &range] {func(Range{ *(++range), range.end(), range.step }); });
    for (auto& thread : threads)
        thread.join();

}


template<typename RangeType, typename Func>
auto parallel(RangeType range, Func&& func, unsigned threadCount = std::thread::hardware_concurrency())
->std::enable_if_t<!std::is_same_v<std::invoke_result_t<std::remove_reference_t<Func>, RangeType>, void>>
{
    const auto steps = range.steps();
    const auto threadNum = std::min<std::common_type_t<decltype(steps), decltype(threadCount)>>(steps, threadCount);
    const auto perThread = steps / threadNum;
    std::vector<std::thread> threads;
    threads.reserve(threadNum);

    using result_type = std::invoke_result_t<std::remove_reference_t<Func>, RangeType>;
    std::vector<result_type> results;
    results.reserve(threadNum);
    for (auto i = 0; i < threadNum; ++i)
        threads.emplace_back([&func, &range, perThread, &results] { results.emplace_back(std::forward<result_type>(func(Range{ *range, *(range += perThread), range.step }))); });
    //the last thread
    threads.emplace_back([&func, &range, &results] {results.emplace_back(std::forward<result_type>(func(Range{ *range, range.end(), range.step }))); });
    for (auto& thread : threads)
        thread.join();
    return results;
}