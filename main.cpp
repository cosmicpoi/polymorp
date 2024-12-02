// Run tests
#include "UnitLib/Unit.h"
#include "UnitLib/UnitMath.h"
#include "UnitLib/Vector.h"
#include "UnitLib/Matrix.h"
#include "UnitLib/Print.h"

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

// Example class that supports add and subtract but not divide/mult
class AdditiveString
{
public:
    AdditiveString(const std::string &val) : value(val) {};
    AdditiveString(const char *val) : value(val) {};
    // concats strings
    AdditiveString operator+(const AdditiveString &other) const
    {
        return AdditiveString(value + other.value);
    }
    // removes the given string only if it at the end - for instance, "hibye" - "bye" == "hi"
    AdditiveString operator-(const AdditiveString &suffix) const
    {
        if (value.size() >= suffix.value.size() &&
            value.compare(value.size() - suffix.value.size(), suffix.value.size(), suffix.value) == 0)
        {
            // Remove the suffix
            return AdditiveString(value.substr(0, value.size() - suffix.value.size()));
        }
        // If the suffix is not at the end, return the original string
        return *this;
    }

    std::string value;
};

std::ostream &operator<<(std::ostream &os, const AdditiveString &val)
{
    return os << val.value; // Forward to the underlying std::string
}

int main()
{
    // Matrix<2, 2, Meter> mat2{1, 2, 3, 4};
    // std::cout << Inv(mat2) << std::endl;
    // std::cout << (HasSquareRoot<std::string>) << std::endl;
    // std::cout << ( Meter{2} * 2.5 ) << std::endl;
    // std::cout << ( 2.5 * Meter{2} ) << std::endl;
    // std::cout << ( Meter{2} / 2.5 ) << std::endl;
    // std::cout << ( 2.5 / Meter{2} ) << std::endl;

    using StrUnit = TypeAtomic<AdditiveString, "string">;
    std::cout << (StrUnit{"hellobye"} + StrUnit{"bye"}) << std::endl;

    // std::cout << (EmptyUnit<AdditiveString>{"hellobye"} + EmptyUnit<AdditiveString>{"bye"}) << std::endl;
    // std::cout << (EmptyUnit<AdditiveString>{"hellobye"} + AdditiveString{"bye"}) << std::endl;

    // std::cout << (EmptyUnit<AdditiveString>{"hellobye"} - EmptyUnit<AdditiveString>{"bye"}) << std::endl;
    // std::cout << (EmptyUnit<AdditiveString>{"hellobye"} - AdditiveString{"bye"}) << std::endl;
    // std::cout << (2.5 + dUEmpty{2}) << std::endl;

    std::cout << (dUEmpty{2} + 2.5) << std::endl;
    std::cout << (2.5 + dUEmpty{2}) << std::endl;

    std::cout << (dUEmpty{2} - 2.5) << std::endl;
    std::cout << (2.5 - dUEmpty{2}) << std::endl;
    // std::cout << ( Meter{2} - 2.5 ) << std::endl;
    // std::cout << ( 2.5 - Meter{2} ) << std::endl;

    // std::cout << unit_ratio_pow<std::ratio<2, 4>>(v) << std::endl;

    // double x = dUEmpty{1.0};
    // x *= dUEmpty{2.0};
    // std::cout << x << std::endl;
    // std::cout << std::string{1} << std::endl;
}