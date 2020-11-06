#include "sugarpp/when/when.hpp"
#include "sugarpp/range/range.hpp"
#include "sugarpp/io/io.hpp"
#include <string>
#include <climits> //for INT_MAX
#include <memory>

using namespace SugarPP;
using namespace std::literals;

int main()
{
    /*Value matching*/
    int x = 10;
    when(x,
        1,         [] { puts("x==1"); },
        2,         [] { puts("x==2"); },
        Else(),    [] { puts("x is neither 1 nor 2"); })(); //"x==1"

    /////*Range matching*/
    std::array validNumbers{ 11,13,17,19 };
    when(x,
        Range(1, 9), [] { print("x is in the range"); },
        Range(validNumbers), [] { print("x is valid"); },
        NOT{ Range(10, 20) }, [] { print("x is outside the range"); },
        Else(), [] { print("none of the above"); }
    )();

    ///*Range matching*/
    int temperature = 10;
    puts(when((temperature),
             Range(INT_MIN, 0),    "freezing",
             Range(1, 15),         "cold",
             Range(16, 20),        "cool",
             Range(21, 25),        "warm",
             Range(26, INT_MAX),   "hot",
             Else(),               "WTF?")); //"cold"

    ///*Type matching*/
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

    /*Argument-less switches*/
    auto isWhat = [](auto&& c) { print(when(
       static_cast<bool>(isdigit(c)), "is digits",
       static_cast<bool>(isalpha(c)), "is character",
       Else(), "Other"
    )); };
    isWhat('1');
    isWhat('a');
    isWhat(' ');
    isWhat('\0');


    /*Argument-less switches */
    auto isOdd = [](auto&& num) {return num % 2 != 0; };
    auto isEven = [](auto&& num) {return num % 2 == 0; };
    auto y = 1;
    auto z = 2;
    when(
        isOdd(y), [] {print("y is odd"); },
        isEven(z), [] {print("z is even"); },
        Else(), [] {print("y+z is even"); }
    )();    //"y is odd"


    /*Fizzbuzz example*/
    for(auto i:Range(1, 101))
    {
        when(std::tuple{ i % 3, i % 5 },
            std::tuple{ 0, 0 }, [] {print("fizzbuzz"); },
            std::tuple{ 0, _ }, [] {print("fizz"); },
            std::tuple{ _, 0 }, [] {print("buzz"); },
            Else(),             [i] {print(i); }
        )();


        /*This doesn't work because it's impossible for passing {...} to std::tuple to deduce the type */
        //when({ i % 3, i % 5 },
        //    { 0, 0 }, [] {print("fizzbuzz"); },
        //    { 0, _ }, [] {print("fizz"); },
        //    { _, 0 }, [] {print("buzz"); },
        //    Else(),   [i] {print(i); }
        //)();

        /*This doesn't work for two reasons, the same as above and the return type is not consistent, eg: const char* vs int */
        //print(
        //    when({ i % 3, i % 5 },
        //    { 0, 0 }, "fizzbuzz",
        //    { 0, _ }, "fizz",
        //    { _, 0 }, "buzz",
        //    Else(),   i 
        //));
    }

    /*Polymorphism type matching*/
    struct Shape { virtual ~Shape() = default; };
    struct Circle :Shape {};
    struct Square :Shape{};

    std::unique_ptr<Shape> pt{ new Circle{} };
    when(*pt,
        is_actually<Circle>{}, [] { print("Circle* pt"); },
        is_actually<Square>{}, [] { print("Square* pt"); },
        Else(),                [] { print("Unknown type"); }
    )();    //"Circle* pt"

    std::cin.get();
}