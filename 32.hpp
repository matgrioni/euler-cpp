#pragma once

#include "Solver.hpp"

class Solver32_1 : public ArithmeticSolver<uint64_t>
{
public:
    using ArithmeticSolver<uint64_t>::operator();

    uint64_t DoSolve() override;
};
