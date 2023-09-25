#pragma once

#include <string>
#include <type_traits>

class Solver
{
public:
	~Solver() = default;

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