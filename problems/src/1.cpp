#include "1.hpp"

namespace euler
{
    Solver1::Solver1(int64_t p_max)
        : m_max(p_max)
    { }

    int64_t Solver1::operator()()
    {
        auto threeCount = (m_max - 1) / 3;
        auto fiveCount = (m_max - 1) / 5;
        auto fifteenCount = (m_max - 1) / 15;

        auto threeSum = ((3 + threeCount * 3) * threeCount);
        auto fiveSum = ((5 + fiveCount * 5) * fiveCount);
        auto fifteenSum = ((15 + fifteenCount * 15) * fifteenCount);

        return (threeSum + fiveSum - fifteenSum) / 2;
    }
}
