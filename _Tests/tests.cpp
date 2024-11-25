#include <cassert>
#include "../UnitLib/Vector.h"
#include "../UnitLib/Unit.h"

// ------------------------------------------------------------
// Testing library utils
// ------------------------------------------------------------

template <typename A, typename B>
concept _CanAdd = requires(A a, B b) { a + b; };

template <typename A, typename B>
constexpr bool _CheckAdd(A, B) {
    return _CanAdd<decltype(std::declval<A>()), decltype(std::declval<B>())>;
}

// Unit definitions

using Meter = dAtomic<"meter">;
using fMeter = fAtomic<"meter">;
using iMeter = iAtomic<"meter">;
using Second = dAtomic<"second">;
using Kilometer = UnitMultRatio<Meter, std::ratio<1000>>;


int main()
{
    // ------------------------------------------------------------
    // Run Unit tests
    // ------------------------------------------------------------

    /** -- Run constructor tests */
    std::cout << "Running constructor tests" << std::endl;
    {
        auto myVal = Meter{0};
        assert(myVal.GetValue() == 0);
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
        // myV
    }


    /** -- Run arithmetic tests --  */
    std::cout << "Running arithmetic tests" << std::endl;

    // Test addition
    {
        assert((Meter{1} + Kilometer{1}) == Kilometer{1.001});
        assert((Kilometer{1.001}) == Meter{1001});
    }
    // Test subtraction
    {
        assert((Meter{1} - Kilometer{1}) == Kilometer{-0.999});
        assert((Meter{1} - Kilometer{1}) == Meter{-999});
    }
    // Test multiplication with units

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
        assert(_CheckAdd(myV, myV2));
        // myV + myV3;
    }

    return 0;
}