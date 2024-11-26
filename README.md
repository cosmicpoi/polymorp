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
  - In other words we should not have to say `Vector(Vector3<Meter>, Second)`, we should be able to say `Vector3<Meter> * Second` and be well-understood.
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

## Units

## Plain scalars
- In general, plain scalars and `EmptyUnit` should be one-to-one. That is, in any situation we can use a plain scalar like `float`, we can use `EmptyUnit`, and vice versa.
  - Therefore, `EmptyUnit{1.0} + (double) 1.0` should be well-understood, and `Kilometer / Meter` should be `Kilo`.
  - Basic arithmetic operations `(+=*/)` between units and scalars are all understood
- The only exception is that assignment from plain scalar to EmptyUnit is OK, but assignment from EmptyUnit to plain scalar is not.
  - The reason is that EmptyUnit holds more information than a plain scalar, as it includes a ratio.

## Vectors




