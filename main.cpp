// Run tests
#include "Unit.h"
#include "UnitMath.h"

using Meter = Unit<double, MakeUnitIdentifier<UnitAtomic<"meter">>>;
using Meter_2 = Unit<double, MakeUnitIdentifier<UnitBase<"meter", std::ratio<2>>>>;

using KM_2 = Unit<
    double,
    MakeUnitIdentifier<UnitBase<"meter", std::ratio<2>>>,
    std::ratio<1000000>
>;

inline Meter operator"" _meter(long double value)
{
    return Meter{static_cast<double>(value)};
}



int main()
{
    KM_2 myVal2 = 1;
    myVal2.Print();
    std::cout << std::endl;

    unit_sqrt(myVal2).Print();
    std::cout << std::endl;

    unit_pow<decltype(myVal2), std::ratio<2,7>>(myVal2).Print();
    std::cout << std::endl;

}