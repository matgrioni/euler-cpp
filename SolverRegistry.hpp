#pragma once

#include <cstdint>
#include <format>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Solver.hpp"

template <typename T>
struct Param 
{
    using Type = T;

    // TODO: Create structural string class to use here...
    std::string m_name;
};

template <typename T>
struct Literal
{
    using Type = T;

    T m_value;
};

namespace detail
{
    template <typename, template <typename...> typename>
    struct IsInstanceImpl : public std::false_type {};

    template <template <typename...> typename U, typename...Ts>
    struct IsInstanceImpl<U<Ts...>, U> : public std::true_type {};

    template <typename T, template <typename ...> typename U>
    using IsInstance = IsInstanceImpl<std::remove_cvref_t<T>, U>;

    template <typename T, template <typename ...> typename U>
    constexpr bool IsInstanceV = IsInstance<T, U>::value;

    // TODO: Really cleanup naming between, arg, param, parameter, etc
    // TODO: Cleanup reference semantics everywhere to make sure they are all valid.
    template <typename T, typename ParameterResolver, typename... Args>
    std::function<std::unique_ptr<T>()> ParameterFactory(ParameterResolver& p_resolver, Args&&... p_args)
    {
        if constexpr (sizeof...(p_args) == 0)
        {
            return std::make_unique<T>;
        }
        else
        {
            auto factory = [&p_resolver, ...args = std::forward<Args>(p_args)]() mutable -> std::unique_ptr<T>
            {
                // TODO: Anyway to do this better, or separate out into separate file...
                auto process = [&p_resolver](auto& p_recurse, auto&& p_tuple, auto& p_arg, auto&... p_args) mutable -> std::unique_ptr<T>
                {
                    auto iter = [&](auto&& p_nextParam)
                    {
                        auto newTuple = std::tuple_cat(std::forward<decltype(p_tuple)>(p_tuple), std::make_tuple(std::forward<decltype(p_nextParam)>(p_nextParam)));

                        if constexpr (sizeof...(p_args) > 0)
                        {
                            return p_recurse(p_recurse, std::move(newTuple), p_args...);
                        }
                        else
                        {
                            auto makeUniqueHelper = [](auto&&... p_tupleArgs)
                            {
                                return std::make_unique<T>(std::forward<decltype(p_tupleArgs)>(p_tupleArgs)...);
                            };
                            return std::apply(makeUniqueHelper, std::move(newTuple));
                        }
                    };

                    if constexpr (IsInstanceV<decltype(p_arg), Literal>)
                    {
                        return iter(p_arg.m_value);
                    }
                    else
                    {
                        return iter(p_resolver(p_arg));
                    }
                };

                return process(process, std::make_tuple(), args...);
            };

            return factory;
        }
    }
}

// TODO: Restructure this as a sort of general dynamic object factory
template <typename ParameterResolver>
class SolverRegistry
{
public:
    SolverRegistry(const ParameterResolver& p_resolver = ParameterResolver())
        : m_resolver(p_resolver)
    { }

    template <typename SolverT, typename... TArgs>
    SolverRegistry& Add(uint32_t p_solutionId, std::string_view p_name, TArgs&&... p_args)
    {
        auto factory = detail::ParameterFactory<SolverT>(m_resolver, std::forward<TArgs>(p_args)...);

        auto& factoriesById = m_solverFactories[p_solutionId];
        auto [it, inserted] = factoriesById.emplace(std::string(p_name), factory);
        if (!inserted)
        {
            auto err = std::format("Factory with the name {} already exists under solution id {}", p_name, p_solutionId);
            throw std::runtime_error(err);
        }

        return *this;
    }

    std::vector<std::string> GetSolutionNamesForId(uint32_t p_solutionId)
    {
        auto factoriesByIdIt = m_solverFactories.find(p_solutionId);
        if (factoriesByIdIt == m_solverFactories.end())
        {
            return {};
        }

        std::vector<std::string> names;
        auto& factoriesByName = factoriesByIdIt->second;
        for (const auto& [name, factory] : factoriesByName)
        {
            names.push_back(name);
        }

        return names;
    }

    std::unique_ptr<Solver> GetSolver(uint32_t p_solutionId, std::string_view p_name)
    {
        auto factoriesByIdIt = m_solverFactories.find(p_solutionId);
        if (factoriesByIdIt == m_solverFactories.end())
        {
            return {};
        }

        auto factoryIt = factoriesByIdIt->second.find(p_name);
        if (factoryIt == factoriesByIdIt->second.end())
        {
            return {};
        }

        return (factoryIt->second)();
    }

private:
    template <typename TMap>
    struct TransparentHashTo
    {
        using is_transparent = void;

        template <typename T>
        std::size_t operator()(const T& p_val) const noexcept
        {
            return std::hash<TMap>{}(p_val);
        }
    };

    ParameterResolver m_resolver;

    std::unordered_map<uint32_t,
        std::unordered_map<std::string,
                           std::function<std::unique_ptr<Solver>()>,
                           TransparentHashTo<std::string_view>,
                           std::equal_to<>>> m_solverFactories;
};
