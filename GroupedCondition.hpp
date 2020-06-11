#pragma once
#include <utility>

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
class NOT: public Operator 
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

    template<typename Expr, typename = std::enable_if_t<comparable<Expr, Case1>::value && comparable<Expr, Case2>::value>>
    bool operator==(Expr&& expr) const
    {
        return (caseExpr1 == expr) && (caseExpr2 == expr);
    }

    template<typename Expr, typename Case1, typename Case2, typename = std::enable_if_t<comparable<Expr, Case1>::value&& comparable<Expr, Case2>::value>>
    friend bool operator==(Expr&& expr, AND<Case1, Case2>&& andCase)
    {
        return andCase == expr;
    }
};

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

    template<typename Expr, typename Case1, typename Case2, typename = std::enable_if_t<comparable<Expr, Case1>::value&& comparable<Expr, Case2>::value>>
    friend bool operator==(Expr&& expr, OR<Case1, Case2>&& orCase)
    {
        return orCase == expr;
    }
};
