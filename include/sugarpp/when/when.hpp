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
#include <memory>

#ifdef SugarPPNamespace
namespace SugarPP
{
#endif

    /**
     * @brief A dummy struct that defines all comparison operators, which return true conditionlessly
     */
    struct Anything
    {
        template<typename T>
        constexpr bool operator==(T const&) const
        {
            return true;
        }

        template<typename T>
        constexpr bool operator!=(T const&) const
        {
            return true;
        }

        template<typename T>
        constexpr bool operator>(T const&) const
        {
            return true;
        }

        template<typename T>
        constexpr bool operator>=(T const&) const
        {
            return true;
        }

        template<typename T>
        constexpr bool operator<(T const&) const
        {
            return true;
        }

        template<typename T>
        constexpr bool operator<=(T const&) const
        {
            return true;
        }

        template<typename T>
        friend constexpr bool operator==(const T&, Anything)
        {
            return true;
        }

        template<typename T>
        friend constexpr bool operator!=(const T&, Anything)
        {
            return true;
        }

        template<typename T>
        friend constexpr bool operator>(const T&, Anything)
        {
            return true;
        }

        template<typename T>
        friend constexpr bool operator>=(const T&, Anything)
        {
            return true;
        }

        template<typename T>
        friend constexpr bool operator<(const T&, Anything)
        {
            return true;
        }

        template<typename T>
        friend constexpr bool operator<=(const T&, Anything)
        {
            return true;
        }
    };

    constexpr inline Anything _;    //The globally available wildcard object

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

