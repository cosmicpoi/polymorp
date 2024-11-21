#pragma once

#include "Unit.h"

/**
 * @brief Base class for Vector2 with units
 * For now, we only support Vectors where all components are the same type and exponent
 */
template <typename U>
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

    // U 

private:
    U x;
    U y;
};