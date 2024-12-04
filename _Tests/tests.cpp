#include <cassert>

#include "../UnitLib/Unit.h"
#include "../UnitLib/UnitMath.h"
#include "../UnitLib/Vector.h"
#include "../UnitLib/VectorMath.h"
#include "../UnitLib/Matrix.h"
#include "../UnitLib/Print.h"

<<<<<<< HEAD
#include "../PhysicsLib/Actor.h"
#include "../PhysicsLib/Collision.h"
=======
#include "AdditiveString.h"
#include "PrimeField.h"
>>>>>>> 1d92a2ced78b45b9137ee982ca8d08d6643bd351

#include <iomanip>

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

template <typename T, typename Ratio>
concept CanUnitMultRatio = IsRatio<Ratio> && requires {
    UnitMultRatio<T, Ratio>{};
};

/** @brief Helper concept to check if a type supports IsZero */
template <typename T>
concept HasIsZero = requires(T t) {
    { t.IsZero() };
};

/** @brief Helper concept to check if determinant is defined for a matrix */
template <typename T>
concept HasDet = IsMatrix<T> && requires(T a) {
    { Det(a) };
};

/** @brief Helper concept to check if inverse is defined for a matrix */
template <typename T>
concept HasInverse = IsMatrix<T> && requires(T a) {
    { Inv(a) };
};

/** @brief Helper concept to check if identity is defined for a matrix */
template <typename T>
concept HasIdentity = IsMatrix<T> && requires(T a) {
    { T::Identity() };
};

/** @brief Helper concept to check if multiplication is defined for two types */
template <typename A, typename B>
concept IsMultDefined = requires(A a, B b) {
    { a *b };
};

/** @brief Helper function to compare floating-point numbers */
bool NearlyEqual(double a, double b, double epsilon = 1e-6)
{
    return std::fabs(a - b) < epsilon;
}

// ------------------------------------------------------------
// Actor and Collision Tests
// ------------------------------------------------------------
void TestActorInitialization()
{
    // Test default initialization
    Actor<double> actor({0.0, 0.0}, {0.0, 0.0}, 0.0, 0.0);
    assert((actor.position == Vector2<double>{0.0, 0.0}));
    assert((actor.velocity == Vector2<double>{0.0, 0.0}));
    assert((NearlyEqual(actor.rotation, 0.0)));
    assert((NearlyEqual(actor.rotationSpeed, 0.0)));
    assert((actor.state == ActorState::Idle));

    // Test initialization with values
    Actor<double> actor2({1.0, 2.0}, {0.5, -0.5}, M_PI / 4, 0.1);
    assert((actor2.position == Vector2<double>{1.0, 2.0}));
    assert((actor2.velocity == Vector2<double>{0.5, -0.5}));
    assert((NearlyEqual(actor2.rotation, M_PI / 4)));
    assert((NearlyEqual(actor2.rotationSpeed, 0.1)));
    assert((actor2.state == ActorState::Idle));

    std::cout << "TestActorInitialization passed.\n";
}

void TestActorMovement()
{
    Actor<double> actor({0.0, 0.0}, {1.0, 0.0}, 0.0, 0.0);
    actor.Update(1.0); // Update for 1 second
    assert(NearlyEqual(actor.position.x(), 1.0));
    assert(NearlyEqual(actor.position.y(), 0.0));
    assert(NearlyEqual(actor.rotation, 0.0));

    actor.Update(0.5); // Update for another 0.5 seconds
    assert(NearlyEqual(actor.position.x(), 1.5));
    assert(NearlyEqual(actor.position.y(), 0.0));

    std::cout << "TestActorMovement passed.\n";
}

void TestActorRotation()
{
    Actor<double> actor({0.0, 0.0}, {0.0, 0.0}, 0.0, M_PI / 2); // 90 degrees per second
    actor.Update(1.0); // Update for 1 second
    assert(NearlyEqual(actor.rotation, M_PI / 2));

    actor.Update(1.0); // Update for another 1 second
    assert(NearlyEqual(actor.rotation, M_PI)); // Should be 180 degrees

    actor.Update(2.0); // Update for another 2 seconds
    assert(NearlyEqual(actor.rotation, 0.0)); // Full rotation back to 0

    std::cout << "TestActorRotation passed.\n";
}

void TestActorMovementWithRotation() {
    Actor<double> actor({0.0, 0.0}, {1.0, 0.0}, 0.0, M_PI / 4); // Rotate 45 degrees/sec
    actor.Update(1.0); // Update for 1 second

    double expectedX = std::sqrt(2) / 2; // cos(45°)
    double expectedY = std::sqrt(2) / 2; // sin(45°)

    std::cout << "Debug: actor.position.x() = " << actor.position.x()
              << ", actor.position.y() = " << actor.position.y() << "\n";
    std::cout << "Expected: (" << expectedX << ", " << expectedY << ")\n";

    assert(NearlyEqual(actor.position.x(), expectedX));
    assert(NearlyEqual(actor.position.y(), expectedY));

    std::cout << "TestActorMovementWithRotation passed.\n";
}



void TestCollisionDetection()
{
    AABB a = {0.0, 0.0, 1.0, 1.0};
    AABB b = {0.5, 0.5, 1.0, 1.0};
    AABB c = {2.0, 2.0, 1.0, 1.0};

    assert(CheckCollision(a, b)); // Should collide
    assert(!CheckCollision(a, c)); // Should not collide

    std::cout << "TestCollisionDetection passed.\n";
}

