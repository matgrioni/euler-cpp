#pragma once

#include "Solver.hpp"

#include <cstdint>

// The solution to the first problem which finds the sum of all the multiples
// of 3 and 5 less than some number.
class Solver1_1 : public ArithmeticSolver<uint64_t>
{
public:
    // Create a new solver, which finds the sum of the relevant multiples up to
    // the exclusive maximum provided.
    Solver1_1(uint64_t p_max);

    using ArithmeticSolver<uint64_t>::operator();

private:
    // The maximum value to consider in the problem.
    uint64_t m_max;

    // Provide the actual solution.
    uint64_t DoSolve() override;
};

// The solution to the first problem which finds the sum of all the multiples
// of 3 and 5 less than some number.
class Solver1_2 : public ArithmeticSolver<int64_t>
{
public:
    // Create a new solver, which finds the sum of the relevant multiples up to
    // the exclusive maximum provided.
    Solver1_2(int64_t p_max);

    using ArithmeticSolver<int64_t>::operator();

private:
    // The maximum value to consider in the problem.
    int64_t m_max;

    // Provide the actual solution.
    int64_t DoSolve() override;
};
