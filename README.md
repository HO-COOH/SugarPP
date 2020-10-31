# SugarPP: syntactic 🍬 for programming in C++
![Build](https://github.com/HO-COOH/SugarPP/workflows/ClangBuild/badge.svg)

SugarPP is a collection of syntactic sugar for C++ code.

- [SugarPP: syntactic 🍬 for programming in C++](#sugarpp-syntactic--for-programming-in-c)
  - [How to Use](#how-to-use)
  - [Requirements](#requirements)
  - [Features](#features)
    - [Kotlin-style `when` syntax](#kotlin-style-when-syntax)
      - [Usage](#usage)
      - [Documentation](#documentation)
      - [Read More](#read-more)
    - [Simpler IO](#simpler-io)
      - [Features](#features-1)
      - [Usage](#usage-1)
      - [Documentation](#documentation-1)
    - [Range](#range)
      - [Features](#features-2)
      - [Usage](#usage-2)
      - [Documentation](#documentation-2)
    - [Type conversion](#type-conversion)
      - [Features](#features-3)
      - [Usage](#usage-3)
  - [Motivation](#motivation)

## How to Use
1. SugarPP is **header only** and **each header file is independent**. Just clone this repository and go to [./include/sugarpp](./include/sugarpp) or copy the corresponding header file you want to use.

    Or, if you want to use it hassle free, copy this Cmake snippet to your root ``CMakeLists.txt`` to automatically download & use this library in your project. No need to clone the project manually!
    ```cmake
    include(FetchContent)
    FetchContent_Declare(
        SugarPP
        GIT_REPOSITORY https://github.com/HO-COOH/SugarPP.git
        GIT_TAG origin/master
    )
    FetchContent_MakeAvailable(SugarPP)

    #Use for your target
    add_executable(<Your target> main.cpp)
    target_link_libraries(<Your target> PRIVATE SugarPP)
    ```

    Or, if you want to use the library globally, copy this Cmake snippet to your roor ``CMakeLists.txt`` to automatically download & use this library in your project. No need to clone the project!
    ```cmake
    include(FetchContent)
    FetchContent_Declare(
        SugarPP
        GIT_REPOSITORY https://github.com/HO-COOH/SugarPP.git
        GIT_TAG origin/master
    )
    FetchContent_MakeAvailable(SugarPP)

    #Use globally
    link_libraries(SugarPP)
    ```

2. Then add ``#include <sugarpp/xxx/xxx.hpp>``. Also **Note: Everything in SugarPP is now inside ``namespace SugarPP``!** So you may want ``using namespace SugarPP;``

You can find **quick** documentation for every modules in [./docs](./docs/)

You can find examples for every modules in [./test/source](./test/source/)

Alternatively, see [generated doxygen document here.](https://ho-cooh.github.io/SugarPPDoc/html/index.html)
## Requirements
SugarPP uses various C++17 language features; thus, it requires a C++17 compatible compiler to use.

~~**GCC 10.1 and older has [a known bug](https://stackoverflow.com/questions/64158484/ambiguous-call-after-adding-a-template-parameter-that-has-a-default-type), which causes issues on the overload resolution of ``detail::when_impl``; consider upgrading to GCC 10.2 or newer**~~ Nope, it is compatible with GCC 9.2 now :)

Tested with:
- GCC 10.2 & GCC 9.2
- Clang 10.0
- Visual Studio 16.7

## Features

### Kotlin-style `when` syntax

[Kotlin](https://kotlinlang.org/) has the `when` expression for matching values,
replacing the traditional `switch/case` in most languages.
C/C++'s native `switch/case` has the drawback of only
matching **integer** values.

SugarPP ``when``

- Value matching (for *any comparable type*, not just integers):
```kotlin
/* Kotlin */
when (x) {
    1 -> print("x == 1")
    2 -> print("x == 2")
    else -> { // Note the block here
        print("x is neither 1 nor 2")
    }
}
```

```cpp
/* SugarPP */
when (x,
    1,      []{ print("x == 1"); },
    2,      []{ print("x == 2"); },
    Else(), []{ print("x is neither 1 nor 2"); }
)(); //returns a function object, use () to call it
```

- Type matching:
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

- Polymorphic type matching:
```cpp
/*SugarPP*/
struct Shape { virtual ~Shape() = default; };
struct Circle :Shape {};
struct Square :Shape{};

std::unique_ptr<Shape> pt{ new Circle{} };
when(*pt,
    is_actually<Circle>(), [] { print("Circle* pt"); },
    is_actually<Square>(), [] { print("Square* pt"); },
    Else(),                [] { print("Unknown type"); }
)();    //"Circle* pt"
```

- Range matching:
```kotlin
/*kotlin*/
val validNumbers = arrayOf(11, 13, 17, 19)
when (x) {
    in 1..10 ->         print("x is in the range")
    in validNumbers ->  print("x is valid")
    !in 10..20 ->       print("x is outside the range")
    else ->             print("none of the above")
}
```

```cpp
/*SugarPP*/
std::array validNumbers{11,13,17,19};
when(x,
    Range(1, 10),       []{ print("x is in the range"); },
    Range(validNumbers),[]{ print("x is valid"); },
    NOT{Range(10, 20)}, []{ print("x is outside the range"); },
    Else(),             []{ print("none of the above"); }
)();
```

- Argument-less switches
```kotlin
/*kotlin*/
when {
    x.isOdd() ->    print("x is odd")
    y.isEven() ->   print("y is even")
    else ->         print("x+y is even.")
}
```

```cpp
/*SugarPP*/
int x = 1, y = 2;
when(
    isOdd(x),   []{ print("x is odd"); },
    isEven(y),  []{ print("y is even"); },
    Else(),     []{ print("x+y is even");}
)();//"x is odd"
```
Note: Unlike C/C++ switch-case, ``kotlin`` ``when`` is short-circuiting; the execution terminates at the first satisfied branch. ``SugarPP`` ``when`` has the same behavior.

- Pattern matching

Kotlin doesn't seems to support ``_`` as a place holder.
```rust
/*Rust*/
for i in 1..=100 {
    match (i % 3, i % 5) {
        (0, 0) => println!("FizzBuzz"),
        (0, _) => println!("Fizz"),
        (_, 0) => println!("Buzz"),
        (_, _) => println!("{}", i),
    }
}
```

```cpp
/*SugarPP*/
for(auto i:Range(1, 101))
{
    when(std::tuple{ i % 3, i % 5 },
        std::tuple{ 0, 0 }, [] { print("fizzbuzz"); },
        std::tuple{ 0, _ }, [] { print("fizz"); },
        std::tuple{ _, 0 }, [] { print("buzz"); },
        Else(),             [i] { print(i); }
    )();
}
```

#### Usage
Just copy [./include/sugarpp/when/when.hpp](./include/sugarpp/when/when.hpp) and add `#include "when.hpp"` in your project.

#### Documentation
See [docs/When.md](./docs/When.md)

#### Read More
At the time of writing this library, I was not aware of the [C++23 pattern matching proposal](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p1371r3.pdf). And yes, ``SugarPP::when`` will have performance penality compared with what can be done with ``switch-case`` statement. ``SugarPP::when`` works as recursively comparing the condition to each branch, so I am not sure whether this has performance penalty compared with the pattern matching proposal.

As I am still an early learner, I will update this part to give you more insight. You can find the original implementation of that proposal [here](https://github.com/mpark/patterns)

-------------------------------------------------------------------------
### Simpler IO
IO in C++ should work how you expect it to. SugarPP's IO functions are simple
and much more intuitive than native C++ IO. No more messing with `getchar()` and
`getline()` nonsense, and `print()` anything!

#### Features
The `input` template function is similar to Python's `input`. It prints a prompt message
and does automatic error handling - for example, if the input is bad, it will
clear the bad bit and re-prompt until an acceptable input is given (this behavior can
be disabled).
- If the type is a **primitive**, the function will work the same as `std::cin >>`
    - If the type given is `unsigned`, `input` will automatically convert the input to an absolute value.
- If the type is `std::string`, it will behave the same as `std::getline`, getting the whole line at once.
```cpp
auto name = input<std::string>("Enter your name: ");
print("Hello,", name, "How old are you?");
auto age = input<int>("Enter your age: ");
print(name, "is", age, "years old");
```

The `print` function also behaves similar to Python's `print`; it can print any number of arguments of any type, separated by a specified delimiter (defaulting to space). SugarPP's `print` can print almost anything:
- Anything `std::cout` has an overload for
- Anything that is iterable (i.e. has a `.begin()` or can be called with ``std::begin``)
- Nested iterables (at any depth)
- `std::tuple` and `std::pair`
- ``bool`` will be printed as ``True`` or ``False``

`printLn` behaves similarly, but prints each argument on a new line.
```cpp
/*print any iterable*/
std::array arr{ 1,2,3 };
print(arr); //[1, 2, 3]

/*print a tuple*/
std::tuple t{ "SugarPP", 123, 45.6f };
print(t); //(SugarPP, 123, 45.6)

/*print any nested printable*/
std::vector<std::vector<int>> v1{ {1,2,3}, {5,6,7,8}, {9,10} };
std::vector<std::vector<std::vector<int>>> v2{ {{1,2,3}, {5,6,7,8}, {9,10}}, {{10,11},{12,13}, {}} };
printLn(v1, v2); //[[1, 2, 3], [5, 6, 7, 8], [9, 10]]...

/*print a bool*/
print(0.1 + 0.2 == 0.3); //"False", you should know why :P
```

There are additional ``ThreadSafe`` versions of these functions with the same name, under ``namespace ThreadSafe``.

#### Usage
Just copy [./include/sugarpp/io/io.hp](./include/sugarpp/io/io.hpp) and add ``#include "io.hpp"``.

More examples in [./test/source/io/io.cpp](./test/source/io/io.cpp).


#### Documentation
See [docs/IO.md](./docs/IO.md).

------------------------------------------------------------
### Range
Use numerical ranges to simplify your range-based `for` loop!
~~Not to be confused with C++20 ranges.~~ Container Ranges are working in progress towards providing C++20 ranges functionality in C++17.

Many other programming languages have a *range syntax* for iteration:
```rust
// e.g. in Rust
for n in 0..10 {
    println!(n);
}
```
```python
# or in Python
for i in range(0, 10):
    print(i)
```
#### Features
SugarPP defines 3 types of Ranges in some sort of "class overloading" way
- Numerical ranges
  - `start`, `end`, and `step (default = 1)` with a C++ foreach loop. Type will be inferred and automatically converted if needed.
    ```cpp
    for (auto i : Range(2.0, 10.0, 3))
        print(i);
    /*
        2
        5
        8
    */
    ```
  - Multiple-dimension ranges
    ```cpp
    for (auto [i, j] : Range(-5, 1) | Range(0, 3))
        print(i, '\t', j);
    /*
        -5       0
        -5       1
        -5       2
        -4       0
        -4       1
        -4       2
        ...
        0        0
        0        1
        0        2
    */
    ```
  - Generating a random number within the range
    ```cpp
    /*use range for a random number*/
    Range r(-1, 100000);
    print("Random number in ", r, " is ", r.rand());

    /*use range to generate several random numbers*/
    auto [num1, num2, num3] = Range(1, 10).rand<3>();
    ```
  - Filling a container with random numbers
    ```cpp
    /*use range to fill a C style array*/
    double arr[10];
    Range(-500.0, 500.0).fillRand(arr);

    /*use range to fill a STL container*/
    std::array<char, 20> arr2;
    Range('A', 'z').fillRand(arr2);

    /*Alternatively .randFast() provides a faster way for generating random number using rand() in C*/
    int arr3[10];
    Range(-200, 300).fillRandFast(arr3);
    ```
- Letter ranges

  Similar functionality with numerical ranges, but works correctly when it is incremented it skips non letter characters

- Container ranges(In progress)

SugarPP also has an `Enumerate` class, which returns a pair of index (default to start at 0) and a reference to the content of iterable, similar to Python's `enumerate()`.
```python
# Python
a = ["enumerate", "in", "python"]
for i, content in enumerate(a):
    print(i, content)
```
```cpp
/*SugarPP*/
std::array a{"Enumerate", "in", "SugarPP"};
for(auto [i, content] : Enumerate(a))
    print(i, content);
```

#### Usage

Just copy [./include/sugarpp/range/range.hpp](./include/sugarpp/range/range.hpp) and add ``#include "range.hpp"`` for ``Range``.

Just copy [./include/sugarpp/range/enumerate.hpp](./include/sugarpp/range/enumerate.hpp) and add `#include "enumerate.hpp"` for ``Enumerate``.

More examples in [./test/source/range/range.cpp](./test/source/range/range.cpp)


#### Documentation
See [docs/Range.md](./docs/Range.md).

-----
### Type conversion

#### Features
- To & from string
  + -> number

    Admit it, ``atoi()``, ``atof()``, ``wcstold()``, ``strtof()`` are some of the ugliest function name in C, and C++ makes it worse by adding more obsecure names like ``std::stoi()``, ``std::stolld()``. That's why SugarPP provides a uniform way of getting numbers from string, which is ``SugarPP::to_num<Type>()``, which accepts both normal strings and wide-strings.
    ```cpp
    /*SugarPP*/
    auto str1 = "42";
    auto num1 = to_num<int>(str1);

    auto str2 = "3.14159";
    auto num2 = to_num<double>(str2);
    ```
  + -> string

    Isn't it wired that something printable can't be converted to string? Isn't it wired that there is a ``std::to_string()`` only works for numerical values?
    ``SugarPP::to_string`` not only works with anything that can be converted with ``std::to_string()``, but also anything that is printable. Additionally, you can also specify whether it's normal character or wide-character using one template argument.
    ```cpp
    auto f_str = to_string(23.43);

    std::ostream& operator<<(std::ostream& os, const MyPrintableClass&);
    auto my_class_str = to_string(my_printable);
    ```
#### Usage
Just copy [./include/sugarpp/types/types.hpp](./include/sugarpp/types/types.hpp) and add ``#include "types.hpp"``.

More example in [./test/source/types/to_string.cpp](./test/source/types/to_string.cpp)

-----
## Motivation
I had so much fun writing these and learned so much. ~~Such a great language that gives you nightmare everytime you want to add stuff. C++ itself is difficult enough, yet you realize that you can't even have a compiler to trust with when 3 different compilers (Visual studio, Clang, GCC) gives you different results.~~
