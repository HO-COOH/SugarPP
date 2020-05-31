# Kotlin ``when`` C++ port
A header only port of ``when`` statement/expression in kotlin.
## Introduction
In [the official kotlin language tutorial](https://kotlinlang.org/docs/reference/basic-syntax.html#using-when-expression), the ``when`` expression is a pretty nice syntax sugar for replacing traditional ``switch case`` statement seens in most languages. The drawback for ``switch`` at least in C/C++ though is that it only supports matching **integer** values. But the kotlin's ``when`` basically supports these operations:
- value matching (for all comparable type)
```kotlin
when (x) {
    1 -> print("x == 1")
    2 -> print("x == 2")
    else -> { // Note the block
        print("x is neither 1 nor 2")
    }
}
```
- type matching
```kotlin
fun describe(obj: Any): String =
    when (obj) {
        1          -> "One"
        "Hello"    -> "Greeting"
        is Long    -> "Long"
        !is String -> "Not a string"
        else       -> "Unknown"
    }
```
- range matching
```kotlin
when (x) {
    in 1..10 -> print("x is in the range")
    in validNumbers -> print("x is valid")
    !in 10..20 -> print("x is outside the range")
    else -> print("none of the above")
}
```

This project tries to mock kotlin's ``when`` in C++. It may sounds stupid, but it shows how powerful ``template`` can be, yet I only used a small portition of its power.
## Usage
Just include the ``When.hpp`` header in your project.

## Example
Example is included in ``main.cpp``. Here I copied them without the comments for a brief intuition.
```cpp
#include "When.hpp"
#include <iostream>
#include <string>
#include <functional>

using namespace std::literals;
int main()
{
    int x = 1;
    when((x),
        1,          std::function{ [] {puts("x==1"); } },
        2,          std::function{ [] {puts("x==2"); } },
        Else(),     std::function{ [] {puts("x is neither 1 nor 2"); } }
    )();//"x==1"

    int temperature = 10;
    puts(when((temperature),
              Range(INT_MIN, 0),    "freezing",
              Range(1, 15),         "cold",
              Range(16, 20),        "cool",
              Range(21, 25),        "warm",
              Range(26, INT_MAX),   "hot",
              Else(),               "WTF?"
    )); //"cold"
    

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
}
```