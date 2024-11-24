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

template<int MyInt>
class IntHolder
{
public:
    std::ostream& operator<<(std::ostream& os) const {
        os << MyInt;
        return os;
    }
};


template<int MyInt>
std::ostream &operator<<(std::ostream &os, IntHolder<MyInt> mh) { 
    return mh.operator<<(os);
}

int main()
{

    KM_2 myVal2 = 1;
    std::cout << myVal2 << std::endl;

    std::cout << unit_sqrt(myVal2) << std::endl;

    std::cout << unit_pow<std::ratio<2,7>>(myVal2) << std::endl;

    std::cout << (2.0_meter * 2.0) << std::endl;

}