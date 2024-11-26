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
#include "Scalar.h"

template <typename A>
concept VectorLike = requires(A a, A b) {
    typename A::type;
    { A::n } -> std::same_as<const size_t &>;
    requires GeneralScalar<typename A::type>;
    a.Dot(b);
};

template <typename From, typename To>
concept VectorIsConvertible_ = requires {
    requires VectorLike<From>;
    requires VectorLike<To>;
    typename From::type;
    typename To::type;
    requires GeneralScalar<typename From::type>;
    requires GeneralScalar<typename To::type>;
    requires ScalarIsConvertible<typename From::type, typename To::type>;
};

//--------------------------------------------------------------------------------
// Vector class
//--------------------------------------------------------------------------------

// Helper function to create a compile-time zero-initialized array of type T and size N
template <typename T, size_t N>
constexpr std::array<T, N> create_array()
{
    return ([]<std::size_t... Is>(std::index_sequence<Is...>) -> std::array<T, N>
            {
                return {((void)Is, T{0})...}; //
            })(std::make_index_sequence<N>{});
}

/**
 * @brief Base class for Vector with units. Provides optimized versions for N=2, 3, 4, corresponding to `Vector2`, `Vector3`, and `Vector4`, but any length is actually supported.
 */
template <size_t N, GeneralScalar Type>
class Vector
{
public:
    static constexpr size_t n = N;
    using type = Type;

    template <GeneralScalar T>
    using VectorN = Vector<N, T>;

    static constexpr void PrintInfo(std::ostream &os = std::cout)
    {
        ScalarPrintInfo<Type>();
        os << "[N=" << N << "];";
    }

    void Print(std::ostream &os = std::cout) const
    {
        os << "v{ ";
        PrintInfo(os);
        os << " (";
        for (uint i = 0; i < N; i++)
        {
            os << ScalarGetValue<Type>(_v[i]);

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
    inline const std::array<Type, N> GetData() const
    {
        return _v;
    }

    const inline Type &x() const
        requires(N >= 1)
    {
        return _v[0];
    };

    inline Type &x()
        requires(N >= 1)
    {
        return _v[0];
    };

    const inline Type &y() const
        requires(N >= 2)
    {
        return _v[1];
    };

    inline Type &y()
        requires(N >= 2)
    {
        return _v[1];
    };

    const inline Type &z() const
        requires(N >= 3)
    {
        return _v[2];
    };

    inline Type &z()
        requires(N >= 3)
    {
        return _v[2];
    };

    const inline Type &w() const
        requires(N >= 4)
    {
        return _v[3];
    };

    inline Type &w()
        requires(N >= 4)
    {
        return _v[3];
    };

    inline Type &operator[](std::size_t index)
    {
        if (index >= N)
        {
            throw std::out_of_range("Index out of range");
        }
        return _v[index];
    }

    inline const Type &operator[](std::size_t index) const
    {
        if (index >= N)
        {
            throw std::out_of_range("Index out of range");
        }
        return _v[index];
    }

    /** Default constructors */

    explicit inline Vector()
        : _v(create_array<Type, N>())
    {
    }

    /** @brief Variadic constructor - can take any types convertible to U.  */
    template <typename... Args>
        requires (sizeof...(Args) <= N && (std::constructible_from<Type, Args> && ...))
    explicit inline Vector(Args... initList)
        : _v{static_cast<Type>(initList)...}
    {
    }

    /** @brief Copy assignment */
    template <VectorIsConvertible_<VectorN<Type>> RHS>
    inline VectorN<Type> &operator=(RHS rhs)
    {
        if constexpr (std::is_same_v<Type, typename RHS::type>)
        {
            _v = rhs.GetData();
            return *this;
        }
        else
        {
            ([this, &rhs]<std::size_t... Is>(std::index_sequence<Is...>)
             {
                 ((_v[Is] = rhs[Is]), ...); //
             })(std::make_index_sequence<N>{});
            return *this;
        }
    }

    /** Check is zero */
    inline bool IsZero() const
    {
        return ([this]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    if constexpr(IsUnit<Type>)
                    {
                        return ((_v[Is].IsZero()) && ...); // Fold expression
                    }
                    else
                    {
                        return ((_v[Is] == 0) && ...); // Fold expression
                    } })(std::make_index_sequence<N>{});
    }

    /** @brief Multiplication by scalar (unit or plain type) */
    template <typename RHS>
    inline VectorN<ScalarMult<Type, RHS>> operator*(RHS rhs) const
        requires requires(Type a, RHS b) { {a * b} -> GeneralScalar; }
    {
        // Zero-overhead solution: generate the expression (_v[0] * rhs, _v[1] * rhs ...) at compile time
        return ([this, &rhs]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    return VectorN<ScalarMult<Type, RHS>>{(_v[Is] * rhs)...}; // Expands the expression for each index
                })(std::make_index_sequence<N>{});
    }

    /** @brief Division by scalar (unit or plain type) */
    // template <typename RHS>
    // inline VectorN<ScalarMult<Type, RHS>> operator*(RHS rhs) const
    //     requires requires(Type a, RHS b) { {a * b} -> GeneralScalar; }
    // {
    //     // Zero-overhead solution: generate the expression (_v[0] * rhs, _v[1] * rhs ...) at compile time
    //     return ([this, &rhs]<std::size_t... Is>(std::index_sequence<Is...>)
    //             {
    //                 return VectorN<ScalarMult<Type, RHS>>{(_v[Is] * rhs)...}; // Expands the expression for each index
    //             })(std::make_index_sequence<N>{});
    // }

