// Run tests
#include "Unit.h"

using Meter = Unit<double, MakeUnitIdentifier<UnitAtomic<"meter">>>;
using Meter_2 = Unit<double, MakeUnitIdentifier<UnitBase<"meter", 2>>>;

inline Meter operator"" _meter(long double value)
{
    return Meter{static_cast<double>(value)};
}



int main()
{
    

    Meter myVal = 1.2;
    Meter_2 myVal2 = 3.0;


    myVal.Print();
    std::cout << std::endl;

    (myVal - 2.0_meter).Print();
    
    std::cout << std::endl; 

}