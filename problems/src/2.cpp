#include "problems.hpp"

namespace euler
{
    int64_t P2Naive(int64_t p_upTo)
    {
        int64_t a = 1;
        int64_t b = 2;

        int64_t sum{};

        while (b < p_upTo)
        {
            if (b % 2 == 0)
            {
                sum += b;
            }
            
            auto tmp = b;
            b += a;
            a = tmp;
        }

        return sum;
    }

    int64_t P2Optimization1(int64_t p_upTo)
    {
        int64_t a = 1;
        int64_t b = 2;

        int64_t sum{};

        // Only evens need to be summed, and evens occur in the fibonacci sequence
        // in the pattern of, e, o, o, e, o, o, e, ... So the evens can be determined directly.
        while (b < p_upTo)
        {
            sum += b;

            auto oddBeforeNextEven = 2 * b + a;
            auto nextEven = oddBeforeNextEven + b + a;

            a = oddBeforeNextEven;
            b = nextEven;
        }

        return sum;
    }
}