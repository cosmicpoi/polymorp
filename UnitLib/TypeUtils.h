#pragma once

#include <concepts>
#include <iostream>

template <typename T>
struct ExtractParameterPack_
{
};

template <template <typename...> class Wrapper, typename... Ts>
struct ExtractParameterPack_<Wrapper<Ts...>>
{
    using type = std::tuple<Ts...>; // Store types in a tuple for further use
};

template <typename T>
using ExtractParameterPack = typename ExtractParameterPack_<T>::type;

/**
 * Check if a type is primitive
 */
template <typename T>
concept IsPrimitive =
    std::is_floating_point_v<T> ||
    std::is_integral_v<T> ||
    std::is_same_v<T, void> ||
    std::is_convertible_v<T, float> ||
    std::is_convertible_v<T, double> ||
    std::is_convertible_v<T, int>;

/**
 * Maybe construct
 */
// We have a lot of convertible types; this function allows us to conditionally run a constructor
// template <typename To, typename From>
// inline constexpr To

/**
 * Array, folding, pack expansion
 */

template <typename T, size_t N>
using Array = std::array<T, N>;

template <typename T, size_t M, size_t N>
using Array2D = std::array<std::array<T, N>, M>;

// Helper function to create a compile-time zero-initialized array of type T and size N
template <typename T, size_t N>
constexpr Array<T, N> create_zero_array()
{
    return ([]<std::size_t... Is>(std::index_sequence<Is...>) -> Array<T, N>
            {
                return {((void)Is, T{0})...}; //
            })(std::make_index_sequence<N>{});
}

template <typename T, size_t M, size_t N>
constexpr Array2D<T, M, N> create_zero_matrix()
{
    return ([]<std::size_t... Js>(std::index_sequence<Js...>) -> Array2D<T, M, N>
            {
                return {((void)Js, create_zero_array<T, N>())...}; //
            })(std::make_index_sequence<M>{});
}

template <size_t M, size_t N>
constexpr size_t get_row(size_t idx)
{
    return idx / M;
}

template <size_t M, size_t N>
constexpr size_t get_col(size_t idx)
{
    return idx % M;
}

template <size_t M, size_t N>
constexpr void iterate_over()
{
    ([]<std::size_t... Idxs>(std::index_sequence<Idxs...>)
     {
         ((std::cout << get_row<M, N>(Idxs) << "," << get_col<M, N>(Idxs) << " "), ...); //
     })(std::make_index_sequence<M * N>{});
}

/**
 * UniversalFalse - a fallback for templates whose values may not always exist,
 * e.g. the type of MultType<A, A> if A cannot be multiplied
 *
 * In particular, consider something like this:
 *      template <typename Type, size_t N>
 *      inline SquareRootType<T> NormSquared(const Vector<N, Type> &v)
 *          requires HasNormSquared<Vector<N, Type>> && HasSquareRoot<decltype(NormSquared(std::declval<Vector<N, Type>>()))>
 *      {
 *          return  {some complex expression}
 *      }
 *
 * The use of UniversalFalse as a fallback from `SquareRootType` ensures this function never fails.
 */

/** @brief UniversalFalse - a falsy value that's constructible from anything */
struct UniversalFalse
{
    template <typename... Args>
    UniversalFalse(Args &...)
    {
    }

    template <typename... Args>
    UniversalFalse(Args &&...)
    {
    }
    explicit operator bool() const noexcept
    {
        return false;
    }
};

// ------------------------------------------------------
// General utils for type arithmetic
// ------------------------------------------------------

/** Multiply */
template <typename, typename>
struct MultiplyType_
{
    using type = UniversalFalse;
};

template <typename A, typename B>
    requires(requires(A a, B b) { {a * b}; })
struct MultiplyType_<A, B>
{
    using type = decltype(std::declval<A>() * std::declval<B>());
};

template <typename A, typename B>
using MultiplyType = typename MultiplyType_<A, B>::type;

template <typename A, typename B>
concept CanMultiply = requires(A a, B b) {
    { a *b } -> std::constructible_from<MultiplyType<A, B>>;
};

// Operator* version
template <typename A, typename B>
    requires requires(A a, B b) { { a.operator*(b) }; }
using OpMultiplyType = decltype(std::declval<A>().operator*(std::declval<B>()));

template <typename A, typename B>
concept CanOpMultiply = requires(A a, B b) { { a.operator*(b) } -> std::same_as<OpMultiplyType<A, B>>; };

/** Divide */
template <typename, typename>
struct DivideType_
{
    using type = UniversalFalse;
};

template <typename A, typename B>
    requires(requires(A a, B b) { { a / b }; })
struct DivideType_<A, B>
{
    using type = decltype(std::declval<A>() / std::declval<B>());
};

template <typename A, typename B>
using DivideType = typename DivideType_<A, B>::type;

template <typename A, typename B>
concept CanDivide = requires(A a, B b) {
    { a / b } -> std::constructible_from<DivideType<A, B>>;
};

// Operator/ version
template <typename A, typename B>
    requires requires(A a, B b) { { a.operator/(b) }; }
using OpDivideType = decltype(std::declval<A>().operator/(std::declval<B>()));

template <typename A, typename B>
concept CanOpDivide = requires(A a, B b) { { a.operator/(b) } -> std::same_as<OpDivideType<A, B>>; };

// Add
/** Add */
template <typename, typename>
struct AddType_
{
    using type = UniversalFalse;
};

template <typename A, typename B>
    requires(requires(A a, B b) { { a + b }; })
struct AddType_<A, B>
{
    using type = decltype(std::declval<A>() + std::declval<B>());
};

template <typename A, typename B>
using AddType = typename AddType_<A, B>::type;

template <typename A, typename B>
concept CanAdd = requires(A a, B b) {
    { a + b } -> std::constructible_from<AddType<A, B>>;
};

// Operator+ version
template <typename A, typename B>
    requires requires(A a, B b) { { a.operator+(b) }; }
using OpAddType = decltype(std::declval<A>().operator+(std::declval<B>()));

template <typename A, typename B>
concept CanOpAdd = requires(A a, B b) { { a.operator+(b) } -> std::same_as<OpAddType<A, B>>; };

/** Subtract */
template <typename, typename>
struct SubtractType_
{
    using type = UniversalFalse;
};

template <typename A, typename B>
    requires(requires(A a, B b) { { a - b }; })
struct SubtractType_<A, B>
{
    using type = decltype(std::declval<A>() - std::declval<B>());
};

template <typename A, typename B>
using SubtractType = typename SubtractType_<A, B>::type;

template <typename A, typename B>
concept CanSubtract = requires(A a, B b) {
    { a - b } -> std::constructible_from<SubtractType<A, B>>;
};

// Operator- version
template <typename A, typename B>
    requires requires(A a, B b) { { a.operator-(b) }; }
using OpSubtractType = decltype(std::declval<A>().operator-(std::declval<B>()));

template <typename A, typename B>
concept CanOpSubtract = requires(A a, B b) { { a.operator-(b) } -> std::same_as<OpSubtractType<A, B>>; };

// ------------------------------------------------------

template <typename A, typename B>
concept CanRatioAdd = requires(std::common_type_t<A, B> x, intmax_t r) {
    { x / r } -> std::same_as<std::common_type_t<A, B>>;
    { x + x } -> std::same_as<std::common_type_t<A, B>>;
};

template <typename A, typename B>
concept CanRatioSubtract = requires(std::common_type_t<A, B> x, intmax_t r) {
    { x / r } -> std::same_as<std::common_type_t<A, B>>;
    { x - x } -> std::same_as<std::common_type_t<A, B>>;
};