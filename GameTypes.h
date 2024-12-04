// Represents the interval [lowerBound, upperBound]
template <typename T>
concept ClipBounds = requires {
    { T::lowerBound } -> std::convertible_to<double>;
    { T::upperBound } -> std::convertible_to<double>;
};

template <ClipBounds Bounds>
double clip(double val)
{
    double width = Bounds::upperBound - Bounds::lowerBound;
    double offsetVal = val - Bounds::lowerBound;

    double rem = std::fmod(offsetVal, width);
    return Bounds::lowerBound + (rem < 0 ? rem + width : rem);
}

// Clips values to [Bounds::lowerBound, Bounds::upperBound]
template <ClipBounds Bounds>
class ClipDouble
{
public:
    ClipDouble(double val) : value(clip<Bounds>(val)) {};

    inline ClipDouble<Bounds> &operator=(double other)
    {
        value = clip<Bounds>(other);
        return *this;
    }

    inline ClipDouble<Bounds> operator+(const ClipDouble<Bounds> &other) const
    {
        return ClipDouble(value + other.value);
    }

    inline ClipDouble<Bounds> operator-(const ClipDouble<Bounds> &other) const
    {
        return ClipDouble(value - other.value);
    }

    inline operator double() const
    {
        return value;
    }

private:
    double value;
};