#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>

#include "1.hpp"
#include "2.hpp"
#include "31.hpp"
#include "32.hpp"
#include "Solver.hpp"
#include "KeyedSchemaFactory.hpp"

#include "cxxopts.hpp"

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

    /// <summary>
    /// 
    /// </summary>
    /// <typeparam name="Key"></typeparam>
    /// <typeparam name="ParameterResolver"></typeparam>
    /// <param name="p_factory"></param>
    template <typename Key, typename Base, typename ParameterResolver>
    void InitializeFactory(KeyedSchemaFactory<Key, Base, ParameterResolver>& p_factory)
    {
        p_factory.Add<Solver1>(ForwardKey(1, "Project Euler"), Bind{ 1000ll })
                 .Add<Solver1>(ForwardKey(1, "Unbound"), Param<int64_t>("MultipleMax"))
                 .Add<Solver2_1>(ForwardKey(2, "Naive -- Project Euler"), Bind{ 4'000'000ll })
                 .Add<Solver2_1>(ForwardKey(2, "Naive -- Unbound"), Param<int64_t>("UpTo"))
                 .Add<Solver2_2>(ForwardKey(2, "Naive Optimized -- Project Euler"), Bind{ 4'000'000ll })
                 .Add<Solver2_2>(ForwardKey(2, "Naive Optimized -- Unbound"), Param<int64_t>("UpTo"))
                 .Add<Solver31_1>(ForwardKey(31, "Main"))
                 .Add<Solver32_1>(ForwardKey(32, "Main"));
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

    KeyedSchemaFactory<Key<uint32_t, std::string>, Solver, CinParameterResolver> factory;
    InitializeFactory(factory);

    uint32_t solverId;
    std::string solverName;
    const auto& selectionType = optionsResult["SolverSelection"].as<std::string>();
    if (selectionType == "dynamic")
    {
        std::cout << "Problem to solve: ";
        std::cin >> solverId;

        std::vector<std::string> names;
        factory.PartialMatch(ForwardKey(solverId), [&](auto, const std::string& p_name) { names.push_back(p_name);  });
        if (names.empty())
        {
            std::cout << "No solver exists for problem " << solverId << std::endl;
            return 1;
        }

        for (auto i = 0u; i < names.size(); ++i)
        {
            std::cout << i + 1 << ": " << names[i] << std::endl;
        }

        uint32_t solverNameId;
        std::cout << "Enter solver to execute: ";
        std::cin >> solverNameId;
        if (solverNameId == 0 || solverNameId > names.size())
        {
            std::cout << "Solver index was not in bounds." << std::endl;
            return 1;
        }
        solverName = names[solverNameId - 1];
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

    auto solver = factory.Create(ForwardKey(solverId, solverName));
    if (!solver)
    {
        std::cout << "Factory creation failed with solver id \"" << solverId << "\" and name \"" << solverName << "\"" << std::endl;
        return 1;
    }

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

    auto start = std::chrono::steady_clock::now();
    for (auto i = 0u; i < runCount; ++i)
    {
        (*solver)();
    }
    auto end = std::chrono::steady_clock::now();
    auto avg = (end - start) / static_cast<double>(runCount);

    auto answer = (*solver)();

    std::cout << "Final Answer: " << answer << std::endl;
    std::cout << "Run Count: " << runCount << std::endl;
    std::cout << "Average Runtime: " << avg << std::endl;

    return 0;
}
