#pragma once

#include "Solver.hpp"

#include <cstdint>

namespace euler
{
    // The solution to the first problem which finds the sum of all the multiples
    // of 3 and 5 less than some number.
    class Solver1 : public Solver
    {
    public:
        // Create a new solver, which finds the sum of the relevant multiples up to
        // the exclusive maximum provided.
        Solver1(int64_t p_max);

        // Provide the actual solution.
        int64_t operator()() override;
    
    private:
        // The maximum value to consider in the problem.
        int64_t m_max;
    };
}
