// Run tests
#include "unit.h"
#include <iostream>

using Meter = Unit<'m', double, 1>;
using Meter_2 = Unit<'m', double, 2>;

Meter_2 square_meter(Meter dist)
{
    return dist * dist;
}

int main()
{
    Meter dist = 2.5_meter;
    // std::cout << dist.value << std::endl;
    dist.Print();
}