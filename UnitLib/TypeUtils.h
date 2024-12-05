#pragma once

#include <concepts>
#include <iostream>
#include <array>


/**
 * Global "container" class that Vector and Matrix inherit from so that
 * unit can have knowledge of them without having to include them
 */
class Container
{
};

template <typename T>
concept IsContainer = std::is_base_of_v<Container, T>;

/**
 * Proxies for builtins like `std::is_arithmetic` and qstd::common_type`.
 * We generally try to avoid using the std type utils, so putting them in
 * a single place makes it easier to search the codebase for exceptions.
 */

/** @brief Proxy for common type */
template <typename A, typename B>
using CommonType = std::common_type_t<A, B>;

/** Proxy for arithmetic */

template <typename A>
concept IsArithmetic_ = std::is_arithmetic_v<A> && requires {
    { std::numeric_limits<A>::epsilon() };
};

/** @brief Check if a list of typenames are arithmetic */
template <typename... As>
concept IsArithmetic = ((IsArithmetic_<As> && ...));

/** Proxy for integral */

template <typename A>
concept IsIntegral_ = std::is_integral_v<A>;

/** @brief Check if a list of typenames are integral */
template <typename... As>
concept IsIntegral = ((IsIntegral_<As> && ...));

/**
 * Check if two types are equality comparable
 */
template <typename A, typename B>
concept IsEqualityComparable = requires(A a, B b) {
    { a == b } -> std::convertible_to<bool>;
};

/**
 * Extract parameter pack
 */

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

// Remove the ith value of a sequence
template <size_t Idx, typename A>
struct RemoveAtIndex_
{
};

template <size_t Idx, size_t H, size_t... Rest>
    requires(Idx != 0)
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
concept ConvertibleTo = ((std::is_convertible_v<FromArgs, ToType>) && ...);

/** @brief Check if FromArgs can be assigned to type ToType */
template <typename ToType, typename... FromArgs>
concept AssignableTo = ((requires(ToType a, FromArgs b) { a = b; }) && ...);

/** @brief Check if non-primitive FromType can be constructed from to ToType (which must be primitive) */
template <typename ToType, typename... FromArgs>
concept ConstructibleFrom = ((std::constructible_from<ToType, FromArgs>) && ...);

/** @brief Helper for ConvertOrConstruct. */
template <typename ToType, typename... FromArgs>
concept ConvertibleOrConstructible = ConvertibleTo<ToType, FromArgs...> || //
                                     ConstructibleFrom<ToType, FromArgs...>;

/** @brief Flipped version for use in concepts */
template <typename FromType, typename ToType>
concept ConvertibleOrConstructibleTo = ConvertibleOrConstructible<ToType, FromType>;

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
    { a *b } -> std::convertible_to<MultiplyType<A, B>>;
};

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
    { a / b } -> std::convertible_to<DivideType<A, B>>;
};

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
    { a + b } -> std::convertible_to<AddType<A, B>>;
};

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
    { a - b } -> std::convertible_to<SubtractType<A, B>>;
};

// Exp helpers

template <size_t N, typename A>
struct CanExp_
{
};

template <typename A>
struct CanExp_<0, A>
{
    static constexpr bool canExp = CanDivide<A, A>;
    using expType = DivideType<A, A>;
};

template <typename A>
struct CanExp_<1, A>
{
    static constexpr bool canExp = true;
    using expType = A;
};
// Check if a type can be raised to the Nth power
template <size_t N, typename A>
    requires(N > 2)
struct CanExp_<N, A>
{
    static constexpr bool canExp = CanMultiply<typename CanExp_<N - 1, A>::expType, A> && CanExp_<N - 1, A>::canExp;
    using expType = MultiplyType<typename CanExp_<N - 1, A>::expType, A>;
};

template <typename A>
    requires CanMultiply<A, A>
struct CanExp_<2, A>
{
    static constexpr bool canExp = true;
    using expType = MultiplyType<A, A>;
};

template <typename A>
    requires(!CanMultiply<A, A>)
struct CanExp_<2, A>
{
    static constexpr bool canExp = false;
    using expType = UniversalFalse;
};

template <size_t N, typename A>
concept CanExp = CanExp_<N, A>::canExp;

template <size_t N, typename A>
    requires CanExp<N, A>
using ExpType = typename CanExp_<N, A>::expType;

// Check if a type can be inverted
template <typename A>
concept CanInvert = requires(A a) {
    requires CanDivide<A, A>;
    requires CanDivide<DivideType<A, A>, A>;
    { a / a } -> std::constructible_from<DivideType<A, A>>;
    { (a / a) / a } -> std::constructible_from<DivideType<DivideType<A, A>, A>>;
};

template <typename A>
using InvertType = DivideType<DivideType<A, A>, A>;

// Check if the type is negatable (non-unary only for now)
template <typename A>
concept Negatable = requires(A a) {
    // Note that these are intended to be +-1 and not +-Type{1}; they're unitless identity
    { -1 * a } -> ConvertibleOrConstructibleTo<A>;
    { 1 * a } -> ConvertibleOrConstructibleTo<A>;
};
