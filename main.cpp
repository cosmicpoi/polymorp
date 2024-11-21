// Run tests
#include "Unit.h"
#include "Vector.h"
#include <iostream>

using Meter = Unit<'m', double, 1>;
using Meter_2 = Unit<'m', double, 2>;

inline Meter operator"" _meter(long double value)
{
    return Meter{static_cast<double>(value)};
}

using Foot = Unit<'f', float, 1>;

Meter_2 square_meter(Meter dist)
{
    return dist * dist;
}

int main()
{
    Meter dist = 2.5_meter;
    // square_meter(dist).Print();

    Foot dist2{2};
    // dist2.Print();

    // Vector2<float> myVec{1, 2};
    Vector2<Meter> myVec{1, 2};
    myVec.Print();
}