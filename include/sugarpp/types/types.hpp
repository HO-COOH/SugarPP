/*****************************************************************//**
 * \file   types.hpp
 * \brief  Type conversion helper utilities
 * 
 * \author Peter
 * \date   October 2020
 *********************************************************************/
#pragma once

#include <string>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <sstream>
#include <cstdlib> //for atoi, atol, atoll

#ifdef SugarPPNamespace
namespace SugarPP
{
#endif
    namespace Type_detail
    {
        template<typename T, typename = void>
        struct has_to_string:std::false_type{};

        template<typename T>
        struct has_to_string<T, decltype(std::to_string(std::declval<T>()), void())>:std::true_type{};

        template<typename T>
        struct is_tuple_impl : std::false_type {};

        template<typename... Ts>
        struct is_tuple_impl<std::tuple<Ts...>> : std::true_type {};

        template<typename T>
        struct is_tuple : is_tuple_impl<std::decay_t<T>> {};

        template<typename T, typename = void>
        struct iterable_impl :std::false_type {};

        template<typename T>
        struct iterable_impl<T, decltype(std::begin(std::declval<T>()), void())> :std::true_type {};  //still works even when T is const

        template<typename T>
        struct iterable :iterable_impl
        <
            std::conditional_t
            <
                std::is_array_v<std::remove_reference_t<T>>,
                T,
                std::decay_t<T>
            >
        > {};

        template<typename T, std::ostream& os = std::cout, typename = void>
        struct printable :std::false_type {};

        template<typename T, std::ostream& os>
        struct printable<T, os, decltype(os << std::declval<T>(), void())> :std::true_type {};

        template<typename Char = char, typename T>
        auto getString(T&& arg)
        {
            std::basic_stringstream<Char> os;
            const Char delim = ' ';
            if constexpr (is_tuple<T>::value)
            {
                os << '(';
                std::apply([&os](auto&&... args)
                    {
                        ((os << args << delim), ...);
                    }, arg);
                os << '\b' << ')';
                return os.str();
            }
            else if constexpr (iterable<T>::value && printable<decltype(*std::begin(std::declval<T>()))>::value)
            {
                os << '[';
                bool printed = false;
                for (auto& element : arg)
                {
                    os << element << delim;
                    printed = true;
                }
                if (printed)
                    os << '\b' << ']';
                else
                    os << ']';
                return os.str();
            }
            else if constexpr (iterable<typename std::remove_reference_t<T>::value_type>::value)
            {
                os << '[';
                for (auto& element : arg)
                    os << print_impl(element);
                os << ']';
                return os.str();
            }
            else
                return os.str();
        }
    }
    template<typename Char = char>
    auto to_string()
    {
        return std::basic_string<Char>{};
    }
    template<typename Char = char, typename Arg>
    auto to_string(Arg&& arg)
    {
        if constexpr (std::is_convertible_v<std::remove_reference_t<Arg>, std::basic_string<Char>>)
            return std::basic_string<Char>{std::forward<Arg>(arg)};
        else if constexpr (Type_detail::has_to_string<Arg>::value)
        {
            if constexpr (std::is_same_v<Char, char>)
                return std::to_string(arg);
            else
                return std::to_wstring(arg);
        }
        else
            return std::basic_string<Char>{};
    }
    template<typename Char = char, typename Arg, typename... Args>
    auto to_string(Arg&& arg, Args&& ...args)
    {
        if constexpr (std::is_convertible_v<std::remove_reference_t<Arg>, std::basic_string<Char>>)
            return std::basic_string<Char>{std::forward<Arg>(arg)} + to_string(std::forward<Args>(args)...);
        else if constexpr (Type_detail::has_to_string<Arg>::value)
        {
            if constexpr (std::is_same_v<Char, char>)
                return std::to_string(arg) + to_string(std::forward<Args>(args)...);
            else
                return std::to_wstring(arg) + to_string(std::forward<Args>(args)...);
        }
        else
            return std::basic_string<Char>{} + to_string(std::forward<Args>(args)...);
    }

    /* string -> number */
    /**
     * @brief Interprets an integer value in a byte string pointed to by str.
     * @details Discards any whitespace characters until the first non-whitespace character is found, then takes as many characters as possible to form a valid integer number representation and converts them to an integer value.
     * @param str pointer to the null-terminated byte string to be interpreted
     * @return Integer value corresponding to the contents of str on success.
     * If the converted value falls out of range of corresponding return type, the return value is undefined.
     * If no conversion can be performed, 0 is returned.
     */
    template<typename NumberType>
    auto to_num(const char* str)
    {
        if constexpr (std::is_same_v<NumberType, int>)
            return std::atoi(str);
        else if constexpr (std::is_same_v<NumberType, long>)
            return std::atol(str);
        else if constexpr (std::is_same_v<NumberType, long long>)
            return std::atoll(str);
    }

    template<typename NumberType, typename Char = char, typename = std::enable_if_t<std::is_same_v<Char, char> || std::is_same_v<Char, wchar_t>>>
    auto to_num(const std::basic_string<Char>& str, std::size_t* pos = nullptr, int base = 10)
    {
        if constexpr (std::is_same_v<NumberType, int>)
            return std::stoi(str, pos, base);
        else if constexpr (std::is_same_v<NumberType, long>)
            return std::stol(str, pos, base);
        else if constexpr (std::is_same_v<NumberType, long long>)
            return std::stoll(str, pos, base);
        else if constexpr (std::is_same_v<NumberType, unsigned long>)
            return std::stoul(str, pos, base);
        else if constexpr (std::is_same_v<NumberType, unsigned long long>)
            return std::stoull(str, pos, base);

    }
#ifdef SugarPPNamespace
}
#endif
