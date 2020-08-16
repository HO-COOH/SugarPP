#include <iostream>
#include <string>

inline void restore(std::istream& is)
{
    std::cin.clear();
    while (std::cin.get() != '\n')
        ;
}

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

template <>
[[nodiscard]] std::string input(const char* prompt, bool retry)
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

template<typename T>
T input(std::string const& prompt, bool retry = true)
{
    return input<T>(prompt.c_str(), retry);
}

#if __cplusplus >= 201703L
#include <string_view>
template<typename T>
T input(std::string_view prompt, bool retry = true)
{
    return input<T>(prompt.data(), retry);
}
#endif


template <char delim = ' ', std::ostream& os = std::cout, typename... Args>
void print(Args &&... args)
{
    ((os << args << delim), ...);
    os << '\n';
}

template <std::ostream& os = std::cout, typename... Args>
void printLn(Args &&... args)
{
    ((os << args << '\n'), ...);
}

#include <fstream>
template<typename Char=char>
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