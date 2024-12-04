#pragma once
#include "UnitLib/Vector.h"

template<typename Type> 
class Triangle
{
     public:
          Vector3<Type> _p1, _p2, _p3;

          Triangle(Vector3<Type> p1, Vector3<Type> p2, Vector3<Type> p3)
          {
               _p1 = p1;
               _p2 = p2;
               _p3 = p3;
          }

};


