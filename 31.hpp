#pragma once

#include "Solver.hpp"

#include <cstdint>

// TODO: Comment on this solution which is the unefficient first one I came up with.
class Solver31_1 : public ArithmeticSolver<uint64_t>
{
public:
    using ArithmeticSolver<uint64_t>::operator();

private:
    uint64_t DoSolve() override;
};
