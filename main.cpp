// Run tests
#include "Unit.h"
#include "UnitMath.h"
#include "Vector.h"

using Meter = Unit<double, MakeUnitIdentifier<UnitAtomic<"meter">>>;
using Meter_2 = Unit<double, MakeUnitIdentifier<UnitBase<"meter", std::ratio<2>>>>;

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
    std::cout << Vector2<Meter>{1, 1}.Dot(Vector2<Meter>{2, 2}) << std::endl;
}