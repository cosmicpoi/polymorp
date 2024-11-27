// Run tests
#include "UnitLib/Unit.h"
// #include "UnitLib/UnitMath.h"
#include "UnitLib/Vector.h"

// using Meter = dAtomic<"meter">;
// using fMeter = fAtomic<"meter">;
// using iMeter = iAtomic<"meter">;
// using Second = dAtomic<"second">;
// using Kilometer = UnitMultRatio<Meter, std::ratio<1000>>;
// // using m__s2 = UnitMult<Meter, UnitExpI<Second, -2>>;

// using Complex = MakeUnitIdentifier<UnitAtomic<"meter">, UnitAtomic<"second">>;

// using KM_2 = Unit<
//     double,
//     MakeUnitIdentifier<UnitBase<"meter", std::ratio<2>>>,
//     std::ratio<1000000>>;

// inline Meter operator"" _meter(long double value)
// {
//     return Meter{static_cast<double>(value)};
// }

using Meter = dAtomic<"meter">;
using Kilometer = UnitMultRatio<Meter, std::ratio<1000>>;

using dUEmpty = EmptyUnit<double>;
using dUKilo = UnitMultRatio<dUEmpty, std::ratio<1000>>;

#include <iostream>

void func(const int& x) {
    std::cout << "const int& called\n";
}


int main()
{
    auto ns = NormSquared(Vector2<Kilometer>{0.003, 0.004});
    std::cout << ns << std::endl;
    std::cout << unit_sqrt(ns) << std::endl;

    auto v = (Kilometer{1} + Kilometer{1} + Kilometer{1});
    std::cout << v << std::endl;


    std::cout << lcm(6, 8) <<std::endl;
    // int x = 10;
    // PrintTypeInfo<decltype(v)>();

}