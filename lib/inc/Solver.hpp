#pragma once

#include <cstdint>
#include <string>

namespace euler
{
    // A Solver can solve Project Euler problems. Usually each Solver is one attempt or approach
    // to solve a problem, which can also be parameterized to evaluate the approach beyond the
    // specific constraints of the problem.
    class Solver
    {
    public:
    	// Create default constructor as virtual for proper polymorphic usage.
    	virtual ~Solver() = default;
    
    	// Execute the solver and get the result. Right now all solutions are assumed to be integers
    	// that fit within a signed 64 bit representation. However, if a larger range is needed this
    	// may need to be updated to some custom large integer type.
    	virtual int64_t operator()() = 0;
    };
}

