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

    /** @brief One-dimensional list constructor (list of size M * N) */
    template <typename... Args>
        requires(
            (sizeof...(Args) <= M * N) &&
            ConvertibleOrConstructible<Type, Args...>)
    explicit inline Matrix(const Args &...initList) : Matrix()
    {
        ([&]<std::size_t... Idxs>(std::index_sequence<Idxs...>)
         { (
               (_v[get_row<M, N>(Idxs)][get_col<M, N>(Idxs)] = ConvertOrConstruct<Type, Args>(initList) //
                ),
               ...); })(std::make_index_sequence<sizeof...(Args)>{});
    }

    /** @brief Generalized initializer list constructor */
    template <typename OtherType>
        requires ConvertibleOrConstructible<Type, OtherType>
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
                _v[i][j++] = ConvertOrConstruct<Type, OtherType>(jt);
            }
            i++;
        }
    }

    /** @brief Construct from compatible array */
    template <typename OtherType>
        requires(requires(Type a, OtherType b) { a = b; })
    inline Matrix(const MatrixMN<OtherType> &rhs)
    {
        ([this, &rhs]<std::size_t... Idxs>(std::index_sequence<Idxs...>)
         {
             (
                 (_v[get_row<M, N>(Idxs)][get_col<M, N>(Idxs)] = rhs[get_row<M, N>(Idxs)][get_col<M, N>(Idxs)]), ...); //
         })(std::make_index_sequence<M * N>{});
    }

    /**
     * Equality and assignment
     */

    /** @brief Assign between compatible types */
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

    /** @brief Equality operator */
    template <typename RHS>
        requires requires(Type a, RHS b) { {a == b} -> std::convertible_to<bool>; }
    inline bool operator==(const MatrixMN<RHS> &rhs) const
    {
        return ([this, &rhs]<std::size_t... Idxs>(std::index_sequence<Idxs...>)
                {
                    return ((_v[get_row<M, N>(Idxs)][get_col<M, N>(Idxs)] == rhs[get_row<M, N>(Idxs)][get_col<M, N>(Idxs)]) && ...); //
                })(std::make_index_sequence<M * N>{});
    }

    /**
     * Arithmetic
     */

    /** @brief Addition with matrix of same size */
    template <typename RHS>
        requires CanAdd<Type, RHS>
    inline MatrixMN<AddType<Type, RHS>> operator+(const MatrixMN<RHS> &rhs) const
    {
        return ([&]<std::size_t... Idxs>(std::index_sequence<Idxs...>)
                {
                    return MatrixMN<AddType<Type, RHS>>{
                        (
                            _v[get_row<M, N>(Idxs)][get_col<M, N>(Idxs)] +      //
                            rhs[get_row<M, N>(Idxs)][get_col<M, N>(Idxs)])...}; //
                })(std::make_index_sequence<M * N>{});
    }

    /** @brief Subtraction with matrix of same size */
    template <typename RHS>
        requires CanSubtract<Type, RHS>
    inline MatrixMN<SubtractType<Type, RHS>> operator-(const MatrixMN<RHS> &rhs) const
    {
        return ([&]<std::size_t... Idxs>(std::index_sequence<Idxs...>)
                {
                    return MatrixMN<AddType<Type, RHS>>{
                        (
                            _v[get_row<M, N>(Idxs)][get_col<M, N>(Idxs)] - //
                            rhs[get_row<M, N>(Idxs)][get_col<M, N>(Idxs)]  //
                            )...                                           //
                    }; //
                })(std::make_index_sequence<M * N>{});
    }

    /** @brief Division by scalar (unit or plain type) */
    template <typename RHS>
        requires CanDivide<Type, RHS>
    inline MatrixMN<DivideType<Type, RHS>> operator/(const RHS &rhs) const
    {
        return ([&]<std::size_t... Idxs>(std::index_sequence<Idxs...>)
                {
                    return MatrixMN<DivideType<Type, RHS>>{
                        (_v[get_row<M, N>(Idxs)][get_col<M, N>(Idxs)] / rhs)... //
                    }; //
                })(std::make_index_sequence<M * N>{});
    }

    /** @brief Unary negation */
    inline MatrixMN<Type> operator-() const
    {
        return -1 * (*this);
    }

    /**
     * Arithmetic assignment
     */

    /** @brief Addition assignment */
    template <typename T>
        requires requires(MatrixMN<Type> a, T b) { a + b; a = a + b; }
    inline MatrixMN<Type> &operator+=(const T &rhs)
    {
        if constexpr (std::is_same_v<T, Type>)
        {
            _v = ((*this) + rhs)._v;
            return *this;
        }
        else
        {
            _v = MatrixMN<Type>{(*this) + rhs}._v;
            return *this;
        }
    }

    /** @brief Subtraction assignment */
    template <typename T>
        requires requires(MatrixMN<Type> a, T b) { a - b; a = a - b; }
    inline MatrixMN<Type> &operator-=(const T &rhs)
    {
        if constexpr (std::is_same_v<T, Type>)
        {
            _v = ((*this) - rhs)._v;
            return *this;
        }
        else
        {
            _v = MatrixMN<Type>{(*this) - rhs}._v;
            return *this;
        }
    }

    /** @brief Multiplication assignment */
    template <typename T>
        requires requires(MatrixMN<Type> a, T b) { a * b; a = a * b; }
    inline MatrixMN<Type> &operator*=(const T &rhs)
    {
        if constexpr (std::is_same_v<T, Type>)
        {
            _v = ((*this) * rhs)._v;
            return *this;
        }
        else
        {
            _v = MatrixMN<Type>{(*this) * rhs}._v;
            return *this;
        }
    }

    /** @brief Division assignment */
    template <typename T>
        requires requires(MatrixMN<Type> a, T b) { a / b; a = a / b; }
    inline MatrixMN<Type> &operator/=(const T &rhs)
    {
        if constexpr (std::is_same_v<T, Type>)
        {
            _v = ((*this) / rhs)._v;
            return *this;
        }
        else
        {
            _v = MatrixMN<Type>{(*this) / rhs}._v;
            return *this;
        }
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

/**
 * IsMatrix concept
 */

template <typename T>
struct IsMatrix_ : std::false_type
{
};

template <size_t M, size_t N, typename T>
struct IsMatrix_<Matrix<M, N, T>> : std::true_type
{
};

/** @brief Concept to check if a type is a `Matrix` */
template <typename T>
concept IsMatrix = IsMatrix_<T>::value;

/**
 * IsScalar concept
 *   See note for NotVector
 */

//--------------------------------------------------------------------------------
// Operator overloads
//--------------------------------------------------------------------------------

/**
 * Scalar multiplication
 */

/** @brief Right-multiply by scalar */
template <typename LHS_MatType, size_t M, size_t N, typename RHS_Type>
    requires((!IsMatrix<RHS_Type>) && (!IsVector<RHS_Type>) && CanMultiply<LHS_MatType, RHS_Type>)
inline Matrix<M, N, MultiplyType<LHS_MatType, RHS_Type>> operator*(const Matrix<M, N, LHS_MatType> &lhs_m, const RHS_Type &rhs)
{
    return ([&]<std::size_t... Idxs>(std::index_sequence<Idxs...>)
            {
                return Matrix<M, N, MultiplyType<LHS_MatType, RHS_Type>>{
                    (lhs_m[get_row<M, N>(Idxs)][get_col<M, N>(Idxs)] * rhs)... //
                }; //
            })(std::make_index_sequence<M * N>{});
}

/** @brief Left-multiply by scalar */
template <typename RHS_MatType, size_t M, size_t N, typename LHS_Type>
    requires((!IsMatrix<LHS_Type>) && (!IsVector<LHS_Type>) && CanMultiply<LHS_Type, RHS_MatType>)
inline Matrix<M, N, MultiplyType<LHS_Type, RHS_MatType>> operator*(const LHS_Type &lhs, const Matrix<M, N, RHS_MatType> &rhs_m)
{
    return ([&]<std::size_t... Idxs>(std::index_sequence<Idxs...>)
            {
                return Matrix<M, N, MultiplyType<LHS_Type, RHS_MatType>>{
                    (lhs * rhs_m[get_row<M, N>(Idxs)][get_col<M, N>(Idxs)])... //
                }; //
            })(std::make_index_sequence<M * N>{});
}

/**
 * @brief Matrix-vector multiplication
 */

// template <typename LHS_MatType, size_t LHS_Rows, size_t RHS_Cols, typename LHS_MatType, size_t LHS_Rows, size_t LHS_Cols>
// inline Matrix<RHS_Rows, LHS_Cols, MultiplyType<LHS_MatType, RHS_MatType>> operator*(const Matrix<LHS_Rows, LHS_Cols, LHS_MatType> &lhs_m, const Matrix<RHS_Rows, RHS_Cols, RHS_MatType> &rhs_m)
//     requires((LHS_Cols == RHS_Rows) && HasDotProduct<LHS_MatType, RHS_MatType>)
// {
//     return Matrix<RHS_Rows, LHS_Cols, MultiplyType<LHS_MatType, RHS_MatType>>{};
// }

/**
 * @brief Matrix multiplication
 */
template <typename RHS_MatType, size_t M, size_t N, typename LHS_MatType, size_t P>
inline Matrix<M, P, MultiplyType<LHS_MatType, RHS_MatType>> operator*(const Matrix<M, N, LHS_MatType> &lhs_m, const Matrix<N, P, RHS_MatType> &rhs_m)
    requires(HasDotProduct<LHS_MatType, RHS_MatType>)
{
    auto getCell = [&](size_t Row, size_t Col) -> MultiplyType<LHS_MatType, RHS_MatType>
    {
        return ([&]<size_t... Idxs>(std::index_sequence<Idxs...>)
                {
                    return ((rhs_m[Row][Idxs] * lhs_m[Idxs][Col]) + ...); //
                })(std::make_index_sequence<N>{});
    };

    return ([&]<size_t... Idxs>(std::index_sequence<Idxs...>)
            {
                return Matrix<M, P, MultiplyType<LHS_MatType, RHS_MatType>>{
                    (getCell(get_row<M, N>(Idxs), get_col<M, N>(Idxs)))... //
                };
            })(std::make_index_sequence<M * N>{});
}