// Run tests
#include "Unit.h"
#include "UnitMath.h"
#include "Vector.h"

using Meter = Unit<double, MakeUnitIdentifier<UnitAtomic<"meter">>>;
using fMeter = Unit<float, MakeUnitIdentifier<UnitAtomic<"meter">>>;
using Kilometer = Unit<double, MakeUnitIdentifier<UnitAtomic<"meter">>, std::ratio<1000>>;
using Meter_2 = Unit<double, MakeUnitIdentifier<UnitBase<"meter", std::ratio<2>>>>;

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
    auto myVal = (Meter{1} + Kilometer{1});
    std::cout << (myVal /= 5) << std::endl;
    // myVal += 5;
    std::cout << myVal << std::endl;

    std::cout << Kilometer{1} << std::endl;
    std::cout << 1 / Meter{5} << std::endl;
    // std::cout << (Meter{1} + Kilometer{1}) * 5.0_meter << std::endl;
    // std::cout << Kilometer{1} - Meter{1} << std::endl;
}