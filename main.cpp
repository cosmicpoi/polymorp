// Run tests
#include "Unit.h"
#include "UnitMath.h"
#include "Vector.h"
#include <iostream>

using Meter = Unit<'m', double, 1>;
using Meter_2 = Unit<'m', double, 2>;

using Foot = Unit<'f', float, 1>;
using Foot_2 = Unit<'f', float, 2>;

using Inch = Unit<'i', int, 1>;
using Inch_2 = Unit<'i', int, 2>;

inline Meter operator"" _meter(long double value)
{
    return Meter{static_cast<double>(value)};
}



int main()
{
    // std::cout << IsSameUnit<Meter, Meter_2> << std::endl;
    // Meter_2 mVal{2.0};
    // squareRoot(mVal).Print();

    // Foot_2 fVal{2.0};
    // squareRoot(fVal).Print();

    // Inch_2 iVal{2};
    // squareRoot(iVal).Print();

    // Unit<'x', float, 4> myV{2.0};
    // myV.Print();
    // squareRoot(myV).Print();


    Vector2<Meter> mVec2(0, 0);

    Vector4<Meter> mVec{};
    std::cout << mVec.Norm().GetValue() << std::endl;
    mVec.Print();
    (mVec * 1.0_meter).Print();
    // Unit<'m', int, 1> badMeter = 0;
    // (mVec * badMeter).Print(); // causes compilation error

    // // Vector2<Foot> dVec{2.0, 2.0};
    // // std::cout << dVec.Norm() << std::endl;

    // Vector2<Inch> iVec{3, 3};
    // iVec.Norm().Print();
    // std::cout << iVec.Norm_d() << std::endl;

    // Vector4<Inch> iVec4{1, 2, 3, 4};
    // iVec4.Print();


}