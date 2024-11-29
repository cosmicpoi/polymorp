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
 * Array, folding, pack expansion
 */

template <typename T, size_t N>
using Array = std::array<T, N>;

template <typename T, size_t M, size_t N>
using Array2D = std::array<std::array<T, N>, M>;

// Helper function to create a compile-time zero-initialized array of type T and size N
template <typename T, size_t N>
constexpr Array<T, N> create_default_array()
{
    return ([]<size_t... Is>(std::index_sequence<Is...>) -> Array<T, N>
            {
                return {((void)Is, T{})...}; //
            })(std::make_index_sequence<N>{});
}

template <typename T, size_t M, size_t N>
constexpr Array2D<T, M, N> create_default_matrix()
{
    return ([]<size_t... Js>(std::index_sequence<Js...>) -> Array2D<T, M, N>
            {
                return {((void)Js, create_default_array<T, N>())...}; //
            })(std::make_index_sequence<M>{});
}

// Helpers for 2D constexpr initializtion
template <size_t M, size_t N>
constexpr size_t get_row(size_t idx)
{
    return idx / N;
}

template <size_t M, size_t N>
constexpr size_t get_col(size_t idx)
{
    return idx % N;
}

// Index sequence manipulation

// Merge index sequences
template <typename A, typename B>
struct MergeSequences_
{
};

template <size_t... As, size_t... Bs>
struct MergeSequences_<std::index_sequence<As...>, std::index_sequence<Bs...>>
{
    using type = std::index_sequence<As..., Bs...>;
};

template <typename A, typename B>
using MergeSequences = typename MergeSequences_<A, B>::type;

// Get the ith

// Filter out a value from a sequence
// template <size_t Val, typename T>
// struct FilterValue_
// {
// };

// template <size_t Val, size_t... Idxs>
// struct FilterValue_<Val, std::index_sequence<Val, Idxs...>>
// {
//     using type = typename FilterValue_<Val, std::index_sequence<Idxs...>>::type;
// };

// template <size_t Val, size_t A, size_t... Idxs>
//     requires(Val != A)
// struct FilterValue_<Val, std::index_sequence<A, Idxs...>>
// {
//     using type = MergeSequences<std::index_sequence<A>, typename FilterValue_<Val, std::index_sequence<Idxs...>>::type>;
// };

// template <size_t Val, size_t... Idxs>
//     requires(sizeof...(Idxs) == 0)
// struct FilterValue_<Val, std::index_sequence<Idxs...>>
// {
//     using type = std::index_sequence<Idxs...>;
// };

// template <size_t Val, typename T>
// using FilterValue = typename FilterValue_<Val, T>::type;

// Remove the ith value of a sequence
template <size_t Idx, typename A>
struct RemoveAtIndex_
{
};

template <size_t Idx, size_t H, size_t... Rest> requires (Idx != 0)
struct RemoveAtIndex_<Idx, std::index_sequence<H, Rest...>>
{
    using type = MergeSequences<std::index_sequence<H>, typename RemoveAtIndex_<Idx - 1, std::index_sequence<Rest...>>::type>;
};

template <size_t H, size_t... Rest>
struct RemoveAtIndex_<0, std::index_sequence<H, Rest...>>
{
    using type = std::index_sequence<Rest...>;
};

template <size_t Idx, typename A>
using RemoveAtIndex = typename RemoveAtIndex_<Idx, A>::type;

// Get a particular element from an index sequence

template <size_t AtIndex, typename A>
struct GetSequenceElement
{
};

template <size_t AtIndex, size_t H, size_t... Rest>
    requires(AtIndex != 0)
struct GetSequenceElement<AtIndex, std::index_sequence<H, Rest...>>
{
    static constexpr size_t idx = GetSequenceElement<AtIndex - 1, std::index_sequence<Rest...>>::idx;
};

template <size_t H, size_t... Rest>
struct GetSequenceElement<0, std::index_sequence<H, Rest...>>
{
    static constexpr size_t idx = H;
};

// template <size_t... Indices>
// void printIdxSequence(std::index_sequence<Indices...>)
// {
//     // Use fold expression (C++17) to print the elements
//     ((std::cout << Indices << " "), ...);
//     std::cout << std::endl;
// }

// Helpers concepts initializer casts and constructibility

/** @brief Check if FromArgs can be converted to primitive ToType */
template <typename ToType, typename... FromArgs>
concept ConvertibleToPrimitive = ((std::is_convertible_v<FromArgs, ToType>) && ...);

/** @brief Check if FromArgs can be assigned to type ToType */
template <typename ToType, typename... FromArgs>
concept AssignableTo = ((requires(ToType a, FromArgs b) { a = b; }) && ...);

/** @brief Check if non-primitive FromType can be constructed from to ToType (which must be primitive) */
template <typename ToType, typename... FromArgs>
concept ConstructibleFrom = ((std::constructible_from<ToType, FromArgs>) && ...);

/** @brief Helper for ConvertOrConstruct. */
template <typename ToType, typename... FromArgs>
concept ConvertibleOrConstructible = ConvertibleToPrimitive<ToType, FromArgs...> || //
                                     ConstructibleFrom<ToType, FromArgs...>;

/**
 * @brief All-purpose low-overhead converter.
 * - If a type is convertible, convert it
 * - Otherwise, if it is constructible, construct it
 * The order is based on assumed speed.
 */
template <typename ToType, typename FromType>
    requires ConvertibleOrConstructible<ToType, FromType>
constexpr ToType ConvertOrConstruct(const FromType &from)
{
    if constexpr (std::is_convertible_v<ToType, FromType>)
    {
        return static_cast<ToType>(from);
    }
    else
    {
        return ToType{from};
    }
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

// Binary operator operator*(a, b) version
// template <typename A, typename B>
//     requires requires(A a, B b) { { operator*(a, b) }; }
// using BinOpMultiplyType = decltype(operator*(std::declval<A>(), std::declval<B>()));

// template <typename A, typename B>
// concept CanBinOpMultiply = requires(A a, B b) { { operator*(a, b) }; };

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