#include "problems.hpp"

#include "Sieve.hpp"

#include <cmath>
#include <vector>
#include <utility>

namespace euler
{
    int64_t P3(int64_t p_number)
    {
        auto lim = static_cast<int64_t>(std::sqrt(p_number)) + 1;
        std::vector<int64_t> primes;
        sieve::Eratosthenes(primes, lim);

        for (auto it = primes.crbegin(); it != primes.crend(); ++it)
        {
            if (p_number % *it == 0)
            {
                return *it;
            }
        }

        // TODO: Figure out what to do here.
        return 0;
    }
}