#include <cassert>

#include "../UnitLib/StringLiteral.h"
#include "../UnitLib/Unit.h"
#include "../UnitLib/Vector.h"
#include <iomanip>

template <typename T>
constexpr const char *type_name()
{
#if defined(__clang__) || defined(__GNUC__)
    return __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
    return __FUNCSIG__;
#else
    return "Unsupported compiler";
#endif
}

// ------------------------------------------------------------
// Testing library utils
// ------------------------------------------------------------

template <typename A, StringLiteral str, typename B>
constexpr bool CanOp()
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
    // Assignment
    else if constexpr (StrEq<str, "<">::value)
    {
        return requires(A a, B b) { a < b; };
    }
    else if constexpr (StrEq<str, "<=">::value)
    {
        return requires(A a, B b) { a <= b; };
    }
    else if constexpr (StrEq<str, ">">::value)
    {
        return requires(A a, B b) { a > b; };
    }
    else if constexpr (StrEq<str, ">=">::value)
    {
        return requires(A a, B b) { a >= b; };
    }
    else
    {
        return false;
    }
}


/** @brief Helper concept to check if a type supports IsZero */
template <typename T>
concept HasIsZero = requires(T t) {
    { t.IsZero() };
};

int main()
{
    // ------------------------------------------------------------
    // Run Math, lib, util tests
    // ------------------------------------------------------------
    std::cout << "------ BEGIN TESTING LIBRARIES ------" << std::endl;
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

        using Exp4 = RatioExp<std::ratio<64, 729>, std::ratio<2, 3>>;
        static_assert(Exp4::hasValue);
        static_assert(std::is_same_v<typename Exp4::value, std::ratio<16, 81>>);

        static_assert(!RatioCanExp<std::ratio<4>, std::ratio<1, 3>>);
        static_assert(RatioCanExp<std::ratio<4>, std::ratio<1, 2>>);
    }

    std::cout << "Running StringLiteral tests" << std::endl;
    {
        constexpr StringLiteral str1 = MakeStrLit<"=">::str;
        constexpr StringLiteral str2 = MakeStrLit<"==">::str;

        static_assert(CompareStrings<str1, str2>::value);
        static_assert(!CompareStrings<str2, str1>::value);
        static_assert(StrEq<str1, str1>::value);
        static_assert(!StrEq<str1, str2>::value);
    }

    // ------------------------------------------------------------
    // Run Unit tests
    // ------------------------------------------------------------

    std::cout << "------ BEGIN TESTING UNIT ------" << std::endl;

    // /** -- Define and test definitions */
    std::cout << "Running unit definition tests" << std::endl;

    using Meter = dAtomic<"meter">;
    static_assert(UnitExpableRatio<Meter, std::ratio<2>>);
    using Meter_2 = UnitExpI<Meter, 2>;
    static_assert(UnitExpableRatio<Meter, std::ratio<1, 2>>);
    using Meter_1_2 = UnitExp<Meter, std::ratio<1, 2>>;
    static_assert(UnitExpableRatio<Meter, std::ratio<-1>>);
    using Meter__1 = UnitExpI<Meter, -1>;
    static_assert(UnitExpableRatio<Meter, std::ratio<-2>>);
    using Meter__2 = UnitExpI<Meter, -2>;

    static_assert((std::is_same_v<Meter_1_2::uid, MakeUnitIdentifier<UnitBase<"meter", std::ratio<1, 2>>>>));
    static_assert((std::is_same_v<Meter_1_2::ratio, std::ratio<1>>));

    using fMeter = fAtomic<"meter">;
    using iMeter = iAtomic<"meter">;

    using Kilometer = UnitMultRatio<Meter, std::ratio<1000>>;
    static_assert(UnitExpableRatio<Meter, std::ratio<2>>);
    using Kilometer_2 = UnitExpI<Kilometer, 2>;
    static_assert(UnitExpableRatio<Meter, std::ratio<-1>>);
    using Kilometer__1 = UnitExpI<Kilometer, -1>;
    static_assert(UnitExpableRatio<Meter, std::ratio<-1>>);
    using Kilometer__2 = UnitExpI<Kilometer, -2>;

    static_assert((std::is_same_v<Kilometer::uid, Meter::uid>));
    static_assert((std::is_same_v<Kilometer::ratio, std::ratio<1000>>));

    static_assert((std::is_same_v<Kilometer_2::uid, Meter_2::uid>));
    static_assert((std::is_same_v<Kilometer_2::ratio, std::ratio<1000000>>));

    static_assert((std::is_same_v<Kilometer__1::uid, Meter__1::uid>));
    static_assert((std::is_same_v<Kilometer__1::ratio, std::ratio<1, 1000>>));

    static_assert((std::is_same_v<Kilometer__2::uid, Meter__2::uid>));
    static_assert((std::is_same_v<Kilometer__2::ratio, std::ratio<1, 1000000>>));

    // // Trying to construct this unit should fail because the ratio 1000 does not have a square root
    // // using Kilometer_1_2 = UnitExp<Kilometer, std::ratio<1,2>>;
    static_assert(!UnitExpableRatio<Kilometer, std::ratio<1, 2>>);

    using Second = dAtomic<"second">;
    using MeterPerSecond_2 = UnitMult<Meter, UnitExpI<Second, -2>>;
    static_assert((std::is_same_v<MeterPerSecond_2::uid, MakeUnitIdentifier<UnitBase<"meter", std::ratio<1>>, UnitBase<"second", std::ratio<-2>>>>));
    static_assert((std::is_same_v<MeterPerSecond_2::ratio, std::ratio<1>>));

    using dUEmpty = EmptyUnit<double>;
    using dUKilo = UnitMultRatio<dUEmpty, std::ratio<1000>>;

    // /** -- Run constructor tests --  */
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

        assert((CanOp<Meter, "==", Meter>()));
        assert((CanOp<Meter, "==", iMeter>()));

        assert((!CanOp<Meter, "==", Second>()));
        assert((!CanOp<Meter, "==", typename Meter::type>()));
        assert((!CanOp<Meter, "==", float>()));
        assert((!CanOp<Meter, "==", double>()));
        assert((!CanOp<Meter, "==", float>()));
    }

    // Comparison between empty units and plain scalars (left and right)
    {
        assert(dUEmpty{5.4} == 5.4);
        assert(dUKilo{5.4} == 5400);
        assert(dUKilo{1} + dUEmpty{1} == (float)1001);

        assert(5.4 == dUEmpty{5.4});
        assert(5400 == dUKilo{5.4});
        assert((float)1001 == dUKilo{1} + dUEmpty{1});

        assert((CanOp<float, "==", dUEmpty>()));
        assert((CanOp<dUEmpty, "==", int>()));
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

        assert((CanOp<Meter, "=", Kilometer>()));
        assert((CanOp<Meter, "=", iMeter>()));

        assert((CanOp<Kilometer, "=", fMeter>()));
        assert((CanOp<Kilometer, "=", Meter>()));

        assert((!CanOp<Meter, "=", Second>()));
        assert((!CanOp<Meter, "=", Meter_2>()));

        assert((!CanOp<Meter, "=", typename Meter::type>()));
        assert((!CanOp<Meter, "=", double>()));
        assert((!CanOp<Meter, "=", int>()));
        assert((!CanOp<Meter, "=", unsigned int>()));
        assert((!CanOp<Meter, "=", float>()));

        assert((!CanOp<typename Meter::type, "=", Meter>()));
    }

    // Conversion between empty units and plain scalars (right side only)
    {
        dUEmpty val{1.2};
        assert((val = 100.7) == dUEmpty{100.7});
        assert((val = 100.7) == 100.7);
        assert((val = dUKilo{1.001}) == 1001);

        val = dUKilo{1.2};
        assert(val == (int)1200);

        assert((CanOp<EmptyUnit<double>, "==", float>()));
        assert((CanOp<int, "==", EmptyUnit<float>>()));
    }

    /** -- Run arithmetic tests --  */
    std::cout << "Running arithmetic tests" << std::endl;

    // Test addition
    {
        assert((Meter{1} + Meter{2.2}) == Meter{3.2});
        assert((Meter{1} + Kilometer{1}) == Kilometer{1.001});
        assert((Meter{1} + Kilometer{1}) == Meter{1001});

        assert((CanOp<Meter, "+", Kilometer>()));
        assert((!CanOp<Meter, "+", double>()));
        assert((!CanOp<double, "+", Meter>()));
    }
    // Test subtraction
    {
        assert((Meter{1} - Meter{2.2}) == Meter{-1.2});
        assert((Meter{1} - Kilometer{1}) == Kilometer{-0.999});
        assert((Meter{1} - Kilometer{1}) == Meter{-999});

        assert((CanOp<Meter, "-", Kilometer>()));
        assert((!CanOp<Meter, "-", double>()));
        assert((!CanOp<double, "-", Meter>()));
    }
    // Test multiplication with units
    {
        assert((Meter{1} * Meter{2.2}) == Meter_2{2.2});
        assert((Meter{1} * Kilometer{1}) == Kilometer_2{0.001});
        assert((Meter{1} * Kilometer{1}) == Meter_2{1000});

        assert((CanOp<Meter, "*", Meter>()));
        assert((CanOp<Meter, "*", Kilometer>()));
        assert((CanOp<Meter, "*", Second>()));
    }

    // Test multiplication with scalars (right and left)
    {
        assert((1.7 * Meter{1}) == Meter{1.7});
        assert(((int)10 * Meter{1.7}) == Meter{17});
        assert(((float)10.0f * Meter{1.7}) == Meter{17});

        assert((Meter{1} * 1.7) == Meter{1.7});
        assert((Meter{1.7} * (int)10) == Meter{17});
        assert((Meter{1.7} * (float)10.0f) == Meter{17});

        static_assert((CanOp<Meter, "*", typename Meter::type>()));
        static_assert((CanOp<typename Meter::type, "*", Meter>()));

        static_assert((CanOp<Meter, "*", float>()));
        static_assert((CanOp<float, "*", Meter>()));

        static_assert((CanOp<Meter, "*", int>()));
        static_assert((CanOp<int, "*", Meter>()));

        static_assert((CanOp<Meter, "*", double>()));
        static_assert((CanOp<double, "*", Meter>()));
    }

    // Test division with units
    {
        // Division resulting in scalar
        assert((Meter{10} / Meter{2}) == dUEmpty{5.0});
        assert((Meter{10} / Meter{2}) == 5.0);
        assert((Kilometer{1} / Meter{1000}) == 1.0);
        assert((Meter{500} / Kilometer{1}) == 0.5);

        // Division resulting in derived units
        assert((Meter_2{10} / Meter{2}) == Meter{5});
        assert((Meter{1} / Second{2} / Second{2}) == MeterPerSecond_2{0.25});
        assert((dUEmpty{1} / Meter{2}) == (Meter__1{0.5}));

        static_assert((CanOp<Meter, "/", Second>()));
        static_assert((CanOp<Meter, "/", Kilometer>()));
        static_assert((CanOp<Meter, "/", dUEmpty>()));
    }

    // Test division with plain scalars (right and left)
    {
        assert((Meter{10} / 2) == Meter{5});
        assert((dUEmpty{10} / 2) == 5);
        assert((Kilometer{1} / (int)2) == Meter{500});

        assert((10.0 / Meter{2}) == Meter__1{5});
        assert((2 / dUEmpty{10}) == 0.2);
        assert((float)2 / Kilometer{1} == Meter__1{0.002});

        assert(2.4 == EmptyUnit<double>{2.4});
        assert(EmptyUnit<double>{2.4} == 2.4);

        static_assert((CanOp<double, "/", Meter>()));
        static_assert((CanOp<int, "/", Kilometer>()));
        static_assert((CanOp<float, "/", dUEmpty>()));
    }

    // Test addition and subtraction between empty units and scalars
    {
        assert((dUEmpty{1.0} + ((double)1.0) == 2.0));
        assert((((double)1.0) + dUEmpty{1.0} == 2.0));
        assert((dUKilo{1.0} + ((double)1.0) == 1001));

        assert((dUEmpty{2.0} - ((double)1.0) == 1.0));
        assert((((double)2.0) - dUEmpty{1.0} == 1.0));
        assert((dUKilo{1.0} - ((double)1.0) == 999));
    }

    // // Test +=, -=, *=, /=

    // Test +=
    std::cout << "Running arithmetic assignment tests" << std::endl;
    {
        Meter myVal{1.2};
        myVal += Meter{1.0};
        assert((myVal == Meter{2.2}));
        assert((myVal += Meter{0.8}) == Meter{3});

        myVal += Kilometer{1};
        assert((myVal += Kilometer{1}) == Meter{2003});

        assert((CanOp<Meter, "+=", Meter>()));
        assert((CanOp<Meter, "+=", Kilometer>()));
        assert((CanOp<Meter, "+=", iMeter>()));
        assert((!CanOp<Meter, "+=", double>()));
        assert((!CanOp<Meter, "+=", float>()));
    }
    // Test -=
    {
        Meter myVal{1.2};
        myVal -= Meter{1.0};
        assert((myVal == Meter{0.2}));
        assert((myVal -= Meter{0.2}) == Meter{0});

        myVal -= Kilometer{1};
        assert((myVal -= Kilometer{1}) == Meter{-2000});

        assert((CanOp<Meter, "-=", Meter>()));
        assert((CanOp<Meter, "-=", Kilometer>()));
        assert((CanOp<Meter, "-=", iMeter>()));
        assert((!CanOp<Meter, "-=", double>()));
        assert((!CanOp<Meter, "-=", float>()));
    }
    // Test *=
    {
        Meter myVal{1.0};
        myVal *= 2;
        assert((myVal == Meter{2.0}));
        assert((myVal *= 2.0) == Meter{4.0});

        myVal *= dUKilo{1};
        assert(myVal == Meter{4000});
        assert((myVal *= dUKilo{0.01}) == Meter{40000});

        assert((CanOp<Meter, "*=", double>()));
        assert((CanOp<Meter, "*=", int>()));
        assert((CanOp<Meter, "*=", float>()));
        assert((CanOp<Meter, "*=", dUEmpty>()));
        assert((CanOp<Meter, "*=", dUKilo>()));
        assert((!CanOp<Meter, "*=", Meter>()));
        assert((!CanOp<Meter, "*=", Second>()));
    }
    // Test /=
    {
        Meter myVal{1.0};
        myVal /= 2;
        assert((myVal == Meter{0.5}));
        assert((myVal /= 2.0) == Meter{0.25});

        myVal /= dUKilo{1};
        assert(myVal == Meter{0.00025});
        assert((myVal /= dUKilo{0.0001}) == Meter{0.0025});

        assert((CanOp<Meter, "/=", double>()));
        assert((CanOp<Meter, "/=", int>()));
        assert((CanOp<Meter, "/=", float>()));
        assert((CanOp<Meter, "/=", dUEmpty>()));
        assert((CanOp<Meter, "/=", dUKilo>()));
        assert((!CanOp<Meter, "/=", Meter>()));
        assert((!CanOp<Meter, "/=", Second>()));
    }

    // /** -- Run comparison tests --  */
    std::cout << "Running comparison tests" << std::endl;

    // Test <
    {
        assert(Meter{1} < Meter{2});
        assert(Meter{999} < Kilometer{1});
        assert(Kilometer{0.999} < Meter{1000});
        assert(Meter{999} < iMeter{1000});

        assert((CanOp<Meter, "<", Meter>()));
        assert((CanOp<Meter, "<", Kilometer>()));
        assert((CanOp<iMeter, "<", Kilometer>()));
        assert((CanOp<Kilometer, "<", iMeter>()));
    }
    // Test <=
    {
        assert(Meter{1} <= Meter{2});
        assert(Meter{1000} <= Kilometer{1});
        assert(Kilometer{0.999} <= Meter{1000});
        assert(Meter{1000} <= iMeter{1000});

        assert((CanOp<Meter, "<=", Meter>()));
        assert((CanOp<Meter, "<=", Kilometer>()));
        assert((CanOp<iMeter, "<=", Kilometer>()));
        assert((CanOp<Kilometer, "<=", iMeter>()));
    }

    // Test >
    {
        assert(Meter{2} > Meter{1});
        assert(Kilometer{1} > Meter{999});
        assert(Meter{1000} > Kilometer{0.999});
        assert(iMeter{1000} > Meter{999});

        assert((CanOp<Meter, ">", Meter>()));
        assert((CanOp<Meter, ">", Kilometer>()));
        assert((CanOp<iMeter, ">", Kilometer>()));
        assert((CanOp<Kilometer, ">", iMeter>()));
    }

    // Test >=
    {
        assert(Meter{2} >= Meter{1});
        assert(Kilometer{1} >= Meter{1000});
        assert(Meter{1000} >= Kilometer{0.999});
        assert(iMeter{1000} >= Meter{1000});

        assert((CanOp<Meter, ">=", Meter>()));
        assert((CanOp<Meter, ">=", Kilometer>()));
        assert((CanOp<iMeter, ">=", Kilometer>()));
        assert((CanOp<Kilometer, ">=", iMeter>()));
    }

    // /** -- Run concept tests --  */

    // /** -- Run UnitMath tests --  */
    // unit_sqrt
    {
        Meter_2 v{100};
        assert(( unit_sqrt(v) == Meter{10} ));
    }

    // unit_pow

    // ------------------------------------------------------------
    // Run Scalar tests
    // ------------------------------------------------------------

    std::cout << "------ BEGIN TESTING SCALAR ------" << std::endl;
    std::cout << "Running scalar tests" << std::endl;
    {
        GeneralScalar auto myV = 10.0;
        GeneralScalar auto myV2 = 10.0;
        GeneralScalar auto myV3 = Meter{1};
        static_assert((CanOp<decltype(myV), "+", decltype(myV2)>()));
    }

    // ------------------------------------------------------------
    // Run Vector tests
    // ------------------------------------------------------------
    std::cout << "------ BEGIN TESTING VECTOR ------" << std::endl;
    std::cout << "Running constructor tests" << std::endl;
    {
        Vector2<double> v0;
        assert(v0[0] == 0 && v0[1] == 0);

        Vector2<Meter> v1;
        assert(v1[0] == Meter{0} && v1[1] == Meter{0});

        Vector3<double> v2{1, 2};
        assert(v2[0] == 1 && v2[1] == 2 && v2[2] == 0);

        Vector3<Meter> v3{Meter{1}, Meter{2}};
        assert(v3[0] == Meter{1} && v3[1] == Meter{2} && v3[2] == Meter{0});

        Vector3<Meter> v4{1, 2};
        assert(v4[0] == Meter{1} && v4[1] == Meter{2} && v4[2] == Meter{0});

        static_assert(std::is_constructible_v<Vector2<double>>);
        static_assert(std::is_constructible_v<Vector2<double>, int>);
        static_assert(std::is_constructible_v<Vector2<double>, int, float>);
        static_assert(!std::is_constructible_v<Vector2<double>, double, double, double>);

        static_assert(std::is_constructible_v<Vector2<Meter>>);
        static_assert(std::is_constructible_v<Vector2<Meter>, Meter>);
        static_assert(std::is_constructible_v<Vector2<Meter>, double>);
        static_assert(std::is_constructible_v<Vector2<Meter>, double, double>);
        static_assert(std::is_constructible_v<Vector2<Meter>, double, Meter>);
        static_assert(!std::is_constructible_v<Vector2<Meter>, double, double, double>);

        Vector<100, Kilometer> v5{1, 2, 3};
        assert(v5[0] == Meter{1000} && v5[1] == Meter{2000} && v5[2] == Meter{3000});
        for (size_t i = 3; i < 100; i++)
        {
            assert(v5[i] == Meter{0});
        }

        Vector2<std::string> strV{"hi", "bye"};
        assert(strV.x() == "hi");
    }
    std::cout << "Running accessor tests" << std::endl;
    // Accessors
    {
        Vector3<float> myVec{1, 2, 3};
        const std::array<float, 3> &arr = myVec.GetData();
        assert(arr[0] == 1 && arr[1] == 2 && arr[2] == 3);

        assert(myVec[0] == 1 && myVec[1] == (double)2 && myVec[2] == 3);
        assert(myVec.x() == 1 && myVec.y() == (double)2 && myVec.z() == 3);

        myVec[0] = 10;
        assert(myVec[0] == 10);

        Vector<100, double> myVec2;
        myVec2[63] = 13.7;
        assert(myVec2[63] == 13.7);

        myVec.y() = 12.4f;
        assert(myVec[1] == 12.4f);

        Vector<5, double> myVec3{1, 2, 3, 4, 5};
        assert(myVec3.x() == 1 && myVec3.y() == 2 && myVec3.z() == 3 && myVec3.w() == 4);

        const float val = myVec[1];
        assert(val == 12.4f);
        assert((!CanOp<decltype(val), "=", float>()));

        double &val2 = myVec3.z();
        val2 = 10.7;
        assert(myVec3[2] == 10.7 && myVec3.z() == 10.7);
    }

    std::cout << "Running equality and assignment tests" << std::endl;
    // Equality
    {
        Vector3<Meter> v1{1000, 2000, 3000};
        Vector3<Meter> v2{1000, 2000, 3000};
        Vector3<Kilometer> v3{1, 2, 3};

        assert(v1 == v3);
        assert(v1 == v2);

        v2[0] = Meter{10};
        assert(v1 != v2);

        assert((CanOp<Vector3<Meter>, "==", Vector3<Kilometer>>()));
        assert((!CanOp<Vector3<Meter>, "==", Vector3<double>>()));

        Vector2<double> v4{1, 2};
        Vector2<dUEmpty> v5{1, 2};
        assert((CanOp<Vector3<double>, "==", Vector3<EmptyUnit<double>>>()));

        assert(v4 == v5);

        assert((Vector2<std::string>{"hi", "bye"} == Vector2<std::string>{"hi", "bye"}));
    }
    // Assignment
    {
        Vector3<double> v1;
        Vector3<double> v2{1, 2, 3};
        assert((v1 == Vector3<double>{0, 0, 0}));
        v1 = v2;
        assert((v1 == Vector3<double>{1, 2, 3}));
        assert(v1 == v2);

        assert(((v1 = Vector3<double>{4, 5, 6}) == Vector3<double>{4, 5, 6}));

        Vector3<Meter> v3;
        assert(((v3 = Vector3<Kilometer>{1, 1, 1}) == Vector3<Meter>{1000, 1000, 1000}));
    }
    // Checking IsZero
    {
        Vector2<std::string> strVec{"hi", "bye"};
        Vector2<double> dVec{1, 2};

        assert(!dVec.IsZero());
        assert((Vector3<double>{}).IsZero());

        static_assert((!HasIsZero<Vector2<std::string>>));
        static_assert((HasIsZero<Vector2<double>>));
    };
    std::cout << "Running arithmetic tests" << std::endl;
    // Componentwise Addition
    {
        assert((Vector3<Kilometer>{1, 1, 1} + Vector3<Meter>{1, 1, 1} == Vector3<Meter>{1001, 1001, 1001}));
        assert((Vector3<Kilometer>{1, 1, 1} + Vector3<Meter>{1, 1, 1} == Vector3<Kilometer>{1.001, 1.001, 1.001}));
        assert((Vector3<double>{1.2, 1.2, 1.2} + Vector3<int>{1, 1, 1} == Vector3<double>{2.2, 2.2, 2.2}));

        assert((CanOp<Vector3<Meter>, "+", Vector3<Kilometer>>()));
        assert((!CanOp<Vector3<Meter>, "+", Vector3<double>>()));
        // assert((CanOp< Vector3<float>, "+", Vector3<dUEmpty> >()));
    };
    // Componentwise Subtraction
    {
        assert((Vector3<int>{5, 5, 5} - Vector3<int>{3, 3, 3} == Vector3<int>{2, 2, 2}));
        assert((Vector3<double>{5.5, 5.5, 5.5} - Vector3<int>{2, 2, 2} == Vector3<double>{3.5, 3.5, 3.5}));
        assert((Vector3<Kilometer>{5, 5, 5} - Vector3<Meter>{500, 500, 500} == Vector3<Kilometer>{4.5, 4.5, 4.5}));

        assert((CanOp<Vector3<int>, "-", Vector3<int>>()));
        assert((CanOp<Vector3<Kilometer>, "-", Vector3<Meter>>()));
        assert((!CanOp<Vector3<Meter>, "-", Vector3<double>>()));
    };

    // Componentwise Multiplication
    {
        assert((Vector3<int>{2, 2, 2} * Vector3<int>{3, 3, 3} == Vector3<int>{6, 6, 6}));
        assert((Vector3<double>{1.5, 1.5, 1.5} * Vector3<int>{2, 2, 2} == Vector3<double>{3.0, 3.0, 3.0}));
        assert((Vector3<Meter>{2, 2, 2} * Vector3<int>{3, 3, 3} == Vector3<Meter>{6, 6, 6}));
        assert((Vector3<Meter>{2, 2, 2} * Vector3<Meter>{2, 2, 2} == Vector3<Meter_2>{4, 4, 4}));

        assert((CanOp<Vector3<int>, "*", Vector3<int>>()));
        assert((CanOp<Vector3<Meter>, "*", Vector3<int>>()));
        assert((CanOp<Vector3<Meter>, "*", Vector3<double>>()));
    };

    // Componentwise Division
    {
        assert((Vector3<int>{6, 6, 6} / Vector3<int>{3, 3, 3} == Vector3<int>{2, 2, 2}));
        assert((Vector3<double>{6.0, 6.0, 6.0} / Vector3<int>{2, 2, 2} == Vector3<double>{3.0, 3.0, 3.0}));
        assert((Vector3<Meter>{500, 500, 500} / Vector3<int>{2, 2, 2} == Vector3<Meter>{250, 250, 250}));
        assert((Vector3<Meter>{4, 4, 4} / Vector3<Meter>{2, 2, 2} == Vector3<dUEmpty>{2, 2, 2}));
        assert((Vector3<Meter>{4, 4, 4} / Vector3<Meter>{2, 2, 2} == Vector3<double>{2, 2, 2}));

        assert((CanOp<Vector3<int>, "/", Vector3<int>>()));
        assert((CanOp<Vector3<Meter>, "/", Vector3<int>>()));
        assert((CanOp<Vector3<Meter>, "/", Vector3<double>>()));
    };

    // Scalar multiplication (right and left)
    {
        Vector2<double> v1{1, 2};
        assert((v1 * 2 == Vector2<double>{2, 4}));

        assert((Meter{2} * v1 == Vector2<Meter>{2, 4}));

        assert((CanOp<Vector3<Meter>, "*", Kilometer>()));
        assert((CanOp<Vector3<Meter>, "*", int>()));

        assert((CanOp<float, "*", Vector3<int>>()));
        assert((CanOp<Second, "*", Vector3<Kilometer>>()));

        assert((!CanOp<Vector3<std::string>, "*", int>()));
    };

    // Scalar division (right only)
    {
        Vector2<double> v1{1, 2};
        assert((v1 / 2 == Vector2<double>{0.5, 1}));
        assert((v1 / Meter{2} == Vector2<Meter__1>{0.5, 1}));
        assert((Vector2<Kilometer>{1, 1} / Meter{1} == Vector2<double>{1000, 1000}));

        assert((CanOp<Vector3<Meter>, "/", Kilometer>()));
        assert((CanOp<Vector3<Meter>, "/", int>()));
        assert((CanOp<Vector3<Meter>, "/", dUEmpty>()));
    }

    std::cout << "Running arithmetic assignment tests" << std::endl;
    // Addition assignment +=
    {
        Vector2<double> v{1, 2};
        v += Vector2<int>{2, 1};
        assert((v == Vector2<double>{3, 3}));
        assert(((v += Vector2<double>{-1, -1}) == Vector2<int>{2, 2}));

        Vector2<Meter> v2{1, 1};
        assert(((v2 += Vector2<Kilometer>{1, 1}) == Vector2<Meter>{1001, 1001}));
    }
    // Subtraction assignment -=
    {
        Vector2<double> v{1, 1};
        v -= Vector2<int>{2, 2};
        assert((v == Vector2<double>{-1, -1}));
        assert(((v -= Vector2<double>{-1, -1}) == Vector2<int>{0, 0}));

        Vector2<Meter> v2{1, 1};
        assert(((v2 -= Vector2<Kilometer>{1, 1}) == Vector2<Meter>{-999, -999}));
    }
    // Multiplication assignment *=
    {
        Vector2<double> v{1, 1};
        v *= 2;
        assert((v == Vector2<double>{2, 2}));
        assert(((v *= Vector2<double>{1, 2}) == Vector2<double>{2, 4}));
        assert(((v *= 2) == Vector2<double>{4, 8}));

        Vector2<Meter> v2{1, 1};
        assert(((v2 *= Vector2<dUKilo>{1, 2}) == Vector2<Meter>{1000, 2000}));
    }
    // Division assignment /=
    {
        Vector2<double> v{1, 1};
        v /= 2;
        assert((v == Vector2<double>{0.5, 0.5}));
        assert(((v /= Vector2<double>{1, 2}) == Vector2<double>{0.5, 0.25}));
        assert(((v /= 2) == Vector2<double>{0.25, 0.125}));

        Vector2<Meter> v2{1, 2};
        assert(((v2 /= Vector2<dUKilo>{1, 2}) == Vector2<Meter>{0.001, 0.001}));
    }

    std::cout << "Running norm tests" << std::endl;
    // NormSquared
    {
        assert(( VectorHasNormSquared<double> ));
        assert(( VectorHasNormSquared<Meter> ));
        assert(( VectorHasNormSquared<Kilometer> ));
        assert(( !VectorHasNormSquared<std::string> ));

        Vector2<double>v{2, 0};
        assert(( NormSquared(v) == 4 ));
        assert(( NormSquared(Vector2<Meter>{3, 4}) == Meter_2{25} )); 
        assert(( NormSquared(Vector2<Kilometer>{0.003, 0.004}) == Meter_2{25} )); 
    }
    // Norm
    {
        assert(( VectorHasNorm<double> ));
        assert(( VectorHasNorm<Meter> ));
        assert(( VectorHasNorm<Kilometer> ));
        assert(( !VectorHasNorm<std::string> ));

        Vector2<double> v{2, 0};
        assert(( Norm(v) == 2 ));
        assert(( Norm(Vector2<Meter>{3, 4}) == Meter{5} )); 
        assert(( Norm(Vector2<Kilometer>{0.003, 0.004}) == Meter{5} )); 
    }
    // Norm_d
    {
        assert(( Norm_d(Vector2<double>{3, 4}) == 5 ));
        assert(( Norm_d(Vector2<float>{3, 4}) == 5 ));
        assert(( Norm_d(Vector2<Meter>{3, 4}) == 5 ));
        assert(( Norm_d(Vector2<Kilometer>{3, 4}) == 5000 ));
    }
    

    std::cout << "Running product tests" << std::endl;
    // Dot product
    // Cross product


    return 0;
}