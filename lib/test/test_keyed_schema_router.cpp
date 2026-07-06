#include "KeyedSchemaRouter.hpp"

#include "Executor.hpp"
#include "ParameterResolver.hpp"

#include <catch2/catch_test_macros.hpp>
#include <mg/functional.hpp>

#include <cstdint>
#include <functional>
#include <string>

TEST_CASE("basic router add usage") {
    auto router = euler::KeyedSchemaRouter<
        euler::Key<std::string>,
        int32_t,
        euler::EmptyParameterResolver,
        euler::StaticExecutor>();

    router.Add(euler::Key("delaware"), euler::S(), mg::constant_func<1>{});
    router.Add(euler::Key("pennsylvania"), euler::S(), mg::constant_func<2>{});

    REQUIRE(router.Route(euler::Key("delaware"))() == 1);
    REQUIRE(router.Route(euler::Key("pennsylvania"))() == 2);
}

TEST_CASE("fluent router add usage") {
    auto router = euler::KeyedSchemaRouter<
        euler::Key<std::string>,
        int32_t,
        euler::EmptyParameterResolver,
        euler::StaticExecutor>();

    router
        .Add(euler::K("delaware"), euler::S(), mg::constant_func<1>{})
        .Add(euler::K("pennsylvania"), euler::S(), mg::constant_func<2>{});

    REQUIRE(router.Route(euler::Key("delaware"))() == 1);
    REQUIRE(router.Route(euler::Key("pennsylvania"))() == 2);
}

TEST_CASE("templated register function call") {
    auto router = euler::KeyedSchemaRouter<
        euler::Key<std::string>,
        std::string,
        euler::EmptyParameterResolver,
        euler::StaticExecutor>();

    constexpr auto post = [](int32_t id) { return "added_" + std::to_string(id); };
    constexpr auto get = [](int32_t id) { return "user_id_" + std::to_string(id); };

    router
        .Register<post>(euler::K("post_1"), euler::S(euler::Bind<int32_t>{ 1 }))
        .Register<get>(euler::K("get_1"), euler::S(euler::Bind<int32_t>{ 1 }));

    REQUIRE(router.Route(euler::K("post_1"))() == "added_1");
    REQUIRE(router.Route(euler::K("get_1"))() == "user_id_1");
}
