# SugarPP
A collection of my syntactic 🍬 when programming in C++. **When can we have nice things?**
- [SugarPP](#sugarpp)
  - [Kotlin ``when`` C++ port](#kotlin-when-c-port)
    - [Introduction](#introduction)
    - [Usage](#usage)
    - [Requirement](#requirement)
    - [Example](#example)
    - [Advanced Usage](#advanced-usage)
    - [To-DO](#to-do)
  - [IO](#io)
    - [Introduction](#introduction-1)
    - [Usage](#usage-1)
    - [Example](#example-1)
  - [Range](#range)
    - [Introduction](#introduction-2)
    - [Example](#example-2)
    - [Usage](#usage-2)


## Kotlin ``when`` C++ port
A header only port of ``when`` statement/expression in kotlin.
### Introduction
In [the official kotlin language tutorial](https://kotlinlang.org/docs/reference/basic-syntaxhtml#using-when-expression), the ``when`` expression is a pretty nice syntax sugar forreplacing traditional ``switch case`` statement seens in most languages. The drawback for``switch`` at least in C/C++ though is that it only supports matching **integer** values. Butthe kotlin's ``when`` basically supports these operations:
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
This project tries to mock kotlin's ``when`` in C++. It may sounds stupid, but it shows howpowerful ``template`` can be, yet I only used a small portition of its power.
### Usage
Just include the ``When/When.hpp`` header in your project.
### Requirement
Any compiler that supports C++17, because if would be so hard to do without ``constexpr if``(*I may consider adding support for pre C++17 if I am more advanced*)
I also implemented the same functionality using C++20 concepts because it's cool :)
### Example
Example is included in ``main.cpp``. Here I copied them without the comments for a briefintuition.
```cpp
#include "When.hpp"
#include "../IO/IO.hpp" //for print()
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
    print(when((temperature),
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
                    is<long>(),             "long"s,
                    is_not<std::string>(),  "Not a string"s,
                    Else(),                 "Unknown string"s);
    };
    print(describe(1));                   //"One"
    print(describe("hello"s));            //"Greeting"
    print(describe(1000l));               //"long"
    print(describe(2));                   //"Not a string"
    print(describe("random string"s));    //"Unknown string"
    /*C string is also supported*/
    auto describe2 = [](auto&& obj) {
        return when((obj),
            1,                              "One",
            "hello",                        "Greeting",
            is<long>(),                     "long",
            is_not<const char*>(),          "Not a string",
            Else(),                         "Unknown string");
    };
    print(describe2(1));                 //"One"
    print(describe2("hello"));           //"Greeting"
    print(describe2(1000l));             //"long"
    print(describe2(2));                 //"Not a string"
    print(describe2("random string"));   //"Unknown string"
}
```
### Advanced Usage
Now it supports grouped logical expression in matching branches, like ``AND, OR, NOT``.
You can find the implementation in ``GroupedCondition.hpp``.
An example (also included in ``main.cpp``):
```cpp
    auto describe = [](auto &&obj) {
        return when((obj),    
                    OR(1,2),                  "One or two"s,
                    "hello"s,                   "Greeting"s,
                    is<long>(),                 "long"s,
                    NOT(is<std::string>()),    "Not a string"s,
                    Else(),                     "Unknown string"s);
    };
    print(describe(1));                   //"One or two"
    print(describe("hello"s));            //"Greeting"
    print(describe(1000l));               //"long"
    print(describe(2));                   //"Not a string"
    print(describe("random string"s));    //"Unknown string"
```
### To-DO
- ~~Support for direct contrary operation (for example, ``!is<long>`` instead of``is_not<long>``).~~ √
- Better lambda support in matching branches

## IO
A convenient helper function for substitute ``std::cin`` that just **works as you intended.** No more ``getchar()`` for *eating* the enter key nonsense! Let ``print() / printLn()`` helps to eliminate ugly ``::`` and ``<<``
### Introduction
The ``input`` template function is similar to ``python``. It prints a ``prompt`` message and does the following things with error handling, which means clearing any bad bit and ignores the rest of the input and can be specify to ``retry`` until the user entered the right thing. (**default is retry enabled**)
- If the type is a primitive, it works the same as ``std::cin >>``
    + If the type is ``unsigned`` number type, and it receives a negative number, it converts to the absolute value
- If the type is ``std::string``, it works the same as ``std::getline``

The ``print()`` function template prints **ANY** number of arguments, separated by a template argument ``delim`` (default to a space).

The ``printLn()`` function template prints **ANY** number of arguments, one line at a time.
### Usage
Just include `./IO/IO.hpp`.
### Example
A more detailed example is in ``./IO/main.cpp``
```cpp
#include "IO.hpp"
int main()
{
    auto date = input<int>("Today's data: ");
    auto day = input<std::string>("What day is today? ");
    auto someChar = input<char>("Enter a character: ", false);  //retry disabled

    print("Today is", date, ", ", day);
}
```

## Range
Simplify your ``for`` loop and everything you want from a numerical range.
*Not to be confused with C++20 ranges!*
### Introduction
In tons of other programming languages, there are the similar syntax as follows:
```
for(var i in [0..10])
{...}

for i, v in enumerate(someArray):
    # python syntax
```
My ``Range`` class template suppors:
- With sepecified ``start``, ``end`` and ``step (default = 1)``, which can be used in C++ range-based for loop. Type will be inferred (with C++17), and if there is discrepancy, will be converted to the right type
- Generate random number in the range
- Fill a container with random number

My ``Enumerate`` class template is also used in range-based for loop. And returns a ``std::pair`` of ``index`` and an ``iterator`` from an ``iterable`` of the constructed ``Enumerate`` class. And it's better to be used together with structured-binding (C++17), eg: ``for(auto [i, v] : Enumerate(someArray))``
### Example
```cpp
#include "../IO/IO.hpp" //for print()
#include "Range.hpp"
#include "Enumerate.hpp"
int main()
{
    /*use Range in range-based for loop*/
    for (auto i : Range(0, 10))
        print(i);

    /*use range for a random number*/
    Range r(-1, 100000);
    print("Random number in ", r, " is ", r.rand());

    /*use range to fill a C style array*/
    double arr[10];
    Range(-500.0, 500.0).fillRand(arr);

    /*use range to fill a STL container*/
    std::array<char, 20> arr2;
    Range('A', 'z').fillRand(arr2);

    /*Alternatively .randFast() provides a faster way for generating random number using rand() in C*/
    int arr3[10];
    Range(-200, 300).fillRandFast(arr3);

    std::array arr{ "cpp", "sugar", "sweet" };
    for(auto [index, string]:Enumerate(arr))
        print<'\t'>(index, string);
    /*
        0       cpp
        1       sugar
        2       sweet
    */
}
```
### Usage
Just include `./Range/Range.hpp` for ``Range`` and ``./Enumerate/Enumerate.hpp`` for ``Enumerate``