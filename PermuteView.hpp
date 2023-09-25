#pragma once

#include <algorithm>
#include <compare>
#include <cstring>
#include <cstdlib>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

// This class permits iterating over the permutations of a set of values from some domain.
// The value type, T, can be any strongly ordered type which is defined by the the Compare
// type parameter. Typical usage for this class would be to get the current state of the
// view (the current permutation) and continue advancing the view until there are no more
// advances. The view can be reset at any time.
//
// There is some support for this in the standard library but specifically support for partial
// permutations does not exist (i.e. all permutations of size M from set of N, where M < N) and
// therefore there is also no way to easily access the hidden state or left over elements for
// any given state.
template <typename T, typename Compare = std::compare_three_way>
class PermuteView : private Compare
{
    static_assert(
        std::is_same_v<std::strong_ordering,
                       decltype(std::declval<Compare>()(std::declval<T>(), std::declval<T>()))>,
        "T must be strongly ordered via Compare.");

public:
    // Create a PermuteView over all the data elements. i.e. there are n! states from this.
    // The values are assumed to be unique. This requirement could be relaxed if necessary
    // but if so, should include proper handling to ensure stability.
    PermuteView(std::span<T> p_data, const Compare& p_compare = Compare())
        : PermuteView(p_data, p_data.size(), p_compare)
    {
    }

    // Create a PermuteView over the data elements only taking n
    PermuteView(std::span<T> p_data, uint64_t p_n, const Compare& p_compare = Compare())
        : Compare(p_compare),
          m_state(p_data),
          m_n(p_n),
          m_lastHiddenStateIdxUsed(-1)
    {
        Reset();

        auto firstDuplicate = std::adjacent_find(
            m_state.begin(),
            m_state.end(),
            [this](const T& p_a, const T& p_b) { return (*static_cast<Compare*>(this))(p_a, p_b) == 0; });
        if (firstDuplicate != m_state.end())
        {
            throw std::runtime_error("Duplicates in the dataset are not supported at the moment.");
        }
    }

    // Not copyable to avoid concurrent access issues on the same data without proper synchronization.
    PermuteView(const PermuteView&) = delete;
    PermuteView& operator=(const PermuteView&) = delete;

    // Moveable.
    PermuteView(PermuteView&&) = default;
    PermuteView& operator=(PermuteView&&) = default;

    bool Advance()
    {
        // The Advance algorithm works assuming there is some non-empty hidden state in the permutation
        // past some index which is controlled by m_n. Permutations of size n - 1, over a set n element is
        // still n! permutations and the hidden state completes the full permutation.
        int64_t hiddenStateIdx = m_n == m_state.size() ? m_n - 1 : m_n;
        auto lastVisibleStateIdx = hiddenStateIdx - 1;
        int64_t swapIdx = -1;

        if (m_lastHiddenStateIdxUsed < 0)
        {
            for (auto i = hiddenStateIdx; i < std::ssize(m_state); ++i)
            {
                if (m_state[lastVisibleStateIdx] < m_state[i])
                {
                    swapIdx = i;
                    m_lastHiddenStateIdxUsed = i;
                    break;
                }
            }
        }
        else if (m_lastHiddenStateIdxUsed < (std::ssize(m_state) - 1))
        {
            swapIdx = m_lastHiddenStateIdxUsed + 1;
            ++m_lastHiddenStateIdxUsed;
        }
        else
        {
            m_lastHiddenStateIdxUsed = -1;
        }

        if (swapIdx >= 0)
        {
            std::swap(m_state[lastVisibleStateIdx], m_state[swapIdx]);
            return true;
        }

        int64_t backtrackIdx = hiddenStateIdx;
        while (--backtrackIdx > 0)
        {
            if (m_state[backtrackIdx] > m_state[backtrackIdx - 1])
            {
                int64_t probeIdx = backtrackIdx;
                int64_t minimumLargerIdx = probeIdx;
                while (probeIdx < std::ssize(m_state))
                {
                    if (m_state[backtrackIdx - 1] < m_state[probeIdx] && m_state[probeIdx] < m_state[minimumLargerIdx])
                    {
                        minimumLargerIdx = probeIdx;
                    }
                    ++probeIdx;
                }

                std::swap(m_state[backtrackIdx - 1], m_state[minimumLargerIdx]);
                std::reverse(m_state.begin() + backtrackIdx, m_state.begin() + hiddenStateIdx);
                std::rotate(m_state.begin() + backtrackIdx, m_state.begin() + hiddenStateIdx, m_state.end());

                return true;
            }
        }

        // All the values in the state were in ascending order starting from the last
        // value which means the last permutation was reached.
        return false;
    }

    // Move the permutation back to the initial state.
    void Reset()
    {
        std::sort(
            m_state.begin(),
            m_state.end(),
            [this](const T& p_left, const T& p_right) { return (*static_cast<Compare*>(this))(p_left, p_right) < 0; });
    }

    // Provides the current view.
    std::span<const T> Current()
    {
        return m_state.subspan(0, m_n);
    }

    // Given any current state, provides the elements not in the current permutation.
    std::span<const T> Hidden()
    {
        return m_state.subspan(m_n);
    }

private:
    // The values that are being permuted over.
    std::span<T> m_state;

    // The number of values to include in the visible state.
    uint64_t m_n;

    // In the hidden state, keeps track of the last index that was swapped into the
    // visible state to allow for a small optimization on the path where the last
    // value in a permutation is incremented via values from the hidden state.
    int64_t m_lastHiddenStateIdxUsed;
};
