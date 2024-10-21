#pragma once

#include <cmath>
#include <vector>

namespace euler
{
    namespace sieve
    {
        /// <summary>
        /// Finds the prime numbers within the range [0, p_n).
        /// </summary>
        /// <typeparam name="Container"></typeparam>
        /// <typeparam name="T"></typeparam>
        /// <typeparam name="Allocator"></typeparam>
        /// <param name="p_container"></param>
        /// <param name="p_n"></param>
        /// <param name="p_alloc"></param>
        template <typename Container, typename T, typename Allocator = std::allocator<bool>>
        void Eratosthenes(Container& p_container, T p_n, const Allocator& p_alloc = Allocator())
        {
            // [-inf, 1] contains no prime numbers.
            if (p_n < 2)
            {
                return;
            }

            // TODO: Preallocation of container to maximum amount of primes in a range...

            // TODO: Consider changing to boost::dynamic_bitset
            // TODO: Consider most space efficient implementation.
            std::vector<bool, Allocator> marked(p_n, false, p_alloc);

            // TODO: Do optimization to remove all even numbers from consideration
            // TODO: Is this the best generic way to do this.
            // TODO: Proper range checking...
            T cur = 2;

            // TODO: More efficient way if the result is being floored.
            const auto sqrt = static_cast<T>(std::sqrt(p_n - 1));

            // Find the next value less than the square root that is not marked. This is the
            // next prime number to process.
            while (cur <= sqrt)
            {
                if (!marked[cur])
                {
                    // Start the cursor at the square of the current prime. Any composites less than this,
                    // will have been marked off when processing a previous prime.
                    T cursor = cur * cur - cur;
                    do
                    {
                        cursor += cur;
                        marked[cursor] = true;
                    } while (cursor < (std::ssize(marked) - cur));
                }
                ++cur;
            }

            for (auto i = 2; i < marked.size(); ++i)
            {
                if (!marked[i])
                {
                    p_container.push_back(i);
                }
            }
        }
    }
}