#include "Range.hpp"
#include <iostream>
#include <array>
int main()
{
    /*use Range in range-based for loop*/
    for (auto i : Range(0, 10))
        std::cout << i << '\n';

    /*use range for a random number*/
    Range r(-1, 100000);
    std::cout << "Random number in " << r << " is " << r.rand() << '\n';

    /*use range to fill a C style array*/
    double arr[10];
    Range(-500.0, 500.0).fillRand(arr);
    std::copy(std::cbegin(arr), std::cend(arr), std::ostream_iterator<double>{std::cout, "\n"});

    /*use range to fill a STL container*/
    std::array<char, 20> arr2;
    Range('A', 'z').fillRand(arr2);
    std::copy(std::cbegin(arr), std::cend(arr), std::ostream_iterator<char>{std::cout});
    std::cout << '\n';

    /*Alternatively .randFast() provides a faster way for generating random number using rand() in C*/
    int arr3[10];
    Range(-200, 300).fillRandFast(arr3);
    std::copy(std::cbegin(arr3), std::cend(arr3), std::ostream_iterator<int>{std::cout, "\n"});
}