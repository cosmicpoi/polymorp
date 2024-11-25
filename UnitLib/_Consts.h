#pragma once

// Factor to increase floating point epsilon by. This is needed, otherwise this kind of check will fail:
//      Meter v0{1};
//      assert((v0 = Kilometer{1.001}) == Meter{1001});
// A factor of 1000 means that the above is OK, but anything greater may be a problem. This is likely unavoidable.
constexpr double EPS_TOLERANCE = 1000;