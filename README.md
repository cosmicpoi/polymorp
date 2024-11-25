# c++ game engine
Polymorp
Timothy Melendez, Yuhao Tang, Faith Luo

# Compilation

Run `make` from root

Run `make clean` to clean build folder

# UnitLib

Some keywords:
- **Unit** - referring specifically to any instance of the Unit class
- **empty Unit** - referring to the unitless Unit class representing a dimensionless constant
- **plain type** - a non-unit value
- **scalar** - any non-vector value (units)
  - **plain scalar** - a non-vector that's a plain type
  - **unit scalar** - a non-vector of type unit
- **GeneralScalar** - a specific `concept` that matches both plain arithmetic types (i.e. `std::is_arithmetic`) and Units
