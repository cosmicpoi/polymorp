// Run tests
#include "UnitLib/Unit.h"
// #include "UnitLib/UnitMath.h"
#include "UnitLib/Vector.h"
#include "UnitLib/Matrix.h"
#include "UnitLib/Print.h"

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

using Second = dAtomic<"second">;

using dUEmpty = EmptyUnit<double>;
using dUKilo = UnitMultRatio<dUEmpty, std::ratio<1000>>;

using M_S = UnitDivide<Meter, Second>;

template <typename T>
struct MyClass
{
public:
};

int main()
{
    Matrix<2, 2, double> m1 {1, 2, 3, 4};
    Matrix<2, 4, double> m2;
    std::cout << m1 << std::endl;
    std::cout << m2 << std::endl;

    std::cout << (2 * m1) << std::endl;

    // std::cout << (v2_a * v2_b) << std::endl;
    // std::cout << (v2_a * v3) << std::endl;

    
    // std::cout << (m1 * m2) << std::endl;
}