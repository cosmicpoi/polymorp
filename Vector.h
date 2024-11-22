#pragma once

#include <cmath>
#include "Unit.h"
#include "UnitMath.h"

template <size_t N>
concept ValidVectorSize = (N == 2 || N == 3 || N == 4);

/**
 * @brief Base class for Vector2 with units
 * For now, we only support Vectors where all components are the same type and exponent
 */
template <IsUnit U, size_t N>
    requires ValidVectorSize<N>
class Vector
{
public:
    void Print()
    {
        std::cout << "Vector print" << std::endl;
        std::cout << UnitTraits<U>::symbol << std::endl;
        v[0].Print();
        v[1].Print();
        if constexpr (N > 2)
        {
            v[2].Print();
        }
        if constexpr (N > 3)
        {
            v[3].Print();
        }
    }

    /**
     * @brief Constructor for passing in units of the same type
     */
    inline Vector(U c0, U c1)
        : v{c0, c1}
    {
    }

    inline Vector(U c0, U c1, U c2) requires (N > 2)
        : v{c0, c1, c2}
    {
    }

    inline Vector(U c0, U c1, U c2, U c3) requires (N == 4)
        : v{c0, c1, c2, c3}
    {
    }

    /**
     * @brief Constructor for converting from literals
     */
    // inline Vector2(Type cx, Type cy) : x(cx), y(cy) {}

    Exp1<U> NormSq()
    {
        return v[0] * v[0] + v[1] * v[1];
    }

    U Norm()
    {
        return squareRoot(NormSq());
    }

    double Norm_d()
    {
        return std::sqrt((double)NormSq().GetValue());
    }

private:
    std::array<U, N> v;
};

template <IsUnit U>
using Vector2 = Vector<U, 2>;

template <IsUnit U>
using Vector3 = Vector<U, 3>;

template <IsUnit U>
using Vector4 = Vector<U, 4>;