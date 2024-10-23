#pragma once

#include <cstdint>

namespace euler
{
	int64_t P1(int64_t p_max);

	int64_t P2Naive(int64_t p_upTo);
	int64_t P2Optimization1(int64_t p_upTo);

	int64_t P3(int64_t p_number);

	int64_t P4(int64_t p_digits);

	int64_t P31();

	int64_t P32();
}