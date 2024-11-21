#pragma once

#include <cmath>
#include "Unit.h"
#include "UnitMath.h"

/**
 * @brief Base class for Vector2 with units
 * For now, we only support Vectors where all components are the same type and exponent
 */
template <IsUnit U>
class Vector2
{
public:
    void Print()
    {
        std::cout << "Vector2 print" << std::endl;
        std::cout << UnitTraits<U>::symbol << std::endl;
        x.Print();
        y.Print();
    }

    /**
     * @brief Constructor for passing in units of the same type
     */
    // template <char Component_Symbol, typename Component_Type, int Component_Exponent>
    inline Vector2(U cx, U cy)
        : x(cx), y(cy) {}

    /**
     * @brief Constructor for converting from literals
     */
    // inline Vector2(Type cx, Type cy) : x(cx), y(cy) {}

    Exp1<U> NormSq()
    {
        return x*x + y*y;
    }

    U Norm()
    {
        return squareRoot( NormSq() );
    }

    double Norm_d()
    {
        return std::sqrt((double) NormSq().GetValue());
    }

private:
    U x;
    U y;
};