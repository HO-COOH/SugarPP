## When
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
