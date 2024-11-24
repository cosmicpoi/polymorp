// Run tests
#include "Unit.h"

using Meter = Unit<double, MakeUnitIdentifier<UnitAtomic<"meter">>>;
using Meter_2 = Unit<double, MakeUnitIdentifier<UnitBase<"meter", std::ratio<2>>>>;

inline Meter operator"" _meter(long double value)
{
    return Meter{static_cast<double>(value)};
}



int main()
{
    PrintRatio<std::ratio<2, 4>>();
    std::cout << std::endl;
    using U2 = Unit<double, 
        MakeUnitIdentifier<
            UnitBase<"meter", std::ratio<2,3>>,
            UnitBase<"second", std::ratio<3,7>>
        >
    >;
    U2::PrintInfo();
    std::cout << std::endl; 

    UIMult<typename U2::uid, typename Meter::uid>::Print();
    std::cout << std::endl; 

    Meter myVal = 1.2;
    Meter_2 myVal2 = 3.0;


    myVal.Print();
    std::cout << std::endl;

    (myVal - 2.0_meter).Print();
    
    std::cout << std::endl; 

}