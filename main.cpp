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
    // MyType<'a', 'b', 'c'>;

    // PrintSingle<'a'>();
    // PrintMany<'a', 'b', 'c'>();

    // PrintIndexSeq<GetIdxSeq<'a', 'b', 'c'>>();

    // using seq = std::index_sequence_for<CharWrapper<'a'>, CharWrapper<'b'>, CharWrapper<'c'>>;
    // IsIntegerSequenceHelper<seq>::Print();

    using Leaf1 = UnitLeaf<"aaa", 1>;
    using Leaf2 = UnitLeaf<"bbb", 2>;
    using Leaf3 = UnitLeaf<"ccc", 3>;
    using Leaf4 = UnitLeaf<"aaaa", 1>;

    using ULV = UnitLeafVector<Leaf3, Leaf2, Leaf1>;

    // std::cout << IsUnitLeaf<Leaf1> << std::endl;
    // std::cout << IsUnitLeafVector<ULV> << std::endl;

    // ULPrepend<Leaf1, ULV>::Print();

    // ULAppend<Leaf1, ULV>::Print();

    // std::cout << std::is_same_v<ULPrepend<Leaf1, ULV>, UnitLeafVector<Leaf1, Leaf1, Leaf2>> << std::endl;


    // ULRemoveFirst<Leaf2, ULV>::Print();

    // std::cout << ULCompare<Leaf1, Leaf2> << std::endl;

    // ULGreaterOf<UnitLeaf<"hello", 3>, UnitLeaf<"hello", 1>>::Print();    

    // ULGreaterOf<UnitLeaf<"sub", -1>, UnitLeaf<"hello", 1>>::Print();    


    // using seq2 = SortSequence_t<std::integer_sequence<int, 1, 3, 2>>;
    // IsIntegerSequenceHelper<seq2>::Print();

    ULMin<UnitLeafVector<Leaf3, Leaf2, Leaf1, Leaf1, Leaf1, Leaf3>>::Print();
    std::cout << std::endl;
    ULMin<UnitLeafVector<Leaf1, Leaf2, Leaf3>>::Print();
    std::cout << std::endl;
    ULMin<UnitLeafVector<Leaf3, Leaf2, Leaf1>>::Print();
    std::cout << std::endl;

    constexpr auto strLit1 = MakeStrLitHelper<"aaaa">::str;
    constexpr auto strLit2 = MakeStrLitHelper<"bbbb">::str;

    std::cout << CompareStrings<strLit1, strLit2>::value << std::endl;
    std::cout << CompareStrings<strLit2, strLit1>::value << std::endl;

    ULSort<UnitLeafVector<Leaf3, Leaf2, Leaf1, Leaf4>>::Print();
    std::cout << std::endl;

    ULSort<UnitLeafVector<Leaf1, Leaf3, Leaf4, Leaf2>>::Print();
    std::cout << std::endl;

    ULSort<ULConcat<ULV, ULV>>::Print();
    ULMerge_<ULSort<ULConcat<ULV, ULV>>>::type::Print();
    ULMerge_<UnitLeafVector<Leaf1, Leaf1, Leaf1>>::type::Print();

    using UL1 = UnitLeaf<"aaaa", 1>;
    using UL2 = UnitLeaf<"aaaa", 2>;
    std::cout << StrEq<UL1::symbol, UL2::symbol>::value << std::endl;
    std::cout << ULIsSameSymbol<UL1, UL2> << std::endl;

    ULCombine<UL1, UL2>::Print();
    std::cout << std::endl;

    
    

    
}