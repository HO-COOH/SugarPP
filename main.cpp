#include "When.hpp"
#include <iostream>
#include <string>
#include <functional>
#include <climits> //for INT_MAX

using namespace std::literals;
int main()
{
    int x = 1;
    when((x),
        1,          std::function{ [] {puts("x==1"); } },
        2,          std::function{ [] {puts("x==2"); } },
        Else(),     std::function{ [] {puts("x is neither 1 nor 2"); } }
    )();//"x==1"
    /*Note: Must be wrapped into a callable object. Because different lambda has different type, therefore the return type is not the same */

    int temperature = 10;
    puts(when((temperature),
              Range(INT_MIN, 0),    "freezing",
              Range(1, 15),         "cold",
              Range(16, 20),        "cool",
              Range(21, 25),        "warm",
              Range(26, INT_MAX),   "hot",
              Else(),               "WTF?"
    )); //"cold"
    
    /*Old version, using verbose std::is_same*/
    // auto describe = [](auto&& obj) {
    //     return when((obj),
    //         1, "One"s,
    //         "hello"s, "Greeting"s,
    //         std::is_same_v<long, std::remove_reference_t<decltype(obj)>>, "long"s,
    //         !std::is_same_v<std::string, std::remove_reference_t<decltype(obj)>>, "Not a string"s,
    //         Else(), "Unknown string"s);
    // };

    auto describe = [](auto &&obj) {
        return when((obj),
                    1,                      "One"s,
                    "hello"s,               "Greeting"s,
                    is<long>{},             "long"s,
                    is_not<std::string>{},  "Not a string"s,
                    Else(),                 "Unknown string"s);
    };
    std::cout << describe(1) << '\n';                   //"One"
    std::cout << describe("hello"s) << '\n';            //"Greeting"
    std::cout << describe(1000l) << '\n';               //"long"
    std::cout << describe(2) << '\n';                   //"Not a string"
    std::cout << describe("random string"s) << '\n';    //"Unknown string"

    auto describe2 = [](auto&& obj) {
        return when((obj),
            1,                              "One",
            "hello",                        "Greeting",
            is<long>{},                     "long",
            is_not<const char*>{},          "Not a string",
            Else(),                         "Unknown string");
    };
    puts(describe2(1));                 //"One"
    puts(describe2("hello"));           //"Greeting"
    puts(describe2(1000l));             //"long"
    puts(describe2(2));                 //"Not a string"
    puts(describe2("random string"));   //"Unknown string"
}
