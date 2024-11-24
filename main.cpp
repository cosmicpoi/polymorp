// Run tests
#include "Unit.h"
#include "UnitMath.h"
#include "Vector.h"

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
    Vector<2, Meter> myVector{4, 3};
    
    Vector2<Meter_2> myVector2{1, 1};

    std::cout << myVector << std::endl;
    std::cout << myVector2 << std::endl;
    
    std::cout << myVector.Dot(myVector2) << std::endl;

}