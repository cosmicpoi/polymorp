// Run tests
#include "Unit.h"
#include "UnitIdentifier.h"
#include "UnitMath.h"
#include "Vector.h"
#include <iostream>



// inline Meter operator"" _meter(long double value)
// {
//     return Meter{static_cast<double>(value)};
// }


int main() {
    using UL1 = UnitBase<"aaaa", 1>;
    using UL2 = UnitBase<"bbbb", 2>;
    using UL3 = UnitBase<"cccc", -1>;
    
    using UI_1 = MakeUnitIdentifier<UL1, UL3, UL2>;
    UI_1::Print();
    // UI_1::type::Print();
    using UI_2 = MakeUnitIdentifier<UL2, UL3, UL3>;
    UI_2::Print();

    UIDivide<UI_1, UI_2>::Print();
}