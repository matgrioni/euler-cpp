#pragma once

#include "Solver.hpp"

namespace euler
{
    class Solver32_1 : public Solver
    {
    public:
        int64_t operator()() override;
    };
}