void TestActorCollisionResponse()
{
    Actor<double> actor({0.0, 0.0}, {1.0, 0.0}, 0.0, 0.0);
    AABB solid = {1.0, -0.5, 1.0, 1.0};

    // Simulate until collision
    for (int i = 0; i < 2; ++i)
    {
        actor.Update(0.5);
    }

    // Check collision
    assert(CheckCollision(actor.boundingBox, solid));

    // Apply collision response
    actor.SetState(ActorState::Colliding);

    // Velocity should be reversed and damped
    assert(NearlyEqual(actor.velocity.x(), -0.8));
    assert(NearlyEqual(actor.velocity.y(), 0.0));

    std::cout << "TestActorCollisionResponse passed.\n";
}

void TestActorRotationAfterCollision()
{
    Actor<double> actor({0.0, 0.0}, {1.0, 0.0}, 0.0, M_PI / 4);
    AABB solid = {1.0, -0.5, 1.0, 1.0};

    double totalTime = 0.0;

    // Simulate until collision
    for (int i = 0; i < 2; ++i)
    {
        actor.Update(0.5);
        totalTime += 0.5;
    }

    assert(CheckCollision(actor.boundingBox, solid));

    actor.SetState(ActorState::Colliding);

    actor.Update(0.5);
    totalTime += 0.5;

    double expectedRotation = actor.rotationSpeed * totalTime;

    std::cout << "Debug: actor.rotation = " << actor.rotation << "\n";
    std::cout << "Expected rotation: " << expectedRotation << "\n";

    assert(NearlyEqual(actor.rotation, expectedRotation));

    std::cout << "TestActorRotationAfterCollision passed.\n";
}

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

    std::cout << "Running AdditiveString tests" << std::endl;
    {
        assert((AdditiveString{"hibye"} + AdditiveString{"bye"} == AdditiveString{"hibyebye"}));
        assert((AdditiveString{"hibye"} - AdditiveString{"bye"} == AdditiveString{"hi"}));

        assert((!CanOp<AdditiveString, "*", AdditiveString>()));
        assert((!CanOp<AdditiveString, "/", AdditiveString>()));
    }

    std::cout << "Running PrimeField tests" << std::endl;
    {
        assert(Z7{4} + Z7{5} == Z7{2});
        assert(Z7{4} - Z7{5} == Z7{6});

        assert(Z7{4} * Z7{5} == Z7{6});
        assert(Z7{6} / Z7{5} == Z7{4});
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
    using MeterPerSecond_2 = MultiplyType<Meter, UnitExpI<Second, -2>>;
    static_assert((std::is_same_v<MeterPerSecond_2::uid, MakeUnitIdentifier<UnitBase<"meter", std::ratio<1>>, UnitBase<"second", std::ratio<-2>>>>));
    static_assert((std::is_same_v<MeterPerSecond_2::ratio, std::ratio<1>>));

    using dUEmpty = EmptyUnit<double>;
    using dUKilo = UnitMultRatio<dUEmpty, std::ratio<1000>>;

    // User-defined types
    using StrUnit = TypeAtomic<AdditiveString, "string">;
    assert((!CanUnitMultRatio<StrUnit, std::ratio<2>>));

    using Z7Unit = TypeAtomic<Z7, "z7">;
    using Z7Unit_2 = UnitExpI<Z7Unit, 2>;
    using Z7Unit_Double = UnitMultRatio<Z7Unit, std::ratio<2>>;
    using Z7Unit_Half = UnitMultRatio<Z7Unit, std::ratio<1, 2>>;

    // /** -- Run constructor tests --  */
    std::cout
        << "Running constructor tests" << std::endl;
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
        // Basic comparisons
        assert(Meter{1} == Meter{1});

        // Basic incompatibility checks
        assert((!CanOp<Meter, "==", Second>()));
        assert((!CanOp<Meter, "==", typename Meter::type>()));
        assert((!CanOp<Meter, "==", float>()));
        assert((!CanOp<Meter, "==", double>()));

        // Comparing units of different ratios
        assert(Meter{1000} == Kilometer{1});
        assert(Meter{1} == Kilometer{0.001});
        assert((dUEmpty{1000} == dUKilo{1}));

        // Comparing units of different underlying types
        assert((CanOp<Meter, "==", iMeter>()));
        assert((Meter{1.0} == iMeter{1}));

        // Comparing non-arithmetic types
        assert((TypeAtomic<std::string, "meter">{"hi"} == TypeAtomic<std::string, "meter">{"hi"}));

        // Comparing empty units and plaintypes
        assert(dUEmpty{1.0} == 1.0);
        assert(1.0 == dUEmpty{1.0});

        // Comparing empty units and plain non-arithmetics
        assert(EmptyUnit<std::string>{"hi"} == std::string{"hi"});
        assert(std::string{"hi"} == EmptyUnit<std::string>{"hi"});
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

    // Real value getters
    {
        assert(((Kilometer{1}).GetRealValue() == 1000));
        assert(((Kilometer{1}).GetBaseUnitValue() == Meter{1000}));
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

    // Assignment between empty units and plain scalars (right side)
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
    // Left-side assignment with plain scalars for empty units
    {
        double x = dUEmpty{1.0};
        assert((x == 1.0));
        assert((x = dUKilo{1.0}) == 1000);
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

        assert((std::is_same_v<decltype(Kilometer{1} + Meter{1})::ratio, std::ratio<1000>>));
        assert((std::is_same_v<decltype(Kilometer{1} + Kilometer{1})::ratio, std::ratio<1000>>));

        using HalfMeter = UnitMultRatio<Meter, std::ratio<1, 2>>;
        using ThirdMeter = UnitMultRatio<Meter, std::ratio<1, 3>>;

        assert((std::is_same_v<decltype(HalfMeter{1} + ThirdMeter{1})::ratio, std::ratio<1>>));
    }
    // Addition for nonratio types
    {
        assert((StrUnit{"hellobye"} + StrUnit{"bye"} == StrUnit{"hellobyebye"}));
        assert((StrUnit{"hellobye"} - StrUnit{"bye"} == StrUnit{"hello"}));

        assert((EmptyUnit<AdditiveString>{"hellobye"} + AdditiveString{"bye"} == AdditiveString{"hellobyebye"}));
        assert((AdditiveString{"hellobye"} + AdditiveString{"bye"} == EmptyUnit<AdditiveString>{"hellobyebye"}));

        assert((EmptyUnit<AdditiveString>{"hellobye"} - AdditiveString{"bye"} == AdditiveString{"hello"}));
        assert((AdditiveString{"hellobye"} - AdditiveString{"bye"} == EmptyUnit<AdditiveString>{"hello"}));
    }
    // Addition for ratio types
    {
        assert((Z7Unit{4} + Z7Unit{5} == Z7Unit{2}));
        assert((Z7Unit{4} - Z7Unit{5} == Z7Unit{6}));
        assert((Z7Unit{4} + Z7Unit_Double{5} == Z7Unit{0}));
        assert((Z7Unit{4} + Z7Unit_Half{5} == Z7Unit{3})); // 2^-1 = 4, so 4 + 4*5 = 24 -> 3

        assert((EmptyUnit<Z7>{1} + Z7{4} == Z7{5}));
        assert((Z7{1} + EmptyUnit<Z7>{4} == Z7{5}));
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
        assert((dUEmpty{1} * dUKilo{1} == (double)1000));

        assert((CanOp<Meter, "*", Meter>()));
        assert((CanOp<Meter, "*", Kilometer>()));
        assert((CanOp<Meter, "*", Second>()));
    }

    // Multiplication for user-defined units
    {
        assert((Z7Unit{4} * Z7Unit{4} == Z7Unit_2{2}));
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
    // Test unary negation
    {
        assert((-Meter{1} == Meter{-1}));
        assert((-Kilometer{1} == Meter{-1000}));
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

        assert((dUEmpty{1} / dUKilo{1} == 0.001));

        static_assert((CanOp<Meter, "/", Second>()));
        static_assert((CanOp<Meter, "/", Kilometer>()));
        static_assert((CanOp<Meter, "/", dUEmpty>()));
    }

    // Multiplication for user-defined units
    {
        assert((Z7Unit{4} / Z7Unit{4} == Z7{1}));
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

        assert((dUEmpty{1} + dUKilo{1} == 1001));
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

    // Left-side compound assignment with empty units and plain types
    std::cout << "Running plaintype/EmptyUnit compound assignment tests" << std::endl;
    // Multiplication assignment
    {
        double x = 1.0;
        x *= 2.0;
        assert(x == 2.0);
        assert((x *= dUKilo{0.001}) == 2.0);

        assert((CanOp<double, "*=", dUEmpty>()));
        assert((CanOp<double, "*=", dUKilo>()));
    }
    // Division assignment
    {
        double x = 1.0;
        x /= 2.0;
        assert(x == 0.5);
        assert((x /= dUKilo{0.001}) == 0.5);

        assert((CanOp<double, "/=", dUEmpty>()));
        assert((CanOp<double, "/=", dUKilo>()));
    }
    // Addition assignment
    {
        double x = 1.0;
        x += 2.0;
        assert(x == 3.0);
        assert((x += dUKilo{0.997}) == 1000);

        assert((CanOp<double, "+=", dUEmpty>()));
        assert((CanOp<double, "+=", dUKilo>()));
    }
    // Subtraction assignment
    {
        double x = 1.0;
        x -= 2.0;
        assert(x == -1.0);
        assert((x -= dUKilo{0.999}) == -1000);

        assert((CanOp<double, "-=", dUEmpty>()));
        assert((CanOp<double, "-=", dUKilo>()));
    }

    // /** -- Run comparison tests --  */
    std::cout << "Running comparison tests" << std::endl;

    // Test <
    {
        assert(Meter{1} < Meter{2});
        assert(Meter{999} < Kilometer{1});
        assert(Kilometer{0.999} < Meter{1000});
        assert(Meter{999} < iMeter{1000});

        assert(int(1) < dUEmpty{2});

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
    std::cout << "Running UnitMath tests" << std::endl;
    // unit_sqrt
    {
        Meter_2 v{100};
        assert((unit_sqrt(v) == Meter{10}));
        assert((unit_sqrt(100) == 10));

        static_assert((HasSquareRoot<Meter_2>));
        static_assert((HasSquareRoot<double>));
        static_assert((!HasSquareRoot<Kilometer>));
        static_assert((!HasSquareRoot<TypeAtomic<std::string, "str_unit">>));
        static_assert((!HasSquareRoot<std::string>));
    }

    // unit_ratio_pow
    {
        // Kilometer v{1};
        // using ResType = Unit<double, MakeUnitIdentifier<UnitBase<"meter", std::ratio<2, 3>>>, std::ratio<100>>;
        // auto res = unit_ratio_pow<std::ratio<2, 3>>(v);
        // assert((std::is_same_v<decltype(res), ResType>));
        // assert((abs(res.GetValue() - 1) < 0.0000001));

        // static_assert((HasRatioPow<std::ratio<1, 3>, UnitExpI<Meter, 3>>));
        // static_assert((!HasRatioPow<std::ratio<1, 3>, TypeAtomic<std::string, "str_unit">>));
        // static_assert((HasRatioPow<std::ratio<1, 3>, double>));
        // static_assert((!HasRatioPow<std::ratio<1, 3>, std::string>));
    }

    // unit_bas - absolute value
    {
        Meter val{-5};
        assert((unit_abs(val) == Meter{5}));
        assert((unit_abs(Meter{5}) == Meter{5}));
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
        static_assert(std::is_constructible_v<Vector2<Meter>, Meter, Meter>);
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
        const Array<float, 3> &arr = myVec.GetData();
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

        struct MyType
        {
        };
        // Weirdly enough, since `std::string{0}` is well-defined, IsZero is OK
        static_assert((HasIsZero<Vector2<std::string>>));
        static_assert((HasIsZero<Vector2<double>>));
        static_assert((!HasIsZero<Vector2<MyType>>));
    };
    std::cout << "Running arithmetic tests" << std::endl;
    // Componentwise Addition
    {
        assert((Vector3<Kilometer>{1, 1, 1} + Vector3<Meter>{1, 1, 1} == Vector3<Meter>{1001, 1001, 1001}));
        assert((Vector3<Kilometer>{1, 1, 1} + Vector3<Meter>{1, 1, 1} == Vector3<Kilometer>{1.001, 1.001, 1.001}));
        assert((Vector3<double>{1.2, 1.2, 1.2} + Vector3<int>{1, 1, 1} == Vector3<double>{2.2, 2.2, 2.2}));

        assert((CanOp<Vector3<Meter>, "+", Vector3<Kilometer>>()));
        assert((!CanOp<Vector3<Meter>, "+", Vector3<double>>()));
        assert((CanOp<Vector3<float>, "+", Vector3<dUEmpty>>()));
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
    // Unary negation
    {
        assert((-Vector3<double>{1, 0, 0} == Vector3<double>{-1, 0, 0}));
        assert((-Vector3<Kilometer>{1, 0, 0} == Vector3<Meter>{-1000, 0, 0}));
    }

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
        assert((VectorHasNormSquared<double>));
        assert((VectorHasNormSquared<Meter>));
        assert((VectorHasNormSquared<Kilometer>));
        assert((!VectorHasNormSquared<std::string>));

        Vector2<double> v{2, 0};
        assert((NormSquared(v) == 4));
        assert((NormSquared(Vector2<Meter>{3, 4}) == Meter_2{25}));
        assert((NormSquared(Vector2<Kilometer>{0.003, 0.004}) == Meter_2{25}));
    }
    // Norm
    {
        assert((VectorHasNorm<double>));
        assert((VectorHasNorm<Meter>));
        assert((VectorHasNorm<Kilometer>));
        assert((!VectorHasNorm<std::string>));

        Vector2<double> v{2, 0};
        assert((Norm(v) == 2));
        assert((Norm(Vector2<Meter>{3, 4}) == Meter{5}));
        assert((Norm(Vector2<Kilometer>{0.003, 0.004}) == Meter{5}));
    }
    // Norm_d
    {
        assert((Norm_d(Vector2<double>{3, 4}) == 5));
        assert((Norm_d(Vector2<float>{3, 4}) == 5));
        assert((Norm_d(Vector2<Meter>{3, 4}) == 5));
        assert((Norm_d(Vector2<Kilometer>{3, 4}) == 5000));
    }

    std::cout << "Running product tests" << std::endl;
    // Dot product
    {
        assert((Vector3<double>{1, 1, 1}.Dot(Vector3<double>{1, 2, 3}) == 6));
        assert((Vector3<double>{1, 1, 1}.Dot(Vector3<Meter>{1, 2, 3}) == Meter{6}));
        assert((Vector3<Kilometer>{1, 1, 1}.Dot(Vector3<Meter>{1, 2, 3}) == Meter_2{6000}));
    }
    // Cross product
    {
        // Unit vectors
        assert((Vector3<double>{1, 0, 0}.Cross(Vector3<double>{0, 1, 0}) == Vector3<double>{0, 0, 1}));
        assert((Vector3<double>{0, 1, 0}.Cross(Vector3<double>{0, 0, 1}) == Vector3<double>{1, 0, 0}));
        assert((Vector3<double>{0, 0, 1}.Cross(Vector3<double>{1, 0, 0}) == Vector3<double>{0, 1, 0}));
        assert((Vector3<double>{0, 1, 0}.Cross(Vector3<double>{1, 0, 0}) == Vector3<double>{0, 0, -1}));
        assert((Vector3<double>{1, 0, 0}.Cross(Vector3<double>{0, 0, 1}) == Vector3<double>{0, -1, 0}));
        assert((Vector3<double>{0, 0, 1}.Cross(Vector3<double>{0, 1, 0}) == Vector3<double>{-1, 0, 0}));

        // Edge cases: parallel and zero vectors
        assert((Vector3<double>{1, 0, 0}.Cross(Vector3<double>{1, 0, 0}) == Vector3<double>{0, 0, 0}));
        assert((Vector3<double>{0, 0, 0}.Cross(Vector3<double>{1, 0, 0}) == Vector3<double>{0, 0, 0}));
        assert((Vector3<double>{1, 1, 1}.Cross(Vector3<double>{1, 1, 1}) == Vector3<double>{0, 0, 0}));

        // Diagonal vectors
        assert((Vector3<double>{1, 1, 0}.Cross(Vector3<double>{0, 1, 1}) == Vector3<double>{1, -1, 1}));
        assert((Vector3<double>{0, 1, 1}.Cross(Vector3<double>{1, 1, 0}) == Vector3<double>{-1, 1, -1}));
        assert((Vector3<double>{1, 0, 1}.Cross(Vector3<double>{1, 1, 0}) == Vector3<double>{-1, 1, 1}));
        assert((Vector3<double>{1, 1, 0}.Cross(Vector3<double>{1, 0, 1}) == Vector3<double>{1, -1, -1}));

        // Mixed types
        assert((Vector3<Meter>{1, 0, 0}.Cross(Vector3<double>{0, 1, 0}) == Vector3<Meter>{0, 0, 1}));
        assert((Vector3<Meter>{1, 0, 0}.Cross(Vector3<Kilometer>{0, 1, 0}) == Vector3<Meter_2>{0, 0, 1000}));

        // Arbitrary floats
        assert((Vector3<double>{1.0, 2.0, 3.0}.Cross(Vector3<double>{3.0, 2.0, 1.0}) == Vector3<double>{-4.0, 8.0, -4.0}));
        assert((Vector3<double>{2.0, 3.0, 1.0}.Cross(Vector3<double>{1.0, 3.0, 2.0}) == Vector3<double>{3.0, -3.0, 3.0}));
        assert((Vector3<double>{3.0, 1.0, 2.0}.Cross(Vector3<double>{2.0, 1.0, 3.0}) == Vector3<double>{1.0, -5.0, 1.0}));
        assert((Vector3<double>{1.0, 1.0, 2.0}.Cross(Vector3<double>{2.0, 3.0, 1.0}) == Vector3<double>{-5.0, 3.0, 1.0}));
        assert((Vector3<double>{3.0, 2.0, 1.0}.Cross(Vector3<double>{1.0, 2.0, 3.0}) == Vector3<double>{4.0, -8.0, 4.0}));
    }

    // ------------------------------------------------------------
    // Run Matrix tests
    // ------------------------------------------------------------
    std::cout << "------ BEGIN TESTING MATRIX ------" << std::endl;

    std::cout << "Running constructor and accessor tests" << std::endl;
    // Constructor and accessor
    {
        // Default constructor
        Matrix2<double> m;
        assert((m.At(0, 0) == 0 && m.At(0, 1) == 0));
        assert((m.At(1, 0) == 0 && m.At(1, 1) == 0));

        // Copy-initialized initializer list syntax
        Matrix2<Meter> m2 = {{1, 2}, {3, 4}};

        // Standard initializer list syntax
        Matrix2<Kilometer> m3{{0.001, 0.002}, {0.003, 0.004}};

        assert((m2.At(0, 0) == m3.At(0, 0) && m2.At(0, 1) == m3.At(0, 1)));
        assert((m2.At(1, 0) == m3.At(1, 0) && m2.At(1, 1) == m3.At(1, 1)));

        // 1D initializer list syntax
        Matrix2<double> m4{1, 2, 3, 4};
        assert((m4.At(0, 0) == 1 && m4.At(0, 1) == 2));
        assert((m4.At(1, 0) == 3 && m4.At(1, 1) == 4));
    }

    std::cout << "Running equality and assignment tests" << std::endl;
    // Equality and assignment
    {
        // Equality
        Matrix2<Meter> m1 = {{1, 2}, {3, 4}};
        Matrix2<Kilometer> m2{{0.001, 0.002}, {0.003, 0.004}};
        assert((m1 == m2));

        // Assignment
        Matrix2<Kilometer> m3 = {{1, 2}, {3, 4}};
        m1 = m3;
        assert((m1 == m3));
        assert(((m1 = Matrix2<Kilometer>{{0.001, 0.002}, {0.003, 0.005}}) == Matrix2<Meter>{{1, 2}, {3, 5}}));
    }

    // std::cout << "Running arithmetic tests" << std::endl;
    // Addition
    {
        Matrix2<double> m1 = {{1, 2}, {3, 4}};
        Matrix2<dUKilo> m2{1, 1, 1, 1};
        assert((((m1 + m2) == Matrix2<double>{1001, 1002, 1003, 1004})));
        assert(((Matrix2<Meter>{1, 0, 0, 0} + Matrix2<Meter>{0, 1, 0, 0}) == Matrix2<Meter>{1, 1, 0, 0}));
    }
    // Subtraction
    {
        Matrix2<double> m1 = {{1, 2}, {3, 4}};
        Matrix2<dUKilo> m2{1, 1, 1, 1};
        assert((((m1 - m2) == Matrix2<double>{-999, -998, -997, -996})));
        assert(((Matrix2<Meter>{1, 0, 0, 0} - Matrix2<Meter>{0, 1, 0, 0}) == Matrix2<Meter>{1, -1, 0, 0}));
    }
    // Multiplication with scalar (right and left)
    {
        Matrix2<double> m1 = {{1, 2}, {3, 4}};
        assert((m1 * 2 == Matrix2<double>{2, 4, 6, 8}));
        assert((2 * m1 == Matrix2<double>{2, 4, 6, 8}));
        assert((Matrix2<Meter>{1, 1, 1, 1} * Meter{1} == Matrix2<Meter_2>{1, 1, 1, 1}));
        assert((Kilometer{0.001} * Matrix2<Meter>{1, 1, 1, 1} == Matrix2<Meter_2>{1, 1, 1, 1}));
    }
    // Division with scalar (right  only)
    {
        Matrix2<double> m1 = {{2, 4}, {6, 8}};
        assert((m1 / 2 == Matrix2<double>{1, 2, 3, 4}));
        assert((Matrix2<Meter>{1, 1, 1, 1} / Meter{1} == Matrix2<double>{1, 1, 1, 1}));
        assert((Matrix2<Kilometer>{1, 1, 1, 1} / Meter{1} == Matrix2<double>{1000, 1000, 1000, 1000}));
    }
    // Unary negation
    {
        assert((-Matrix2<Kilometer>{1, 2, 3, 4} == Matrix2<Meter>{-1000, -2000, -3000, -4000}));
    }

    std::cout << "Running matrix multiplication tests" << std::endl;
    {
        // Basic unit conversions
        assert((Matrix2<double>{{1, 0}, {0, 1}} * Matrix2<double>{{1, 0}, {0, -1}} == Matrix2<double>{{1, 0}, {0, -1}}));
        assert((Matrix2<Meter>{{1, 0}, {0, 1}} * Matrix2<Meter>{{1, 0}, {0, -1}} == Matrix2<Meter_2>{{1, 0}, {0, -1}}));
        assert((Matrix2<Meter>{{1, 0}, {0, 1}} * Matrix2<Kilometer>{{1, 0}, {0, -1}} == Matrix2<Meter_2>{{1000, 0}, {0, -1000}}));

        // Matrices of various sizes
        assert((Matrix<2, 3, double>{{1, 0, 1}, {0, 1, 1}} * Matrix<3, 2, double>{{1, 0}, {0, 1}, {1, 1}} == Matrix<2, 2, double>{{2, 1}, {1, 2}}));
        assert((Matrix<3, 2, double>{{1, 0}, {0, 1}, {1, 1}} * Matrix<2, 3, double>{{1, 0, 1}, {0, 1, 1}} == Matrix<3, 3, double>{{1, 0, 1}, {0, 1, 1}, {1, 1, 2}}));

        assert((Matrix<3, 3, double>{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}} * Matrix<3, 3, double>{{2, 3, 4}, {5, 6, 7}, {8, 9, 10}} == Matrix<3, 3, double>{{2, 3, 4}, {5, 6, 7}, {8, 9, 10}}));
        assert((Matrix<2, 3, double>{{1, 2, 3}, {4, 5, 6}} * Matrix<3, 2, double>{{7, 8}, {9, 10}, {11, 12}} == Matrix<2, 2, double>{{58, 64}, {139, 154}}));
        assert((Matrix<2, 2, double>{{1, 2}, {3, 4}} * Matrix<2, 2, double>{{5, 6}, {7, 8}} == Matrix<2, 2, double>{{19, 22}, {43, 50}}));
        assert((Matrix<2, 3, double>{{1, 2, 3}, {4, 5, 6}} * Matrix<3, 3, double>{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}} == Matrix<2, 3, double>{{0, 0, 0}, {0, 0, 0}}));
        assert((Matrix<2, 2, double>{{2, 3}, {4, 5}} * Matrix<2, 2, double>{{2, 3}, {4, 5}} == Matrix<2, 2, double>{{16, 21}, {28, 37}}));

        // Converting from one unit to another
        using Worldspace = dAtomic<"world">;
        using Screenspace = dAtomic<"screen">;
        using Screen_per_World = DivideType<Screenspace, Worldspace>;
        Matrix4<Worldspace> mat_ws = Matrix4<Worldspace>::Identity();
        Matrix4<Screen_per_World> world_screen_convert = Matrix4<Screen_per_World>::Identity();
        auto res = mat_ws * world_screen_convert;
        assert((std::is_same_v<decltype(res), Matrix4<Screenspace>>));

        // Dimension checks
        assert((IsMultDefined<Matrix<2, 3, double>, Matrix<3, 2, double>>));
        assert((!IsMultDefined<Matrix<2, 3, double>, Matrix<4, 2, double>>));
    }

    std::cout << "Running compound assignment tests" << std::endl;
    // Addition assignment
    {
        Matrix2<double> m;
        Matrix2<double> m1{0, 1, 2, 3};
        m += m1;
        assert((m == Matrix2<double>{0, 1, 2, 3}));
        Matrix2<Meter> m2{0, 1, 2, 3};
        assert(((m2 += Matrix2<Kilometer>{1, 0, 0, 0}) == Matrix2<Meter>{1000, 1, 2, 3}));
    }
    // Subtraction assignment
    {
        Matrix2<double> m;
        Matrix2<double> m1{0, 1, 2, 3};
        m -= m1;
        assert((m == Matrix2<double>{0, -1, -2, -3}));
        Matrix2<Meter> m2{0, 1, 2, 3};
        assert(((m2 += Matrix2<Kilometer>{-1, 0, 0, 0}) == Matrix2<Meter>{-1000, 1, 2, 3}));
    }
    // Multiplication assignment
    {
        Matrix2<double> m1{0, 1, 2, 3};
        m1 *= 2;
        assert((m1 == Matrix2<double>{0, 2, 4, 6}));
        Matrix2<Meter> m2{0, 1, 2, 3};
        assert(((m2 *= dUKilo{1}) == Matrix2<Meter>{0, 1000, 2000, 3000}));
    }
    // Multiplication assignment with matrices
    {
        Matrix m1 = Matrix2<double>{{1, 0}, {0, 1}};
        m1 *= Matrix2<double>{{1, 0}, {0, -1}};
        assert((m1 == Matrix2<double>{{1, 0}, {0, -1}}));

        Matrix m3 = Matrix<3, 3, double>{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        assert(((m3 *= Matrix<3, 3, double>{{2, 3, 4}, {5, 6, 7}, {8, 9, 10}}) == Matrix<3, 3, double>{{2, 3, 4}, {5, 6, 7}, {8, 9, 10}}));

        Matrix m4 = Matrix<2, 2, double>{{1, 2}, {3, 4}};
        m4 *= Matrix<2, 2, double>{{5, 6}, {7, 8}};
        assert((m4 == Matrix<2, 2, double>{{19, 22}, {43, 50}}));

        Matrix m5 = Matrix<2, 2, double>{{2, 3}, {4, 5}};
        m5 *= Matrix<2, 2, double>{{2, 3}, {4, 5}};
        assert((m5 == Matrix<2, 2, double>{{16, 21}, {28, 37}}));
    }

    // Division assignment
    {
        Matrix2<double> m1{0, 1, 2, 3};
        m1 /= 2;
        assert((m1 == Matrix2<double>{0, 0.5, 1, 1.5}));
        Matrix2<Meter> m2{0, 1, 2, 3};
        assert(((m2 /= dUKilo{1}) == Matrix2<Meter>{0, 0.001, 0.002, 0.003}));
    }

    std::cout << "Running transposition tests" << std::endl;
    {
        assert(((Matrix<2, 3, double>{1, 2, 3, 4, 5, 6}).Transpose() == Matrix<3, 2, double>{{1, 4}, {2, 5}, {3, 6}}));
        assert((Matrix<3, 3, int>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}.Transpose() == Matrix<3, 3, int>{{1, 4, 7}, {2, 5, 8}, {3, 6, 9}}));
        assert((Matrix<2, 4, float>{{1.1f, 2.2f, 3.3f, 4.4f}, {5.5f, 6.6f, 7.7f, 8.8f}}.Transpose() == Matrix<4, 2, float>{{1.1f, 5.5f}, {2.2f, 6.6f}, {3.3f, 7.7f}, {4.4f, 8.8f}}));
        assert((Matrix<4, 2, double>{{1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}, {7.0, 8.0}}.Transpose() == Matrix<2, 4, double>{{1.0, 3.0, 5.0, 7.0}, {2.0, 4.0, 6.0, 8.0}}));
        assert((Matrix<1, 5, int>{10, 20, 30, 40, 50}.Transpose() == Matrix<5, 1, int>{{10}, {20}, {30}, {40}, {50}}));
    }

    std::cout << "Running zero and identity tests" << std::endl;
    // Zero tests
    {
        assert((Matrix<2, 3, double>::Zero().IsZero()));
        assert((Matrix<2, 3, double>{}.IsZero()));

        struct MyType
        {
        };
        assert((HasIsZero<Matrix<2, 3, Meter>>));
        assert((HasIsZero<Matrix<2, 3, std::string>>));
        assert((!HasIsZero<Matrix<2, 3, MyType>>));
        assert((Matrix3<double>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}} * Matrix3<double>::Zero() == Matrix3<double>::Zero()));
    }
    // Identity tests
    {
        struct MyType
        {
        };
        assert((HasIdentity<Matrix<3, 3, double>>));
        assert((HasIdentity<Matrix<3, 3, std::string>>));
        assert((!HasIdentity<Matrix<3, 3, MyType>>));
        assert((!HasIdentity<Matrix<2, 3, double>>));

        assert((Matrix3<double>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}} * Matrix3<double>::Identity() == Matrix3<double>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}));
    }

    std::cout << "Running determinant tests" << std::endl;
    {
        // Basic test
        Matrix<3, 3, double> mat = {{1, 7, 3}, {-1, 2, 4}, {2, 2, 1}};
        assert((Det(mat) == 39));

        // Test validity on dimensions
        static_assert((HasDet<Matrix<3, 3, double>>));
        static_assert((!HasDet<Matrix<2, 3, double>>));

        static_assert((HasDet<Matrix<3, 3, Meter>>));
        static_assert((!HasDet<Matrix<2, 3, Meter>>));

        // Test types
        Matrix<3, 3, Meter> mat2 = {{1, 7, 3}, {-1, 2, 4}, {2, 2, 1}};
        assert((Det(mat2) == UnitExpI<Meter, 3>{39}));

        // Test for a simple identity matrix
        assert((Det(Matrix<2, 2, double>{{1, 0}, {0, 1}}) == 1));
        assert((Det(Matrix<3, 3, double>{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}) == 1));

        // Test for a matrix with all zeros (determinant should be 0)
        assert((Det(Matrix<2, 2, double>{{0, 0}, {0, 0}}) == 0));
        assert((Det(Matrix<3, 3, double>{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}) == 0));

        // Test for a 1x1 matrix
        assert((Det(Matrix<1, 1, double>{12}) == 12));

        // Test for a 2x2 matrix
        assert((Det(Matrix<2, 2, double>{{1, 2}, {3, 4}}) == -2));
        assert((Det(Matrix<2, 2, double>{{5, 6}, {7, 8}}) == -2));

        // Test for a 3x3 matrix
        assert((Det(Matrix<3, 3, double>{{2, 3, 1}, {4, 5, 6}, {7, 8, 9}}) == 9));
        assert((Det(Matrix<3, 3, double>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}) == 0)); // Singular matrix

        // Test for a matrix with negative numbers
        assert((Det(Matrix<3, 3, double>{{-1, -2, -3}, {-4, -5, -6}, {-7, -8, -9}}) == 0));
        assert((Det(Matrix<2, 2, double>{{-1, 2}, {3, -4}}) == -2));

        // Test for larger values
        assert((Det(Matrix<3, 3, double>{{10, 20, 30}, {40, 50, 60}, {70, 80, 90}}) == 0)); // Singular matrix
        assert((Det(Matrix<3, 3, double>{{5, 7, 2}, {3, 1, 8}, {4, 9, 6}}) == -186));

        // Test for fractional values
        assert((std::abs(Det(Matrix<2, 2, double>{{0.5, 1.5}, {1.2, 2.8}}) - -0.4) < 0.00000001));

        // Test for a larger size 4x4 matrix
        assert((Det(Matrix<4, 4, double>{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}) == 1));
        assert((Det(Matrix<4, 4, double>{{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}}) == 0)); // Singular matrix

        // Test for an upper triangular matrix
        assert((Det(Matrix<3, 3, double>{{3, 2, 1}, {0, 4, 5}, {0, 0, 6}}) == 72));

        // Test for a lower triangular matrix
        assert((Det(Matrix<3, 3, double>{{6, 0, 0}, {7, 5, 0}, {8, 9, 4}}) == 120));
    }

    std::cout << "Running inversion tests" << std::endl;
    {
        // Basic validity checks
        assert((HasInverse<Matrix<3, 3, double>>));
        assert((!HasInverse<Matrix<2, 3, double>>));
        assert((HasInverse<Matrix<3, 3, Meter>>));
        assert((!HasInverse<Matrix<3, 3, std::string>>));

        // Identity
        assert((Inv(Matrix<3, 3, double>{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}) == Matrix<3, 3, double>{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}));

        // Noninvertible matrix
        assert((Inv(Matrix<3, 3, double>{{1, 0, 0}, {0, 1, 0}, {0, 0, 0}}) == Matrix<3, 3, double>::Zero()));

        // Dimension and type checks

        // Test for a 1x1 matrix
        assert((Inv(Matrix<1, 1, double>{4}) == Matrix<1, 1, double>{0.25}));

        // Test for a 2x2 matrix

        // Test for a 3x3 matrix

        // Large matrix

        // Value stability tests
        assert((Inv(Matrix<3, 3, double>{{4, 7, 2}, {3, 6, 1}, {2, 5, 1}}) ==
                Matrix<3, 3, double>{{1.0 / 3.0, 1.0, -5.0 / 3.0}, {-1.0 / 3.0, 0.0, 2.0 / 3.0}, {1.0, -2.0, 1.0}}));
        assert((Inv(Matrix<3, 3, double>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}) == Matrix<3, 3, double>::Zero()));
        assert((Inv(Matrix<3, 3, double>{{2, 4, 6}, {1, 3, 5}, {0, 0, 0}}) == Matrix<3, 3, double>::Zero()));
        assert((Inv(Matrix<2, 2, double>{{1.5, 2.5}, {3.5, 4.5}}) ==
                Matrix<2, 2, double>{{-2.25, 1.25}, {1.75, -0.75}}));

        assert((Inv(Matrix<4, 4, double>{{1, 0, 0, 0}, {0, 2, -3, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}) ==
                Matrix<4, 4, double>{{1., 0., 0., 0.}, {0., 0.5, 1.5, 0.}, {0., 0., 1., 0.}, {0., 0., 0., 1.}}));
    }
    
    
    std::cout << "------ BEGIN TESTING ACTOR AND COLLISION ------" << std::endl;

    TestActorInitialization();
    TestActorMovement();
    TestActorRotation();
    TestActorMovementWithRotation();
    TestCollisionDetection();
    TestActorCollisionResponse();
    TestActorRotationAfterCollision();

    std::cout << "All tests passed successfully.\n";


    return 0;
}
