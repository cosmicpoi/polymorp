#pragma once
#include <type_traits>
#include "../UnitLib/Unit.h"
#include "../UnitLib/Vector.h"
#include "../UnitLib/Matrix.h"
#include "Triangle.h"

using Pixel = dAtomic<"pixel">;

template<typename T>
class Graphics {
    public:
        static void DrawTriangle(const Triangle<Screenspace>& triangle) {
        
        }
};


