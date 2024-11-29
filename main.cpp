// Run tests
#include "UnitLib/Unit.h"
// #include "UnitLib/UnitMath.h"
#include "UnitLib/Vector.h"
#include "UnitLib/Matrix.h"
#include "UnitLib/Print.h"

// using Meter = dAtomic<"meter">;
// using fMeter = fAtomic<"meter">;
// using iMeter = iAtomic<"meter">;
// using Second = dAtomic<"second">;
// using Kilometer = UnitMultRatio<Meter, std::ratio<1000>>;
// // using m__s2 = UnitMult<Meter, UnitExpI<Second, -2>>;

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

using M_S = UnitDivide<Meter, Second>;

template <typename T>
struct MyClass
{
public:
};




int main()
{
    Matrix<2, 2, Meter> mat2{1, 2, 3, 4};
    std::cout << Inv(mat2) << std::endl;

    Matrix<4, 4, Meter> mat3{1, 2, 3, 4, 5, 6, 7, 8, 0, 1, 1, 3, 6, 7, 8};
    std::cout << Inv(mat3) << std::endl;

    // Matrix<3, 3, Meter> mat{1, 2, 3, 4, 5, 6, 7, 8, 0};
    // auto v = GetCofactor<0, 1>(mat);
    // std::cout << v << std::endl;

    // std::cout << Det(mat) << std::endl;
    // std::cout << Inv(mat) << std::endl;
    // std::cout << CanExp_<0, double>::canExp << std::endl;
    // std::cout << std::is_same_v<decltype(v), UnitExpI<Meter, 3>> << std::endl;

    // std::cout << ((Inv(mat) == Matrix3<double>{})) << std::endl;
    // std::cout << ((Inv(Matrix3<Meter>{}) == Matrix3<Meter__1>{})) << std::endl;

    // std::cout <<  (CanExp<3, std::string>) <<std::endl;
    // std::cout <<  (CanExp<3, int>) <<std::endl;

    // using Removed = typename RemoveAtIndex_<2, std::make_index_sequence<5>>::type;

    // // remove_index<2>(std::make_index_sequence<4>{});
    // using filtered = typename FilterValue<2, std::index_sequence<1, 2, 3>>::type;
    // j
    // std::cout << get_sequence_element(1, filtered>::idx  << std::endl;
    // printIdxSequence(Removed{});
    // printIdxSequence(std::index_sequence<1,2, 3>{});
    // Matrix<2, 3, double> m1{1, 2, 3, 4, 5, 6};
    // std::cout << m1 << std::endl;
    // std::cout << m1.Transpose() << std::endl;

    // std::cout << (Matrix<2, 3, double>{{1, 2, 3}, {4, 5, 6}} * Matrix<3, 3, double>{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}) << std::endl;
    //     std::cout << Matrix<2, 3, double>{{0, 0, 0}, {0, 0, 0}} << std::endl;
    //     std::cout << (((Matrix<2, 3, double>{{1, 2, 3}, {4, 5, 6}} * Matrix<3, 3, double>{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}) == Matrix<2, 3, double>{{0, 0, 0}, {0, 0, 0}})) << std::endl;

    // std::cout << (v2_a * v2_b) << std::endl;
    // std::cout << (v2_a * v3) << std::endl;

    // std::cout << (m1 * m2) << std::endl;
}