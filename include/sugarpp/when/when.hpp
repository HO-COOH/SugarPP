/*****************************************************************//**
 * \file   When.hpp
 * \brief  Kotlin's when statement port
 * 
 * \author Peter
 * \date   September 2020
 *********************************************************************/

#pragma once
#include <type_traits>
#include <utility>
#include <cstring>  //for strcmp()
#include <utility>
#include <functional>

#ifdef SugarPPNamespace
namespace SugarPP
{
#endif
    /**
     * @brief Type traits for determining whether two types can be compared with an equal operator
     * @tparam lhsType Type of Left-hand-side
     * @tparam rhsType Type of Right-hand-side
     */
    template <typename lhsType, typename rhsType, typename = void>
    struct comparable : std::false_type
    {
    };

    template <typename lhsType, typename rhsType>
    struct comparable<lhsType, rhsType, decltype((std::declval<lhsType>() == std::declval<rhsType>()), void())>
        : std::true_type
    {
    };

    /*Dummy structs representing logical relation used by [GroupedExpression]*/
    class Operator {
    public:
    };
    /**
     * @brief A dummy struct for logic NOT
    */
    template<typename Case>
    class NOT : public Operator
    {
        Case caseExpr;
    public:
        NOT(Case&& expr) :caseExpr(std::move(expr)) {}

        template<typename Expr, typename = std::enable_if_t<comparable<Expr, Case>::value>>
        bool operator==(Expr&& expr) const
        {
            return expr != caseExpr;
        }
    };
    template<typename Expr, typename NotType, typename = std::enable_if_t<comparable<Expr, NotType>::value>>
    bool operator==(Expr&& expr, NOT<NotType> const& notCase)
    {
        return notCase == expr;
    }

    /**
     * @brief A dummy struct for logic AND
    */
    template<typename Case1, typename Case2>
    class AND : public Operator
    {
        Case1 caseExpr1;
        Case2 caseExpr2;
    public:
        AND(Case1&& expr1, Case2&& expr2) :caseExpr1(std::move(expr1)), caseExpr2(std::move(expr2)) {}

        template<typename Expr, typename = std::enable_if_t<comparable<Expr, Case1>::value&& comparable<Expr, Case2>::value>>
        bool operator==(Expr&& expr) const
        {
            return (caseExpr1 == expr) && (caseExpr2 == expr);
        }

    };
    template<typename Expr, typename Case1, typename Case2, typename = std::enable_if_t<comparable<Expr, Case1>::value&& comparable<Expr, Case2>::value>>
    bool operator==(Expr&& expr, AND<Case1, Case2> const& andCase)
    {
        return andCase == expr;
    }

    /**
     * @brief A dummy struct for logic OR
    */
    template<typename Case1, typename Case2>
    struct OR
    {
        Case1 caseExpr1;
        Case2 caseExpr2;
    public:
        OR(Case1&& expr1, Case2&& expr2) :caseExpr1(std::move(expr1)), caseExpr2(std::move(expr2)) {}

        template<typename Expr, typename = std::enable_if_t<comparable<Expr, Case1>::value&& comparable<Expr, Case2>::value>>
        bool operator==(Expr&& expr) const
        {
            return (caseExpr1 == expr) || (caseExpr2 == expr);
        }
    };
    template<typename Expr, typename Case1, typename Case2, typename = std::enable_if_t<comparable<Expr, Case1>::value&& comparable<Expr, Case2>::value>>
    bool operator==(Expr&& expr, OR<Case1, Case2> const& orCase)
    {
        return orCase == expr;
    }
    /**********************************************************************/

    /**
     * @brief A dummy struct for Else() cases
     */
    struct Else
    {
        constexpr operator bool() const { return true; }
    };

    /**
     * @brief Dummy struct for is<Type> query
     * @tparam Type The original type, which will be stored after removing reference type
     */
    template <typename Type>
    struct is
    {
        using type = std::remove_reference_t<Type>;
    };

