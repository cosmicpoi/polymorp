#pragma once

template <uint P>
constexpr uint PositiveMod(const intmax_t &val)
{
    intmax_t rem = (val % P);
    return rem < 0 ? static_cast<uint>(rem + P) : static_cast<uint>(rem);
}

// Example class that supports add and subtract and ALSO ratio
template <uint P>
class PrimeField
{
public:
    static constexpr uint p = P;
    inline PrimeField(intmax_t val) : value(PositiveMod<P>(val)) {};

    // Add/subtract operations
    inline PrimeField<P> operator+(const PrimeField<P> &other) const
    {
        return PrimeField(PositiveMod<P>(value + other.value));
    }

    inline PrimeField<P> operator-(const PrimeField<P> &other) const
    {
        return PrimeField(PositiveMod<P>(static_cast<int>(value) - static_cast<int>(other.value)));
    }
    // Mult/divide
    inline PrimeField<P> operator*(const PrimeField<P> &other) const
    {
        return PrimeField(PositiveMod<P>(value * other.value));
    }

    inline PrimeField<P> MultInv() const
    {
        // Use fermat's little theorem
        // x = a^(P-2), so need to multiply P-3 times
        uint res = value;
        for (uint i = 0; i < P - 3; i++)
        {
            res = (res * value) % P;
        }

        return res;
    }

    inline PrimeField<P> operator/(const PrimeField<P> &other) const
    {
        if (other.value == 0)
        {
            throw std::runtime_error("Divide by zero");
        }
        return (*this) * other.MultInv();
    }

    // Ratio operations
    inline PrimeField<P> operator*(const intmax_t &other) const
    {
        return PrimeField(PositiveMod<P>(value * other));
    }

    inline PrimeField<P> operator/(const intmax_t &other) const
    {
        if (other % P == 0)
        {
            throw std::runtime_error("Divide by zero");
        }
        return (*this) * PrimeField<P>{PositiveMod<P>(other)}.MultInv();
    }

    inline bool operator==(const PrimeField<P> &other) const
    {
        return value == other.value;
    }

    // Value holder
    uint value;
};

template <uint P>
std::ostream &operator<<(std::ostream &os, const PrimeField<P> &val)
{
    return os << val.value; // Forward to the underlying std::string
}

using Z7 = PrimeField<7>;