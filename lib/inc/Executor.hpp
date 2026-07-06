#pragma once

#include <utility>

namespace euler
{
    struct StaticExecutor
    {
        template <auto V, typename... Ts>
        decltype(auto) operator()(Ts&&... p_ts)
        {
            return V(std::forward<Ts>(p_ts)...);
        }
    };

}