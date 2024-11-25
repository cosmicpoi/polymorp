#pragma once

#include <concepts>
#include <iostream>

/** Concept to ensure ::Print() exists */
template <typename T>
concept HasPrint = requires(std::ostream &os) {
    { T::Print(os) } -> std::same_as<void>;
};

// Helper function to create a compile-time zero-initialized array of type T and size N
template <typename T, size_t N>
constexpr std::array<T, N> create_array()
{
    return ([]<std::size_t... Is>(std::index_sequence<Is...>) -> std::array<T, N>
            { return {((void)Is, 0)...}; })(std::make_index_sequence<N>{});
}

template <typename A, typename B>
concept CanMultiply = requires(A a, B b) {
    { a *b } -> std::same_as<std::common_type_t<A, B>>;
};

template <typename A, typename B>
concept CanDivide = requires(A a, B b) {
    { a / b } -> std::same_as<std::common_type_t<A, B>>;
};

template <typename A, typename B>
concept CanAdd = requires(A a, B b) {
    { a + b } -> std::same_as<std::common_type_t<A, B>>;
};

template <typename A, typename B>
concept CanSubtract = requires(A a, B b) {
    { a - b } -> std::same_as<std::common_type_t<A, B>>;
};

template <typename A, typename B>
concept CanRatioAdd = requires(std::common_type_t<A, B> x, intmax_t r)
{
    { x / r } -> std::same_as<std::common_type_t<A, B>>;
    { x + x } -> std::same_as<std::common_type_t<A, B>>;
};

template <typename A, typename B>
concept CanRatioSubtract = requires(std::common_type_t<A, B> x, intmax_t r)
{
    { x / r } -> std::same_as<std::common_type_t<A, B>>;
    { x - x } -> std::same_as<std::common_type_t<A, B>>;
};