    /**
     * @brief Dummy struct for is_not<Type> query
     * @tparam Type The original type, which will be stored after removing reference type
     */
    template<typename Type>
    struct is_not
    {
        using type = std::remove_reference_t<Type>;
    };


    /**************C++20 concept and abbreviated function template implementation *****************/

#if 0
    template <typename T1, typename T2>
    constexpr bool comparable = requires(T1 const& lhs, T2 const& rhs)
    {
        lhs == rhs;
    };

    /**
     * @brief The non-Else Final Case Primary template
     *
     * @param expr: the expression to match
     * @param to_match: the case to match with expression
     * @param ReturnResult: the return expression if [expr] matches [to_match]
     * @return [ReturnResult] if matches, else return the default constructed object the same type as [expr]
     */
    auto when_impl(auto&& expr, auto&& to_match, auto&& ReturnResult)
    {
        using ReturnType = std::remove_reference_t<decltype(ReturnResult)>;
        /*Handle the special case when_impl [to_match] is already a boolean type */
        if constexpr (std::is_same_v<std::remove_reference_t<decltype(to_match)>, bool>)
        {
            if (to_match)
                return ReturnResult;
        }
        /*If [to_match] is something comparable to [expr]*/
        if constexpr (comparable<decltype(expr), decltype(to_match)>)
        {
            if (to_match == expr)
                return ReturnResult;
        }
        return ReturnType{};
    }

    /**
     * @brief The final Else case
     *
     * @param auto&&: place holder for non-used [expression]
     * @param ReturnResult: return value for Else case
     */
    auto when_impl(auto&&, Else, auto&& ReturnResult)
    {
        return ReturnResult;
    }

    /**
     * @brief The special case for handling is<SomeType> expression
     *
     * @tparam is_type: the type query for whether [expr] has the same type
     * @param expr: the expression to match
     * @param ReturnResult: the return expression if [expr] has the same type as [is_type]
     */
    template <typename is_type>
    auto when_impl(auto&& expr, is<is_type>, auto&& ReturnResult)
    {
        if constexpr (std::is_same_v<decltype(expr), typename is<is_type>::type>)
            return ReturnResult;
        return decltype(ReturnResult){};
    }
    template <typename is_not_type>
    auto when_impl(auto&& expr, is_not<is_not_type>, auto&& ReturnResult)
    {
        if constexpr (!std::is_same_v<decltype(expr), typename is_not<is_not_type>::type>)
            return ReturnResult;
        return decltype(ReturnResult){};
    }

    /**
     * @brief Special case for handling C string
    */
    auto when_impl(const char* expr, const char* to_match, auto&& ReturnResult)
    {
        if (strcmp(expr) == strcmp(to_match))
            return ReturnResult;
        return decltype(ReturnResult){};
    }


    /**
     * @brief primary recursive template
    */
    auto when_impl(auto&& expr, auto&& case1, auto&& return1, auto&& case2, auto&&... args)
    {
        if constexpr (std::is_same_v<decltype(case1), bool>)
        {
            if (case1)
                return return1;
        }
        if constexpr (comparable<decltype(expr), decltype(case1)>)
        {
            if (case1 == expr)
                return return1;
            else
                return when_impl(expr, case2, args...);
        }
        return when_impl(expr, case2, args...);
    }
    /* A note: It seems we don't need to forward the argument here. Because no copy/move is performed when_impl I tried:
        auto func2(auto&&){}
        auto func1(auto&& object) { return func2(object); }
        The object won't get moved nor copied.
    */


    /**
     * @brief Special case for handling C string
    */
    auto when_impl(const char* expr, const char* case1, auto&& return1, auto&& case2, auto&&... args)
    {
        if (strcmp(expr) == strcmp(case1))
            return return1;
        else
            return when_impl(expr, case2, args...);
    }


