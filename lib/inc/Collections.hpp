#pragma once

#include <cstdlib>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace mg
{
    namespace detail
    {
        /// <summary>
        /// Implementation helper for iter_zipped_tuples which actually does the recursion.
        /// </summary>
        /// <typeparam name="TFn">The type of the callable to use when iterating.</typeparam>
        /// <typeparam name="...Tuples">The type list for the tuples to iterate over.</typeparam>
        /// <typeparam name="Idx">The current index of the iteration.</typeparam>
        /// <typeparam name="Max">The max index of the iteration. Idx will always be less than Max.</typeparam>
        /// <param name="p_fn">The callable to invoke when iterating over the tuples which can also signal to stop
        /// iteration by returning false.</param>
        /// <param name="...p_tuples">The tuples to iterate over up to the size of the smallest one.</param>
        /// <returns>False if the iteration was stopped by the callable and true otherwise.</returns>
        template <std::size_t Idx, std::size_t Max, typename TFn, typename... Tuples>
        bool iter_zipped_tuples_impl(TFn&& p_fn, Tuples&&... p_tuples)
        {
            if constexpr (std::is_assignable_v<bool&, std::invoke_result_t<TFn&&, decltype(std::get<Idx>(std::forward<Tuples>(p_tuples)))...>>)
            {
                bool continueIteration = std::invoke(p_fn, std::get<Idx>(std::forward<Tuples>(p_tuples))...);
                if (!continueIteration)
                {
                    return false;
                }
            }
            else
            {
                std::invoke(p_fn, std::get<Idx>(std::forward<Tuples>(p_tuples))...);
            }

            if constexpr ((Idx + 1) < Max)
            {
                return iter_zipped_tuples_impl<Idx + 1, Max>(std::forward<TFn>(p_fn), std::forward<Tuples>(p_tuples)...);
            }
            else
            {
                return true;
            }
        }

        template <typename Fn, typename Tuple, std::size_t... I>
        auto tuple_map_impl(Tuple&& p_tuple, Fn&& p_fn, std::index_sequence<I...>)
        {
            return std::make_tuple(p_fn(std::get<I>(std::forward<Tuple>(p_tuple)))...);
        }
    }

    /// <summary>
    /// Iterate in parallel over the elements in a defined set of tuples. Iteration will stop when the end of the
    /// smallest tuple is reached. Additionally, the callable signature may return a bool which can be used to signal
    /// whether iteration is to continue.
    /// </summary>
    /// <typeparam name="Fn">The type of the callable to invoke.</typeparam>
    /// <typeparam name="...Tuples">The set of tuples to iterate over.</typeparam>
    /// <param name="p_fn">The callable to invoke over the tuples. Will have one argument in parallel for each tuple
    /// and can signal to stop iteration by returning false.</param>
    /// <param name="...p_tuples">The tuples to iterate over.</param>
    /// <returns>False if the callable signaled iteration to stop and true otherwise.</returns>
    template <typename Fn, typename... Tuples>
    bool iter_zipped_tuples(Fn&& p_fn, Tuples&&... p_tuples)
    {
        constexpr static std::size_t DimCount = std::min({ std::tuple_size_v<std::remove_cvref_t<Tuples>>... });
        if constexpr (DimCount == 0)
        {
            return true;
        }

        return detail::iter_zipped_tuples_impl<0, DimCount>(std::forward<Fn>(p_fn), std::forward<Tuples>(p_tuples)...);
    }

    /// <summary>
    /// Apply a callable to all members of a tuple resulting in a new tuple.
    /// </summary>
    /// <remarks>This method will take the value category of p_tuple into account when passing arguments to the callable
    /// but the returned value is created with the same semantics as make_tuple. This means that if one of the tuple
    /// element types should be an l-value reference, then a reference_wrapper should be returned. If the type should be
    /// an r-value reference, use another library, because I don't know how to do that or if that would be safe.</remarks>
    /// <typeparam name="Tuple">The type of the tuple being mapped.</typeparam>
    /// <typeparam name="Fn">The type of the callable being applied.</typeparam>
    /// <param name="p_tuple">The tuple being mapped.</param>
    /// <param name="p_fn">The callable being applied to each tuple element.</param>
    /// <returns>The mapped tuple.</returns>
    template <typename Fn, typename Tuple>
    auto tuple_map(Tuple&& p_tuple, Fn&& p_fn)
    {
        return detail::tuple_map_impl(
            std::forward<Tuple>(p_tuple),
            std::forward<Fn>(p_fn),
            std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<Tuple>>>{});
    }
}