#include "problems.hpp"

#include "Math.hpp"

#include <algorithm>
#include <array>
#include <limits>

namespace
{
    constexpr bool IsPalindromeDigit(int64_t p_num)
    {
        if (p_num < 0)
        {
            return false;
        }

        if (p_num < 10)
        {
            return true;
        }

        constexpr auto maxDigits = std::numeric_limits<int64_t>::digits10 + 1;
        std::array<int8_t, maxDigits> reverseDigits;

        auto idx = 0;
        while (p_num > 0)
        {
            auto remainder = static_cast<int8_t>(p_num % 10);
            p_num /= 10;
            reverseDigits[idx] = remainder;
            ++idx;
        }

        auto numOfDigits = idx;
        for (auto i = 0; i < (numOfDigits + 1) / 2; ++i)
        {
            if (reverseDigits[i] != reverseDigits[numOfDigits - (i + 1)])
            {
                return false;
            }
        }

        return true;
    }
}

namespace euler
{
    int64_t P4(int64_t p_digits)
    {
        auto limit = Exp(10ll, p_digits);

        int64_t maxPalindrome = 0;
        for (int64_t first = 0; first < limit; ++first)
        {
            for (int64_t second = 0; second < limit; ++second)
            {
                auto product = first * second;
                if (IsPalindromeDigit(product))
                {
                    maxPalindrome = std::max(product, maxPalindrome);
                }
            }
        }

        return maxPalindrome;

    }
}