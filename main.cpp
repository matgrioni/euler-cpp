#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>

#include "problems.hpp"
#include "Sieve.hpp"
#include "Solver.hpp"
#include "KeyedSchemaRouter.hpp"

#include <cxxopts.hpp>

#include <vector>

using namespace euler;

namespace
{
    /// <summary>
    /// 
    /// </summary>
    class CinParameterResolver
    {
    public:
        /// <summary>
        /// 
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="p_param"></param>
        /// <returns></returns>
        template <typename T>
        T operator()(Param<T> p_param)
        {
            T in;
            std::cout << "Please input parameter with name " << p_param.m_name << ": ";
            std::cin >> in;
            return in;
        }
    };

    struct StaticExecutor
    {
        template <auto V, typename... Ts>
        auto operator()(Ts&&... p_ts)
        {
            return V(std::forward<Ts>(p_ts)...);
        }
    };

    using SolutionRouter = KeyedSchemaRouter<Key<uint32_t, std::string>, int64_t, CinParameterResolver, StaticExecutor>;

    /// <summary>
    /// 
    /// </summary>
    /// <typeparam name="Key"></typeparam>
    /// <typeparam name="ParameterResolver"></typeparam>
    /// <param name="p_factory"></param>
    void InitializeRouter(SolutionRouter& p_router)
    {
        p_router
            .Register<P1>(K(1, "Project Euler"), S(1000ll))
            .Register<P1>(K(1, "Unbound"), S(Param<int64_t>("MultipleMax")))
            .Register<P2Naive>(K(2, "Naive -- Project Euler"), S(4'000'000ll))
            .Register<P2Naive>(K(2, "Naive -- Unbound"), S(Param<int64_t>("UpTo")))
            .Register<P2Optimization1>(K(2, "Naive Optimized -- Project Euler"), S(4'000'000ll))
            .Register<P2Optimization1>(K(2, "Naive Optimized -- Unbound"), S(Param<int64_t>("UpTo")))
            .Register<P3>(K(3, "Sieve -- Project Euler"), S(600'851'475'143ll))
            .Register<P3>(K(3, "Sieve -- Unbound"), S(Param<int64_t>("Factorize")))
            .Register<P4>(K(4, "Project Euler"), S(3))
            .Register<P4>(K(4, "Unbound"), S(Param<int64_t>("Digits")))
            .Register<P31>(K(31, "Main"))
            .Register<P31>(K(32, "Main"));
    }
}

int main(int argc, char* argv[])
{
    cxxopts::Options options("euler-cpp", "The driver program for executing Project Euler solutions in different ways.");

    options.add_options()
        ("SolverSelection", "The way to do solver selection. One of dynamic or cmd.", cxxopts::value<std::string>())
        ("SolverNumber", "The problem number of the solver. Only used in dynamic SolverSelection.", cxxopts::value<uint32_t>())
        ("SolverName", "The name of the solver. Only used in dynamic SolverSelection.", cxxopts::value<std::string>())
        ("ParameterResolution", "The strategies for parameter resolution. Only cin is accepted currently.", cxxopts::value<std::string>())
        ("ExecType", "The type of execution to run. One of single or experiment.", cxxopts::value<std::string>())
        ("ExecCount", "The number of times to run the problem. Only used in experiment ExecType.", cxxopts::value<uint32_t>());
    options.allow_unrecognised_options();
    auto optionsResult = options.parse(argc, argv);

    SolutionRouter router;
    InitializeRouter(router);

    uint32_t solverId;
    std::string solverName;
    const auto& selectionType = optionsResult["SolverSelection"].as<std::string>();
    if (selectionType == "dynamic")
    {
        std::cout << "Problem to solve: ";
        std::cin >> solverId;

        std::vector<const std::string*> names;
        router.PartialMatch(K(solverId), [&](uint32_t, const std::string& p_name) { names.push_back(&p_name);  });
        if (names.empty())
        {
            std::cout << "No solver exists for problem " << solverId << std::endl;
            return 1;
        }

        for (auto i = 0u; i < names.size(); ++i)
        {
            std::cout << i + 1 << ": " << *names[i] << std::endl;
        }

        uint32_t solverNameId;
        std::cout << "Enter solver to execute: ";
        std::cin >> solverNameId;
        if (solverNameId == 0 || solverNameId > names.size())
        {
            std::cout << "Solver index was not in bounds." << std::endl;
            return 1;
        }
        solverName = *names[solverNameId - 1];
    }
    else if (selectionType == "cmd")
    {
        solverId = optionsResult["SolverNumber"].as<uint32_t>();
        solverName = optionsResult["SolverName"].as<std::string>();
    }
    else
    {
        std::cout << "\"" << selectionType << "\" is not a valid SelectionType" << std::endl;
    }

    auto solver = router.Route(K(solverId, solverName));

    const auto& execType = optionsResult["ExecType"].as<std::string>();
    uint32_t runCount{};
    if (execType == "single")
    {
        runCount = 1;
    }
    else if (execType == "experiment")
    {
        runCount = optionsResult["ExecCount"].as<uint32_t>();
    }

    auto baseline = solver();

    auto start = std::chrono::steady_clock::now();
    for (auto i = 0u; i < runCount; ++i)
    {
        auto iter = solver();
        if (baseline != iter)
        {
            throw std::runtime_error("Answer instability detected.");
        }
    }
    auto end = std::chrono::steady_clock::now();
    auto avg = (end - start) / static_cast<double>(runCount);

    std::cout << "Final Answer: " << baseline << std::endl;
    std::cout << "Run Count: " << runCount << std::endl;
    std::cout << "Average Runtime: " << avg << std::endl;

    return 0;
}
