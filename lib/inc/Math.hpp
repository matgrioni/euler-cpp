#pragma once

namespace euler
{
    template <typename T>
    constexpr T Exp(const T& p_base, const T& p_power)
    {
        if (p_power == 0)
        {
            return 1;
        }

        if (p_power == 1)
        {
            return p_base;
        }

        auto result = p_base;
        for (T i = 0; i < p_power - 1; ++i)
        {
            result *= p_base;
        }

        return result;
    }
}