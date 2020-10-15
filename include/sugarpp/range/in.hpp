#pragma once

#include <algorithm>
#include <type_traits>


template<typename Container>
struct in
{
    Container&& range;
    using value_type = typename std::remove_reference_t<Container>::value_type;
    bool operator==(value_type&& value)
    {
        return std::find(std::cbegin(range), std::cend(range), value) != std::cend(range);
    }
};

template<typename Container>
in(Container&&)->in<Container>;

template<typename Container>
in(Container const&)->in<Container&>;

template<typename Container>
bool operator==(typename in<Container>::value_type&& value, in<Container> const& in)
{
    return in == value;
}
