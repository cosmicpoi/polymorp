# c++ game engine
Polymorp
Timothy Melendez, Yuhao Tang, Faith Luo

# Compilation

Run `make` from root

Run `make clean` to clean build folder

# UnitLib

The overall goal of `UnitLib` is to allow compile-time typed arithmetic with arbitrary dimensional units in arbitrary rectangular shapes and arbitrary integral ratios. This means:

- Any possible units can be defined and used (as opposed to built-in SI units)
- Units can be used in any combination and well-understood (for instance, `Meter * Second == Second * Meter`)
- Units can be organized as vectors and matrices
- Units are well-interoperable with builtin numeric types, which act essentially as unitless scalars. i.e. `float * Meter` should be understandable as `Meter`.
- Arithmetic between units, unitless scalars, Vectors, and Matrices (the four basic types of UnitLib) uses reasonable and expressive syntax that differs little from natural mathematical syntax.
  - In other words we should not have to say `Mult<Vector3<Meter>, Second>`, we should be able to say `Vector3<Meter> * Second` and be well-understood.
- Logical conversion and interpretation of ratios at compile-time.
  - `Meter + Kilometer` should be OK, and `Kilometer / Meter` should be a unitless ratio of 1000.
- As much information as possible (in our case, nearly all information) is preserved through arithmetic. For instance, if we add two units with ratios 1/2 and 1/3, even if they use underlying doubles, instead of computing and coercing into one of the two ratios, we construct a unit with ratio 1/6.
- And, all of this is done at compile time when possible (and again is done so in nearly all cases).

**UnitLib** comprises, essentially, of three types: `Unit`, `Vector`, and `Matrix`, in addition to built-in numeric ("plain") types such as `double`, `float`, and `int`, which are referred to as **plain scalars**.

There is also a special unitless Unit (it uses the `Unit` class but does not represent any dimension), referred to as the `EmptyUnit`.

The design goals and implementation of the four basic types are described below.

## Some Keywords
- **Unit** - referring specifically to any instance of the Unit class
- **EmptyUnit** - referring to the unitless Unit class representing a dimensionless constant
- **plain type** - a non-unit value
- **scalar** - any non-vector value (units)
  - **plain scalar** - a non-vector that's a plain type
  - **unit scalar** - a non-vector of type unit
- **GeneralScalar** - a specific `concept` that matches both plain arithmetic types (i.e. `std::is_arithmetic`) and Units

Also, some example types (used for explanation):
```
Meter - SI atomic unit of meter
Kilometer - Meter with ratio 1000

EmptyUnit - empty unit 
Kilo - empty unit with ratio 1000
```

## Overall Structure
In addition to the general goals of units as listed above, we also have the following design pillars:

- Separability of `Unit`, `Vector`, and `Matrix`
- Full interoperability between primary types of scalars, Unit, Vector, and Matrix in "mathematician's expected" ways, using natural syntax
  - e.g. `Mat4 * Vec4 = Vec4`
- Zero overhead in both space and performance relative to primitive types
- Competitive in performance and similar in expressibility to common libraries like [glm](https://github.com/g-truc/glm)

Since the library is header-only, one important consequence of this, and a key advantage relative to `glm`, is that any of the three components can be used without the others quite easily. One can use the full functionality of `Vector` just by deleting the headers corresponding to `Unit` and `Matrix`. We minimizing coupling wherever possible.

## Units
- Zero-overhead. This can be checked easily with `sizeof(double) == sizeof(Unit<double, ...>)`.

## Plain scalars
- In general, plain scalars and `EmptyUnit` should be one-to-one. That is, in any situation we can use a plain scalar like `float`, we can use `EmptyUnit`, and vice versa.
  - Therefore, `EmptyUnit{1.0} + (double) 1.0` should be well-understood, and `Kilometer / Meter` should be `Kilo`.
  - Basic arithmetic operations `(+=*/)` between units and scalars are all understood
- The only exception is that assignment from plain scalar to EmptyUnit is OK, but assignment from EmptyUnit to plain scalar is not.
  - The reason is that EmptyUnit holds more information than a plain scalar, as it includes a ratio.

## Containers - Vector and Matrix
- Both `Vector` and `Matrix` are implemented as thin containers with zero overhead.
  - Verify: `sizeof(Vector2<double>) == 2 * sizeof(double)`
- Their operations should be unaware of the types they hold, only how they behave. We accomplish this by extensive use of `requires(A a, B b) {a + b;}` and similar constructions.
  - For instance, addition between `Vector2<std::string>` is not well-defined but is for `Vector2<float>`.
- Supports any dimensions of `size_t` (`N` for vectors and `M` x `N` for matrices)
- ...And does so with zero overhead. This is accomplished through compile-time loops using `std::make_integer_sequence`
- Compete with standard libraries such as [glm::vec3](https://github.com/g-truc/glm) in performance and usability
- For `N = 2, 3, 4`, i.e. corresponding to traditional `Vector2/3/4`, and `Mat2/3/4`, prioritize performance, usability, and keeping par with libraries like glm over generality.
  - What this means is, if the `std::make_integer_sequence` is even slightly slower for e.g. size `N=3` (due to indirection or something), it would be better to use `if constexpr(N==3)` and special-case the logic.

One instance of a situation where these principles help to guide our design is in the inclusion of `.x()`, `.y()`, `.z()`, and `.w()` accessors for Vectors. Technically this runs against our usual design philosophy of having the vectors be size-generic as possible. However, because these patterns are common in industry-standard libraries such as `glm`, we consider them important enough to special-case.

Another example of design principles at play: We choose to define the typical `NormSq()` and `Norm()` functions as non-members. The reason is that `Norm()` needs to use square root, and we need to provide a generalized square root that's compatible with units (so that we can support `sqrt(m^1) -> m^1/2`). If `Norm()` was a member of `Vector`, then `Vector` would need to be aware of the details of `Unit`, which goes against our philosophy.