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
    static inline std::mt19937 rdEngine{std::random_device{}()};
};

//forward declaration for Range because it is needed in MultiRange::operator|()
template <typename T, typename T2 = int, typename T3 = std::conditional_t<std::is_integral_v<T>&& std::is_floating_point_v<T2>, T2, T>>
class Range;

template<typename... Ranges>
class MultiRange
{
    std::tuple<Ranges...> ranges;
public:
    MultiRange(Ranges...ranges) :ranges{ ranges... } {}
    MultiRange(std::tuple<Ranges...> ranges):ranges{ranges}{}
    template<typename RangeIteratorTuple, typename EndValueTuple>
    class MultiRangeIterator
    {
        RangeIteratorTuple iters;
        const EndValueTuple endValues;

        template<size_t I = 0, typename...T, typename Func>
        static inline void tuple_apply_each(std::tuple<T...>& t, Func&& func)
        {
            func(std::get<I>(t));
            if constexpr (I + 1 != sizeof...(T))
                tuple_apply_each<I + 1>(t, std::forward<Func>(func));
        }

        template<size_t I = 0, typename...T, typename Func>
        static inline void tuple_apply_I(std::tuple<T...>& t, Func&& func)
        {
            func(std::get<I>(t));
        }

        template<size_t I = 0, typename Tuple1, typename Tuple2>
        static inline bool iters_equal(Tuple1 const& t1, Tuple2 const& t2)
        {
            if (std::get<I>(t1) != std::get<I>(t2))
                return false;
            if constexpr (I + 1 != std::tuple_size_v<Tuple1>)
                return iters_equal<I + 1>(t1, t2);
            return true;
        }

        template<size_t I = std::tuple_size_v<RangeIteratorTuple> -1 >
        void incIter()
        {
            if (auto& iter = std::get<I>(iters); ++iter == std::get<I>(endValues))
            {
                if constexpr (I != 0)
                {
                    iter.rewind();
                    incIter<I - 1>();
                }
            }
        }

    public:
        MultiRangeIterator(RangeIteratorTuple iters, EndValueTuple endValues) : iters{ iters }, endValues{ endValues } {}
        MultiRangeIterator& operator++()
        {
            incIter();
            return *this;
        }
        auto operator*()
        {
            return std::apply([](auto&... iters) { return std::make_tuple(*iters...); }, iters);
        }
        bool operator!=(MultiRangeIterator const& rhs) const
        {
            return std::get<0>(iters) != std::get<0>(endValues);
        }
    };

    auto begin()
    {
        return std::apply([](auto&... ranges) { return MultiRangeIterator{ std::make_tuple(ranges.begin()...), std::make_tuple(ranges.end()...) }; }, ranges);
    }
    auto end()
    {
        return std::apply([](auto&... ranges) {return MultiRangeIterator{ std::make_tuple(ranges.end()...), std::make_tuple(ranges.end()...) }; }, ranges);
    }

    template<typename Range>
    auto operator|(Range rhs)
    {
        return MultiRange{ std::tuple_cat(ranges, rhs) };
    }
};


template <typename T, typename T2, typename T3>
class Range: RangeRandomEngineBase
{
    T3 const min;
    T3 const max;
    T2 const step;
public:
    Range(T start, T end, T2 step = 1) : min(static_cast<T3>(start)), max(static_cast<T3>(end)), step(step) {}
    /*RangeIterator which can be used in range-based for loop*/
    class RangeIterator
    {
        const T3 start;
        T3 current;
        const T2 step;
    public:
        using value_type = T3;
        using step_type = T2;
        RangeIterator(T3 start, T2 step) : start(start), current(start), step(step) {}
        RangeIterator& operator++()
        {
            current += step;
            return *this;
        }
        T3 operator*() { return current; }
        bool operator!=(const RangeIterator& iter) const { return current < iter.current; }
        bool operator==(const RangeIterator& iter) const { return current == iter.current; }
        void rewind() { current = start; }
    };
    auto begin() { return RangeIterator(min, step); }
    auto end() { return RangeIterator(max, step); }

    /*Random number functions*/
    [[nodiscard]]auto rand()
    {
        if constexpr(std::is_integral_v<T3>)
        {
            static std::uniform_int_distribution<std::conditional_t<std::is_same_v<T, char>, int, T3>> rdInt{ min, max };
            return rdInt(rdEngine);
        }
        if constexpr(std::is_floating_point_v<T3>)
        {
            static std::uniform_real_distribution<T3> rdDouble{ min, max };
            return rdDouble(rdEngine);
        }
    }
    [[nodiscard]]T3 randFast() const
    {
        return (static_cast<double>(::rand()) / RAND_MAX) * (max - min) + min;
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
        return (number >= rhs.min) && (number <= rhs.max);
    }

    template<typename Num, typename = std::enable_if_t<std::is_arithmetic_v<Num>>>
    bool operator==(Num number) const
    {
        return number == (*this);
    }

    friend std::ostream& operator<<(std::ostream& os, Range const& range)
    {
        os << '[' << range.min << ',' << range.max << ']';
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