    /**
     * @brief Special case for handling is<> and is_not<> type query
    */
    template <typename is_type>
    auto when_impl(auto&& expr, is<is_type>, auto&& return1, auto&& case2, auto&&... args)
    {
        if constexpr (std::is_same_v<decltype(expr), typename is<is_type>::type>)
            return return1;
        return when_impl(expr, case2, args...);
    }
    template <typename is_not_type>
    auto when_impl(auto&& expr, is_not<is_not_type>, auto&& return1, auto&& case2, auto&&... args)
    {
        if constexpr (!std::is_same_v<decltype(expr), typename is_not<is_not_type>::type>)
            return return1;
        return when_impl(expr, case2, args...);
    }

#else
/*****************************C++17 template implementation ***************************************/

    namespace detail
    {

        template <bool convertToFunction = false, typename ExprType, typename CaseType, typename ReturnType>
        auto when_impl(ExprType&& expr, CaseType&& to_match, ReturnType&& ReturnResult)
        {
            /*Handle the special case when [to_match] is already a boolean type */
            if constexpr (std::is_same_v<std::remove_reference_t<CaseType>, bool>)
            {
                if (to_match)
                {
                    if constexpr (convertToFunction)
                        return std::function{ ReturnResult };
                    else
                        return ReturnResult;
                }
            }
            /*If [to_match] is something comparable to [expr]*/
            if constexpr (comparable<std::remove_reference_t<ExprType>, std::remove_reference_t<CaseType>>::value)
            {
                if (to_match == expr)
                {
                    if constexpr (convertToFunction)
                        return std::function{ ReturnResult };
                    else
                        return ReturnResult;
                }
            }
            if constexpr (convertToFunction)
                return std::function{ [] {} };
            else
                return std::decay_t<ReturnType>{};
        }

        template <bool convertToFunction = false, typename ExprType, typename is_type, typename ReturnType>
        auto when_impl(ExprType&&, is<is_type>, ReturnType&& ReturnResult)
        {
            if constexpr (std::is_same_v<std::remove_reference_t<ExprType>, typename is<is_type>::type>)
            {
                if constexpr (convertToFunction)
                    return std::function{ ReturnResult };
                else
                    return ReturnResult;
            }
            if constexpr (convertToFunction)
                return std::function{ [] {} };
            else
                return std::decay_t<ReturnType>{};
        }

        template <bool convertToFunction = false, typename ExprType, typename is_not_type, typename ReturnType>
        auto when_impl(ExprType&&, is_not<is_not_type>, ReturnType&& ReturnResult)
        {
            if constexpr (!std::is_same_v<std::remove_reference_t<ExprType>, typename is_not<is_not_type>::type>)
            {
                if constexpr (convertToFunction)
                    return std::function{ ReturnResult };
                else
                    return ReturnResult;
            }
            if constexpr (convertToFunction)
                return std::function{ [] {} };
            else
                return std::decay_t<ReturnType>{};
        }


        template <bool convertToFunction = false, typename ReturnType>
        auto when_impl(const char* expr, const char* Case, ReturnType&& ReturnResult)
        {
            if (strcmp(expr, Case) == 0)
            {
                if constexpr (convertToFunction)
                    return std::function{ ReturnResult };
                else
                    return ReturnResult;
            }
            if constexpr (convertToFunction)
                return std::function{ [] {} };
            return std::decay_t<ReturnType>{};
        }

        template <bool convertToFunction, typename ExprType, typename ReturnType>
        auto when_impl(ExprType&&, Else, ReturnType&& ReturnResult)
        {
            if constexpr (convertToFunction)
                return std::function{ ReturnResult };
            else
                return ReturnResult;
        }

