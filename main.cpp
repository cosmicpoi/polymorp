// Run tests
#include "Unit.h"
#include "UnitMath.h"
#include "Vector.h"

using Meter = dAtomic<"meter">;
using Second = dAtomic<"second">;
using fMeter = Unit<float, MakeUnitIdentifier<UnitAtomic<"meter">>>;
using Kilometer = Unit<double, MakeUnitIdentifier<UnitAtomic<"meter">>, std::ratio<1000>>;
using m__s2 = UnitMult<Meter, UnitExpI<Second, -2>>;

using Complex = MakeUnitIdentifier<UnitAtomic<"meter">, UnitAtomic<"second">>;

using KM_2 = Unit<
    double,
    MakeUnitIdentifier<UnitBase<"meter", std::ratio<2>>>,
    std::ratio<1000000>>;

inline Meter operator"" _meter(long double value)
{
    return Meter{static_cast<double>(value)};
}

int main()
{
    std::cout << m__s2{1} << std::endl;
    // std::cout << Kilometer{1} - Meter{1} << std::endl;
}