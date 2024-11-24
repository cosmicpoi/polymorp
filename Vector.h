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
 * @brief Base class for Vector with units. Provides optimized versions for N=2, 3, 4, corresponding to `Vector2`, `Vector3`, and `Vector4`, but any length is actually supported.
 */
template <size_t N, IsUnit U>
class Vector
{
public:
    static constexpr size_t n = N;
    using unit = U;

    template <IsUnit UU>
    using VectorN = Vector<N, UU>;

    static constexpr void PrintInfo(std::ostream &os = std::cout)
    {
        U::PrintInfo(os);
        os << "[N=" << N << "];";
    }

    void Print(std::ostream &os = std::cout) const
    {
        os << "v{ ";
        PrintInfo(os);
        os << " (";
        for (uint i = 0; i < N; i++)
        {
            os << _v[i].value;
            if (i != N - 1)
            {
                os << " ";
            }
        }
        os << ") }";
    }

    std::ostream &operator<<(std::ostream &os) const
    {
        Print(os);
        return os;
    }

    /** Accessors */
    U &x()
        requires(N >= 1)
    {
        return _v[0];
    };

    U &y()
        requires(N >= 2)
    {
        return _v[1];
    };

    U &z()
        requires(N >= 3)
    {
        return _v[2];
    };

    U &w()
        requires(N >= 4)
    {
        return _v[3];
    };

    U &operator[](std::size_t index)
    {
        if (index >= N)
        {
            throw std::out_of_range("Index out of range");
        }
        return _v[index];
    }

    const U &operator[](std::size_t index) const
    {
        if (index >= N)
        {
            throw std::out_of_range("Index out of range");
        }
        return _v[index];
    }

    /** Default constructors */
    inline Vector()
        requires(N == 2)
        : _v{0, 0} {};

    inline Vector()
        requires(N == 3)
        : _v{0, 0, 0} {};

    inline Vector()
        requires(N == 4)
        : _v{0, 0, 0, 0} {};

    inline Vector()
    {
        std::fill(_v, _v + N, 0);
    }

    /** Constructors for passing in units of the same type */
    inline Vector(U c0, U c1)
        requires(N == 2)
        : _v{c0, c1}
    {
    }

    inline Vector(U c0, U c1, U c2)
        requires(N == 3)
        : _v{c0, c1, c2}
    {
    }

    inline Vector(U c0, U c1, U c2, U c3)
        requires(N == 4)
        : _v{c0, c1, c2, c3}
    {
    }

    // TODO implement arbitrary size

    /**
     * @brief Compute norm-squared of this vector
     */
    inline ExpUnit_T<U, std::ratio<2>> NormSq()
    {
        using U_2 = ExpUnit_T<U, std::ratio<2>>;

        if constexpr (N == 2)
        {
            return _v[0] * _v[0] + _v[1] * _v[1];
        }
        else if constexpr (N == 3)
        {
            return _v[0] * _v[0] + _v[1] * _v[1] + _v[2] * _v[2];
        }
        else if constexpr (N == 4)
        {
            return _v[0] * _v[0] + _v[1] * _v[1] + _v[2] * _v[2] + _v[3] * _v[3];
        }
        else
        {
            U_2 sum = 0;
            for (uint i = 0; i < N; i++)
            {
                sum += _v[i] * _v[i];
            }
            return sum;
        }

        // TODO implement other sizes
    }

    /**
     * @brief Compute norm of this vector (in the underlying unit)
     */
    U Norm()
    {
        return unit_sqrt(NormSq());
    }

    /**
     * @brief Compute norm of this vector as a double
     */
    double Norm_d()
    {
        return std::sqrt((double)NormSq().value);
    }

    /**
     * @brief Multiplication by scalar unit
     */
    template <IsUnit RHS>
    VectorN<UnitMult<U, RHS>> operator*(RHS rhs)
    {
        using MultVectorN = VectorN<UnitMult<U, RHS>>;
        if constexpr (N == 2)
        {
            MultVectorN myV{_v[0] * rhs, _v[1] * rhs};
            return myV;
        }
        else
        {
            MultVectorN myV;
            return myV;
        }
    }

    /**
     * @brief Dot product
     */
    template <IsUnit RHS>
    UnitMult<U, RHS> Dot(VectorN<RHS> rhs)
    {
        if constexpr (N == 2)
        {
            return _v[0] * rhs[0] + _v[1] * rhs[1];
        }
        else if constexpr (N == 3)
        {
            return _v[0] * rhs[0] + _v[1] * rhs[1] + _v[2] * rhs[2];
            // return MultVectorN{};
        }
        else if constexpr (N == 4)
        {
            return _v[0] * rhs[0] + _v[1] * rhs[1] + _v[2] * rhs[2] + _v[3] * rhs[3];
        }
        else
        {
            UnitMult<U, RHS> sum = 0;
            for (size_t i =0; i < N; i++)
            {
                sum += _v[i] * rhs[i];
            }
            return sum;
        }
    }

private:
    U _v[N] = {};
};

template <size_t N, IsUnit U>
std::ostream &operator<<(std::ostream &os, Vector<N, U> val)
{
    return val.operator<<(os);
}

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
concept IsVectorN = IsVector<T> && IsOfSize<N, T>;

/* ******* Is Vector with like units ********* */
