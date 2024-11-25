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
    if constexpr (StrEq<str, "=">::value)
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
concept CanOp = _CanOp<A, str, B>();

int main()
{
    // ------------------------------------------------------------
    // Run Math, lib, util tests
    // ------------------------------------------------------------
    std::cout << "Running ratio tests" << std::endl;
    /** -- Test helpers -- */
    {
        static_assert((IntPow<2, 10>() == 1024));
        static_assert((std::is_same_v<RatioPowI<std::ratio<2, 3>, 3>, std::ratio<8, 27>>));
        static_assert((IsIntegralRoot<1024, 2, 10>()));

        static_assert((HighestRoot<100, 3>() == 5));
        static_assert((HighestRoot<37, 1>() == 37));
        static_assert((HighestRoot<9, 2>() == 3));
        static_assert((HighestRoot<1000, 3>() == 10));
        static_assert((HighestRoot<1000, 2>() == 32));
        static_assert((HighestRoot<5, 2>() == 3));

        static_assert((HasIntegralRoot<37, 1>));
        static_assert((HasIntegralRoot<1000, 3>));
        static_assert((!HasIntegralRoot<1000, 2>));

        using Exp1 = RatioExp<std::ratio<4>, std::ratio<3>>;
        static_assert(Exp1::hasValue);
        static_assert(std::is_same_v<typename Exp1::value, std::ratio<64>>);

        using Exp2 = RatioExp<std::ratio<4>, std::ratio<1, 2>>;
        static_assert(Exp2::hasValue);
        static_assert(std::is_same_v<typename Exp2::value, std::ratio<2>>);

        using Exp3 = RatioExp<std::ratio<5>, std::ratio<1, 2>>;
        static_assert(!Exp3::hasValue);

        using Exp4 = RatioExp<std::ratio<64,729>, std::ratio<2, 3>>;
        static_assert(Exp4::hasValue);
        static_assert(std::is_same_v<typename Exp4::value, std::ratio<16, 81>>);

        static_assert(!RatioCanExp<std::ratio<4>, std::ratio<1, 3>>);
        static_assert(RatioCanExp<std::ratio<4>, std::ratio<1, 2>>);
    }

    // ------------------------------------------------------------
    // Run Unit tests
    // ------------------------------------------------------------

    /** -- Define and test definitions */
    using Meter = dAtomic<"meter">;
    static_assert( UnitExpableRatio<Meter, std::ratio<2>> );
    using Meter_2 = UnitExpI<Meter, 2>;
    static_assert( UnitExpableRatio<Meter, std::ratio<1, 2>> );
    using Meter_1_2 = UnitExp<Meter, std::ratio<1, 2>>;

    static_assert((std::is_same_v<Meter_1_2::uid, MakeUnitIdentifier<UnitBase<"meter", std::ratio<1, 2>>>>));
    static_assert((std::is_same_v<Meter_1_2::ratio, std::ratio<1>>));
     
    using fMeter = fAtomic<"meter">;
    using iMeter = iAtomic<"meter">;
    
    using Kilometer = UnitMultRatio<Meter, std::ratio<1000>>;
    static_assert( UnitExpableRatio<Meter, std::ratio<2>> );
    using Kilometer_2 = UnitExpI<Kilometer, 2>;

    static_assert((std::is_same_v<Kilometer_2::uid, MakeUnitIdentifier<UnitBase<"meter", std::ratio<2>>>>));
    static_assert((std::is_same_v<Kilometer_2::ratio, std::ratio<1000000>>));

    // Trying to construct this unit should fail because the ratio 1000 does not have a square root
    // using Kilometer_1_2 = UnitExp<Kilometer, std::ratio<1,2>>;
    static_assert( !UnitExpableRatio<Kilometer, std::ratio<1,2>> );
    

    using Second = dAtomic<"second">;
    // using Second__2 = UnitExpI<Second, -2>;


    // std::cout << Kilometer_1_2{} << std::endl;

    /** -- Run constructor tests --  */
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

        static_assert((CanOp<Meter, "=", Kilometer>));
        static_assert((CanOp<Meter, "=", iMeter>));

        static_assert((CanOp<Kilometer, "=", fMeter>));
        static_assert((CanOp<Kilometer, "=", Meter>));

        static_assert((!CanOp<Meter, "=", Second>));
        static_assert((!CanOp<Meter, "=", Meter_2>));

        static_assert((CanOp<Meter, "=", typename Meter::type>));
        static_assert((CanOp<Meter, "=", double>));
        static_assert((CanOp<Meter, "=", int>));
        static_assert((CanOp<Meter, "=", unsigned int>));
        static_assert((CanOp<Meter, "=", float>));

        static_assert((CanOp<iMeter, "=", typename Meter::type>));
        static_assert((CanOp<iMeter, "=", double>));
        static_assert((CanOp<iMeter, "=", int>));
        static_assert((CanOp<iMeter, "=", unsigned int>));
        static_assert((CanOp<iMeter, "=", float>));

        static_assert((!CanOp<typename Meter::type, "=", Meter>));
    }

    /** -- Run arithmetic tests --  */
    std::cout << "Running arithmetic tests" << std::endl;

    // Test addition
    {
        assert((Meter{1} + Meter{2.2}) == Meter{3.2});
        assert((Meter{1} + Kilometer{1}) == Kilometer{1.001});
        assert((Meter{1} + Kilometer{1}) == Meter{1001});

        static_assert((CanOp<Meter, "+", Kilometer>));
        static_assert((!CanOp<Meter, "+", double>));
        static_assert((!CanOp<double, "+", Meter>));
    }
    // Test subtraction
    {
        assert((Meter{1} - Meter{2.2}) == Meter{-1.2});
        assert((Meter{1} - Kilometer{1}) == Kilometer{-0.999});
        assert((Meter{1} - Kilometer{1}) == Meter{-999});

        static_assert((CanOp<Meter, "-", Kilometer>));
        static_assert((!CanOp<Meter, "-", double>));
        static_assert((!CanOp<double, "-", Meter>));
    }
    // Test multiplication with units
    {
        assert((Meter{1} * Meter{2.2}) == Meter_2{2.2});
        // assert((Meter{1} * Kilometer{1}) == Kilometer{1});
        assert((Meter{1} * Kilometer{1}) == Meter_2{1000});

        static_assert((CanOp<Meter, "-", Kilometer>));
        static_assert((!CanOp<Meter, "-", double>));
        static_assert((!CanOp<double, "-", Meter>));
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
        static_assert((CanOp<decltype(myV), "+", decltype(myV2)>));
        // myV + myV3;
    }

    return 0;
}