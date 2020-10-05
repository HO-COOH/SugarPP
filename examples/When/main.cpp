#include "../../When/When.hpp"
#include "../../Range/Range.hpp"
#include "../../IO/IO.hpp"
#include <iostream>
#include <string>
#include <climits> //for INT_MAX

using namespace std::literals;
int main()
{
    int x = 10;
    when(x,
        1,         [] { puts("x==1"); },
        2,         [] { puts("x==2"); },
        Else(),    [] { puts("x is neither 1 nor 2"); })(); //"x==1"

    std::array validNumbers{ 11,13,17,19 };
    when(x,
        Range(1, 9), [] { print("x is in the range"); },
        Range(validNumbers), [] { print("x is valid"); },
        NOT{ Range(10, 20) }, [] { print("x is outside the range"); },
        Else(), [] { print("none of the above"); }
    )();

    int temperature = 10;
    puts(when((temperature),
             Range(INT_MIN, 0),    "freezing",
             Range(1, 15),         "cold",
             Range(16, 20),        "cool",
             Range(21, 25),        "warm",
             Range(26, INT_MAX),   "hot",
             Else(),               "WTF?")); //"cold"

    auto describe = [](auto &&obj) {
        return detail::when_impl<false>(obj,
                    OR{1, 2},               "One or two"s,
                    "hello"s,               "Greeting"s,
                    is<long>{},             "long"s,
                    NOT{is<std::string>{}}, "Not a string"s,
                    Else(),                 "Unknown string"s);
    };
    print(describe(1));                //"One or two"
    print(describe("hello"s));         //"Greeting"
    print(describe(1000l));            //"long"
    print(describe(2));                //"Not a string"
    print(describe("random string"s)); //"Unknown string"

    /*C string comparison is also supported*/
    auto describe2 = [](auto &&obj) {
       return when((obj),
                   1,                      "One",
                   "hello",                "Greeting",
                   is<long>{},             "long",
                   is_not<const char *>{}, "Not a string",
                   Else(),                 "Unknown string");
    };
    puts(describe2(1));               //"One"
    puts(describe2("hello"));         //"Greeting"
    puts(describe2(1000l));           //"long"
    puts(describe2(2));               //"Not a string"
    puts(describe2("random string")); //"Unknown string"

    auto isWhat = [](auto&& c) { print(when(
       static_cast<bool>(isdigit(c)), "is digits",
       static_cast<bool>(isalpha(c)), "is character",
       Else(), "Other"
    )); };
    isWhat('1');
    isWhat('a');
    isWhat(' ');
    isWhat('\0');

    auto isOdd = [](auto&& num) {return num % 2 != 0; };
    auto isEven = [](auto&& num) {return num % 2 == 0; };
    auto y = 1;
    auto z = 2;
    when(
        isOdd(y), [] {print("y is odd"); },
        isEven(z), [] {print("z is even"); },
        Else(), [] {print("y+z is even"); }
    )();    //"y is odd"
}