        template <bool convertToFunction, typename ExprType, typename is_not_type, typename Return1Type, typename Case2Type, typename... Args>
        auto when_impl(ExprType&& expr, is_not<is_not_type>, Return1Type&& return1, Case2Type&& case2, Args &&... args);
        template <bool convertToFunction, typename ExprType, typename is_type, typename Return1Type, typename Case2Type, typename... Args>
        auto when_impl(ExprType&& expr, is<is_type>, Return1Type&& return1, Case2Type&& case2, Args &&... args);
        template <bool convertToFunction, typename Return1Type, typename Case2Type, typename... Args>
        auto when_impl(const char* Expr, const char* Case1, Return1Type&& return1, Case2Type&& case2, Args &&... args);
        template <bool convertToFunction, typename ReturnType>
        auto when_impl(Else, ReturnType&& returnResult);

        template <bool convertToFunction, typename ExprType, typename Case1Type, typename Return1Type, typename Case2Type, typename... Args, typename = std::enable_if_t<(4 + sizeof...(Args)) % 2 != 0>>
        auto when_impl(ExprType&& expr, Case1Type&& case1, Return1Type&& return1, Case2Type&& case2, Args &&... args)
        {
            if constexpr (std::is_same_v<std::remove_reference_t<Case1Type>, bool>)
            {
                if (case1)
                {
                    if constexpr (convertToFunction)
                        return std::function{ return1 };
                    else
                        return return1;
                }
            }
            else if constexpr (comparable<ExprType, Case1Type>::value)
            {
                if (expr == case1)
                {
                    if constexpr (convertToFunction)
                        return std::function{ return1 };
                    else
                        return return1;
                }
                else
                    return when_impl<convertToFunction>(std::forward<ExprType>(expr), std::forward<Case2Type>(case2), std::forward<Args>(args)...);
            }
            else
                return when_impl<convertToFunction>(std::forward<ExprType>(expr), std::forward<Case2Type>(case2), std::forward<Args>(args)...);
        }
        template <bool convertToFunction, typename Return1Type, typename Case2Type, typename... Args>
        auto when_impl(const char* Expr, const char* Case1, Return1Type&& return1, Case2Type&& case2, Args&&... args)
        {
            if (strcmp(Expr, Case1) == 0)
            {
                if constexpr (convertToFunction)
                    return std::function{ return1 };
                else
                    return return1;
            }
            return when_impl<convertToFunction>(Expr, std::forward<Case2Type>(case2), std::forward<Args>(args)...);
        }

        template <bool convertToFunction, typename ExprType, typename is_type, typename Return1Type, typename Case2Type, typename... Args>
        auto when_impl(ExprType&& expr, is<is_type>, Return1Type&& return1, Case2Type&& case2, Args&&... args)
        {
            if constexpr (std::is_same_v<std::remove_reference_t<ExprType>, typename is<is_type>::type>)
            {
                if constexpr (convertToFunction)
                    return std::function{ return1 };
                else
                    return std::forward<Return1Type>(return1);
            }
            return when_impl<convertToFunction>(std::forward<ExprType>(expr), std::forward<Case2Type>(case2), std::forward<Args>(args)...);
        }

        template <bool convertToFunction, typename ExprType, typename is_not_type, typename Return1Type, typename Case2Type, typename... Args>
        auto when_impl(ExprType&& expr, is_not<is_not_type>, Return1Type&& return1, Case2Type&& case2, Args&&... args)
        {
            if constexpr (!std::is_same_v<std::remove_reference_t<ExprType>, typename is_not<is_not_type>::type>)
            {
                if constexpr (convertToFunction)
                    return std::function{ return1 };
                else
                    return std::forward<Return1Type>(return1);
            }
            else
                return when_impl<convertToFunction>(std::forward<ExprType>(expr), std::forward<Case2Type>(case2), std::forward<Args>(args)...);
        }

        template<bool convertToFunction, typename ReturnType>
        auto when_impl(bool Case, ReturnType&& returnResult);

        template<bool convertToFunction, typename Return1Type, typename ...Args, typename = std::enable_if_t<(3 + sizeof...(Args)) % 2 == 0>>
        auto when_impl(bool case1, Return1Type&& return1, bool case2, Args&&...args)
        {
            if constexpr (convertToFunction)
            {
                if (case1)
                    return std::function{ return1 };
                return when_impl<convertToFunction>(case2, std::forward<Args>(args)...);
            }
            else
            {
                if (case1)
                    return return1;
                else
                    return when_impl<convertToFunction>(case2, std::forward<Args>(args)...);
            }
        }

