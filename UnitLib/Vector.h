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
template <size_t N, typename Type>
class Vector
{
public:
    static constexpr size_t n = N;
    using type = Type;

    template <typename T>
    using VectorN = Vector<N, T>;

    static constexpr void PrintInfo(std::ostream &os = std::cout)
    {
        if constexpr (GeneralScalar<Type>)
        {
            ScalarPrintInfo<Type>();
        }
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
        requires(sizeof...(Args) <= N && (std::constructible_from<Type, Args> && ...))
    explicit inline Vector(Args... initList)
        : _v{static_cast<Type>(initList)...}
    {
    }

    /** @brief Copy assignment */
    inline VectorN<Type> &operator=(const VectorN<Type> &rhs)
    {
        _v = rhs.GetData();
        return *this;
    }

    /** @brief Assign between compatible types */
    template <typename OtherType>
        requires(requires(Type a, OtherType b) {
            { a = b } -> std::convertible_to<Type>;
        } && !std::is_same_v<Type, OtherType>)
    inline VectorN<Type> &operator=(const VectorN<OtherType> &rhs)
    {
        ([this, &rhs]<std::size_t... Is>(std::index_sequence<Is...>)
         {
             ((_v[Is] = rhs[Is]), ...); //
         })(std::make_index_sequence<N>{});
        return *this;
    }

    /** Check is zero */
    inline bool IsZero() const
        requires(requires(Type a) { {a == 0} -> std::convertible_to<bool>; }) //
                || (requires(Type a) { {a.IsZero() } -> std::convertible_to<bool>; })
    {
        return ([this]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    if constexpr(requires(Type a) { a.IsZero(); })
                    {
                        return ((_v[Is].IsZero()) && ...); // Fold expression
                    }
                    else
                    {
                        static_assert(requires(Type a){ a==0; });
                        return ((_v[Is] == 0) && ...); // Fold expression
                    } })(std::make_index_sequence<N>{});
    }

