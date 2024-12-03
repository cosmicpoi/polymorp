// Run tests
#include "UnitLib/Unit.h"
#include "UnitLib/UnitMath.h"
#include "UnitLib/Vector.h"
#include "UnitLib/Matrix.h"
#include "UnitLib/Print.h"

#include <iomanip>
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

template <uint P>
constexpr uint PositiveMod(const intmax_t &val)
{
    uint rem = (val % P);
    return rem < 0 ? (rem + P) : rem;
}

// Example class that supports add and subtract and ALSO ratio
template <uint P>
class PrimeField
{
public:
    static constexpr uint p = P;
    inline PrimeField(intmax_t val) : value(PositiveMod<P>(val)) {};

    // Add/subtract operations
    inline PrimeField<P> operator+(const PrimeField<P> &other) const
    {
        return PrimeField(PositiveMod<P>(value + other.value));
    }

    inline PrimeField<P> operator-(const PrimeField<P> &other) const
    {
        return PrimeField(PositiveMod<P>(value - other.value));
    }
    // Mult/divide
    inline PrimeField<P> operator*(const PrimeField<P> &other) const
    {
        return PrimeField(PositiveMod<P>(value * other.value));
    }

    inline PrimeField<P> MultInv() const
    {
        // Use fermat's little theorem
        // x = a^(P-2), so need to multiply P-3 times
        uint res = value;
        for (uint i = 0; i <= P - 3; i++)
        {
            res = (res * res) % P;
        }

        return res;
    }

    inline PrimeField<P> operator/(const PrimeField<P> &other) const
    {
        if (other.value == 0)
        {
            throw std::runtime_error("Divide by zero");
        }
        return (*this) * other.MultInv();
    }

    // Ratio operations
    inline PrimeField<P> operator*(const intmax_t &other) const
    {
        return PrimeField(PositiveMod<P>(value * other));
    }

    inline PrimeField<P> operator/(const intmax_t &other) const
    {
        if (other % P == 0)
        {
            throw std::runtime_error("Divide by zero");
        }
        return (*this) * PrimeField<P>{PositiveMod<P>(other)}.MultInv();
    }

    inline bool operator==(const PrimeField<P> &other) const
    {
        return value == other.value;
    }

    // Value holder
    uint value;
};

template <uint P>
std::ostream &operator<<(std::ostream &os, const PrimeField<P> &val)
{
    return os << val.value; // Forward to the underlying std::string
}

using Z7 = PrimeField<7>;

// Example class that supports add and subtract but NOT ratio
class AdditiveString
{
public:
    inline AdditiveString(const std::string &val) : value(val) {};
    inline AdditiveString(const char *val) : value(val) {};
    // concats strings
    inline AdditiveString operator+(const AdditiveString &other) const
    {
        return AdditiveString(value + other.value);
    }
    // removes the given string only if it at the end - for instance, "hibye" - "bye" == "hi"
    inline AdditiveString operator-(const AdditiveString &suffix) const
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

