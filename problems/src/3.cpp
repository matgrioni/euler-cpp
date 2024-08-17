#include "3.hpp"

#include "Sieve.hpp"

#include <cmath>
#include <vector>
#include <utility>

namespace euler
{
	Solver3_1::Solver3_1(int64_t p_number)
		: m_number(p_number)
	{ }

	int64_t Solver3_1::operator()()
	{
		auto lim = static_cast<int64_t>(std::sqrt(m_number)) + 1;
		std::vector<int64_t> primes;
		sieve::Eratosthenes(primes, lim);

		for (auto it = primes.crbegin(); it != primes.crend(); ++it)
		{
			if (m_number % *it == 0)
			{
				return *it;
			}
		}

		// TODO: Figure out what to do here.
		return 0;
	}
}