    /** @brief Addition with another vector */
    template <typename RHS>
    inline VectorN<ScalarAdd<Type, RHS>> operator+(VectorN<RHS> rhs) const
        requires requires(Type a, RHS b) { {a + b} -> GeneralScalar; }
    {
        // Zero-overhead solution: generate the expression (_v[0] * rhs, _v[1] * rhs ...) at compile time
        return ([this, &rhs]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    return VectorN<ScalarAdd<Type, RHS>>{(_v[Is] + rhs[Is])...}; // Expands the expression for each index
                })(std::make_index_sequence<N>{});
    }

    /** @brief Subtraction with another vector */
    template <typename RHS>
    inline VectorN<ScalarSubtract<Type, RHS>> operator-(VectorN<RHS> rhs) const
        requires requires(Type a, RHS b) { {a - b} -> GeneralScalar; }
    {
        // Zero-overhead solution: generate the expression (_v[0] * rhs, _v[1] * rhs ...) at compile time
        return ([this, &rhs]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    return VectorN<ScalarSubtract<Type, RHS>>{(_v[Is] - rhs[Is])...}; // Expands the expression for each index
                })(std::make_index_sequence<N>{});
    }

    /** @brief Check for equality */
    template <typename RHS>
    inline bool operator==(VectorN<RHS> rhs) const
        requires requires(Type a, RHS b) { {a == b} -> std::convertible_to<bool>; }
    {
        return ([this, &rhs]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    return ((_v[Is] == rhs[Is]) && ...); // Expands the expression for each index
                })(std::make_index_sequence<N>{});
    }

    /**
     * @brief Compute norm-squared of this vector
     */
    inline ScalarExp<Type, std::ratio<2>> NormSq() const
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
    inline Type Norm() const
    {
        return unit_sqrt(NormSq());
    }

    /**
     * @brief Compute norm of this vector as a double
     */
    inline double Norm_d() const
    {
        return std::sqrt((double)NormSq().value);
    }

    /** @brief Dot product */
    template <GeneralScalar RHS>
    inline ScalarMult<Type, RHS> Dot(VectorN<RHS> rhs) const
        requires requires(Type a, RHS b) { {a * b} -> GeneralScalar; }
    {
        // Zero-overhead solution: generate the expression (_v[0] * rhs, _v[1] * rhs ...) at compile time
        return ([this, &rhs]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    return ((_v[Is] * rhs[Is]) + ...); // Fold expression
                })(std::make_index_sequence<N>{});
    }

    /** @brief Shorthand for dot product */
    template <GeneralScalar RHS>
    inline ScalarMult<Type, RHS> operator^(VectorN<RHS> rhs) const
        requires requires(Type a, RHS b) { {a * b} -> GeneralScalar; }
    {
        return Dot(rhs);
    }

    /** @brief Cross product */
    template <GeneralScalar RHS>
    inline VectorN<ScalarMult<Type, RHS>> Cross(VectorN<RHS> rhs) const
        requires requires(Type a, RHS b) { { a * b } -> GeneralScalar; } && (N == 3)
    {
        return VectorN<ScalarMult<Type, RHS>>{
            _v[1] * rhs[2] - _v[2] * rhs[1],
            _v[2] * rhs[0] - _v[0] * rhs[2],
            _v[0] * rhs[1] - _v[1] * rhs[0]};
    }

    /** @brief Shorthand for cross product */
    template <GeneralScalar RHS>
    inline VectorN<ScalarMult<Type, RHS>> operator%(VectorN<RHS> rhs) const
        requires requires(Type a, RHS b) { { a * b } -> GeneralScalar; } && (N == 3)
    {
        return Cross(rhs);
    }

private:
    std::array<Type, N> _v;
};

// Some aliases

template <GeneralScalar T>
using Vector2 = Vector<2, T>;

template <GeneralScalar T>
using Vector3 = Vector<3, T>;

template <GeneralScalar T>
using Vector4 = Vector<4, T>;

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
template <size_t N, GeneralScalar T>
struct IsVectorHelper<Vector<N, T>> : std::true_type
{
};

/**
 * @brief Concept to check if a type is a `Vector` with `Units`
 */
template <typename T>
concept IsVector = IsVectorHelper<T>::value;

// Operator overloads

template <size_t N, GeneralScalar T>
std::ostream &operator<<(std::ostream &os, Vector<N, T> val)
{
    return val.operator<<(os);
}

/** @brief Left-multiply by scalar */
template <typename LHS, IsVector Vector_RHS>
    requires requires(LHS a, Vector_RHS b) { b.operator*(a); }
auto operator*(LHS lhs, Vector_RHS rhs)
{
    return rhs * lhs;
}

/* ******* Is Vector of size N ********* */
template <size_t N, typename T>
concept IsOfSize = requires {
    { T::n } -> std::same_as<const size_t &>;
    requires(T::n == N);
};

template <size_t N, typename T>
concept IsVectorN = IsVector<T> && IsOfSize<N, T>;

/* ******* Is Vector with like units ********* */
