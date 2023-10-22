#include <chrono>
#include <cstdlib>
#include <iostream>
#include <utility>

#include "1.hpp"
#include "31.hpp"
#include "32.hpp"
#include "Solver.hpp"
#include "KeyedSchemaFactory.hpp"

using namespace euler;

namespace
{
    class CinParameterResolver
    {
    public:
        template <typename T>
        T operator()(Param<T> p_param)
        {
            T in;
            std::cout << "Please input parameter with name " << p_param.m_name << ": ";
            std::cin >> in;
            return in;
        }
    };

    template <typename ParamKey>
    void Test(ParamKey&& p_key)
    {
        static_assert(std::is_same_v<std::decay_t<ParamKey>, std::tuple<int&, double&>>);
    }
}

int main(int argc, char* argv[])
{
    // TODO: This need to be changed to use an actual library.
    std::string inputMethod;
    std::unordered_map<std::string, std::string> nameToValue;

    if (argc > 1)
    {
        std::string inputMethod = argv[1];
        std::unordered_map<std::string, std::string> nameToValue;
        for (auto i = 2; i < argc; i += 2)
        {
            std::string name = argv[i];
            std::string value = argv[i + 1];
            nameToValue[std::move(name)] = std::move(value);
        }
    }

    KeyedSchemaFactory<Key<uint32_t, std::string>, Solver, CinParameterResolver> registry;
    registry.Add<Solver1_1>(ForwardKey(1, "Signed Arithmetic"), Param<uint64_t>("MultipleMax"))
            .Add<Solver1_1>(ForwardKey(1, "Project Euler -- Signed Arithmetic"), Bind{ 1000ull })
            .Add<Solver1_2>(ForwardKey(1, "Arithmetic"), Param<int64_t>("MultipleMax"));

    registry.Add<Solver31_1>(ForwardKey(31, "Main"));

    registry.Add<Solver32_1>(ForwardKey(32, "Main"));

    uint32_t solverId;
    std::cout << "Problem to solve: ";
    std::cin >> solverId;

    std::vector<std::string> names;
    registry.PartialMatch(ForwardKey(solverId), [&](auto, const std::string& p_name) { names.push_back(p_name);  });
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

    // It is guaranteed that the solver entry will be valid and found since the name is taken
    // from the list of already registered names.
    auto solver = registry.Create(ForwardKey(solverId, names[solverNameId - 1]));

    auto start = std::chrono::steady_clock::now();
    for (auto i = 0; i < 1000000; ++i)
    {
        (*solver)();
    }
    auto end = std::chrono::steady_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;

    return 0;
}
