/*****************************************************************//**
 * \file   Range.hpp
 * \brief  Numeric ranges/multi-range
 * 
 * \author Peter
 * \date   September 2020
 * \note Not to be confused with C++20's ranges
 *********************************************************************/

#pragma once
#include <type_traits>
#include <random>
#include <iostream>
#include <algorithm>
#include <variant>
#include <array>


/**
 * @brief Shared random engine for all @ref Range
 * @details Stored a `static` protected `std::mt19937` member, which will be initialized at first use.
 */
class RangeRandomEngineBase
{
protected:
    static inline std::mt19937 rdEngine{ std::random_device{}() };
};


/**
 * @brief Return the correct arithmetic type of \p T1 and \p T2
 * @details
 * The purpose of these types is to solve the problem of type conversion.
 * For example:
 * {signed shorter, unsigned longer} -> signed longer
 * {unsigned shorter, unsigned longer} -> unsigned longer
 * {unsigned shorter, signed longer} -> signed longer
 * {signed shorter, signed longer} -> signed longer
 */
template<typename T1, typename T2, typename>
struct CommonValueType
{
    using type = std::conditional_t          //when both are integer types
        <
        std::is_signed_v<T1> || std::is_signed_v<T2>,
        std::make_signed_t<std::common_type_t<T1, T2>>, //either is signed, return longer signed integer type
        std::common_type_t<T1, T2> //none is signed, return the longer unsigned type
        >;
};

template<typename T1, typename T2>
struct CommonValueType<T1, T2, std::enable_if_t<std::is_floating_point_v<T1> || std::is_floating_point_v<T2>>>
{
    using type = std::common_type_t<T1, T2>;
};


template <typename T1,  //start
    typename T2,        //end
    typename T3 = int,
    typename ValueType = typename CommonValueType<T1, T2, void>::type,
    typename StepType=std::common_type_t<ValueType, T3>>
class Range;

/**
 * @brief A multiple ranges wrapper which handles any number/type of Range objects which can be used in a range-based for loop
 * @details
 * A typical usage is
 * ~~~~{.cpp}
 *     for(auto [i, j] ： Range(0, 10) | Range(0, 100))
 *     {
 *         ...
 *     }
 * ~~~~
 *
 * A MultiRange object stores 2 things:
 *
 * 1. The Range objects it is constructed from
 *
 * 2. The starting values of these objects, so that they can be reset once they reached to their end values
 * @tparam Ranges type of different ranges constructed 
 */
template<typename... Ranges>
class MultiRange
{
    std::tuple<Ranges...> ranges;
    std::tuple<typename Ranges::value_type...> startValues;


    /**
     * @brief A helper function to increment the stored ranges
     * @details
     * The ranges is incremented from the lowest index -> highest index.
     *
     * Once a range object reaches its end value, it is reset to the original begins and the next range object is incremented, as what you typically do in a nested for-loop.
     *
     * For example, suppose we have a @ref MultiRange constructed from Range(0,3) and Range(0,5), it will be incremented as:
     *
     * [0,0] [0,1] [0,2]...[0,4] [1,0] [1,1] [1,2]...[2,0]...[2,4], then stop and exit
     *
     * @tparam I Current "index" into the ranges tuple
     */
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
    /**
     * @brief Construct a MultiRange object from any number and any type of Range objects 
     */
    MultiRange(Ranges...ranges) :ranges{ ranges... }, startValues{ ranges.current ... } {}


    /**
     * @brief Construct a MultiRange object from a std::tuple of Range objects
     */
    MultiRange(std::tuple<Ranges...> ranges) :ranges{ ranges }, startValues{ std::apply(
		[](auto const&... ranges)
		{
			return std::make_tuple(ranges.current...);
		}, ranges
	) } {}

    /**
     * @brief Return *this, unchanged
     */
    auto begin()
    {
        return *this;
    }

    /**
     * @brief Return a std::tuple of all the end values of the ranges it was constructed
     */
    auto end()
    {
        return std::apply([](auto&... ranges) { return std::make_tuple(ranges.end()...); }, ranges);
    }

    /**
     * @brief Increment *this
     * @see incRange
     */
    MultiRange& operator++()
    {
        incRange();
        return *this;
    }

    /**
     * @brief Compares whether two MultiRanges objects are equal
     * @details
     * Comparison is done by comparing the highest index Range object, as you typically do in the outer-most layer of a nested for loop
     */
    bool operator!=(MultiRange const& rhs) const
    {
        return std::get<0>(ranges) != std::get<0>(rhs.ranges);
    }

    /**
     * @brief Compares whether the current values of Range objects in @a *this are the same as in @p rhs
     * @param rhs Should be a std::tuple of end values
     */
    template<typename EndValueTuple>
    bool operator!=(EndValueTuple const& rhs)
    {
        return std::get<0>(ranges) != std::get<0>(rhs);
    }

    /**
     * @brief Return a tuple of the current values in *this 
     */
    auto operator*() const
    {
        return std::apply([](auto&... ranges) { return std::make_tuple(*ranges...); }, ranges);
    }

