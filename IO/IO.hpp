#pragma once

#include <iostream>
#include <string>

/**
 * @brief Restore the error state of the stream and "eat" the rest invalid input left in the stream
 * @param is The stream object to restore
 */
inline void restore(std::istream& is)
{
    is.clear();
    while (is.get() != '\n')
        ;
}

/**
 * @brief Get the correct type of value from `stdin`
 * @tparam T The expected return type
 * @param prompt A short prompt message printed on `stdout`, which will appear before user input
 * @param retry The flag for whether to retry the whole process when an input exception occurs, whether it's the type mismatch or invalid input
 * @return If retry flag is set to false and an exception does occurs, the function clears any invalid states, and return a default-constructed object of the expected type. Otherwise return the correct input.
 */
template <typename T>
[[nodiscard]] T input(const char* prompt = nullptr, bool retry = true)
{
    T i{};
    if (!retry)
    {
        if (prompt)
            std::cout << prompt;
        if (!std::cin)
            restore(std::cin);
        std::cin >> i;
    }
    else
    {
        do
        {
            if (!std::cin)
                restore(std::cin);
            if (prompt)
                std::cout << prompt;
            if constexpr (std::is_integral_v<T>)
            {
                //handles negative number and convert to its absolute value when expected an unsigned
                if (std::cin.peek() == '-')
                {
                    std::make_signed_t<T> i_abs;
                    std::cin >> i_abs;
                    if (!std::cin)
                        continue;
                    restore(std::cin);
                    return abs(i_abs);
                }
            }
            std::cin >> i;
        } while (!std::cin);
    }
    restore(std::cin);
    return i;
}

/**
 * @brief Specialization for `std::string`, which uses `std::getline` internally.
 * @see input
 */
template <>
[[nodiscard]] inline std::string input(const char* prompt, bool retry)
{
    std::string s;
    if (!retry)
    {
        if (prompt)
            std::cout << prompt;
        if (!std::cin)
            restore(std::cin);
        std::getline(std::cin, s);
    }
    else
    {
        do {
            if (!std::cin)
                restore(std::cin);
            if (prompt)
                std::cout << prompt;
            std::getline(std::cin, s);
        } while (s.empty());
    }
    restore(std::cin);
    return s;
}

/**
 * @brief Overload for `std::string` as prompt message
 * @see input
 */
template<typename T>
T input(std::string const& prompt, bool retry = true)
{
    return input<T>(prompt.c_str(), retry);
}

/**
 * @brief Overload for `std::string` as prompt message and specialized for `std::string` as return type.
 * @see input
 */
template<>
[[nodiscard]] inline std::string input(std::string const& prompt, bool retry)
{
    return input<std::string>(prompt.c_str(), retry);
}

#if __cplusplus >= 201703L
#include <string_view>
/**
 * @brief Overload for `std::string_view` as prompt message, if C++17 is available
 * @see input
 */
template<typename T>
T input(std::string_view prompt, bool retry = true)
{
    return input<T>(prompt.data(), retry);
}
/**
 * @brief Overload for `std::string_view` as prompt message and specialized for `std::string` as return type, if C++17 is available
 */
template<>
inline std::string input(std::string_view prompt, bool retry = true)
{
    return input<std::string>(prompt.data(), retry);
}
#endif


/**
 * @brief Print any number of arguments to `stdout`, separated by `delim`
 * @tparam delim The separator between each thing to print, default to a space
 */
template <char delim = ' ', std::ostream& os = std::cout, typename... Args>
void print(Args &&... args)
{
    ((os << args << delim), ...);
    os << '\n';
}

/**
 * @brief Print any number of arguments to `stdout`, separated by a line, same as `print<'\n>`
 * @see print
 */
template <std::ostream& os = std::cout, typename... Args>
void printLn(Args &&... args)
{
    ((os << args << '\n'), ...);
}

#include <mutex>
/*Thread-safe IO*/

/**
 * @brief The thread-safe IO class. Every functions inside the class are static, which acts like a namespace but holding a mutex to regulate all the calls to print.
 */
template<std::ostream& os = std::cout>
struct ThreadSafe
{
    static std::mutex m;

    /**
     * @brief Thread-safe version of print, blocks until the printing is finished.
     * @see ::print
     */
    template<char delim = ' ', typename...Args>
    static inline void print(Args&& ...args)
    {
        std::lock_guard lock{ m };
        ((os << args << delim), ...);
        os << '\n';
    }

    /**
     * @brief Non-blocking version of print, which returns immediately if another thread is printing.
     */
    template<char delim = ' ', typename...Args>
    static inline void tryPrint(Args&& ...args)
    {
        std::unique_lock const lock{ m, std::try_to_lock_t{} };
        if (lock)
        {
            ((os << args << delim), ...);
            os << '\n';
        }
    }

    /**
     * @brief Thread-safe version of printLn, blocks until the printing is finished.
     */
    template<typename... Args>
    static inline void printLn(Args&&... args)
    {
        std::lock_guard lock{ m };
        ((os << args << '\n'), ...);
    }

    /**
     * @brief Non-blocking version of printLn, which returns immediately if another thread is printing.
     */
    template<typename... Args>
    static inline void tryPrintLn(Args&&... args)
    {
        std::unique_lock const lock{ m, std::try_to_lock_t{} };
        if (lock)
            ((os << args << '\n'), ...);
    }
};

#include <fstream>
template<typename Char = char>
class FileIterator
{
    std::ifstream fs;
public:
    using value_type = std::basic_string<Char>;
    using difference_type = std::ptrdiff_t;
    using pointer = std::basic_string<Char>*;
    using reference = std::basic_string<Char>&;
    using iterator_category = std::forward_iterator_tag;

    FileIterator(Char const* fileName);
    FileIterator(std::basic_string<Char> const& fileName);

#if __cplusplus >= 201703L
    FileIterator(std::basic_string_view<Char> const fileName);
    FileIterator(std::filesystem::directory_entry const& file);
#endif
    value_type operator*() const
    {

    }
    FileIterator& operator++()
    {

    }

};