#pragma once

#include <cstdint>
#include <string>

// TODO: Create a flexible integer sized type. Can basically 

class DynamicInteger { };

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
	// may need to be updated.
	virtual std::string operator()() = 0;
};

template <typename T>
class ArithmeticSolver : public Solver
{
	static_assert(std::is_arithmetic_v<T>, "The type of the result must be an arithmetic type.");

public:
	std::string operator()()
	{
		return std::to_string(DoSolve());
	}

private:
	virtual T DoSolve() = 0;
};