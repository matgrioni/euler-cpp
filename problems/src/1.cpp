#include "problems.hpp"

namespace euler
{
    int64_t P1(int64_t p_max)
    {
        auto threeCount = (p_max - 1) / 3;
        auto fiveCount = (p_max - 1) / 5;
        auto fifteenCount = (p_max - 1) / 15;

        auto threeSum = ((3 + threeCount * 3) * threeCount);
        auto fiveSum = ((5 + fiveCount * 5) * fiveCount);
        auto fifteenSum = ((15 + fifteenCount * 15) * fifteenCount);

        return (threeSum + fiveSum - fifteenSum) / 2;
    }
}
