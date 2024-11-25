//--------------------------------------------------------------------------------
// Vector class
//
//   Support for building Vectors out of Units
//--------------------------------------------------------------------------------

#pragma once

#include <cmath>
#include <initializer_list>
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

    inline std::ostream &operator<<(std::ostream &os) const
    {
        Print(os);
        return os;
    }

    /** Accessors */
    inline U &x()
        requires(N >= 1)
    {
        return _v[0];
    };

    inline U &y()
        requires(N >= 2)
    {
        return _v[1];
    };

    inline U &z()
        requires(N >= 3)
    {
        return _v[2];
    };

    inline U &w()
        requires(N >= 4)
    {
        return _v[3];
    };

    inline U &operator[](std::size_t index)
    {
        if (index >= N)
        {
            throw std::out_of_range("Index out of range");
        }
        return _v[index];
    }

    inline const U &operator[](std::size_t index) const
    {
        if (index >= N)
        {
            throw std::out_of_range("Index out of range");
        }
        return _v[index];
    }

    /** Default constructors */

    inline Vector()
        : _v(create_array<U, N>())
    {
    }

    /** @brief Variadic constructor - can take any types convertible to U.  */
    template <typename... Args>
    inline Vector(Args... initList)
        requires((std::is_convertible_v<Args, U> && ...) && sizeof...(initList) <= N)
        : _v{initList...}
    {
    }

    /**
     * @brief Compute norm-squared of this vector
     */
    inline UnitExp<U, std::ratio<2>> NormSq()
    {
        // Zero-overhead solution: generate the expression (_v[0] * rhs + _v[1] * rhs ...) at compile time
        return ([this]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    return ((_v[Is] * _v[Is]) + ...); // Fold expression
                })(std::make_index_sequence<N>{});
    }

    /**
     * @brief Compute norm of this vector (in the underlying unit)
     */
    inline U Norm()
    {
        return unit_sqrt(NormSq());
    }

    /**
     * @brief Compute norm of this vector as a double
     */
    inline double Norm_d()
    {
        return std::sqrt((double)NormSq().value);
    }

    /**
     * @brief Multiplication by scalar unit
     */
    template <IsUnit RHS>
    inline VectorN<UnitMult<U, RHS>> operator*(RHS rhs)
    {
        using MultVectorN = VectorN<UnitMult<U, RHS>>;
        // Zero-overhead solution: generate the expression (_v[0] * rhs, _v[1] * rhs ...) at compile time
        return ([this, &rhs]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    return MultVectorN{(_v[Is] * rhs)...}; // Expands the expression for each index
                })(std::make_index_sequence<N>{});
    }

    /** @brief Dot product */
    template <IsUnit RHS>
    inline UnitMult<U, RHS> Dot(VectorN<RHS> rhs)
    {
        // Zero-overhead solution: generate the expression (_v[0] * rhs, _v[1] * rhs ...) at compile time
        return ([this, &rhs]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    return ((_v[Is] * rhs[Is]) + ...); // Fold expression
                })(std::make_index_sequence<N>{});
    }

    /** @brief Shorthand for dot product */
    template <IsUnit RHS>
    inline UnitMult<U, RHS> operator^(VectorN<RHS> rhs)
    {
        return Dot(rhs);
    }

private:
    std::array<U, N> _v;
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
    requires(T::n == N);
};

template <size_t N, typename T>
concept IsVectorN = IsVector<T> && IsOfSize<N, T>;

/* ******* Is Vector with like units ********* */