    // comparison
    inline bool operator==(const AdditiveString &other) const
    {
        return value == other.value;
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
    // Doesn't compile:
    // using StrDouble = UnitMultRatio<StrUnit, std::ratio<2>>;
    // std::cout << (StrUnit{"hellobye"} + StrUnit{"bye"}) << std::endl;

    using Z7Unit = TypeAtomic<Z7, "z7">;
    using Z7Unit_Double = UnitMultRatio<Z7Unit, std::ratio<2>>;
    using Z7Unit_Half = UnitMultRatio<Z7Unit, std::ratio<1, 2>>;
    // std::cout << ((Z7{1} / Z7{2}).value == 4) << std::endl;

    // std::cout << (EmptyUnit<AdditiveString>{"hellobye"} + EmptyUnit<AdditiveString>{"bye"}) << std::endl;
    // std::cout << (EmptyUnit<AdditiveString>{"hellobye"} + AdditiveString{"bye"}) << std::endl;

    // std::cout << (EmptyUnit<AdditiveString>{"hellobye"} - EmptyUnit<AdditiveString>{"bye"}) << std::endl;
    // std::cout << (EmptyUnit<AdditiveString>{"hellobye"} - AdditiveString{"bye"}) << std::endl;
    // std::cout << (2.5 + dUEmpty{2}) << std::endl;

    // std::cout << ( Meter{2} - 2.5 ) << std::endl;
    // std::cout << ( 2.5 - Meter{2} ) << std::endl;

    /**
     * Multiplication and Division
     */
    // std::cout << (Meter{1} * 2) << std::endl;

    /**
     * Addition and Subtraction
     */

    // Builtin
    // std::cout << (Meter{1} + Kilometer{1}) << std::endl;
    // std::cout << (Kilometer{1} + Meter{1}) << std::endl;
    // // std::cout << (StrUnit{"hellobye"} + StrUnit{"bye"}) << std::endl;

    // // User-defined ratio-compatible
    // std::cout << (Z7Unit{1} + Z7Unit_Double{2}) << std::endl;
    // std::cout << (Z7Unit_Double{2} + Z7Unit{1}) << std::endl;

    // // User-defined ratio-incompatible
    // // std::cout << StrUnit{"hibye"} << std::endl;
    // std::cout << (StrUnit{"hibye"} + StrUnit{"bye"}) << std::endl;
    // std::cout << (StrUnit{"bye"} + StrUnit{"hibye"}) << std::endl;

    /**
     * Addition with empty units
     */

    // Builtin types
    // std::cout << (dUEmpty{1} + 1.5) << std::endl;
    // std::cout << (1.5 + dUEmpty{1}) << std::endl;

    // std::cout << (dUEmpty{1} - 1.5) << std::endl;
    // std::cout << (1.5 - dUEmpty{1}) << std::endl;

    // // User-defined ratio-compatible
    // std::cout << (EmptyUnit<Z7>{1} + Z7{2}) << std::endl;
    // std::cout << (Z7{2} + EmptyUnit<Z7>{1}) << std::endl;

    // std::cout << (EmptyUnit<Z7>{1} - Z7{2}) << std::endl;
    // std::cout << (Z7{2} - EmptyUnit<Z7>{1}) << std::endl;

    // // User-defined ratio-incompatible

    // std::cout << (EmptyUnit<AdditiveString>{"hibye"} + AdditiveString{"bye"}) << std::endl;
    // std::cout << (AdditiveString{"hibye"} + EmptyUnit<AdditiveString>{"bye"}) << std::endl;

    // std::cout << (EmptyUnit<AdditiveString>{"hibye"} - AdditiveString{"bye"}) << std::endl;
    // std::cout << (AdditiveString{"hibye"} - EmptyUnit<AdditiveString>{"bye"}) << std::endl;

    /**
     * Comparison with empty units
     */

    // Builtin
    // std::cout << (dUEmpty{2} == 2) << std::endl;
    // std::cout << (2 == dUEmpty{2}) << std::endl;

    // // User-defined with ratio
    // std::cout << (EmptyUnit<Z7>{1} == Z7{1}) << std::endl;
    // std::cout << (Z7{1} == EmptyUnit<Z7>{1}) << std::endl;

    // // User-defined without ratio
    // std::cout << (EmptyUnit<AdditiveString>{"hellobye"} == AdditiveString{"hellobye"}) << std::endl;
    // std::cout << (AdditiveString{"hellobye"} == EmptyUnit<AdditiveString>{"hellobye"}) << std::endl;

    // std::cout << unit_ratio_pow<std::ratio<2, 4>>(v) << std::endl;

    // double x = dUEmpty{1.0};
    // x *= dUEmpty{2.0};
    // std::cout << x << std::endl;
    // std::cout << std::string{1} << std::endl;

    /**
     * Vectors
     */
    // std::cout << CanMultiply<double, Unit<double, UnitLeafVector<UnitLeaf<"meter", std::ratio<1, 1>>>>> << std::endl;
    // Vector2<double> v1{1, 2};
    // std::cout << ((v1 * Meter{2})) << std::endl;
    // std::cout << ((Meter{2} * v1)) << std::endl;

    // std::cout << ((v1 / Meter{2})) << std::endl;
    // std::cout << Vector2<Meter>{2, 4} << std::endl;
    // std::cout << ((Meter{2} * v1) == Vector2<Meter>{2, 4}) << std::endl;

    std::cout << ((double)1 + dUKilo{1}) << std::endl;
    // std::cout <<( ((double) 1 +  dUKilo{1}) == 1001) <<std::endl;

    Matrix2<double> m1 = {{1, 2}, {3, 4}};
    Matrix2<dUKilo> m2{1, 1, 1, 1};
    std::cout << std::setprecision(20);
    std::cout << (m1 + m2) << std::endl;
    std::cout << Matrix2<double>{1001, 1002, 1003, 1004} << std::endl;
    std::cout << ((m1 + m2) == Matrix2<double>{1001, 1002, 1003, 1004}) << std::endl;
}