// Run tests
#include "Unit.h"
#include "UnitIdentifier.h"
#include "UnitMath.h"
#include "Vector.h"
#include "CompoundUnit.h"
#include "StringLiteral.h"
#include <iostream>

// using Meter = Unit<'m', double, 1>;
// using Meter_2 = Unit<'m', double, 2>;

// using Foot = Unit<'f', float, 1>;
// using Foot_2 = Unit<'f', float, 2>;

// using Inch = Unit<'i', int, 1>;
// using Inch_2 = Unit<'i', int, 2>;

// inline Meter operator"" _meter(long double value)
// {
//     return Meter{static_cast<double>(value)};
// }


// using Yard = UnitBase<'y', 1>;
// using Pop = UnitBase<'p', 2>;

#include <utility>
#include <type_traits>
#include <array>
#include <algorithm>
#include <initializer_list>


/**
 * 1. Define a vector of UnitLeaf (already done)
 */

/**
 * 2. Add printability (already done)
 */

/**
 * 3. Define prepend
 */

// ---------

template <typename T>
struct IsIntegerSequenceHelper : std::false_type
{};

// Specialization for the `Unit` template
template <typename T, T... ints>
struct IsIntegerSequenceHelper<std::integer_sequence<T, ints...>> : std::true_type
{
    static void Print()
    {
        // std::cout << 'z' << std::endl;
        // PrintMany_St<ints...>();
    }
};

template <typename T>
concept IsIntegerSequence = IsIntegerSequenceHelper<T>::value;

// ----


// template<typename T>
// void PrintIndexSeq()
// {
//     std::cout << 'x' << std::endl;
// }

template<IsIntegerSequence T>
void PrintIndexSeq()
{
    std::cout << 'y' << std::endl;
}

// template <char... Ns>
// using GetIdxSeq = std::index_sequence_for<Ns...>;

template <char C>
struct CharWrapper
{   
};

// Assertion checks
using Leaf1 = UnitLeaf<"hello", 1>;
using Leaf2 = UnitLeaf<"sup", 1>;

using ULV1 = ULPrepend<Leaf1, UnitLeafVector<Leaf1, Leaf2>>;
using ULV2 = UnitLeafVector<Leaf1, Leaf1, Leaf2>;

static_assert(std::is_same_v<ULV1, ULV2>, "Prepend OK");

int main() {
    using UL0 = UnitLeaf<"baaa", 0>;
    using UL1 = UnitLeaf<"aaaa", 1>;
    using UL2 = UnitLeaf<"aaaa", 2>;
    

    using test = UnitLeafVector<UL1, UL1, Leaf2>;
    ULGetUnique<test>::Print();
    test::Print();
    std::cout << UnitIdentifier<test> << std::endl;
    std::cout << UnitIdentifier<ULGetUnique<test>> << std::endl;

    
    

    
}