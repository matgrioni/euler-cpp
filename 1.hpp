#pragma once

#include "Solver.hpp"

#include <cstdint>

// The solution to the first problem which finds the sum of all the multiples
// of 3 and 5 less than some number.
template <typename ArithmeticBase>
class Solver1 : public ArithmeticSolver<ArithmeticBase>
{
public:
    // Create a new solver, which finds the sum of the relevant multiples up to
    // the exclusive maximum provided.
    Solver1(ArithmeticBase p_max)
        : m_max(p_max)
    { }

    using ArithmeticSolver<ArithmeticBase>::operator();

private:
    // Provide the actual solution.
    ArithmeticBase DoSolve() override
    {
        auto threeCount = (m_max - 1) / 3;
        auto fiveCount = (m_max - 1) / 5;
        auto fifteenCount = (m_max - 1) / 15;
        using x = decltype(threeCount);

        auto threeSum = ((3 + threeCount * 3) * threeCount);
        auto fiveSum = ((5 + fiveCount * 5) * fiveCount);
        auto fifteenSum = ((15 + fifteenCount * 15) * fifteenCount);

        return (threeSum + fiveSum - fifteenSum) / 2;
    }

    // The maximum value to consider in the problem.
    ArithmeticBase m_max;
};