        template<bool convertToFunction, typename ReturnType>
        auto when_impl(bool Case, ReturnType&& returnResult)
        {
            if constexpr (convertToFunction)
            {
                if (Case)
                    return std::function{ returnResult };
                return std::function{ [] {} };
            }
            else
            {
                if (Case)
                    return returnResult;
                else
                    return std::decay_t<ReturnType>{};
            }
        }

        template<bool convertToFunction, typename ReturnType>
        auto when_impl(Else, ReturnType&& returnResult)
        {
            if constexpr (convertToFunction)
                return std::function{ returnResult };
            else
                return returnResult;
        }
    }

    /////////////////////////////////////////////////////////////////////////////

    template <size_t I, typename Tuple>
    constexpr bool shouldConvert()
    {
        using tuple_type = Tuple;
        if constexpr (std::is_array_v<std::remove_reference_t<std::tuple_element_t<I, tuple_type>>>)
            return false;
        else
        {
            const bool current = std::is_same_v<
                std::remove_reference_t<std::tuple_element_t<I, tuple_type>>,
                std::remove_reference_t<std::tuple_element_t<I + 2, tuple_type>>
            >;
            if constexpr (I + 2 >= std::tuple_size_v<tuple_type> -1)
                return current;
            else //forget this you get 3K errors
                return !(current && shouldConvert<I + 2, Tuple>());
        }
    }

    /**
     * @brief The non-Else Final Case Primary template
     *
     * @param expr the expression to match
     * @param toMatch the case to match with expression
     * @param returnResult the object to return
     * @return \preturnResult if matches, else return the default constructed object the same type as \pexpr
     */
    template <typename ExprType, typename CaseType, typename ReturnType>
    auto when(ExprType&& expr, CaseType&& toMatch, ReturnType&& returnResult)
    {
        return detail::when_impl<false>(std::forward<ExprType>(expr), std::forward<CaseType>(toMatch), std::forward<ReturnType>(returnResult));
    }

    /**
     * @brief The special case for handling is<SomeType> expression
     *
     * @tparam is_type the type query for whether [expr] has the same type
     * @param expr the expression to match
     * @param dummy place holder for is<is_type>
     * @param returnResult the object to return
     * @return \preturnResult if \bExprType is the same as \bis_type, otherwise return a default constructed \breturnType object
     */
    template <typename ExprType, typename is_type, typename ReturnType>
    auto when(ExprType&& expr, is<is_type> dummy, ReturnType&& returnResult)
    {
        return detail::when_impl<false>(std::forward<ExprType>(expr), dummy, std::forward<ReturnType>(returnResult));
    }

    /**
     * @brief The special case for handling is<SomeType> expression
     *
     * @tparam is_not_type the type query for whether [expr] has the same type
     * @param expr the expression to match
     * @param dummy place holder for is_not<is_not_type>
     * @param returnResult the object to return
     * @return \pReturnResult if \bExprType is \bNOT the same as \bis_not_type, otherwise return a default constructed \breturnType object
     */
    template <typename ExprType, typename is_not_type, typename ReturnType>
    auto when(ExprType&& expr, is_not<is_not_type> dummy, ReturnType&& returnResult)
    {
        return detail::when_impl<false>(std::forward<ExprType>(expr), dummy, std::forward<ReturnType>(returnResult));
    }

    /**
     * @brief Special case for handling C string
     * @tparam ReturnType type of \p ReturnResult
     * @param expr The C string to be matched with
     * @param Case The C string to be match with \p expr (capital 'C' to avoid conflict with "case" keyword)
     * @param returnResult the object to return
     * @return \p ReturnResult if ``strcmp(expr, Case)==0``, otherwise return a default constructed \b ReturnType object
     */
    template <typename ReturnType>
    auto when(const char* expr, const char* Case, ReturnType&& returnResult)
    {
        return detail::when_impl<false>(expr, Case, std::forward<ReturnType>(returnResult));
    }

