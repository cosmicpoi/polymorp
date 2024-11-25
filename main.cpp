// Run tests
#include "Unit.h"
#include "UnitMath.h"
#include "Vector.h"

using Meter = dAtomic<"meter">;
using fMeter = fAtomic<"meter">;
using iMeter = iAtomic<"meter">;
using Second = dAtomic<"second">;
using Kilometer = UnitMultRatio<Meter, std::ratio<1000>>;
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
    std::cout << Vector2<Meter>{10.0} + Vector2<Kilometer>{20} << std::endl;  
    // std::cout << GeneralScalar<Test> << std::endl;
    Vector2<Meter> myV{1000, 1000};
    Vector2<Kilometer> myVi{1.0, 1};

    std::cout << (myV == myVi) << std::endl;
    // std::cout << myV << std::endl;
    // std::cout << myVi << std::endl;
    // std::cout << unit_pow<std::ratio<1, 3>>(Meter{100.0}) << std::endl;
    // std::cout << myV * 10.0 << std::endl;
}