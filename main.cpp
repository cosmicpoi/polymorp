// Run tests
#include "Unit.h"
#include "UnitMath.h"
#include "Vector.h"
#include <iostream>

using Meter = Unit<'m', double, 1>;
using Meter_2 = Unit<'m', double, 2>;

inline Meter operator"" _meter(long double value)
{
    return Meter{static_cast<double>(value)};
}

using Foot = Unit<'f', float, 1>;

using Inch = Unit<'f', int, 1>;

int main()
{
    Meter mVal{2.0};
    squareRoot(mVal).Print();

    Foot fVal{2.0};
    squareRoot(fVal).Print();

    Inch iVal{2};
    squareRoot(iVal).Print();


    // Vector2<Meter> mVec{1.0, 1.0};
    // std::cout << mVec.Norm() << std::endl;

    // Vector2<Foot> dVec{2.0, 2.0};
    // std::cout << dVec.Norm() << std::endl;

    Vector2<Inch> iVec{3, 3};
    iVec.Norm().Print();
    std::cout << iVec.Norm_d() << std::endl;

}