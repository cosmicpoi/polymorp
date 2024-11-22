#pragma once

#include <cmath>
#include "Unit.h"
#include "UnitMath.h"

//--------------------------------------------------------------------------------
// Vector class
//--------------------------------------------------------------------------------

template <size_t N>
concept ValidVectorSize = (N == 2 || N == 3 || N == 4);

/**
 * @brief Base class for Vector2 with units
 * For now, we only support Vectors where all components are the same type and exponent
 */
template <size_t N, IsUnit U>
    requires ValidVectorSize<N>
class Vector
{
public:
    void Print()
    {
        std::cout << "Vector print" << std::endl;
        std::cout << U::symbol << U::exponent;

        std::cout << " " << v[0].GetValue();
        std::cout << " " << v[1].GetValue();
        if constexpr (N > 2)
        {
            std::cout << " " << v[2].GetValue();
        }
        if constexpr (N > 3)
        {
            std::cout << " " << v[3].GetValue();
        }
        std::cout << std::endl;
    }

    template <IsUnit UU>
    using VectorN = Vector<N, UU>;

    /** Default constructors */
    inline Vector()
        requires(N == 2)
        : v{0, 0} {};

    inline Vector()
        requires(N == 3)
        : v{0, 0, 0} {};

    inline Vector()
        requires(N == 4)
        : v{0, 0, 0, 0} {};

    /** Constructors for passing in units of the same type */
    inline Vector(U c0, U c1)
        requires(N == 2)
        : v{c0, c1}
    {
    }

    inline Vector(U c0, U c1, U c2)
        requires(N == 3)
        : v{c0, c1, c2}
    {
    }

    inline Vector(U c0, U c1, U c2, U c3)
        requires(N == 4)
        : v{c0, c1, c2, c3}
    {
    }

    /**
     * @brief Compute norm-squared of this vector
     */
    DoubleExp<U> NormSq()
    {
        if constexpr (N == 2)
        {
            return v[0] * v[0] + v[1] * v[1];
        }
        else if constexpr (N == 3)
        {
            return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
        }
        else
        {
            static_assert(N == 4, "N is not valid");
            return v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
        }
    }

    /**
     * @brief Compute norm of this vector (in the underlying unit)
     */
    U Norm()
    {
        return squareRoot(NormSq());
    }

    /**
     * @brief Compute norm of this vector as a double
     */
    double Norm_d()
    {
        return std::sqrt((double)NormSq().GetValue());
    }

    /**
     * @brief Multiplication by scalar unit
     */
    template <IsUnit RHS>
    VectorN<AdjustExponent<U, RHS::exponent>> operator*(RHS rhs)
        requires IsSameUnit<U, RHS>
    {
        VectorN<AdjustExponent<U, RHS::exponent>> myV;
        return myV;
    }

    /**
     * @brief Dot product
     */
    template <IsUnit RHS>
    VectorN<AdjustExponent<U, RHS::exponent>> Dot(RHS rhs)
        requires IsSameUnit<U, RHS>
    {
        VectorN<AdjustExponent<U, RHS::exponent>> myV;
        return myV;
    }
    

private:
    U v[N] = {};
};

// Some aliases

template <IsUnit U>
using Vector2 = Vector<2, U>;

template <IsUnit U>
using Vector3 = Vector<3, U>;

template <IsUnit U>
using Vector4 = Vector<4, U>;

//--------------------------------------------------------------------------------
// Concepts
//--------------------------------------------------------------------------------