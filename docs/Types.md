## To string

```cpp
template<typename Char = char>
auto to_string();                           //1

template<typename Char = char, typename Arg>
auto to_string(Arg&& arg);                  //2

template<typename Char = char, typename Arg, typename... Args>
auto to_string(Arg&& arg, Args&& ...args);  //3
```
- 2-3 Convert arguments to ``std::basic_string<Char>`` and concatenate them (if multiple arguments are supplied), if
  + if ``arg`` has an overload of ``std::to_string()``
  + otherwise, call ``std::basic_ostream<Char>& operator<<``, where the left-hand side is ``std::basic_stringstream``. Then return the string recorded in the string stream.

## To number
```cpp
template<typename NumberType>
auto to_num(const char* str);                                       //1

template<typename NumberType>
auto to_num(const char* str, char** str_end, int base = 10);        //2

template<typename NumberType>
auto to_num(const wchar_t* str, wchar_t** str_end, int base = 10);  //3

template<typename NumberType, typename Char = char, typename = std::enable_if_t<std::is_same_v<Char, char> || std::is_same_v<Char, wchar_t>>>
auto to_num(const std::basic_string<Char>& str, std::size_t* pos = nullptr, int base = 10);//4
```

- 1 According to ``NumberType``, the function calls
  + ``std::atoi``
  + ``std::atol``
  + ``std::atoll``
- 2 According to ``NumberType``, the function calls
  + ``std::strtol``
  + ``std::strtoll``
  + ``std::strtoul``
  + ``std::strtoull``
  + ``std::strtof``
  + ``std::strtod``
  + ``std::strtold``
- 3 Same as 2, but calls the corresponding wide-string versions
- 4 According to ``NumberType``, the function calls
  + ``std::stoi``
  + ``std::stol``
  + ``std::stoll``
  + `std::stoul`
  + `std::stoull`
  + `std::stof`
  + `std::stod`
  + `std::stold`