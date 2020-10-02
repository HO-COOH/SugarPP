# When
Examples are in `examples/When`.
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

template<typename Return1Type, typename ...Args>
auto when(bool case1, Return1Type&& return1, bool case2, Args&&...args);                                    //5

/*Ending templates*/
template <typename ExprType, typename CaseType, typename ReturnType>
auto when(ExprType&& expr, CaseType&& to_match, ReturnType&& ReturnResult);                                 //6

template <typename ReturnType>
auto when(const char* Expr, const char* Case, ReturnType&& ReturnResult);                                   //7

template <typename ExprType, typename is_type, typename ReturnType>
auto when(ExprType&&, is<is_type>, ReturnType&& ReturnResult);                                              //8

template <typename ExprType, typename is_not_type, typename ReturnType>
auto when(ExprType&&, is_not<is_not_type>, ReturnType&& ReturnResult);                                      //9

template <typename ExprType, typename ReturnType>
auto when(ExprType&&, Else, ReturnType&& ReturnResult);                                                     //10

template <typename ReturnType>
auto when(Else, ReturnType &&returnResult);                                                                 //11
```
- 1-4 are the primary recursive templates
   - 2 Specialization for ``const char*`` of ``Expression`` to match, performing ``strcmp``
   - 3 Specialization for ``is<Type>``, performing type query, as if checking ``std::is_same_v<std::remove_reference_t<ExprType>, Type>``
   - 4 Specialization for ``is_not<Type>``, performing type query, as if checking ``!std::is_same_v<std::remove_reference_t<ExprType>, Type>``
- 5 is the primary recursive template of argument-less ``when``
- 6-9 are the ending templates which functions similarly as 1-4
- 10 handles the ``Else`` cases and returns ``ReturnResult`` if non of the previous cases are matched described in 1-9
- 11 handles the ``Else`` cases of argument-less ``when`` similar to ``10``

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