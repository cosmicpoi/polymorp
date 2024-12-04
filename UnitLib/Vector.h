//--------------------------------------------------------------------------------
// Vector class
//
//   Support for building Vectors out of arbitrary values
//--------------------------------------------------------------------------------

#pragma once

#include "TypeUtils.h"
#include <initializer_list>

//--------------------------------------------------------------------------------
// Vector class
//--------------------------------------------------------------------------------

/**
 * @brief HasDotProduct concept. Takes two types and check if they have the
 * algebraic properties needed for dot product.
 * This is surprisingly helpful because we can use the same logic to check for
 * NormSquared (dot product with itself), matrix multiplication, etc
 */

template <typename L, typename R>
concept HasDotProduct = requires(L l, R r) {
    { l *r };
    { (l * r) + (l * r) } -> std::same_as<decltype(l * r)>;
    { (l * r) + (l * r) } -> std::convertible_to<MultiplyType<L, R>>;
};

/**
 * @brief HasCrossProduct concept. Takes two types and check if they have the
 * algebraic properties needed for cross product.
 * Surprisingly helpful because if a type supports cross product, it also
 * supports Matrix determinant
 */
template <typename L, typename R>
concept HasCrossProduct = HasDotProduct<L, R> &&
                          (requires(L l, R r) {
                              { (l * r) - (l * r) } -> std::same_as<decltype(l * r)>;
                          });

/**
 * @brief Base class for Vector with units. Provides optimized versions for N=2, 3, 4, corresponding to `Vector2`, `Vector3`, and `Vector4`, but any length is actually supported.
 */
template <size_t N, typename Type>
class Vector : public Container
{
public:
    static constexpr size_t n = N;
    using type = Type;

    template <typename T>
    using VectorN = Vector<N, T>;

    /** Accessors */
    inline const Array<Type, N> GetData() const
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

    inline Type &operator[](size_t index)
    {
        return _v[index];
    }

    inline const Type &operator[](size_t index) const
    {
        return _v[index];
    }

    /**
     * Safe versions with range checks (slightly slower than access)
     */

    inline Type &Get(size_t index)
    {
        if (index >= N)
        {
            throw std::out_of_range("Index out of range");
        }
        return _v[index];
    }

    inline const Type &Get(size_t index) const
    {
        if (index >= N)
        {
            throw std::out_of_range("Index out of range");
        }
        return _v[index];
    }

    /**
     * Constructors
     */

    /** @brief Default constructor - use default constructor of underlying type */
    explicit inline Vector()
        : _v(create_default_array<Type, N>())
    {
    }

    /** @brief Construct list of from convertible/constructible values */
    template <typename... Args>
        requires(
            (sizeof...(Args) <= N) &&
            ConvertibleOrConstructible<Type, Args...>)
    inline Vector(const Args &...initList)
        : _v{ConvertOrConstruct<Type, Args>(initList)...}
    {
    }

    /** @brief Construct from list of assignable values */
    template <typename... Args>
        requires(
            (sizeof...(Args) <= N) &&
            (!ConvertibleOrConstructible<Type, Args...>) &&
            AssignableTo<Type, Args...>)
    inline Vector(const Args &...initList)
    {
        ([&]<size_t... Is>(std::index_sequence<Is...>) constexpr
         {
             ((_v[Is] = initList), ...); //
         })(std::make_index_sequence<N>{});
    }

    /**
     * @brief Construct from vector of convertible/constructible type
     * Note: need to check !is_same_v to avoid overriding copy constructor
     */
    template <typename OtherType>
        requires ConvertibleOrConstructible<Type, OtherType> &&
                 (!std::is_same_v<Type, OtherType>)
    inline Vector(const VectorN<OtherType> &other)
        : _v{([&other]<size_t... Is>(std::index_sequence<Is...>) constexpr
              {
                  return std::array<Type, N>{
                      ConvertOrConstruct<Type, OtherType>(other[Is])...}; //
              })(std::make_index_sequence<N>{})}
    {
    }

    /**
     * @brief Construct from vector of assignable type
     * Note: need to check !is_same_v to avoid overriding copy constructor
     */
    template <typename OtherType>
        requires((!ConvertibleOrConstructible<Type, OtherType>) &&
                 AssignableTo<Type, OtherType> &&
                 (!std::is_same_v<Type, OtherType>))
    explicit inline Vector(const VectorN<OtherType> &other)
    {
        ([&]<size_t... Is>(std::index_sequence<Is...>) constexpr
         {
             ((_v[Is] = other[Is]), ...); //
         })(std::make_index_sequence<N>{});
    }

    /** @brief Construct from rvalue of convertible/constructible type */
    template <typename OtherType>
        requires ConvertibleOrConstructible<Type, OtherType> &&
                 (!std::is_same_v<Type, OtherType>)
    explicit inline Vector(const VectorN<OtherType> &&other) : Vector(other)
    {
    }

    /** @brief Construct from rvalue of assignable type */
    template <typename OtherType>
        requires((!ConvertibleOrConstructible<Type, OtherType>) &&
                 AssignableTo<Type, OtherType> &&
                 (!std::is_same_v<Type, OtherType>))
    explicit inline Vector(const VectorN<OtherType> &&other) : Vector(other)
    {
    }

