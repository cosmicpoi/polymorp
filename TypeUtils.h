#pragma once

#include <concepts>

template <typename T>
concept HasPrint = requires {
    { T::Print() } -> std::same_as<void>;
};




template <char S>
void PrintSingle()
{
    std::cout << S << std::endl;
}

template <int S>
void PrintSingle()
{
    std::cout << S << std::endl;
}

template <size_t S>
void PrintSingle()
{
    std::cout << S << std::endl;
}

template <char S>
void PrintSingle_Noline()
{
    std::cout << S << " ";
}

template <int S>
void PrintSingle_Noline()
{
    std::cout << S << " ";
}

template <size_t S>
void PrintSingle_Noline_St()
{
    std::cout << S << " ";
}

template <char... Ss>
void PrintMany()
{
    (PrintSingle_Noline<Ss>(), ...);
    std::cout << std::endl;
}

template <int... Ss>
void PrintMany()
{
    (PrintSingle_Noline<Ss>(), ...);
    std::cout << std::endl;
}

template <size_t... Ss>
void PrintMany_St()
{
    (PrintSingle_Noline_St<Ss>(), ...);
    std::cout << std::endl;
}