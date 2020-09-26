# SugarPP: syntactic 🍬 for programming in C++
SugarPP is a collection of syntactic candy for C++ code.

- [SugarPP: syntactic 🍬 for programming in C++](#sugarpp-syntactic--for-programming-in-c)
  - [How to Use](#how-to-use)
  - [Requirements](#requirements)
  - [Features](#features)
    - [Kotlin-style `when` syntax](#kotlin-style-when-syntax)
      - [Usage](#usage)
      - [Documentation](#documentation)
    - [Simpler IO](#simpler-io)
      - [Features](#features-1)
      - [Usage](#usage-1)
      - [Documentation](#documentation-1)
    - [Range](#range)
      - [Usage](#usage-2)
      - [Documentation](#documentation-2)

## How to Use
SugarPP is **header only**. Just clone this repository or copy the corresponding header files you want to use.

You can find **quick** documentation for every modules in [./docs](./docs/)

You can find examples for every modules in [./examples](./examples/)

Alternatively, see [generated doxygen document here.](https://ho-cooh.github.io/SugarPPDoc/html/index.html)
## Requirements
SugarPP uses various C++17 language features thus requires a C++17 compatible compiler to use.

## Features

### Kotlin-style `when` syntax

[Kotlin](https://kotlinlang.org/) has the `when` expression for matching values,
replacing the traditional `switch/case` in most languages.
C/C++'s native `switch/case` has the drawback of only
matching **integer** values; however, Kotlin's `when` can match any comparable type.

SugarPP has its own analogue to Kotlin's `when`:

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
#include "When/When.hpp"
when (x,
    1,      []{ puts("x == 1"); },
    2,      []{ puts("x == 2"); },
    Else(), []{ puts("x is neither 1 nor 2"); }
);
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
#include "When/When.hpp"
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
#include "Range/In.hpp"     // Range
#include "When/When.hpp"    // When
std::array validNumbers{11,13,17,19};
when(x,
    Range(1, 10),       []{ puts("x is in the range"); },
    Range(validNumbers),[]{ puts("x is valid"); },
    NOT{Range(10, 20)}, []{ puts("x is outside the range"); },
    Else(),             []{ puts("none of the above"); }
);
```

Although mocking Kotlin's `when` functionality in C++ may seem like a dumb
idea, it's actually quite powerful and demonstrates the flexibility of
`template`.

#### Usage
Just copy [When.hpp](./When/When.hpp) and add `#include "When.hpp"` in your project.

#### Documentation
See [docs/When.md](./docs/When.md) 

-------------------------------------------------------------------------
### Simpler IO
IO in C++ should work how you expect it to. SugarPP's IO functions are simple
and much more intuitive than native C++ IO. No more messing with `getchar()` and
`getline()` nonsense, and `print()` anything!

#### Features

SugarPP includes two main IO convenience functions:

The `input` template function is similar to Python's `input`. It prints a prompt message
and does automatic error handling - for example, if the input is bad, it will
clear the bad bit and re-prompt until an acceptable input is given (this behavior can
be disabled).
- If the type is a **primitive**, the function will work the same as `std::cin >>`
    - If the type given is `unsigned`, `input` will automatically convert the input to an absolute value.
- If the type is `std::string`, it will behave the same as `std::getline`, getting the whole line at once.

The `print` function also behaves similar to Python's `print`; it can print any number of arguments of any type, separated by a specified delimiter (defaulting to space). SugarPP's `print` can print almost anything:
- Anything `std::cout` has an overload for (primitives, strings, etc.)
- Anything that is iterable (i.e. has a `.begin()`)
    - Nested iterables work too! (at *any* depth)
- `std::tuple` and `std::pair`

`printLn` behaves similarly, but prints each argument on a new line.

#### Usage
Just copy [./IO/IO.hpp](./IO/IO.hpp) and add ``#include "IO.hpp"``.

More examples in [./examples/IO/main.cpp](./examples/IO/main.cpp).

```cpp
#include "IO/IO.hpp"
 
int main()
{
    print("Hello, what's your name?");
    auto name = input<std::string>("Enter your name: ");
    print("Hello,", name, "How old are you?");
    auto age = input<int>("Enter your age: ");
    print(name, "is", age, "years old");

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
}
```

#### Documentation
See [docs/IO.md](./docs/IO.md).

------------------------------------------------------------
### Range
Use numerical ranges in your `for` loop!
*Not to be confused with C++20 ranges.*

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

SugarPP adds support for this with the `Range` class. `Range` supports:
- `start`, `end`, and `step (default = 1)` with a C++ foreach loop. Type will be inferred and automatically converted if needed.
- Multiple-dimension ranges
- Generating a random number within the range
- Filling a container with random numbers

SugarPP also has an `Enumerate` class, which accomplishes a similar task to Python's `enumerate()`; it returns an `std::pair` of the `index` and the `iterator`. It can be combined with C++17 structured binding (e.g. `for (auto [i, v]: Enumerate(array))`) for maximum effectiveness.

#### Usage

Just copy [./Range/Range.hpp](./Range/Range.hpp) and add ``#include "Range.hpp"`` for ``Range``.

Just copy [./Range/Enumerate.hpp](./Range/Enumerate.hpp) and add `#include "Enumerate.hpp"` for ``Enumerate``.

More examples in [./examples/Range/main.cpp](./examples/Range/main.cpp)

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

#### Documentation
See [docs/Range.md](./docs/Range.md).