    /**
     * @brief An intuitive way to concat a Range object to a MultiRange object
     * @param rhs Should be a Range type object
     */
    template<typename Range>
    auto operator|(Range rhs)
    {
        return MultiRange{ std::tuple_cat(ranges, rhs) };
    }
};



/**
 * @brief A range represents a collection of values between a minimum -> maximum, where maximum is exclusive
 * @tparam T1 Type of the start value
 * @tparam T2 Type of the end value
 * @tparam T3 Type of the stepping value
 * @tparam ValueType @see CommonValueType
 * @tparam StepType `std::common_type_t<ValueType, StepType>`
 * @details
 * An example for ValueType conversion is:
 * ~~~~{.cpp}
 *      std::vector<int> v{1,2,3};
 *      for(auto i:Range(0, v.size()) //here T1:int, T2:size_t, ValueType:long long
 *      {//...}
 * ~~~~
 */
template <typename T1, typename T2, typename T3, typename ValueType, typename StepType>
class Range : RangeRandomEngineBase
{
    ValueType current;
    ValueType const max;
public:
    StepType const step;
    using value_type = ValueType;
    /**
     * @brief Construct a range object, where `start` is incremented by `step` until >= `end`, `end` is exclusive, meaning the last value you get is always < `end`
     * @note All parameters are expected to be arithmetic values
     */
    Range(T1 start, T2 end, T3 step = 1) : current(static_cast<ValueType>(start)), max(static_cast<ValueType>(end)), step(static_cast<StepType>(step)) {}

    /**
     * @brief Return the current value
     */
    auto operator*() const { return current; }

    /**
     * @brief Return the object itself, unchanged, for support of range-based for loop
     */
    auto begin() { return *this; }

    /**
     * @brief Return the end value, for support of range-based for loop
     */
    [[nodiscard]]auto end() const { return max; }

    /**
     * @brief Return how many steps it takes from `start` -> `end`, which means `start + steps() * step` >= `end`
     */
    [[nodiscard]]auto steps() const { return (max - current) / step + 1; }

    /**
     * @brief Return the span of the range, that is `max-min`
     */
    [[nodiscard]] auto span() const { return max - current; }

    /**
     * @brief Return the next value of the current range object
     */
    [[nodiscard]] auto next() const { return static_cast<value_type>(current + step); }

    /**
     * @brief Return whether the current value of `this` == `rhs`
     */
    bool operator!=(Range rhs) const
    {
        if constexpr (std::is_arithmetic_v<value_type>)
            return current < rhs.current;
        else
            return current != rhs.current;
    }

    /**
     * @brief Return whether the current value of `this` == `value`
     */
    bool operator!=(value_type value) const
    {
        if constexpr (std::is_arithmetic_v<value_type>)
            return current < value;
        else
            return current != value;
    }

    /**
     * @brief Increment the current value by `step`
     */
    Range& operator++() { current += step; return *this; }

    /**
     * @brief Increment the current value by `i*step`
     * @param i Number of steps to increment
     */
    Range& operator+=(unsigned i) { current += i * step; return *this; }

    /*Random number functions*/

    /**
     * @brief Represent the correct Uniform distribution type. `std::uniform_int_distribution` if the range is constructed from integer type, otherwise return the correct type of `std::uniform_real_distribution`
     * @details
     * `std::uniform_int_distribution<>` does not accepts `char` or `signed char` type, and they are not the same types either.
     *  Therefore, if `value_type` is either of those 2 types, we need to convert to `int`
     */
    using DistType = std::conditional_t <
        std::is_integral_v<value_type>,
        std::uniform_int_distribution<std::conditional_t<std::is_same_v<value_type, char>||std::is_same_v<value_type, signed char>||std::is_same_v<value_type, unsigned char>, int, value_type>>,
        std::uniform_real_distribution<value_type>
    >;

    /**
     * @brief Return the initialized `DistType`
     * @note `std::uniform_int_distribution` produces uniformly distributed values on the @b closed interval [a,b].
     * Therefore we need to minus 1 from the maximum value to get the expected behavior.
     */
    [[nodiscard]] auto getDistribution() const
    {
        if constexpr(std::is_integral_v<value_type>)
            return DistType(current, max - 1);
        return DistType(current, max);
    }

    /**
     * @brief Return the inherited random engine
     */
    [[nodiscard]] static auto& getRandomEngine()
    {
        return rdEngine;
    }

    /**
     * @brief Return a correct type of random number within the range
     */
    [[nodiscard]] auto rand() const
    {
        return getDistribution()(rdEngine);
    }

    /**
     * @brief Return a correct type of several random numbers within the range
     * @tparam N Compile time constant
     * @note
     * Intended usage is with C++17 structured binding, so that you can define multiple random numbers with one line.
     * ~~~~{.cpp}
     * auto [num1, num2, num3]=Range(0, 100).rand<3>();
     * ~~~~
     */
    template<size_t N>
    [[nodiscard]] auto rand() const
    {
        std::array<value_type, N> values;
        std::generate(values.begin(), values.end(), [dist = getDistribution()]{ return dist(rdEngine); });
        return values;
    }

