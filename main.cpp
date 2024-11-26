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

int main()
{
    std::cout << "hello world" << std::endl;
    // std::cout << dUEmpty{1.0} + ((double) 1) << std::endl;

    std::array<float, 5> a1 {1, 2, 3, 4, 5};
    // std::cout << a2[0] << std::endl;
    std::array<float, 5> a2;

    std::cout << a2[0] << std::endl;
    a2 = a1;
    std::cout << a2[0] << std::endl;

    // Vector2<Meter> myV{1000, 1000};
    // Vector2<Kilometer> myVi{1.0, 1.01};

    // Vector3<Meter> i{1, 0ma, 0};
    // Vector3<Meter> j{0, 1, 0};

    // Meter val{1000};
    // Kilometer val2{1.2};

    // Vector2<Meter> vx;
    // Vector2<Meter> vy{1, 1};

    // vx = vy;


    // // std::cout << (true * 10) << std::endl;
    // std::cout << Meter{1} - Kilometer{1} << std::endl;
    // std::cout << vx << std::endl;
    // // std::cout << myV << std::endl;
    // // std::cout << myVi << std::endl;
    // // std::cout << unit_pow<std::ratio<1, 3>>(Meter{100.0}) << std::endl;
    // // std::cout << myV * 10.0 << std::endl;
}