    /**
     * Assignment
     */

    /** @brief Assign between compatible types */
    template <typename OtherType>
        requires(requires(Type a, OtherType b) { a = b; })
    inline VectorN<Type> &operator=(const VectorN<OtherType> &rhs)
    {
        ([&]<size_t... Is>(std::index_sequence<Is...>) constexpr
         {
             ((_v[Is] = rhs[Is]), ...); //
         })(std::make_index_sequence<N>{});
        return *this;
    }

    /** Check is zero */
    inline bool IsZero() const
        requires(requires(Type a) { {a == Type{0}} -> std::convertible_to<bool>; }) //
                || (requires(Type a) { {a.IsZero() } -> std::convertible_to<bool>; })
    {
        return ([&]<size_t... Is>(std::index_sequence<Is...>) constexpr
                {
                    if constexpr(requires(Type a) { a.IsZero(); })
                    {
                        return ((_v[Is].IsZero()) && ...); // Fold expression
                    }
                    else
                    {
                        return ((_v[Is] == 0) && ...); // Fold expression
                    } })(std::make_index_sequence<N>{});
    }

    /** @brief Unary negation */
    inline VectorN<Type> operator-() const
    {
        return -1 * (*this);
    }

    /** @brief Addition with another vector */
    template <typename RHS>
        requires CanAdd<Type, RHS>
    inline VectorN<AddType<Type, RHS>> operator+(const VectorN<RHS> &rhs) const
    {
        return ([&]<size_t... Is>(std::index_sequence<Is...>) constexpr
                {
                    return VectorN<AddType<Type, RHS>>{(_v[Is] + rhs[Is])...}; // Expands the expression for each index
                })(std::make_index_sequence<N>{});
    }

    /** @brief Subtraction with another vector */
    template <typename RHS>
        requires CanSubtract<Type, RHS>
    inline VectorN<SubtractType<Type, RHS>> operator-(const VectorN<RHS> &rhs) const
    {
        return ([&]<size_t... Is>(std::index_sequence<Is...>) constexpr
                {
                    return VectorN<SubtractType<Type, RHS>>{(_v[Is] - rhs[Is])...}; // Expands the expression for each index
                })(std::make_index_sequence<N>{});
    }

    /** @brief Multiplication with another vector */
    template <typename RHS>
        requires CanMultiply<Type, RHS>
    inline VectorN<MultiplyType<Type, RHS>> operator*(const VectorN<RHS> &rhs) const
    {
        return ([&]<size_t... Is>(std::index_sequence<Is...>) constexpr
                {
                    return VectorN<MultiplyType<Type, RHS>>{(_v[Is] * rhs[Is])...}; // Expands the expression for each index
                })(std::make_index_sequence<N>{});
    }

    /** @brief Division with another vector */
    template <typename RHS>
        requires CanDivide<Type, RHS>
    inline VectorN<DivideType<Type, RHS>> operator/(const VectorN<RHS> &rhs) const
    {
        return ([&]<size_t... Is>(std::index_sequence<Is...>) constexpr
                {
                    return VectorN<DivideType<Type, RHS>>{(_v[Is] / rhs[Is])...}; // Expands the expression for each index
                })(std::make_index_sequence<N>{});
    }

    /** @brief Check for equality */
    template <typename RHS>
        requires requires(Type a, RHS b) { {a == b} -> std::convertible_to<bool>; }
    inline bool operator==(const VectorN<RHS> &rhs) const
    {
        return ([&]<size_t... Is>(std::index_sequence<Is...>) constexpr
                {
                    return ((_v[Is] == rhs[Is]) && ...); // Expands the expression for each index
                })(std::make_index_sequence<N>{});
    }

    /* Compute-and-assign operators */

    /** Addition assignment operator */
    template <typename T>
        requires requires(VectorN<Type> a, T b) { a + b; a = a + b; }
    inline VectorN<Type> &operator+=(const T &rhs) // const T&
    {
        if constexpr (std::is_same_v<T, Type>)
        {
            _v = ((*this) + rhs)._v;
            return *this;
        }
        else
        {
            _v = VectorN<Type>{(*this) + rhs}._v;
            return *this;
        }
    }

    /** Subtraction assignment operator */
    template <typename T>
        requires requires(VectorN<Type> a, T b) { a + b; a = a - b; }
    inline VectorN<Type> &operator-=(const T &rhs) // const T&
    {
        if constexpr (std::is_same_v<T, Type>)
        {
            _v = ((*this) - rhs)._v;
            return *this;
        }
        else
        {
            _v = VectorN<Type>{(*this) - rhs}._v;
            return *this;
        }
    }

    /** Multiplication assignment operator */
    template <typename T>
        requires requires(VectorN<Type> a, T b) { a * b; a = a * b; }
    inline VectorN<Type> &operator*=(const T &rhs)
    {
        if constexpr (std::is_same_v<T, Type>)
        {
            _v = ((*this) * rhs)._v;
            return *this;
        }
        else
        {
            _v = VectorN<Type>{(*this) * rhs}._v;
            return *this;
        }
    }

