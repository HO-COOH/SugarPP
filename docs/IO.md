## Input

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

## Output

```cpp
template <char delim = ' ', std::ostream& os = std::cout, typename... Args>
void print(Args &&... args);    //1
template <std::ostream& os = std::cout, typename... Args>
void printLn(Args &&... args);  //2
```
1. Print any number of objects specified by the parameter pack ``args`` as if calling ``std::cout << args;``, each followed by a ``delim``. Print a new line after the call.
2. Equivalent to ``print<'\n'>(agrs...)``

## Thread Safe Output

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
