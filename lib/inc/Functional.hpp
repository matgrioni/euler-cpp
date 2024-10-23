#pragma once

#include <type_traits>
#include <utility>

namespace mg
{
    template <auto Fn>
    struct func
    {
        template <typename... Ts>
        decltype(auto) operator()(Ts&&... p_ts) const noexcept(std::is_nothrow_invocable_r_v<std::invoke_result_t<decltype(Fn), Ts&&...>, decltype(Fn), Ts&&...>)
        {
            return Fn(std::forward<Ts>(p_ts)...);
        }
    };
}