#include "1.hpp"

#include <cstdlib>

Solver1_1::Solver1_1(uint64_t p_max)
	: m_max(p_max)
{ }

uint64_t Solver1_1::DoSolve()
{
	auto threeCount = (m_max - 1) / 3;
	auto fiveCount = (m_max - 1) / 5;
	auto fifteenCount = (m_max - 1) / 15;

	auto threeSum = ((3 + threeCount * 3) * threeCount) / 2;
	auto fiveSum = ((5 + fiveCount * 5) * fiveCount) / 2;
	auto fifteenSum = ((15 + fifteenCount * 15) * fifteenCount) / 2;

	return threeSum + fiveSum - fifteenSum;
}

Solver1_2::Solver1_2(int64_t p_max)
	: m_max(p_max)
{ }

int64_t Solver1_2::DoSolve()
{
	auto threeCount = (m_max - 1) / 3;
	auto fiveCount = (m_max - 1) / 5;
	auto fifteenCount = (m_max - 1) / 15;

	auto threeSum = ((3 + threeCount * 3) * threeCount);
	auto fiveSum = ((5 + fiveCount * 5) * fiveCount);
	auto fifteenSum = ((15 + fifteenCount * 15) * fifteenCount);

	return (threeSum + fiveSum - fifteenSum) / 2;
}