        template<typename Arg>
        constexpr bool operator()(Arg&& arg) const
        {
            return
            std::is_same_v<
                std::conditional_t<
                    std::is_array_v<std::remove_reference_t<Arg>>,
                    std::decay_t<Arg>,
                    std::remove_reference_t<Arg>
                >,
                Type
            >;
        }
    };

    /**
     * @brief Dummy struct for is_not<Type> query
     * @tparam Type The original type, which will be stored after removing reference type
     */
    template<typename Type>
    struct is_not
    {
        using type = std::remove_reference_t<Type>;

        template<typename Arg>
        constexpr bool operator()(Arg&& arg) const
        {
            return 
            !std::is_same_v<
                std::conditional_t<
                    std::is_array_v<std::remove_reference_t<Arg>>,
                    std::decay_t<Arg>,
                    std::remove_reference_t<Arg>
                >,
                Type
            >;
        }
    };

    template<typename Type>
    struct is_actually
    {
        using type = std::add_const_t<std::remove_reference_t<Type>>;

        template<typename Arg/*, typename = std::enable_if_t<!std::is_pointer_v<std::remove_reference_t<Arg>>>*/>  //for reference
        bool operator()(Arg const& arg) const
        {
            return (dynamic_cast<type*>(&arg) != nullptr);
        }

        template<typename Pointer, typename = std::enable_if_t<std::is_pointer_v<std::remove_reference_t<Pointer>>>>  //for raw pointer
        bool operator()(Pointer const& arg) const
        {
            return (dynamic_cast<type*>(arg) != nullptr);
        }

        template<typename UniquePtrType>    //for std::unique_ptr
        bool operator()(std::unique_ptr<UniquePtrType> const& ptr) const
        {
            return (*this)(ptr.get());
        }

        template<typename SharedPtrType>
        bool operator()(std::shared_ptr<SharedPtrType> const& ptr) const
        {
            return (*this)(ptr.get());
        }
    };

    namespace detail
    {

        template <bool convertToFunction, typename ExprType, typename ReturnType>
        auto when_impl(ExprType&&, Else, ReturnType&& ReturnResult)
        {
            if constexpr (convertToFunction)
                return std::function{ ReturnResult };
            else
                return ReturnResult;
        }

        //.............................................................................................................................................v Number of arguments must be odd
        template <bool convertToFunction, typename ExprType, typename Case1Type, typename Return1Type, typename... Args, typename = std::enable_if_t<(3 + sizeof...(Args)) % 2 != 0>>
        auto when_impl(ExprType&& expr, Case1Type&& case1, Return1Type&& return1, Args &&... args)
        {
            if constexpr(convertToFunction)
            {
                if constexpr (sizeof...(Args) == 0)//end condition
                    return std::function{ [] {} };
                else
                {
                    if constexpr (std::is_invocable_r_v<bool, Case1Type, ExprType>)
                    {
                        if(case1(std::forward<ExprType>(expr)))
                            return std::function{ return1 };
                        else
                            return when_impl<convertToFunction>(std::forward<ExprType>(expr), std::forward<Args>(args)...);
                    }
                    else if constexpr (std::is_same_v<std::remove_reference_t<Case1Type>, bool>)
                    {
                        if (case1)
                            return std::function{ return1 };
                         else
                            return when_impl<convertToFunction>(std::forward<ExprType>(expr), std::forward<Args>(args)...);
                    }
                    else if constexpr (comparable<ExprType, Case1Type>::value)
                    {
                        if (expr == case1)
                            return std::function{ return1 };
                        else
                            return when_impl<convertToFunction>(std::forward<ExprType>(expr), std::forward<Args>(args)...);
                    }
                    return when_impl<convertToFunction>(std::forward<ExprType>(expr), std::forward<Args>(args)...);
                }
            }
            else
            {
                if constexpr (sizeof...(Args) == 0)//end condition
                    return std::remove_reference_t<Return1Type>{};
                else
                {
                    if constexpr (std::is_invocable_r_v<bool, Case1Type, ExprType>)
                    {
                        if (case1(std::forward<ExprType>(expr)))
                            return return1;
                        else
                            return when_impl<convertToFunction>(std::forward<ExprType>(expr), std::forward<Args>(args)...);
                    }
                    else if constexpr (std::is_same_v<std::remove_reference_t<Case1Type>, bool>)
                    {
                        if (case1)
                            return return1;
                        else
                            return when_impl<convertToFunction>(std::forward<ExprType>(expr), std::forward<Args>(args)...);
                    }
                    else if constexpr (comparable<ExprType, Case1Type>::value)
                    {
                        if (expr == case1)
                            return return1;
                        else
                            return when_impl<convertToFunction>(std::forward<ExprType>(expr), std::forward<Args>(args)...);
                    }
                    return when_impl<convertToFunction>(std::forward<ExprType>(expr), std::forward<Args>(args)...);
                }
            }
        }

        template <bool convertToFunction, typename Return1Type, typename... Args>
        auto when_impl(const char* Expr, const char* Case1, Return1Type&& return1, Args&&... args)
        {
            if constexpr(convertToFunction)
            {
                if constexpr (sizeof...(Args) == 0)//end condition
                    return std::function{ [] {} };
                else
                {
                    if(strcmp(Expr, Case1))
                        return std::function{ return1 };
                    else
                        return when_impl<convertToFunction>(Expr, std::forward<Args>(args)...);
                }
            }
            else
            {
                if constexpr (sizeof...(Args) == 0)//end condition
                    return std::remove_reference_t<Return1Type>{};
                else
                {
                    if (strcmp(Expr, Case1) == 0)
                        return return1;
                    else
                        return when_impl<convertToFunction>(Expr, std::forward<Args>(args)...);
                }
            }
        }


        /*Argument-less when_impl*/
        template<bool convertToFunction, typename ReturnType>
        auto when_impl(Else, ReturnType&& returnResult)
        {
            if constexpr (convertToFunction)
                return std::function{ returnResult };
            else
                return returnResult;
        }

        template<bool convertToFunction, typename Return1Type, typename ...Args, typename = std::enable_if_t<(2 + sizeof...(Args)) % 2 == 0>>
        auto when_impl(bool case1, Return1Type&& return1, Args&&...args)
        {
            if constexpr (convertToFunction)
            {
                if constexpr (sizeof...(Args) == 0) //end condition
                    return std::function{ [] {} };
                if (case1)
                    return std::function{ return1 };
                return when_impl<convertToFunction>(std::forward<Args>(args)...);
            }
            else
            {
                if constexpr (sizeof...(Args) == 0) //end condition
                    return std::remove_reference_t<Return1Type>{};
                else
                {
                    if (case1)
                        return return1;
                    return when_impl<convertToFunction>(std::forward<Args>(args)...);
                }
            }
        }

    }
    //namespace detail 

    /**
     * @brief Because lambdas are different types, so it tests if the return objects are different types to determine whether to return a std::function wrapping the lambdas
     */
    template <size_t I, typename Tuple>
    constexpr bool shouldConvert()
    {
        using tuple_type = Tuple;
        //Reference to different length arrays should not be considered as different type in this case, thus don't convert to function
        if constexpr (std::is_array_v<std::remove_reference_t<std::tuple_element_t<I, tuple_type>>>)
            return false;
        else
        {
            const bool current = std::is_same_v<
                std::remove_reference_t<std::tuple_element_t<I, tuple_type>>,
                std::remove_reference_t<std::tuple_element_t<I + 2, tuple_type>>
            >;
            if constexpr (I + 2 >= std::tuple_size_v<tuple_type> -1)
                return !current;
            else //forget this you get 3K errors
                return !(current && shouldConvert<I + 2, Tuple>());
        }
    }



    /**
     * @brief primary recursive template
    */
    template <typename ExprType, typename Case1Type, typename Return1Type, typename... Args, typename = std::enable_if_t<(3 + sizeof...(Args)) % 2 != 0>>
    auto when(ExprType&& expr, Case1Type&& case1, Return1Type&& return1, Args&&... args)
    {
        return detail::when_impl
            <
                shouldConvert
                <
                    2,
                    decltype(std::forward_as_tuple(expr, case1, return1, args...))
                >()
            >(std::forward<ExprType>(expr),
                std::forward<Case1Type>(case1),
                std::forward<Return1Type>(return1),
                std::forward<Args>(args)...);
    }

    /**
     * @brief primary recursive template for argument-less switches
     */
    template<typename Return1Type, typename ...Args, typename = std::enable_if_t<(2 + sizeof...(Args)) % 2 == 0>>
    auto when(bool case1, Return1Type&& return1, Args&&...args)
    {
        return detail::when_impl
            <
                shouldConvert
                <
                    1,
                    decltype(std::forward_as_tuple(case1, return1, args...))
                >()
            >(case1, std::forward<Return1Type>(return1), std::forward<Args>(args)...);
    }

#ifdef SugarPPNamespace
}
#endif