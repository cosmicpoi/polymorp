#pragma once

// Example class that supports add and subtract but NOT ratio
class AdditiveString
{
public:
    inline AdditiveString(const std::string &val) : value(val) {};
    inline AdditiveString(const char *val) : value(val) {};
    // concats strings
    inline AdditiveString operator+(const AdditiveString &other) const
    {
        return AdditiveString(value + other.value);
    }
    // removes the given string only if it at the end - for instance, "hibye" - "bye" == "hi"
    inline AdditiveString operator-(const AdditiveString &suffix) const
    {
        if (value.size() >= suffix.value.size() &&
            value.compare(value.size() - suffix.value.size(), suffix.value.size(), suffix.value) == 0)
        {
            // Remove the suffix
            return AdditiveString(value.substr(0, value.size() - suffix.value.size()));
        }
        // If the suffix is not at the end, return the original string
        return *this;
    }

    // comparison
    inline bool operator==(const AdditiveString &other) const
    {
        return value == other.value;
    }

    std::string value;
};

std::ostream &operator<<(std::ostream &os, const AdditiveString &val)
{
    return os << val.value; // Forward to the underlying std::string
}