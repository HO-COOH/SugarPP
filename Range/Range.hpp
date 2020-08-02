#pragma once
#include <type_traits>
#include <random>
#include <iostream>
#include <algorithm>

class RangeRandomEngineBase
{
protected:
    static inline std::mt19937 rdEngine{std::random_device{}()};
};

template <typename T, typename T2 = int, typename T3 = typename std::conditional<std::is_integral_v<T>&& std::is_floating_point_v<T2>, T2, T>::type>
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
        T3 current;
        const T2& step;
    public:
        RangeIterator(T3 start, const T2& step) : current(start), step(step) {}
        RangeIterator& operator++()
        {
            current += step;
            return *this;
        }
        T3 operator*() { return current; }
        bool operator!=(const RangeIterator& iter) { return abs(current) <= abs(iter.current); }
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
    template<typename ElementType, size_t SIZE>    //specialization for C-array
    void fillRand(ElementType (&arr)[SIZE])
    {
        std::generate(std::begin(arr), std::end(arr), [this] {return rand(); });
    }
    template<typename InputIt>
    void fillRand(InputIt begin, InputIt end)
    {
        std::generate(begin, end, [this] {return rand(); });
    }

    template<typename Container>
    void fillRandFast(Container& container) const
    {
        std::generate(std::begin(arr), std::end(container), [this] {return randFast(); });
    }
    template<typename ElementType, size_t SIZE>
    void fillRandFast(ElementType(&arr)[SIZE])
    {
        std::generate(std::begin(arr), std::end(arr), [this] {return randFast(); });
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
};