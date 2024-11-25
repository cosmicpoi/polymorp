#include <cassert>

#include "../UnitLib/StringLiteral.h"
#include "../UnitLib/Unit.h"
#include "../UnitLib/Vector.h"

// ------------------------------------------------------------
// Testing library utils
// ------------------------------------------------------------

template <typename A, StringLiteral str, typename B>
constexpr bool _CanOp()
{
    // Assignment
    if constexpr(StrEq<str, "=">::value)
    {
        return requires(A a, B b) { a = b; };
    }
    // Equality
    else if constexpr (StrEq<str, "==">::value)
    {
        return requires(A a, B b) { a == b; };
    }
    // Arithmetic
    else if constexpr (StrEq<str, "+">::value)
    {
        return requires(A a, B b) { a + b; };
    }
    else if constexpr (StrEq<str, "-">::value)
    {
        return requires(A a, B b) { a - b; };
    }
    else if constexpr (StrEq<str, "*">::value)
    {
        return requires(A a, B b) { a *b; };
    }
    else if constexpr (StrEq<str, "/">::value)
    {
        return requires(A a, B b) { a / b; };
    }
    // Arithmetic Assignment
    else if constexpr (StrEq<str, "+=">::value)
    {
        return requires(A a, B b) { a += b; };
    }
    else if constexpr (StrEq<str, "-=">::value)
    {
        return requires(A a, B b) { a -= b; };
    }
    else if constexpr (StrEq<str, "*=">::value)
    {
        return requires(A a, B b) { a *= b; };
    }
    else if constexpr (StrEq<str, "/=">::value)
    {
        return requires(A a, B b) { a /= b; };
    }
    else
    {
        return false;
    }
}

template <typename A, StringLiteral str, typename B>
concept _CanOpC = _CanOp<A, str, B>();

using Meter = dAtomic<"meter">;
using Meter_2 = UnitExpI<Meter, 2>;
using fMeter = fAtomic<"meter">;
using iMeter = iAtomic<"meter">;
using Second = dAtomic<"second">;
using Second__2 = UnitExpI<Meter, -2>;
using Kilometer = UnitMultRatio<Meter, std::ratio<1000>>;
using Kilometer_2 = UnitExpI<Kilometer, 2>;

int main()
{
    // ------------------------------------------------------------
    // Run Unit tests
    // ------------------------------------------------------------

    /** -- Run constructor tests */
    std::cout << "Running constructor tests" << std::endl;
    {
        Meter v0;
        assert(v0.GetValue() == 0);
        Meter v1{10.2};
        assert(v1.GetValue() == 10.2);
        Meter v2{(int)10};
        assert(v2.GetValue() == (int)10);
        Meter v3{10.2f};
        assert(v3.GetValue() == (double)10.2f);

        Kilometer v4{1001.0};
        assert(v4.GetValue() == 1001.0);
    }

    /** -- Run equality and assignment tests --  */
    std::cout << "Running equality and assignment tests" << std::endl;

    // Test equality
    {
        assert(Meter{1000} == Kilometer{1});
        assert(Meter{1} == Kilometer{0.001});
        assert(Meter{1} == Meter{1});
    }

    // Test assignment
    {
        Meter v0{1};
        v0 = Meter{2};
        assert(v0 == Meter{2});
        assert((v0 = Meter{3}) == Meter{3});
        // Conversion from different ratio
        assert((v0 = Kilometer{1.001}) == Meter{1001});
        Kilometer v1{0};
        assert((v1 = Meter{1001}) == Kilometer{1.001});

        static_assert((_CanOpC<Meter, "=", Kilometer>));
        static_assert((_CanOpC<Meter, "=", iMeter>));

        static_assert((!_CanOpC<Meter, "=", Second>));
        static_assert((!_CanOpC<Meter, "=", Meter_2>));

        static_assert((_CanOpC<Meter, "=", typename Meter::type>));
        static_assert((_CanOpC<Meter, "=", double>));
        static_assert((_CanOpC<Meter, "=", int>));
        static_assert((_CanOpC<Meter, "=", unsigned int>));
        static_assert((_CanOpC<Meter, "=", float>));

        static_assert((_CanOpC<iMeter, "=", typename Meter::type>));
        static_assert((_CanOpC<iMeter, "=", double>));
        static_assert((_CanOpC<iMeter, "=", int>));
        static_assert((_CanOpC<iMeter, "=", unsigned int>));
        static_assert((_CanOpC<iMeter, "=", float>));

        static_assert((!_CanOpC<typename Meter::type, "=", Meter>));
    }

    /** -- Run arithmetic tests --  */
    std::cout << "Running arithmetic tests" << std::endl;

    // Test addition
    {
        assert((Meter{1} + Meter{2.2}) == Meter{3.2});
        assert((Meter{1} + Kilometer{1}) == Kilometer{1.001});
        assert((Meter{1} + Kilometer{1}) == Meter{1001});

        static_assert((_CanOpC<Meter, "+", Kilometer>));
        static_assert((!_CanOpC<Meter, "+", double>));
        static_assert((!_CanOpC<double, "+", Meter>));
    }
    // Test subtraction
    {
        assert((Meter{1} - Meter{2.2}) == Meter{-1.2});
        assert((Meter{1} - Kilometer{1}) == Kilometer{-0.999});
        assert((Meter{1} - Kilometer{1}) == Meter{-999});

        static_assert((_CanOpC<Meter, "-", Kilometer>));
        static_assert((!_CanOpC<Meter, "-", double>));
        static_assert((!_CanOpC<double, "-", Meter>));
    }
    // Test multiplication with units
    {
        assert((Meter{1} * Meter{2.2}) == Meter_2{2.2});
        // assert((Meter{1} * Kilometer{1}) == Kilometer{1});
        assert((Meter{1} * Kilometer{1}) == Meter_2{1000});

        static_assert((_CanOpC<Meter, "-", Kilometer>));
        static_assert((!_CanOpC<Meter, "-", double>));
        static_assert((!_CanOpC<double, "-", Meter>));
    }

    // Test multiplication with scalars (right and left)

    // Test division

    // Test division with scalars (right and left)

    // Test +=, -=, *=, /=

    /** -- Run comparison tests --  */
    std::cout << "Running comparison tests" << std::endl;

    // Test Comparison

    /** -- Run concept tests --  */

    /** -- Run UnitMath tests --  */

    // ------------------------------------------------------------
    // Run Scalar tests
    // ------------------------------------------------------------
    {
        GeneralScalar auto myV = 10.0;
        GeneralScalar auto myV2 = 10.0;
        GeneralScalar auto myV3 = Meter{1};
        static_assert((_CanOpC<decltype(myV), "+", decltype(myV2)>));
        // myV + myV3;
    }

    return 0;
}