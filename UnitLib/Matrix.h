//--------------------------------------------------------------------------------
// Matrix class
//
//   Support for building Matrices out of arbitrary values
//--------------------------------------------------------------------------------

#pragma once
#include "Vector.h"
#include <array>

/**
 * @brief Base class for an `M` row by `N` column matrix holding values of type `Type`
 */
template <size_t M, size_t N, typename Type>
    requires((M > 0) && (N > 0))
class Matrix : public Container
{
public:
    static constexpr size_t m = M;
    static constexpr size_t n = N;
    using type = Type;

    template <typename T>
    using MatrixMN = Matrix<M, N, T>;

    /**
     * Accessors
     */

    inline constexpr Array<Type, N> &operator[](size_t index)
    {
        return _v[index];
    }

    inline constexpr const Array<Type, N> &operator[](size_t index) const
    {
        return _v[index];
    }

    /**
     * @brief Directly access member `i`, `j`.
     * Slightly faster than `[i][j]` because the intermediate `[i]` is not constructed.
     */
    inline constexpr Type &At(size_t i, size_t j)
    {
        return _v[i][j];
    }

    inline constexpr const Type &At(size_t i, size_t j) const
    {
        return _v[i][j];
    }

    /**
     * @brief Safe and slightly slower versions of accessors that do range checking
     */
    inline constexpr Type &Get(size_t i, size_t j)
    {
        if (i >= M || j >= N)
        {
            throw std::out_of_range("Index out of range");
        }
        return _v[i][j];
    }

    inline constexpr const Type &Get(size_t i, size_t j) const
    {
        if (i >= M || j >= N)
        {
            throw std::out_of_range("Index out of range");
        }
        return _v[i][j];
    }

    /**
     * Special accessors
     */
    static inline constexpr MatrixMN<Type> Zero()
    {
        return MatrixMN<Type>{};
    }

    inline constexpr bool IsZero() const
        requires(requires(Type a) { {a == Type{0}} -> std::convertible_to<bool>; }) //
                || (requires(Type a) { {a.IsZero() } -> std::convertible_to<bool>; })
    {
        return ([&]<size_t... Idxs>(std::index_sequence<Idxs...>) constexpr
                {
                    if constexpr(requires(Type a) { a.IsZero(); })
                    {
                        return ((_v[get_row<M, N>(Idxs)][get_col<M, N>(Idxs)].IsZero()) && ...); // Fold expression
                    }
                    else
                    {
                        return ((_v[get_row<M, N>(Idxs)][get_col<M, N>(Idxs)] == 0) && ...); // Fold expression
                    } })(std::make_index_sequence<M * N>{});
    }

    static inline constexpr MatrixMN<Type> Identity()
        requires requires(Type a) {
            Type{1};
            Type{0};
        } && (M == N)
    {
        return ([&]<size_t... Idxs>(std::index_sequence<Idxs...>) constexpr
                {
                    return MatrixMN<Type>{
                        ((get_col<N, N>(Idxs) == get_row<N, N>(Idxs)) ? Type{1} : Type{0})...}; //
                })(std::make_index_sequence<N * N>{});
    }

    // IsZero

    /**
     * Constructors
     */

    /** @brief Default constructor - use underlying default constructors */
    explicit inline constexpr Matrix() : _v(create_default_matrix<Type, M, N>()) {}

    /** @brief One-dimensional list constructor (list of size M * N) */
    template <typename... Args>
        requires(
            (sizeof...(Args) <= M * N) &&
            ConvertibleOrConstructible<Type, Args...>)
    explicit inline constexpr Matrix(const Args &...initList) : Matrix()
    {
        ([&]<size_t... Idxs>(std::index_sequence<Idxs...>) constexpr
         { (
               (_v[get_row<M, N>(Idxs)][get_col<M, N>(Idxs)] = ConvertOrConstruct<Type, Args>(initList) //
                ),
               ...); })(std::make_index_sequence<sizeof...(Args)>{});
    }

