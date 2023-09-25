#include "31.hpp"

#include <algorithm>
#include <array>
#include <vector>

namespace
{
    // Stores necessary state on the queue for what needs to be processed.
    struct QueueState
    {
        // The last amount computed from the previous step.
        uint32_t m_amount;

        // The valid denominations must be tracked for each queue state because we only want to traverse
        // the list of denominations in order. Otherwise it is possible to back-track and get a sequence like
        // 1, 2, 1, 2 as separate from 1, 1, 2, 2.
        uint32_t m_firstValidDenominationIndex;
    };
}

uint64_t Solver31_1::DoSolve()
{
    constexpr std::array<uint32_t, 8> denominations = { 1, 2, 5, 10, 20, 50, 100, 200 };
    constexpr uint32_t c_desiredAmount = 200u;

    std::array<uint32_t, c_desiredAmount + 1> dpState{};
    std::vector<QueueState> queue{ { 0, 0 } };

    while (!queue.empty())
    {
        auto lastState = queue.back();
        queue.pop_back();

        for (auto idx = lastState.m_firstValidDenominationIndex; idx < denominations.size(); ++idx)
        {
            auto next = lastState.m_amount + denominations[idx];
            if (next <= c_desiredAmount)
            {
                ++dpState[next];
                queue.push_back(QueueState{ next, idx });
            }
            else
            {
                break;
            }
        }
    }

    return dpState[c_desiredAmount];
}
