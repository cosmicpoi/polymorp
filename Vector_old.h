//--------------------------------------------------------------------------------
// Vector class
//
//   Support for building Vectors out of Units
//--------------------------------------------------------------------------------

#pragma once

#include <cmath>
#include "Unit.h"
#include "UnitMath.h"

//--------------------------------------------------------------------------------
// Vector class
//--------------------------------------------------------------------------------

/**
 * @brief Base class for Vector with units
 */
template <size_t N, IsUnit U>
class Vector
{
public:
    static constexpr size_t n = N;
    using unit = U;

    template <IsUnit UU>
    using VectorN = Vector<N, UU>;

    static constexpr void PrintInfo()
    {
        U::PrintInfo();
        std::cout << "[N=";
        std::cout << N;
        std::cout <<"];";
    }
    

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
        requires IsLikeUnit<U, RHS>
    {
        if constexpr (N == 2)
        {
            VectorN<AdjustExponent<U, RHS::exponent>> myV{v[0] * rhs, v[1] * rhs};
            return myV;
        }
        else
        {
            VectorN<AdjustExponent<U, RHS::exponent>> myV;
            return myV;
        }
    }

    /**
     * @brief Dot product
     */
    // template <IsUnit RHS>
    // VectorN<AdjustExponent<U, RHS::exponent>> Dot(RHS rhs)
    //     requires IsLikeUnit<U, RHS>
    // {
    //     VectorN<AdjustExponent<U, RHS::exponent>> myV;
    //     return myV;
    // }

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

/* ******* Is Vector ********* */

// Define the primary template
template <typename T>
struct IsVectorHelper : std::false_type
{
};

// Specialization for the `Unit` template
template <size_t N, IsUnit U>
    requires ValidVectorSize<N>
struct IsVectorHelper<Vector<N, U>> : std::true_type
{
};

/**
 * @brief Concept to check if a type is a `Vector` with `Units`
 */
template <typename T>
concept IsVector = IsVectorHelper<T>::value;

/* ******* Is Vector of size N ********* */
template <size_t N, typename T>
concept IsOfSize = requires {
    { T::n } -> std::same_as<const size_t &>;
} && (T::n == N);

template <size_t N, typename T>
concept IsVectorN = IsVector<T> && IsOfSize<N, T> && ValidVectorSize<N>;

/* ******* Is Vector with like units ********* */
