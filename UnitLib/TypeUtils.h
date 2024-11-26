#pragma once

#include <concepts>
#include <iostream>

/** Concept to ensure ::Print() exists */
template <typename T>
concept HasPrint = requires(std::ostream &os) {
    { T::Print(os) } -> std::same_as<void>;
};

#include <cxxabi.h>

// Helper to demangle type names
std::string demangle(const char *name)
{
    int status = 0;
    char *demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
    std::string result = (status == 0) ? demangled : name;
    free(demangled);
    return result;
}

// ------------------------------------------------------
// General utils for type arithmetic
// ------------------------------------------------------

/** Multiply */
template <typename A, typename B>
    requires(requires(A a, B b) { {a * b}; })
using MultiplyType = decltype(std::declval<A>() * std::declval<B>());

template <typename A, typename B>
concept CanMultiply = requires(A a, B b) {
    { a *b } -> std::convertible_to<MultiplyType<A, B>>;
};

// Operator* version
template <typename A, typename B>
    requires requires(A a, B b) { { a.operator*(b) }; }
using OpMultiplyType = decltype(std::declval<A>().operator*(std::declval<B>()));

template <typename A, typename B>
concept CanOpMultiply = requires(A a, B b) { { a.operator*(b) } -> std::same_as<OpMultiplyType<A, B>>; };

/** Divide */
template <typename A, typename B>
    requires(requires(A a, B b) { { a / b }; })
using DivideType = decltype(std::declval<A>() / std::declval<B>());

template <typename A, typename B>
concept CanDivide = requires(A a, B b) {
    { a / b } -> std::convertible_to<DivideType<A, B>>;
};

// Operator/ version
template <typename A, typename B>
    requires requires(A a, B b) { { a.operator/(b) }; }
using OpDivideType = decltype(std::declval<A>().operator/(std::declval<B>()));

template <typename A, typename B>
concept CanOpDivide = requires(A a, B b) { { a.operator/(b) } -> std::same_as<OpDivideType<A, B>>; };

// Add
/** Add */
template <typename A, typename B>
    requires(requires(A a, B b) { { a + b }; })
using AddType = decltype(std::declval<A>() + std::declval<B>());

template <typename A, typename B>
concept CanAdd = requires(A a, B b) {
    { a + b } -> std::convertible_to<AddType<A, B>>;
};

// Operator+ version
template <typename A, typename B>
    requires requires(A a, B b) { { a.operator+(b) }; }
using OpAddType = decltype(std::declval<A>().operator+(std::declval<B>()));

template <typename A, typename B>
concept CanOpAdd = requires(A a, B b) { { a.operator+(b) } -> std::same_as<OpAddType<A, B>>; };

/** Subtract */
template <typename A, typename B>
    requires(requires(A a, B b) { { a - b }; })
using SubtractType = decltype(std::declval<A>() - std::declval<B>());

template <typename A, typename B>
concept CanSubtract = requires(A a, B b) {
    { a - b } -> std::convertible_to<SubtractType<A, B>>;
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