// To find all multiplicand/multiplier/product identities that are 1 through 9 pandigital
// it suffices to look through all 4 digit products only. Consider that since every digit 
// can only appear once, they have to be split between these three options. It is not possible
// to make a 5 digit number with only 4 numbers available on the other side (for example 97*86=8342,
// while two 3 digit numbers cannot be multiplied and give a 3 digit number).

// A naive solution to this is to generate all 9!/4! combinations that can be the multiplicand and the
// multiplier. Multiply them and decompose the result into the digit list, and which point it can be
// compared with the left over numbers.

#include "problems.hpp"

#include <array>
#include <cstdlib>
#include <optional>
#include <unordered_set>

#include "PermuteView.hpp"

namespace
{
    template <typename T>
    T DigitsToNumber(std::span<const uint8_t> p_digits)
    {
        T result{};
        T multiplier = 1;

        for (auto reverseIt = p_digits.rbegin(); reverseIt != p_digits.rend(); ++reverseIt)
        {
            result += *reverseIt * multiplier;
            multiplier *= 10;
        }

        return result;
    }

    std::vector<uint8_t> NumberToDigits(int64_t p_number)
    {
        if (p_number == 0)
        {
            return { 0 };
        }

        std::vector<uint8_t> result; 
        while (p_number > 0)
        {
            uint8_t digit = p_number % 10;
            result.push_back(digit);
            p_number /= 10;
        }

        return result;
    }
}

namespace euler
{
    int64_t P32()
    {
        std::array<uint8_t, 9> data{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        PermuteView<uint8_t> permuteView(data, 5);
        std::unordered_set<int64_t> pandigitalProducts;

        do
        {
            auto lhsDigits = permuteView.Current();
            auto rhsDigits = permuteView.Hidden();
            std::unordered_multiset<int64_t> rhsDigitsSet(rhsDigits.begin(), rhsDigits.end());

            auto checkPandigitalAt = [&](std::size_t p_divisionPoint)
            {
                auto a = DigitsToNumber<int32_t>(lhsDigits.subspan(0, p_divisionPoint));
                auto b = DigitsToNumber<int32_t>(lhsDigits.subspan(p_divisionPoint));
                auto product = a * b;
                auto productDigits = NumberToDigits(product);
                std::unordered_multiset<int64_t> productDigitSet(productDigits.begin(), productDigits.end());

                if (rhsDigitsSet == productDigitSet)
                {
                    pandigitalProducts.insert(product);
                }
            };

            checkPandigitalAt(1);
            checkPandigitalAt(2);
        } while (permuteView.Advance());

        int64_t sum{};
        for (auto product : pandigitalProducts)
        {
            sum += product;
        }

        return sum;
    }
}
