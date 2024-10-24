#pragma once

#include <cstdlib>
#include <type_traits>
#include <utility>

namespace mg
{
    namespace detail
    {
        template <std::size_t N>
        struct iter_n_impl;

        template <>
        struct iter_n_impl<1>
        {
            template <typename Fn, typename T1, typename... Ts>
            void operator()(Fn&& p_fn, T1&& p_t1, Ts&&... p_ts)
            {
                p_fn(std::forward<T1>(p_t1));

                if constexpr (sizeof...(p_ts) != 0)
                {
                    (*this)(p_fn, std::forward<Ts>(p_ts)...);
                }
            }
        };

        template <>
        struct iter_n_impl<2>
        {
            template <typename Fn, typename T1, typename T2, typename... Ts>
            void operator()(Fn&& p_fn, T1&& p_t1, T2&& p_t2, Ts&&... p_ts)
            {
                p_fn(std::forward<T1>(p_t1), std::forward<T2>(p_t2));

                if constexpr (sizeof...(p_ts) != 0)
                {
                    (*this)(p_fn, std::forward<Ts>(p_ts)...);
                }
            }
        };
    }

    template <auto Fn>
    struct func
    {
        template <typename... Ts>
        decltype(auto) operator()(Ts&&... p_ts) const noexcept(std::is_nothrow_invocable_r_v<std::invoke_result_t<decltype(Fn), Ts&&...>, decltype(Fn), Ts&&...>)
        {
            return Fn(std::forward<Ts>(p_ts)...);
        }
    };

    template <std::size_t N, typename Fn, typename... Ts>
    void iter_n(Fn&& p_fn, Ts&&... p_ts)
    {
        if constexpr (sizeof...(p_ts) == 0)
        {
            return;
        }

        detail::iter_n_impl<N>{}(std::forward<Fn>(p_fn), std::forward<Ts>(p_ts)...);
    }
}