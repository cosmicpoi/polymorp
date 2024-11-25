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