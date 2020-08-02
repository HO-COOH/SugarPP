#include "IO.hpp"
void printSeparator()
{
    std::cout << "----------------------------------";
}

template<typename Type>
void printResult(Type result)
{
    std::cout << "You entered: " << result << '\n';
}

template<typename Type>
void test()
{
    const auto value = input<Type>(std::string{ "Enter a " } + typeid(Type).name() + ": ");
    printResult<Type>(value);
}

int main()
{
    test<int>();
    test<unsigned>();
    test<char>();
    test<double>();
    test<float>();
    test<std::string>();
    test<long long>();
    test<unsigned long long>();
}
