# When
Examples are in `examples/When`.
```cpp
/*Primary templates*/
template <typename ExprType, typename Case1Type, typename Return1Type, typename... Args>
auto when(ExprType&& expr, Case1Type&& case1, Return1Type&& return1, Args&&... args);     //1

template<typename Return1Type, typename ...Args>
auto when(bool case1, Return1Type&& return1, Args&&...args);                              //2
```
- 1 ``when`` that has an expression/pattern to be matched
- 2 ``when`` that doesn't has an expression/pattern to be matched and works as boolean switches

## Globals
```cpp
struct _Anything
{};
constexpr inline _Anything _;
```
Which defines ``operator==`` and returns true conditionlessly as a place-holder for supporting pattern matching.

## !Important Node!
Because in C++, you have to return the same time from a function, and because lambda expressions are unique types, I did a little trick to get it done. Basically I check if the returned objects are not the same type and are not arrays, ``when`` will return a ``std::function`` object to wrap up the lambdas.

```cpp
when(expression,
   case1, return1,  // --
                    //   | check if they are same type or not?
   case2, return2,  // --
   ...
);
```

Therefore, as a side effect, if you are not intended to get a function, you **must** ensure the return statements are the same type after ``std::remove_reference_t``. Use casting if they are not.