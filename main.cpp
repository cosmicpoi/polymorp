// Run tests
#include "UnitLib/Unit.h"
#include "UnitLib/UnitMath.h"
#include "UnitLib/Vector.h"
#include "UnitLib/Matrix.h"
#include "UnitLib/Print.h"

// using Meter = dAtomic<"meter">;
// using fMeter = fAtomic<"meter">;
// using iMeter = iAtomic<"meter">;
// using Second = dAtomic<"second">;
// using Kilometer = UnitMultRatio<Meter, std::ratio<1000>>;

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
using Meter__1 = UnitExpI<Meter, -1>;

using Second = dAtomic<"second">;

using dUEmpty = EmptyUnit<double>;
using dUKilo = UnitMultRatio<dUEmpty, std::ratio<1000>>;

template <typename A>
concept CanSqrt = requires(A a) {
    { std::sqrt(a) };
};

int main()
{
    // Matrix<2, 2, Meter> mat2{1, 2, 3, 4};
    // std::cout << Inv(mat2) << std::endl;
    // std::cout << (HasSquareRoot<std::string>) << std::endl;
    std::cout << ( Meter{2} * 2.5 ) << std::endl;
    std::cout << ( 2.5 * Meter{2} ) << std::endl;
    std::cout << ( Meter{2} / 2.5 ) << std::endl;
    std::cout << ( 2.5 / Meter{2} ) << std::endl;

    Kilometer v{1};
    // std::cout << unit_ratio_pow<std::ratio<2, 4>>(v) << std::endl;

    // double x = dUEmpty{1.0};
    // x *= dUEmpty{2.0};
    // std::cout << x << std::endl;
    // std::cout << std::string{1} << std::endl;
}