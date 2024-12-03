// Run tests
#include "UnitLib/Unit.h"
#include "UnitLib/UnitMath.h"
#include "UnitLib/Vector.h"
#include "UnitLib/Matrix.h"
#include "UnitLib/Print.h"

#include "AsciiGraphics.h"

#include <iomanip>
#include <unistd.h>

// using Meter = dAtomic<"meter">;
// using fMeter = fAtomic<"meter">;
// using iMeter = iAtomic<"meter">;
// using Second = dAtomic<"second">;
// using Kilometer = UnitMultRatio<Meter, std::ratio<1000>>;

// using Complex = MakeUnitIdentifier<UnitAtomic<"meter">, UnitAtomic<"second">>;

// using KM_2 = Unit<
//     double,
//     MakeUnitIdentifier<UnitBase<"meter", std::ratio<2>>>,
//     std::ratio<1000000>>;

// inline Meter operator"" _meter(long double value)
// {
//     return Meter{static_cast<double>(value)};
// }

using Meter = dAtomic<"meter">;
using Kilometer = UnitMultRatio<Meter, std::ratio<1000>>;
using Meter__1 = UnitExpI<Meter, -1>;

using Second = dAtomic<"second">;

using dUEmpty = EmptyUnit<double>;
using dUKilo = UnitMultRatio<dUEmpty, std::ratio<1000>>;

template <typename A>
concept CanSqrt = requires(A a) {
    { std::sqrt(a) };
};

int main()
{
    AsciiGraphics ascii{};
    ascii.ClearScreen();
    ascii.MoveCursor(5, 5);
    // ascii.DrawRect(1, 1, 5, 5, "✊", false);
    ascii.DrawRect(1, 6, 5, 5, '.', false);
    ascii.DrawText(10, 10, "✊");
    
    // ascii.DrawChar(10, 10, 'h');
    // ascii.EndFrame();

    // while(1) {
    //     usleep(1000 * 16);
    //     std::cout << "hi";
    //     ascii.EndFrame();
    // }
    ascii.EndFrame();
}