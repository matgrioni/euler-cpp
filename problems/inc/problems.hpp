/// <summary>
/// This file contains the declarations of all the Project Euler solutions, and each definition will be a defined in its
/// separate cpp file to reduce the number of files which are mostly simple declarations.
/// </summary>

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