    /**
     * @brief The special case for handling <Else> expression
     *
     * @tparam ExprType type of not-used expression
     * @param expr the expression to match
     * @param dummy the placeholder of Else
     * @param returnResult the object to return
     * @return \p ReturnResult
     */
    template <typename ExprType, typename ReturnType>
    auto when(ExprType&& expr, Else dummy, ReturnType&& returnResult)
    {
        return detail::when_impl<false>(std::forward<ExprType>(expr), dummy, std::forward<ReturnType>(returnResult));
    }

    /**
     * @brief primary recursive template
    */
    template <typename ExprType, typename Case1Type, typename Return1Type, typename Case2Type, typename... Args, typename = std::enable_if_t<(4 + sizeof...(Args)) % 2 != 0>>
    auto when(ExprType&& expr, Case1Type&& case1, Return1Type&& return1, Case2Type&& case2, Args&&... args)
    {
        return detail::when_impl
            <
            shouldConvert
            <
            2,
            decltype(std::forward_as_tuple(expr, case1, return1, case2, args...))
            >()
            >(std::forward<ExprType>(expr),
                std::forward<Case1Type>(case1),
                std::forward<Return1Type>(return1),
                std::forward<Case2Type>(case2),
                std::forward<Args>(args)...);
    }

    /**
     * @brief primary recursive template specialized for C string
    */
    template <typename Return1Type, typename Case2Type, typename... Args>
    auto when(const char* expr, const char* case1, Return1Type&& return1, Case2Type&& case2, Args&&... args)
    {
        return detail::when_impl
            <
            shouldConvert
            <
            2, decltype(std::forward_as_tuple(expr, case1, return1, case2, args...))
            >()
            >(expr, case1, std::forward<Return1Type>(return1), std::forward<Case2Type>(case2), std::forward<Args>(args)...);
    }

    /**
     * @brief primary recursive template specialized for is<is_type> query
    */
    template <typename ExprType, typename is_type, typename Return1Type, typename Case2Type, typename... Args>
    auto when(ExprType&& expr, is<is_type> dummy, Return1Type&& return1, Case2Type&& case2, Args... args)
    {
        return detail::when_impl
            <
            shouldConvert
            <
            2, decltype(std::forward_as_tuple(expr, dummy, return1, case2, args...))
            >()
            >(std::forward<ExprType>(expr), dummy, std::forward<Return1Type>(return1), std::forward<Case2Type>(case2), std::forward<Args>(args)...);
    }

    /**
     * @brief primary recursive template specialized for is_not<is_not_type> query
     */
    template <typename ExprType, typename is_not_type, typename Return1Type, typename Case2Type, typename... Args>
    auto when(ExprType&& expr, is_not<is_not_type> dummy, Return1Type&& return1, Case2Type&& case2, Args... args)
    {
        return detail::when_impl
            <
            shouldConvert
            <
            2, decltype(std::forward_as_tuple(expr, dummy, return1, case2, args...))
            >()
            >(std::forward<ExprType>(expr), dummy, std::forward<Return1Type>(return1), std::forward<Case2Type>(case2), std::forward<Args>(args)...);
    }

    template<typename Return1Type, typename ...Args, typename = std::enable_if_t<(3 + sizeof...(Args)) % 2 == 0>>
    auto when(bool case1, Return1Type&& return1, bool case2, Args&&...args)
    {
        return detail::when_impl
            <
            shouldConvert
            <
            1, decltype(std::forward_as_tuple(case1, return1, case2, args...))
            >()
            >(case1, std::forward<Return1Type>(return1), case2, std::forward<Args>(args)...);
    }

#endif
    
#ifdef SugarPPNamespace
}
#endif