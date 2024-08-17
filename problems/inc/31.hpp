#pragma once

#include "Solver.hpp"

#include <cstdint>

namespace euler
{
    // TODO: Comment on this solution which is the unefficient first one I came up with.
    class Solver31_1 : public Solver
    {
    public:
        int64_t operator()() override;
    };
}