    /** @brief Multiplication by scalar (unit or plain type) */
    template <typename RHS>
    inline VectorN<MultiplyType<Type, RHS>> operator*(const RHS &rhs) const
        requires CanMultiply<Type, RHS>
    {
        // Zero-overhead solution: generate the expression (_v[0] * rhs, _v[1] * rhs ...) at compile time
        return ([this, &rhs]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    return VectorN<MultiplyType<Type, RHS>>{(_v[Is] * rhs)...}; // Expands the expression for each index
                })(std::make_index_sequence<N>{});
    }

    /** @brief Division by scalar (unit or plain type) */
    template <typename RHS>
    inline VectorN<DivideType<Type, RHS>> operator/(const RHS &rhs) const
        requires CanDivide<Type, RHS>
    {
        return ([this, &rhs]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    return VectorN<DivideType<Type, RHS>>{(_v[Is] / rhs)...}; // Expands the expression for each index
                })(std::make_index_sequence<N>{});
    }

    /** @brief Addition with another vector */
    template <typename RHS>
    inline VectorN<AddType<Type, RHS>> operator+(const VectorN<RHS> &rhs) const
        requires CanAdd<Type, RHS>
    {
        return ([this, &rhs]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    return VectorN<AddType<Type, RHS>>{(_v[Is] + rhs[Is])...}; // Expands the expression for each index
                })(std::make_index_sequence<N>{});
    }

    /** @brief Subtraction with another vector */
    template <typename RHS>
    inline VectorN<SubtractType<Type, RHS>> operator-(const VectorN<RHS> &rhs) const
        requires CanSubtract<Type, RHS>
    {
        return ([this, &rhs]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    return VectorN<SubtractType<Type, RHS>>{(_v[Is] - rhs[Is])...}; // Expands the expression for each index
                })(std::make_index_sequence<N>{});
    }

    /** @brief Multiplication with another vector */
    template <typename RHS>
    inline VectorN<MultiplyType<Type, RHS>> operator*(const VectorN<RHS> &rhs) const
        requires CanMultiply<Type, RHS>
    {
        return ([this, &rhs]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    return VectorN<MultiplyType<Type, RHS>>{(_v[Is] * rhs[Is])...}; // Expands the expression for each index
                })(std::make_index_sequence<N>{});
    }

    /** @brief Division with another vector */
    template <typename RHS>
    inline VectorN<DivideType<Type, RHS>> operator/(const VectorN<RHS> &rhs) const
        requires CanDivide<Type, RHS>
    {
        return ([this, &rhs]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    return VectorN<DivideType<Type, RHS>>{(_v[Is] / rhs[Is])...}; // Expands the expression for each index
                })(std::make_index_sequence<N>{});
    }

    /** @brief Check for equality */
    template <typename RHS>
    inline bool operator==(const VectorN<RHS> &rhs) const
        requires requires(Type a, RHS b) { {a == b} -> std::convertible_to<bool>; }
    {
        return ([this, &rhs]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    return ((_v[Is] == rhs[Is]) && ...); // Expands the expression for each index
                })(std::make_index_sequence<N>{});
    }

    /* Compute-and-assign operators */
    template <typename T>
        requires requires(VectorN<Type> a, T b) { a + b; a = a + b; }
    inline VectorN<Type> &operator+=(const T &rhs)
    {
        // _v= VectorN<Type>{(*this) + rhs}._v;
        return *this;
    }

    /**
     * @brief Compute norm-squared of this vector
     */
    // inline ScalarExp<Type, std::ratio<2>> NormSq() const
    // {
    //     // Zero-overhead solution: generate the expression (_v[0] * rhs + _v[1] * rhs ...) at compile time
    //     return ([this]<std::size_t... Is>(std::index_sequence<Is...>)
    //             {
    //                 return ((_v[Is] * _v[Is]) + ...); // Fold expression
    //             })(std::make_index_sequence<N>{});
    // }

    /**
     * @brief Compute norm of this vector (in the underlying unit)
     */
    // inline Type Norm() const
    // {
    //     return unit_sqrt(NormSq());
    // }

    /**
     * @brief Compute norm of this vector as a double
     */
    // inline double Norm_d() const
    // {
    //     return std::sqrt((double)NormSq().value);
    // }

    /** @brief Dot product */
    // template <GeneralScalar RHS>
    // inline ScalarMult<Type, RHS> Dot(VectorN<RHS> rhs) const
    //     requires requires(Type a, RHS b) { {a * b} -> GeneralScalar; }
    // {
    //     // Zero-overhead solution: generate the expression (_v[0] * rhs, _v[1] * rhs ...) at compile time
    //     return ([this, &rhs]<std::size_t... Is>(std::index_sequence<Is...>)
    //             {
    //                 return ((_v[Is] * rhs[Is]) + ...); // Fold expression
    //             })(std::make_index_sequence<N>{});
    // }

    /** @brief Shorthand for dot product */
    // template <GeneralScalar RHS>
    // inline ScalarMult<Type, RHS> operator^(VectorN<RHS> rhs) const
    //     requires requires(Type a, RHS b) { {a * b} -> GeneralScalar; }
    // {
    //     return Dot(rhs);
    // }

    /** @brief Cross product */
    // template <GeneralScalar RHS>
    // inline VectorN<ScalarMult<Type, RHS>> Cross(VectorN<RHS> rhs) const
    //     requires requires(Type a, RHS b) { { a * b } -> GeneralScalar; } && (N == 3)
    // {
    //     return VectorN<ScalarMult<Type, RHS>>{
    //         _v[1] * rhs[2] - _v[2] * rhs[1],
    //         _v[2] * rhs[0] - _v[0] * rhs[2],
    //         _v[0] * rhs[1] - _v[1] * rhs[0]};
    // }

private:
    std::array<Type, N> _v;
};

// Some aliases

template <typename T>
using Vector2 = Vector<2, T>;

template <typename T>
using Vector3 = Vector<3, T>;

template <typename T>
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
template <size_t N, typename T>
struct IsVectorHelper<Vector<N, T>> : std::true_type
{
};

/**
 * @brief Concept to check if a type is a `Vector` with `Units`
 */
template <typename T>
concept IsVector = IsVectorHelper<T>::value;

// Operator overloads

// template <size_t N, GeneralScalar T>
// std::ostream &operator<<(std::ostream &os, Vector<N, T> val)
// {
//     return val.operator<<(os);
// }

/** @brief Left-multiply by scalar */
template <typename LHS, IsVector Vector_RHS>
    requires CanOpMultiply<Vector_RHS, LHS>
OpMultiplyType<Vector_RHS, LHS> operator*(LHS lhs, Vector_RHS rhs)
{
    return rhs * lhs;
}

/* ******* Is Vector of size N ********* */
// template <size_t N, typename T>
// concept IsOfSize = requires {
//     { T::n } -> std::same_as<const size_t &>;
//     requires(T::n == N);
// };

// template <size_t N, typename T>
// concept IsVectorN = IsVector<T> && IsOfSize<N, T>;

/* ******* Is Vector with like units ********* */
