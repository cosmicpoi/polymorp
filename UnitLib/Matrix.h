//--------------------------------------------------------------------------------
// Matrix class
//
//   Support for building Matrices out of arbitrary values
//--------------------------------------------------------------------------------

#pragma once
#include "Vector.h"

/**
 * @brief Base class for an `M` row by `N` column matrix holding values of type `Type`
 */
template <size_t M, size_t N, typename Type>
class Matrix
{
public:
    static constexpr size_t m = M;
    static constexpr size_t n = N;
    using type = Type;

    static constexpr bool IsSquare = (M == N);

    /**
     * Constructors
     */

    explicit inline Matrix() : _v(create_zero_matrix<Type, M, N>()) {}

    /**
     * Accessors
     */

    inline Array<Type, N> &operator[](std::size_t index)
    {
        return _v[index];
    }

    inline const Array<Type, N> &operator[](std::size_t index) const
    {
        return _v[index];
    }

    /**
     * @brief Directly access member `i`, `j`.
     * Slightly faster than `[i][j]` because the intermediate `[i]` is not constructed.
     */
    inline Type &At(size_t i, size_t j)
    {
        return _v[i][j];
    }

    inline const Type &At(size_t i, size_t j) const
    {
        return _v[i][j];
    }

    /**
     * @brief Safe and slightly slower versions of accessors that do range checking
     */
    inline Type &Get(size_t i, size_t j)
    {
        if (i >= M || j >= N)
        {
            throw std::out_of_range("Index out of range");
        }
        return _v[i][j];
    }

    inline const Type &Get(size_t i, size_t j) const
    {
        if (i >= M || j >= N)
        {
            throw std::out_of_range("Index out of range");
        }
        return _v[i][j];
    }

private:
    std::array<std::array<Type, N>, N> _v;
};

// Some aliases

template <typename T>
using Matrix2 = Matrix<2, 2, T>;

template <typename T>
using Matrix3 = Matrix<3, 3, T>;

template <typename T>
using Matrix4 = Matrix<4, 4, T>;

//--------------------------------------------------------------------------------
// Concepts
//--------------------------------------------------------------------------------

// Define the primary template
template <typename T>
struct IsMatrix_ : std::false_type
{
};

// Specialization for the `Unit` template
template <size_t M, size_t N, typename T>
struct IsMatrix_<Matrix<M, N, T>> : std::true_type
{
};

/**
 * @brief Concept to check if a type is a `Matrix`
 */
template <typename T>
concept IsMatrix = IsMatrix_<T>::value;