    /**
     * @brief Return a correct type of random number within the range, using C `rand()` function, which is less ideal, but maybe faster
     */
    [[nodiscard]] value_type randFast() const
    {
        return static_cast<value_type>(static_cast<double>(::rand()) / RAND_MAX * (static_cast<double>(max) - current) + current);
    }


    /**
     * @brief Fill the container with random number, the container is expected to have the space to be filled
     * @tparam Container Type of the container
     * @param container The container to be filled
     * @note The `container` needs to support `std::begin` and `std::end`
     */
    template<typename Container>
    void fillRand(Container& container)
    {
        fillRand(std::begin(container), std::end(container));
    }

    /**
     * @brief Fill the container with specified number of random numbers with `push_back`
     * @tparam Container Type of the container
     * @param container The container to be filled
     * @param count number of random numbers to be pushed back to the container
     * @note The `container` needs to support `push_back()`, which is used by `std::back_inserter`, and `size()`
     */
    template<typename Container>
    void fillRand(Container& container, size_t count)
    {
        std::generate_n(container.size()>=count? std::begin(container): std::back_inserter(container), count, [ dist = getDistribution() ]() mutable
        {
            return dist(rdEngine);
        });
    }

    /**
     * @brief Fill the range of [begin, end) with random numbers
     * @tparam InputIt The type of the two iterators, at least an input iterator
     * @param begin The iterator pointing to the start of the range
     * @param end The iterator pointing to the pass-end of the range
     */
    template<typename InputIt>
    void fillRand(InputIt begin, InputIt end)
    {
        //The maximum value of std::uniform_int_distribution is inclusive so need to -1 to exclude the max value edge case
        std::generate(begin, end, [dist = getDistribution()]() mutable
        {
            return dist(rdEngine);
        });
    }

    /**
     * @brief Same as `FillRand(container)` but uses C random functions
     */
    template<typename Container>
    void fillRandFast(Container& container) const
    {
        std::generate(std::begin(container), std::end(container), [this] {return randFast(); });
    }

    /**
     * @brief Same as `FillRand(container, count)` but uses C random functions
     */
    template<typename Container>
    void fillRandFast(Container& container, size_t count)
    {
        std::generate_n(container.size() >= count ? std::begin(container) : std::back_inserter(container), count, [this]
        {
            return randFast();
        });
    }

    /**
     * @brief Same as `FillRand(begin, end)` but uses C random functions
     */
    template<typename InputIt>
    void fillRandFast(InputIt begin, InputIt end)
    {
        std::generate(begin, end, [this] {return randFast(); });
    }

    /**
     * @brief Return whether a number is within a range
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

    template<typename Num, typename = std::enable_if_t<std::is_arithmetic_v<Num>>>
    bool contain(Num number) const
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

    /**
     * @brief Compose a MultiRange object with `this` and `rhs`
     * @tparam Range Type of the right-hand-side range object
     * @param rhs The range object to compose with `this`
     * @return A `MultiRange` object
     * @see `MultiRange`
     */
    template<typename Range>
    auto operator|(Range rhs)
    {
        return MultiRange{ *this, rhs };
    }
};


/*Substitute the in<Container> */
template<typename T, typename ContainerType>
class ContainerRangeBase;

template <typename Container>
class Range<void, void, void, Container, void>:public ContainerRangeBase<Range<void, void, void, Container, void>, Container>
{
    Container& range;
public:
    Range(Container& container) :range(container) {}
    Range(Container&& container) = delete;  //Another overload is necessary to handle the case when [container] is an rvalue, otherwise dangling reference
    friend ContainerRangeBase<Range<void, void, void, Container, void>, Container>;
};

template <typename Container>
class Range<void, void, void, Container&&, void> :public ContainerRangeBase<Range<void, void, void, Container&&, void>, Container>
{
    Container range;
public:
    Range(Container& container) = delete;
    Range(Container&& container) :range(std::forward<Container>(container)){}//copy the temporary 
    friend ContainerRangeBase<Range<void, void, void, Container&&, void>, Container>;
};

template<typename T, typename ContainerType>
class ContainerRangeBase
{
public:
    using value_type = typename ContainerType::value_type;
    bool operator==(value_type const& value) const
    {
        auto&& range = static_cast<T const&>(*this).range;
        return std::find(std::cbegin(range), std::cend(range), value) != std::cend(range);
    }
};


template<typename Range>
bool operator==(typename Range::value_type&& value, Range const& in)
{
    return in == value;
}

template<typename Container>
Range(Container&)->Range<void, void, void, Container, void>;

template<typename Container>
Range(Container&&)->Range<void, void, void, Container&&, void>;

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
auto parallel(RangeType range, Func&& func, unsigned threadCount)
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