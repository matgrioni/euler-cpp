#include "2.hpp"

namespace euler
{
    Solver2_1::Solver2_1(int64_t p_upTo)
        : m_upTo(p_upTo)
    { }

    int64_t Solver2_1::operator()()
    {
        int64_t a = 1;
        int64_t b = 2;

        int64_t sum{};

        while (b < m_upTo)
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

    Solver2_2::Solver2_2(int64_t p_upTo)
        : m_upTo(p_upTo)
    { }

    int64_t Solver2_2::operator()()
    {
        int64_t a = 1;
        int64_t b = 2;

        int64_t sum{};

        // Only evens need to be summed, and evens occur in the fibonacci sequence
        // in the pattern of, e, o, o, e, o, o, e, ... So the evens can be determined directly.
        while (b < m_upTo)
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