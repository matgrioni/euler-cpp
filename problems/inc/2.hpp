#pragma once

#include "Solver.hpp"

#include <cstdint>

namespace euler
{
    // A completely naive approach which generates all fibonacci sequence
    // numbers and sums those that are even up to some limit.
    class Solver2_1 : public Solver
    {
    public:
        Solver2_1(int64_t p_upTo);

        // Provide the actual solution.
        int64_t operator()() override;

    private:
        int64_t m_upTo;
    };

    // An optimized naive approach. It iterates through the whole sequence up to some limit,
    // but since the even fibonacci numbers are at known indices in the sequence and with a
    // known gap between them, this optimization skips over unnecessary elements.
    class Solver2_2 : public Solver
    {
    public:
        Solver2_2(int64_t p_upTo);

        // Provide the actual solution.
        int64_t operator()() override;

    private:
        int64_t m_upTo;
    };
}