    /** Division assignment operator */
    template <typename T>
        requires requires(VectorN<Type> a, T b) { a / b; a = a / b; }
    inline VectorN<Type> &operator/=(const T &rhs)
    {
        if constexpr (std::is_same_v<T, Type>)
        {
            _v = ((*this) / rhs)._v;
            return *this;
        }
        else
        {
            _v = VectorN<Type>{(*this) / rhs}._v;
            return *this;
        }
    }

    /** @brief Dot product */
    template <typename RHS_Type>
        requires HasDotProduct<Type, RHS_Type>
    inline MultiplyType<Type, RHS_Type> Dot(const VectorN<RHS_Type> &rhs) const
    {
        auto res = ([&]<size_t... Is>(std::index_sequence<Is...>) constexpr
                    {
                        return ((_v[Is] * rhs[Is]) + ...); //
                    })(std::make_index_sequence<N>{});
        if constexpr (std::is_same_v<MultiplyType<Type, RHS_Type>, decltype(res)>)
        {
            return res;
        }
        else
        {
            return MultiplyType<Type, RHS_Type>{res};
        }
    }

    /** @brief Cross product */
    template <typename RHS_Type>
        requires(HasCrossProduct<Type, RHS_Type> && (N == 3))
    inline VectorN<MultiplyType<Type, RHS_Type>> Cross(const VectorN<RHS_Type> &rhs) const
    {
        using ResType = decltype((std::declval<Type>() * std::declval<RHS_Type>()) + (std::declval<Type>() * std::declval<RHS_Type>()));

        return VectorN<ResType>{_v[1] * rhs[2] - _v[2] * rhs[1],  //
                                _v[2] * rhs[0] - _v[0] * rhs[2],  //
                                _v[0] * rhs[1] - _v[1] * rhs[0]}; //
    }

private:
    Array<Type, N> _v;
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

/**
 * IsVector
 */

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

/** @brief Concept to check if a type is a `Vector` */
template <typename T>
concept IsVector = IsVectorHelper<T>::value;

//------------------------------------------------------------------------------
// Operator overloads
//------------------------------------------------------------------------------

/** @brief Right-multiply by scalar */
template <typename LHS_VecType, size_t N, typename RHS_Type>
    requires((!IsContainer<RHS_Type>) &&
             CanMultiply<LHS_VecType, RHS_Type>)
inline Vector<N, MultiplyType<LHS_VecType, RHS_Type>> operator*(const Vector<N, LHS_VecType> &lhs_v, const RHS_Type &rhs)
{
    return ([&]<size_t... Is>(std::index_sequence<Is...>) constexpr
            {
                return Vector<N, MultiplyType<LHS_VecType, RHS_Type>>{(lhs_v[Is] * rhs)...}; // Expands the expression for each index
            })(std::make_index_sequence<N>{});
}

/** @brief Left-multiply by scalar */
template <typename RHS_VecType, size_t N, typename LHS_Type>
    requires((!IsContainer<LHS_Type>) &&
             CanMultiply<LHS_Type, RHS_VecType>)
inline Vector<N, MultiplyType<LHS_Type, RHS_VecType>> operator*(const LHS_Type &lhs, const Vector<N, RHS_VecType> &rhs_v)
{
    return ([&]<size_t... Is>(std::index_sequence<Is...>) constexpr
            {
                return Vector<N, MultiplyType<LHS_Type, RHS_VecType>>{(lhs * rhs_v[Is])...}; // Expands the expression for each index
            })(std::make_index_sequence<N>{});
}

/** @brief Division by scalar (unit or plain type) */
template <typename LHS_VecType, size_t N, typename RHS_Type>
    requires((!IsContainer<RHS_Type>) &&
             CanDivide<LHS_VecType, RHS_Type>)
inline Vector<N, DivideType<LHS_VecType, RHS_Type>> operator/(const Vector<N, LHS_VecType> &lhs_v, const RHS_Type &rhs)
{
    return ([&]<size_t... Is>(std::index_sequence<Is...>) constexpr
            {
                return Vector<N, DivideType<LHS_VecType, RHS_Type>>{(lhs_v[Is] / rhs)...}; // Expands the expression for each index
            })(std::make_index_sequence<N>{});
}

// Vector math functions

/** @brief Helper concept to check if a type supports norm squared */
template <typename Type>
concept VectorHasNormSquared = HasDotProduct<Type, Type>;

/** @brief Compute norm-squared of a vector */
template <typename Type, size_t N>
inline MultiplyType<Type, Type> NormSquared(const Vector<N, Type> &v)
    requires VectorHasNormSquared<Type>

{
    // Theoretically we could implement this as v.dot(v) but this is less overhead
    return ([&]<size_t... Is>(std::index_sequence<Is...>) constexpr
            {
                return MultiplyType<Type, Type>{((v[Is] * v[Is]) + ...)}; // Fold expression
            })(std::make_index_sequence<N>{});
}