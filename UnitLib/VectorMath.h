#include "UnitMath.h"

/** @brief Helper concept to check if a type supports norm */
template <typename Type>
concept VectorHasNorm = requires(Type a) {
    requires VectorHasNormSquared<Type>;
    requires HasSquareRoot<MultiplyType<Type, Type>>;
};

/** @brief Compute norm of a vector */
template <typename Type, size_t N>
inline Type Norm(const Vector<N, Type> &v)
    requires VectorHasNorm<Type>
{
    return unit_sqrt(NormSquared(v));
}

/** @brief Compute norm of this vector as a double */
template <typename Type, size_t N>
inline double Norm_d(const Vector<N, Type> &v)
    requires IsUnit<Type> || std::is_convertible_v<Type, double>
{
    if constexpr (IsUnit<Type>)
    {
        return std::sqrt(static_cast<double>(NormSquared(v).GetRealValue()));
    }
    else
    {
        static_assert(std::is_convertible_v<Type, double>);
        return std::sqrt(static_cast<double>(NormSquared(v)));
    }
}