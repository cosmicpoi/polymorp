#pragma once
#include "../UnitLib/Vector.h"
#include "../UnitLib/Matrix.h"


template<typename space> 
class Triangle
{
    public:
       Vector3<space> _p1, _p2, _p3;

       Triangle(Vector3 p1, Vector3 p2, Vector3 p3)
       {
            _p1 = p1;
            _p2 = p2;
            _p3 = p3;
       }
};


