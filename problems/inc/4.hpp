#pragma once

#include "Solver.hpp"

namespace euler
{
	class Solver4 : public Solver
	{
	public:
		Solver4(int64_t p_digits);

		int64_t operator()() override;

	private:
		int64_t m_digits;
	};
}
