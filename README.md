# SugarPP
A collection of my syntactic 🍬 when programming in C++. **When can we have nice things?**
- [SugarPP](#sugarpp)
  - [Kotlin ``when`` C++ port](#kotlin-when-c-port)
    - [Introduction](#introduction)
    - [Usage](#usage)
    - [Requirement](#requirement)
    - [Example](#example)
    - [Advanced Usage](#advanced-usage)
    - [Documentation](#documentation)
    - [To-DO](#to-do)
  - [IO](#io)
    - [Introduction](#introduction-1)
    - [Usage](#usage-1)
    - [Example](#example-1)
    - [Documentation](#documentation-1)
  - [Range](#range)
    - [Introduction](#introduction-2)
    - [Example](#example-2)
    - [Usage](#usage-2)
    - [Documentation](#documentation-2)
      - [Type](#type)
      - [Constructor](#constructor)
      - [Public Member functions](#public-member-functions)
      - [Non Member functions](#non-member-functions)


## Kotlin ``when`` C++ port
A header only port of ``when`` statement/expression in kotlin.
### Introduction
In [the official kotlin language tutorial](https://kotlinlang.org/docs/reference/basic-syntaxhtml#using-when-expression), the ``when`` expression is a pretty nice syntax sugar forreplacing traditional ``switch case`` statement seens in most languages. The drawback for``switch`` at least in C/C++ though is that it only supports matching **integer** values. Butthe kotlin's ``when`` basically supports these operations:
- value matching (for all comparable type)
```kotlin
/*kotlin*/
when (x) {
    1 -> print("x == 1")
    2 -> print("x == 2")
    else -> { // Note the block
        print("x is neither 1 nor 2")
    }
}
```
```cpp
/*SugarPP*/
when(x,
    1, []{ puts("x == 1"); },
    2, []{ puts("x == 2"); },
    Else(), []{ puts("x is neither 1 nor 2"); }
);
```
- type matching
```kotlin
/*kotlin*/
fun describe(obj: Any): String =
    when (obj) {
        1          -> "One"
        "Hello"    -> "Greeting"
        is Long    -> "Long"
        !is String -> "Not a string"
        else       -> "Unknown"
    }
```
```cpp
/*SugarPP*/
auto describe = [](auto&& obj) {
    return when(obj,
        1,                      "One",
        "hello",                "Greeting",
        is<long>(),             "long",
        is_not<const char*>(),  "Not a string",
        Else(),                 "Unknown string"
    );
};
```
- range matching
```kotlin
/*kotlin*/
when (x) {
    in 1..10 ->         print("x is in the range")
    in validNumbers ->  print("x is valid")
    !in 10..20 ->       print("x is outside the range")
    else ->             print("none of the above")
}
```

```cpp
/*SugarPP*/
#include "../Range/In.hpp"
std::array validNumbers{11,13,17,19};
when(x,
    Range(1, 10),       []{ puts("x is in the range"); },
    Range(validNumbers),[]{ puts("x is valid"); },
    NOT{Range(10, 20)}, []{ puts("x is outside the range"); },
    Else(),             []{ puts("none of the above"); }
);
```
This project tries to mock kotlin's ``when`` in C++. It may sounds stupid, but it shows how powerful ``template`` can be, yet I only used a small portition of its power.
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
    when(x,
        1,          [] {puts("x==1"); },
        2,          [] {puts("x==2"); },
        Else(),     [] {puts("x is neither 1 nor 2"); }
    )();//"x==1"
    int temperature = 10;
    print(when(temperature,
              Range(INT_MIN, 0),    "freezing",
              Range(1, 15),         "cold",
              Range(16, 20),        "cool",
              Range(21, 25),        "warm",
              Range(26, INT_MAX),   "hot",
              Else(),               "WTF?"
    )); //"cold"

    auto describe = [](auto &&obj) {
        return when(obj,
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
### Documentation

<details>
<summary>When</summary>

```cpp
/*Primary templates*/
template <typename ExprType, typename Case1Type, typename Return1Type, typename Case2Type, typename... Args>
auto when(ExprType&& expr, Case1Type&& case1, Return1Type&& return1, Case2Type&& case2, Args&&... args);    //1

template <typename Return1Type, typename Case2Type, typename... Args>
auto when(const char* Expr, const char* Case1, Return1Type&& return1, Case2Type&& case2, Args&&... args);   //2

template <typename ExprType, typename is_type, typename Return1Type, typename Case2Type, typename... Args>
auto when(ExprType&& expr, is<is_type>, Return1Type&& return1, Case2Type&& case2, Args... args);            //3

template <typename ExprType, typename is_not_type, typename Return1Type, typename Case2Type, typename... Args>
auto when(ExprType&& expr, is_not<is_not_type>, Return1Type&& return1, Case2Type&& case2, Args... args);    //4

/*Ending templates*/
template <typename ExprType, typename CaseType, typename ReturnType>
auto when(ExprType&& expr, CaseType&& to_match, ReturnType&& ReturnResult);                                 //5

template <typename ReturnType>
auto when(const char* Expr, const char* Case, ReturnType&& ReturnResult);                                   //6

template <typename ExprType, typename is_type, typename ReturnType>
auto when(ExprType&&, is<is_type>, ReturnType&& ReturnResult);                                              //7

template <typename ExprType, typename is_not_type, typename ReturnType>
auto when(ExprType&&, is_not<is_not_type>, ReturnType&& ReturnResult);                                      //8

template <typename ExprType, typename ReturnType>
auto when(ExprType&&, Else, ReturnType&& ReturnResult);                                                     //9
```
- 1-4 are the primary recursive templates
   2. Specialization for ``const char*`` of ``Expression`` to match, performing ``strcmp``
   3. Specialization for ``is<Type>``, performing type query, as if checking ``std::is_same_v<std::remove_reference_t<ExprType>, Type>``
   4. Specialization for ``is_not<Type>``, performing type query, as if checking ``!std::is_same_v<std::remove_reference_t<ExprType>, Type>``
- 5-8 are the ending templates which functions similarly as 1-4
- 9 handles the ``Else`` cases and returns ``ReturnResult`` if non of the previous cases are matched described in 1-8

</details>

### To-DO
- ~~Support for direct contrary operation (for example, ``!is<long>`` instead of``is_not<long>``).~~ √
- ~~Better lambda support in matching branches~~ √

## IO
A convenient helper function for substitute ``std::cin`` that just **works as you intended.** No more ``getchar()`` for *eating* the enter key nonsense! And ``print`` anything!
### Introduction
The ``input`` template function is similar to ``python``. It prints a ``prompt`` message and does the following things with error handling, which means clearing any bad bit and ignores the rest of the input and can be specify to ``retry`` until the user entered the right thing. (**default is retry enabled**)
- If the type is a primitive, it works the same as ``std::cin >>``
    + If the type is ``unsigned`` number type, and it receives a negative number, it converts to the absolute value
- If the type is ``std::string``, it works the same as ``std::getline``

The ``print()`` function template prints **ANY** number of arguments, separated by a template argument ``delim`` (default to a space). And it's able to print almost anything:
- anything that ``std::cout`` has an overload
- anything that is iterable, eg. has a ``.begin()`` function, and can be printed after dereference that iterator 
- ``std::tuple``/``std::pair``

Just like ``print()``, ``printLn()`` function template prints **ANY** number of arguments, one line at a time.
### Usage
Just include `./IO/IO.hpp`.
### Example
A more detailed example is in ``./IO/main.cpp``
```cpp
#include "IO.hpp"
int main()
{
    print("Hello, what's your name?");
    auto name = input<std::string>("Enter your name: ");
    print("Hello,", name, "How old are you?");
    auto age = input<int>("Enter your age: ");
    print("Thanks you,", name, "who is", age, "years old");
}
```
### Documentation
<details>
<summary>Input</summary>

```cpp
template<typename T>
[[nodiscard]]T input(const char* prompt = nullptr, bool retry = true);  //1
template<typename T>
[[nodiscard]]T input(const std::string& prompt, bool retry = true);     //2
template<typename T>
[[nodiscard]]T input(std::string_view prompt, bool retry = true);       //3

template<>
[[nodiscard]]std::string input(const char* prompt, bool retry = true);  //4
template<>
[[nodiscard]]std::string input(const std::string& prompt, bool retry);  //5
template<>
[[nodiscard]]std::string input(std::string_view prompt, bool retry);    //6
```
- 1-3 First print a message ``prompt``, then get input from ``std::cin`` as if calling ``std::cin >> object;`` Returns the object of type ``T``. If the operation is failing, it clears the error state of ``std::cin`` and returns either a default constructed object of type ``T`` or retry the whole process if ``retry`` flag is ``true``.
- 4-6 First print a message ``prompt``, then get input from ``std::cin`` as if calling ``std::getline``. Returns the input string. If the string is empty and ``retry`` flag is ``true``, it retry the whole process.
</details>

<details>
<summary>Output</summary>

```cpp
template <char delim = ' ', std::ostream& os = std::cout, typename... Args>
void print(Args &&... args);    //1
template <std::ostream& os = std::cout, typename... Args>
void printLn(Args &&... args);  //2
```
1. Print any number of objects specified by the parameter pack ``args`` as if calling ``std::cout << args;``, each followed by a ``delim``. Print a new line after the call.
2. Equivalent to ``print<'\n'>(agrs...)``

</details>

<details>
<summary>Thread Safe Output</summary>

Thread-safe output functions are ``static`` functions inside ``ThreadSafe`` class.
```cpp
template<std::ostream& os = std::cout>
struct ThreadSafe
{
    template<char delim=' ', typename...Args>
    static inline void print(Args&& ...args);       //1

    template<char delim = ' ', typename...Args>
    static inline void tryPrint(Args&& ...args);    //2

    template<typename... Args>
    static inline void printLn(Args&&... args);     //3

    template<typename... Args>
    static inline void tryPrintLn(Args&&... args);  //4
};
```
- 1,3 is the thread-safe version of `print` and ``printLn``, it blocks the current thread until it is able to print.
- 2,4 will try to lock the internal mutex and print. If the mutex is currently locked, it immediately returns without blocking.

</details>

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

    /*use range to generate several random numbers*/
    auto [num1, num2, num3] = Range(1, 10).rand<3>();

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

    print("1D range");
    for (auto i : Range(2.0, 10.0, 3))
        print(i);
    
    /*
        1D range
        2
        5
        8
    */
    print("2D range");
    for (auto [i, j] : Range(-5, 1) | Range(0, 3))
        print(i, '\t', j);

    /*
        2D range
        -5       0
        -5       1
        -5       2
        -4       0
        -4       1
        -4       2
        -3       0
        -3       1
        -3       2
        -2       0
        -2       1
        -2       2
        -1       0
        -1       1
        -1       2
        0        0
        0        1
        0        2
    */
}
```
### Usage
Just include `./Range/Range.hpp` for ``Range`` and ``./Enumerate/Enumerate.hpp`` for ``Enumerate``
### Documentation

<details>
<summary>Range</summary>

#### Type
```cpp
class RangeRandomEngineBase
{
protected:
    static inline std::mt19937 rdEngine{ std::random_device{}() };
};
template <typename T, typename StepType, typename ValueType = std::common_type_t<T, StepType>)
class Range : RangeRandomEngineBase
{   
    /*...*/
public:
    using value_type = ValueType;
    /*...*/
}
```

#### Constructor
```cpp
template<typename T, typename StepType>
Range(T start, T end, StepType step);
```
Construct a ``Range`` class representing [start, end). When incremented, the ``current`` value is incremented with ``step``. Note: **according to deduction rules, ``start`` and ``end`` must have the same type.**

#### Public Member functions
```cpp 
value_type operator*() const;
```
Returns the ``current`` value.

```cpp
Range<T, StepType, ValueType> begin();
```
Returns ``*this`` unchanged.

```cpp
value_type end();
```
Returns the ``end`` value.

```cpp
auto steps() const;
```
Returns the number of ramaining steps to go in the current ``Range``.

```cpp
bool operator!=(Range rhs) const;           //1
bool operator!=(value_type value) const;    //2
```
1. Returns ``this->current`` =?= ``rhs.current``
2. Returns ``this->current`` =?= ``value``

```cpp
template<typename Num, typename = std::enable_if_t<std::is_arithmetic_v<Num>>>
bool operator==(Num number) const; 
```
Returns whether ``this->current``<=``number``<=``this->end``, only instantiated when ``number`` is a number type.


```cpp
Range& operator++();                //1
Range& operator+=(unsigned steps);  //2
```
1. increment ``*this``
2. increment ``*this`` ``steps`` times

```cpp
value_type rand();                                      //1
template<typename Container>
void fillRand(Container& container);                    //2
template<typename Container>
void fillRand(Container& container, size_t count);      //3
template<typename InputIt>
void fillRand(InputIt begin, InputIt end);              //4

value_type randFast() const;                            //5
template<typename Container>
void fillRandFast(Container& container);                //6
template<typename Container>
void fillRandFast(Container& container, size_t count);  //7
template<typename InputIt>
void fillRandFast(InputIt begin, InputIt end);          //8
```
- 1-4 uses ``std::uniform_<T>_distribution`` where ``T`` is some numeric types depending on ``value_type``
  1. Returns a single random number within [current, end)
  2. Fill ``container`` with random numbers within [current, end)
  3. Fill ``container`` with ``count`` random numbers within [current, end), equivalent to:
        ```cpp
        fillRand(std::begin(container), std::begin(container) + count)
        ```
  4. Fill the range pointed by the iterators [begin, end) with random numbers within [current, end)
- 5-8 uses ``rand()`` from ``<stdlib>``, which have the same usage as 1-4

#### Non Member functions
```cpp
friend std::ostream& operator<<(std::ostream& os, Range const& range);
```
Print range in the format of: ``[current,end]``
</details>
