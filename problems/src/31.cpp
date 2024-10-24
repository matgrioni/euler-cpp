#include "problems.hpp"

#include <algorithm>
#include <array>
#include <vector>

namespace
{
    // Stores necessary state on the queue for what needs to be processed.
    struct QueueState
    {
        // The last amount computed from the previous step.
        int32_t m_amount;

        // The valid denominations must be tracked for each queue state because we only want to traverse
        // the list of denominations in order. Otherwise it is possible to back-track and get a sequence like
        // 1, 2, 1, 2 as separate from 1, 1, 2, 2.
        int32_t m_firstValidDenominationIndex;
    };
}

namespace euler
{
    int64_t P31()
    {
        constexpr std::array<int32_t, 8> denominations = { 1, 2, 5, 10, 20, 50, 100, 200 };
        constexpr int32_t c_desiredAmount = 200u;

        std::array<int32_t, c_desiredAmount + 1> dpState{};
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
}

