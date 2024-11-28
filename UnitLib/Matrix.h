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

    template <typename T>
    using MatrixMN = Matrix<M, N, T>;

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

    /**
     * Constructors
     */

    /** @brief Default constructor - use underlying default constructors */
    explicit inline Matrix() : _v(create_default_matrix<Type, M, N>()) {}

    /** @brief Generalized initializer list constructor */
    template <typename OtherType>
        requires ConvertibleOrAssignableOrConstructible<Type, OtherType>
    inline Matrix(std::initializer_list<std::initializer_list<OtherType>> initList)
        : Matrix()
    {
        if (initList.size() > M)
        {
            throw std::invalid_argument("Initializer list size exceeds the maximum allowed size.");
        }
        uint i = 0;
        for (const auto &it : initList)
        {
            if (it.size() > N)
            {
                throw std::invalid_argument("Initializer list size exceeds the maximum allowed size.");
            }
            uint j = 0;
            for (const auto &jt : it)
            {
                ConvertOrAssignOrConstruct<Type, OtherType>(_v[i][j++], jt);
            }
            i++;
        }
    }

    /** @brief  */

    /**
     *
     */

    /**
     * @brief Assign between compatible types
     */
    template <typename OtherType>
        requires(requires(Type a, OtherType b) { a = b; })
    inline MatrixMN<Type> &operator=(const MatrixMN<OtherType> &rhs)
    {
        ([this, &rhs]<std::size_t... Idxs>(std::index_sequence<Idxs...>)
         {
             (
                 (_v[get_row<M, N>(Idxs)][get_col<M, N>(Idxs)] = rhs[get_row<M, N>(Idxs)][get_col<M, N>(Idxs)]), ...); //
         })(std::make_index_sequence<M * N>{});
        return *this;
    }

    /**
     * Equality
     */
    template <typename RHS>
        requires requires(Type a, RHS b) { {a == b} -> std::convertible_to<bool>; }
    inline bool operator==(const MatrixMN<RHS> &rhs) const
    {
        return ([this, &rhs]<std::size_t... Idxs>(std::index_sequence<Idxs...>)
                {
                    return ((_v[get_row<M, N>(Idxs)][get_col<M, N>(Idxs)] == rhs[get_row<M, N>(Idxs)][get_col<M, N>(Idxs)]) && ...); //
                })(std::make_index_sequence<M * N>{});
    }

private:
    Array2D<Type, M, N> _v;
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