    /** @brief Generalized initializer list constructor */
    template <typename OtherType>
        requires ConvertibleOrConstructible<Type, OtherType>
    inline constexpr Matrix(std::initializer_list<std::initializer_list<OtherType>> initList)
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

    /**
     * @brief Construct from compatible matrix
     * Note: need to check !is_same_v to avoid overriding copy constructor
     */
    template <typename OtherType>
        requires AssignableTo<Type, OtherType> && (!std::is_same_v<Type, OtherType>)
    inline constexpr Matrix(const MatrixMN<OtherType> &rhs)
    {
        ([&]<size_t... Idxs>(std::index_sequence<Idxs...>) constexpr
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
    inline constexpr MatrixMN<Type> &operator=(const MatrixMN<OtherType> &rhs)
    {
        ([&]<size_t... Idxs>(std::index_sequence<Idxs...>) constexpr
         {
             (
                 (_v[get_row<M, N>(Idxs)][get_col<M, N>(Idxs)] = rhs[get_row<M, N>(Idxs)][get_col<M, N>(Idxs)]), ...); //
         })(std::make_index_sequence<M * N>{});
        return *this;
    }

    /** @brief Equality operator */
    template <typename RHS>
        requires requires(Type a, RHS b) { {a == b} -> std::convertible_to<bool>; }
    inline constexpr bool operator==(const MatrixMN<RHS> &rhs) const
    {
        return ([&]<size_t... Idxs>(std::index_sequence<Idxs...>) constexpr -> bool
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
    inline constexpr MatrixMN<AddType<Type, RHS>> operator+(const MatrixMN<RHS> &rhs) const
    {
        return ([&]<size_t... Idxs>(std::index_sequence<Idxs...>) constexpr
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
    inline constexpr MatrixMN<SubtractType<Type, RHS>> operator-(const MatrixMN<RHS> &rhs) const
    {
        return ([&]<size_t... Idxs>(std::index_sequence<Idxs...>) constexpr
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
    inline constexpr MatrixMN<DivideType<Type, RHS>> operator/(const RHS &rhs) const
    {
        return ([&]<size_t... Idxs>(std::index_sequence<Idxs...>) constexpr
                {
                    return MatrixMN<DivideType<Type, RHS>>{
                        (_v[get_row<M, N>(Idxs)][get_col<M, N>(Idxs)] / rhs)... //
                    }; //
                })(std::make_index_sequence<M * N>{});
    }

    /** @brief Unary negation */
    inline constexpr MatrixMN<Type> operator-() const
    {
        return -1 * (*this);
    }

    /**
     * Arithmetic assignment
     */

    /** @brief Addition assignment */
    template <typename T>
        requires requires(MatrixMN<Type> a, T b) { a + b; a = a + b; }
    inline constexpr MatrixMN<Type> &operator+=(const T &rhs)
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
    inline constexpr MatrixMN<Type> &operator-=(const T &rhs)
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
    inline constexpr MatrixMN<Type> &operator*=(const T &rhs)
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
    inline constexpr MatrixMN<Type> &operator/=(const T &rhs)
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

    /**
     * Transpose
     */

    inline constexpr Matrix<N, M, Type> Transpose()
    {
        return ([&]<size_t... Idxs>(std::index_sequence<Idxs...>) constexpr
                {
                    return Matrix<N, M, Type>{(_v[get_col<N, M>(Idxs)][get_row<N, M>(Idxs)])...}; //
                })(std::make_index_sequence<N * M>{});
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
    return ([&]<size_t... Idxs>(std::index_sequence<Idxs...>) constexpr
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
    return ([&]<size_t... Idxs>(std::index_sequence<Idxs...>) constexpr
            {
                return Matrix<M, N, MultiplyType<LHS_Type, RHS_MatType>>{
                    (lhs * rhs_m[get_row<M, N>(Idxs)][get_col<M, N>(Idxs)])... //
                }; //
            })(std::make_index_sequence<M * N>{});
}

/**
 * @brief Matrix-vector multiplication
 */
template <typename LHS_MatType, size_t M, size_t N, typename RHS_VecType>
inline Vector<M, MultiplyType<LHS_MatType, RHS_VecType>> operator*(const Matrix<M, N, LHS_MatType> &lhs_m, const Vector<N, RHS_VecType> &rhs_v)
    requires(HasDotProduct<LHS_MatType, RHS_VecType>)
{
    auto getComponent = [&](size_t i) -> MultiplyType<LHS_MatType, RHS_VecType>
    {
        return ([&]<size_t... Idxs>(std::index_sequence<Idxs...>) constexpr
                {
                    return ((lhs_m[i][Idxs] * rhs_v[Idxs]) + ...); //
                })(std::make_index_sequence<N>{});
    };

    return ([&]<size_t... Idxs>(std::index_sequence<Idxs...>) constexpr
            { return Vector<M, MultiplyType<LHS_MatType, RHS_VecType>>{
                  (getComponent(Idxs))... //
              }; })(std::make_index_sequence<M>{});
}

/**
 * @brief Matrix multiplication
 */
template <typename LHS_MatType, size_t M, size_t N, typename RHS_MatType, size_t P>
inline Matrix<M, P, MultiplyType<LHS_MatType, RHS_MatType>> operator*(const Matrix<M, N, LHS_MatType> &lhs_m, const Matrix<N, P, RHS_MatType> &rhs_m)
    requires(HasDotProduct<LHS_MatType, RHS_MatType>)
{
    auto getCell = [&](size_t Row, size_t Col) constexpr -> MultiplyType<LHS_MatType, RHS_MatType>
    {
        return ([&]<size_t... Idxs>(std::index_sequence<Idxs...>)
                {
                    return ((lhs_m[Row][Idxs] * rhs_m[Idxs][Col]) + ...); //
                })(std::make_index_sequence<N>{});
    };

    return ([&]<size_t... Idxs>(std::index_sequence<Idxs...>) constexpr
            { return Matrix<M, P, MultiplyType<LHS_MatType, RHS_MatType>>{
                  (getCell(get_row<M, P>(Idxs), get_col<M, P>(Idxs)))... //
              }; })(std::make_index_sequence<M * P>{});
}

//--------------------------------------------------------------------------------
// Determinant
//--------------------------------------------------------------------------------

/**
 * @brief Helper for Determinant. Get the determinant of the matrix resulting from
 * using `Rows` for the rows and `Cols` for the cols, i.e. after having eliminated
 * `(N - sizeof(Rows))` rows/cols from the original matrix.
 * N - the size of the square (N x N) matrix
 * Rows - the current set of rows
 * Cols - the current set of cols
 * IterSize - the size of the current iteration (starting from N going down to 2)
 */
template <size_t IterSize, typename Type, size_t N, size_t... Rows, size_t... Cols>
    requires HasCrossProduct<Type, Type> &&          //
             CanExp<IterSize, Type> &&               //
             (sizeof...(Rows) == sizeof...(Cols)) && //
             (sizeof...(Rows) == IterSize) &&        //
             (IterSize <= N && IterSize >= 1)
constexpr inline ExpType<IterSize, Type> _Det(const Matrix<N, N, Type> &mat, std::index_sequence<Rows...>, std::index_sequence<Cols...>)
{
    using Rows_t = std::index_sequence<Rows...>;
    using Cols_t = std::index_sequence<Cols...>;

    if constexpr (IterSize == 1)
    {
        return mat[GetSequenceElement<0, Rows_t>::idx][GetSequenceElement<0, Cols_t>::idx];
    }
    else if constexpr (IterSize == 2)
    {
        constexpr size_t r0 = GetSequenceElement<0, Rows_t>::idx;
        constexpr size_t r1 = GetSequenceElement<1, Rows_t>::idx;
        constexpr size_t c0 = GetSequenceElement<0, Cols_t>::idx;
        constexpr size_t c1 = GetSequenceElement<1, Cols_t>::idx;
        return (mat[r0][c0] * mat[r1][c1]) - (mat[r0][c1] * mat[r1][c0]);
    }
    else
    {
        return ([&]<size_t... I>(std::index_sequence<I...>) constexpr
                { return ((
                              (I % 2 == 0 ? 1 : -1)                                                             //
                              * mat[GetSequenceElement<0, Rows_t>::idx][GetSequenceElement<I, Cols_t>::idx]     //
                              * _Det<IterSize - 1>(mat, RemoveAtIndex<0, Rows_t>{}, RemoveAtIndex<I, Cols_t>{}) //
                              ) +
                          ...); })(std::make_index_sequence<IterSize>{});
    }
}

/**
 * @brief Compute the determinant of a square matrix via laplace expansion
 */
template <typename Type, size_t N>
    requires HasCrossProduct<Type, Type> && CanExp<N, Type>
inline ExpType<N, Type> Det(const Matrix<N, N, Type> &mat)
{
    return _Det<N>(mat, std::make_index_sequence<N>{}, std::make_index_sequence<N>{});
}

//--------------------------------------------------------------------------------
// Inverse
//--------------------------------------------------------------------------------
template <size_t Row, size_t Col, typename Type, size_t N>
    requires HasCrossProduct<Type, Type> && //
             (Row <= N && Col <= N) &&      //
             CanExp<N - 1, Type> &&         //
             Negatable<Type> &&
             (N >= 2)
constexpr inline ExpType<N - 1, Type> GetCofactor(const Matrix<N, N, Type> &mat)
{
    return ConvertOrConstruct<ExpType<N - 1, Type>>(                   //
        (((Row + Col) % 2 == 0) ? 1 : -1) *                            //
        _Det<N - 1>(mat,                                               //
                    RemoveAtIndex<Row, std::make_index_sequence<N>>{}, //
                    RemoveAtIndex<Col, std::make_index_sequence<N>>{}) //
    );
}

/**
 * @brief Compute the inverse of a matrix. If it is not invertbile, return
 * a default-constructed empty matrix.
 */

template <typename Type, size_t N>
    requires requires(ExpType<N - 1, Type> en1, ExpType<N, Type> en) {
        requires HasCrossProduct<Type, Type>;
        requires CanInvert<Type>;
        requires CanExp<N, Type>;
        requires Negatable<Type>;
        // Inverse is computed as cofactor (exp n-1) / det (exp n)
        requires CanDivide<ExpType<N - 1, Type>, ExpType<N, Type>>;
        { en1 / en } -> std::constructible_from<InvertType<Type>>;
        { ExpType<N, Type>{0} }; // For checking determinant
    }
inline Matrix<N, N, InvertType<Type>> Inv(const Matrix<N, N, Type> &mat)
{
    // return Matrix<N, N, InvertType<Type>>{};
    ExpType<N, Type> d = Det(mat);
    if (d == ExpType<N, Type>{0})
    {
        return Matrix<N, N, InvertType<Type>>{};
    }
    else if constexpr (N == 1)
    {
        return Matrix<N, N, InvertType<Type>>{(mat[0][0] / mat[0][0]) / mat[0][0]};
    }
    else if constexpr (N == 2)
    {
        return Matrix<N, N, InvertType<Type>>{mat[1][1] / d, (-1 * mat[0][1]) / d, (-1 * mat[1][0]) / d, mat[0][0] / d};
    }
    else
    {
        return ([&]<size_t... Idxs>(std::index_sequence<Idxs...>) constexpr
                { return (Matrix<N, N, InvertType<Type>>{
                      (GetCofactor<get_col<N, N>(Idxs), get_row<N, N>(Idxs)>(mat) / d)... //
                  }); })(std::make_index_sequence<N * N>{});
    }
}

//--------------------------------------------------------------------------------
// Transformation matrices
//--------------------------------------------------------------------------------
Matrix2<double> Get2DRotationMatrix(double theta)
{
    return Matrix2<double>{
        {std::cos(theta), std::sin(theta)},
        {-std::sin(theta), std::cos(theta)}};
}

template <typename T>
    requires requires {
        T{0};
        T{1};
    }
Matrix2<T> Get2DScaleMatrix(T scaleX = T{1}, T scaleY = T{1})
{
    return Matrix2<T>{
        {scaleX, T{0}},
        {T{0}, scaleY}};
}