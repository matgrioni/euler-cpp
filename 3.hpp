#pragma once

#include "Solver.hpp"

#include <cstdint>

namespace euler
{
	/// <summary>
	/// 
	/// </summary>
	class Solver3_1 : public Solver
	{
	public:
		/// <summary>
		/// 
		/// </summary>
		/// <param name="p_number"></param>
		Solver3_1(int64_t p_number);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		int64_t operator()() override;

	private:
		/// <summary>
		/// 
		/// </summary>
		int64_t m_number;
	};
}