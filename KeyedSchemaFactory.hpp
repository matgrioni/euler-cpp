#pragma once

#include <compare>
#include <cstdint>
#include <format>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace euler
{
    /// <summary>
    /// A dynamically resolved parameter in a schema.
    /// </summary>
    /// <typeparam name="T">The type of the parameter.</typeparam>
    template <typename T>
    struct Param 
    {
        /// <summary>
        /// Exposes the type that this parameter is for.
        /// </summary>
        using Type = T;

        /// <summary>
        /// The name of the parameter. The name is used for dynamic resolution by the
        /// ParameterResolver.
        /// </summary>
        std::string m_name;
    };

    /// <summary>
    /// A parameter in the schema which is already bound.
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template <typename T>
    struct Bind
    {
        /// <summary>
        /// Exposes the type that this parameter is for.
        /// </summary>
        using Type = T;

        /// <summary>
        /// The bound value for the parameter.
        /// </summary>
        T m_value;
    };

    namespace detail
    {
        /// <summary>
        /// Implementation false case when checking if a class is an implementation of a template.
        /// </summary>
        /// <typeparam name="">Anonymous type in the false case.</typeparam>
        template <typename, template <typename...> typename>
        struct IsInstanceImpl : public std::false_type {};

        /// <summary>
        /// 
        /// </summary>
        /// <typeparam name="...Ts"></typeparam>
        template <template <typename...> typename U, typename...Ts>
        struct IsInstanceImpl<U<Ts...>, U> : public std::true_type {};

        /// <summary>
        /// 
        /// </summary>
        /// <typeparam name="T"></typeparam>
        template <typename T, template <typename ...> typename U>
        using IsInstance = IsInstanceImpl<std::remove_cvref_t<T>, U>;

        /// <summary>
        /// 
        /// </summary>
        /// <typeparam name="T"></typeparam>
        template <typename T, template <typename ...> typename U>
        constexpr bool IsInstanceV = IsInstance<T, U>::value;

        /// <summary>
        /// 
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <typeparam name="ParameterResolver"></typeparam>
        /// <typeparam name="...SchemaSpecs"></typeparam>
        /// <param name="p_resolver"></param>
        /// <param name="...p_schemaSpecs"></param>
        /// <returns></returns>
        template <typename T, typename ParameterResolver, typename... SchemaSpecs>
        std::function<std::unique_ptr<T>()> CreateSchemaFactory(ParameterResolver& p_resolver, SchemaSpecs&&... p_schemaSpecs)
        {
            if constexpr (sizeof...(p_schemaSpecs) == 0)
            {
                return std::make_unique<T>;
            }
            else
            {
                auto resolve = [&p_resolver](auto&& p_spec) -> decltype(auto)
                {
                    if constexpr (IsInstanceV<decltype(p_spec), Bind>)
                    {
                        return p_spec.m_value;
                    }
                    else
                    {
                        return p_resolver(p_spec);
                    }
                };

                auto factory = [&resolve, ...specs = std::forward<SchemaSpecs>(p_schemaSpecs)]() mutable -> std::unique_ptr<T>
                {
                    return std::make_unique<T>(resolve(specs)...);
                };

                return factory;
            }
        }

        /// <summary>
        /// Iterate in parallel over the elements in a defined set of tuples. Iteration stops
        /// with the shortest tuple.
        /// </summary>
        /// <typeparam name="TFn">The type of the callable to invoke.</typeparam>
        /// <typeparam name="...Tuples">The set of tuples to iterate over.</typeparam>
        /// <param name="p_fn">The callable to invoke over the tuples. Will have one argument in parallel
        /// for each tuple.</param>
        /// <param name="...p_tuples">The tuples to iterate over.</param>
        template <typename TFn, typename... Tuples>
        void IterTuples(TFn&& p_fn, Tuples&&... p_tuples)
        {
            constexpr static auto DimCount = std::min({ std::tuple_size_v<std::decay_t<Tuples>>... });
            if constexpr (DimCount == 0)
            {
                return;
            }

            IterTuplesImpl<0, DimCount>(std::forward<TFn>(p_fn), std::forward<Tuples>(p_tuples)...);
        }

        /// <summary>
        /// Implementation helper for the IterTuples method which actually does recursion.
        /// </summary>
        /// <typeparam name="TFn">The type of the callable to use when iterating.</typeparam>
        /// <typeparam name="...Tuples">The type list for the tuples to iterate over.</typeparam>
        /// <typeparam name="Idx">The current index of the iteration.</typeparam>
        /// <typeparam name="Max">The max index of the iteration. Idx will always be less than Max.</typeparam>
        /// <param name="p_fn">The callable to invoke when iterating over the tuples.</param>
        /// <param name="...p_tuples">The tuples to iterate over up to the size of the smallest one.</param>
        template <auto Idx, auto Max, typename TFn, typename... Tuples>
        void IterTuplesImpl(TFn&& p_fn, Tuples&&... p_tuples)
        {
            if constexpr (std::is_assignable_v<bool&, std::invoke_result_t<TFn&&, decltype(std::get<Idx>(std::forward<Tuples>(p_tuples)))...>>)
            {
                bool continueIteration = std::invoke(p_fn, std::get<Idx>(std::forward<Tuples>(p_tuples))...);
                if (!continueIteration)
                {
                    return;
                }
            }
            else
            {
                std::invoke(p_fn, std::get<Idx>(std::forward<Tuples>(p_tuples))...);
            }

            if constexpr ((Idx + 1) < Max)
            {
                IterTuplesImpl<Idx + 1, Max>(std::forward<TFn>(p_fn), std::forward<Tuples>(p_tuples)...);
            }
        }
    }

    /// <summary>
    /// The key type that is parameterized for KeyedSchemaFactory.
    /// </summary>
    /// <typeparam name="...Ts">The type list of the key.</typeparam>
    template <typename... Ts>
    using Key = std::tuple<Ts...>;

    /// <summary>
    /// A helper function to help with lookup on KeyedSchemaFactory. This will perfectly forward the parameters
    /// to the lookup which may allow for more efficient lookup.
    /// </summary>
    /// <typeparam name="...Ts">The types of the parameters that are forwarded.</typeparam>
    /// <param name="...p_ts">The heterogenous list of parameters to use for lookup.</param>
    /// <returns></returns>
    template <typename... Ts>
    decltype(auto) ForwardKey(Ts&&... p_ts) noexcept
    {
        return std::forward_as_tuple(std::forward<Ts>(p_ts)...);
    }

    /// <summary>
    /// A KeyedSchemaFactory has three main parts to it. It is a factory (used to create object instances)
    /// that is schema based (information is provided on how to create an individual object), and the schemas
    /// are associated with a specific key.
    /// </summary>
    /// <typeparam name="Key">The key type that the different schema are stored under.</typeparam>
    /// <typeparam name="Base">The base type that this factory is for. All instances created by the factory should be a subclass of this.</typeparam>
    /// <typeparam name="ParameterResolver">The type of object that is used to resolve unbound parameters.</typeparam>
    template <typename Key, typename Base, typename ParameterResolver>
    class KeyedSchemaFactory
    {
    public:
        /// <summary>
        /// Create a new KeyedSchemaFactory with the resolver provided.
        /// </summary>
        /// <param name="p_resolver">The resolver to use for unbound parameter.</param>
        KeyedSchemaFactory(const ParameterResolver& p_resolver = ParameterResolver())
            : m_resolver(p_resolver)
        { }

        /// <summary>
        /// Add (register) a new type on this factory. The registration must create a subtype of the type the overall
        /// factory is for. This adds an association between the provided key and the specified schema for the creation
        /// of the concrete type.
        /// </summary>
        /// <typeparam name="Concrete">The specific type this is adding to the factory.</typeparam>
        /// <typeparam name="LookupKey">The key to add this schema instance under.</typeparam>
        /// <typeparam name="...SchemaSpecs">The types of the parameters that define the schema.</typeparam>
        /// <param name="p_key">The actual key instance to add an entry for.</param>
        /// <param name="...p_schemaSpecs">The different schema specifications (one of Param, Bind, etc) which
        /// detail how this type should be created at runtime for this key.</param>
        /// <returns>This factory instance to allow for a builder interface.</returns>
        template <typename Concrete, typename LookupKey, typename... SchemaSpecs>
        KeyedSchemaFactory& Add(LookupKey&& p_key, SchemaSpecs&&... p_schemaSpecs)
        {
            auto factory = detail::CreateSchemaFactory<Concrete>(m_resolver, std::forward<SchemaSpecs>(p_schemaSpecs)...);

            // Use emplace over try_emplace for the following reasons:
            //   * Insertion failures are expected to be very rare and cause an exception anyway so the cost of
            //     construction in this case is an acceptable tradeoff.
            //   * This allows for immovable and uncopyable types to be used in the key and passed through directly
            //     during construction.
            auto [_, inserted] = m_factories.emplace(
                std::piecewise_construct,
                std::forward<LookupKey>(p_key),
                std::forward_as_tuple(std::move(factory)));
            if (!inserted)
            {
                throw std::runtime_error("Factory already exists with the given key.");
            }

            return *this;
        }

        /// <summary>
        /// Iterate over the keys in the current that match a partial lookup key. The lookup
        /// key can be a partial starting subsequence of an already registered key. The callable
        /// is invoked with the matching keys.
        /// </summary>
        /// <typeparam name="LookupKey">The type of key passed to do the lookup with.</typeparam>
        /// <typeparam name="TFn">The type of the provided callable.</typeparam>
        /// <param name="p_key">The key value to do lookup with. Matching keys will start with this key argument.</param>
        /// <param name="p_fn">The callable to invoke with every matching key.</param>
        template <typename LookupKey, typename TFn>
        void PartialMatch(LookupKey&& p_key, TFn&& p_fn)
        {
            // Check that the provided key is actually of the appropriate size. If not, it cannot match.
            if constexpr (std::tuple_size_v<LookupKey> > std::tuple_size_v<Key>)
            {
                return;
            }

            auto [start, end] = m_factories.equal_range(std::forward<LookupKey>(p_key));

            for (auto it = start; it != end; ++it)
            {
                std::apply(p_fn, it->first);
            }
        }

        /// <summary>
        /// Creates an instance of the type using the factory registered under the given key. The lookup key is
        /// heterogenous and comparisons will be done transparently at a per element level. If the key does not
        /// exist then, an empty object is returned. Otherwise, the result of calling the factory is returned.
        /// </summary>
        /// <typeparam name="LookupKey">The type of the passed in key. Should not usually need to be specified.</typeparam>
        /// <param name="p_key">The key to use for looking up the registered factory.</param>
        /// <returns>Any instance created by the factory if it was found via the key.</returns>
        template <typename LookupKey>
        std::unique_ptr<Base> Create(LookupKey&& p_key)
        {
            // The factory map considers partial keys as equal to the full key, so this is to protect against
            // a tuple that is a partial match of a key in the map from being considered found in the map.
            if constexpr (std::tuple_size_v<LookupKey> != std::tuple_size_v<Key>)
            {
                return {};
            }

            auto it = m_factories.find(std::forward<LookupKey>(p_key));
            if (it == m_factories.end())
            {
                return {};
            }

            return (it->second)();
        }

    private:
        /// <summary>
        /// Internal helper struct to help with comparison between two comparable tuple
        /// types. This will compare two tuple types lexicographically up to the largest
        /// ordinal element in the tuple. If they are equal up to this point, then the
        /// first instance is not less than the first. Essentially it defines two tuples
        /// as equal if one is a starting subsequence of the other.
        /// </summary>
        struct HeterogenousTupleLess
        {
            /// <summary>
            /// Used by std::map to allow for heterogenous lookup and allow for non-key
            /// types to be passed into certain APIs.
            /// </summary>
            using is_transparent = void;

            /// <summary>
            /// Compares two tuple like objects and returns true if the first is less than the second.
            /// </summary>
            /// <typeparam name="T1">The type of the first parameter.</typeparam>
            /// <typeparam name="T2">The type of the second parameter.</typeparam>
            /// <param name="p_t1">The first parameter which is perfectly forwarded through.</param>
            /// <param name="p_t2">The second parameter which is perfectly forwarded through.</param>
            /// <returns>True if the first instance is less than the second instance.</returns>
            template <typename T1, typename T2>
            bool operator()(T1&& p_t1, T2&& p_t2) const
            {
                bool lt = false;
                detail::IterTuples(
                    [&](auto&& p_lhs, auto&& p_rhs)
                    {
                        using U1 = decltype(p_lhs);
                        using U2 = decltype(p_rhs);

                        int compare{};
                        if constexpr (std::three_way_comparable<U1, U2>)
                        {
                            compare = std::forward<U1>(p_lhs) <=> std::forward<U2>(p_rhs);
                        }
                        else
                        {
                            if (p_lhs < p_rhs)
                            {
                                compare = -1;
                            }
                            else if (p_rhs < p_lhs)
                            {
                                compare = 1;
                            }
                        }

                        if (compare == 0)
                        {
                            return true;
                        }
                        else
                        {
                            lt = compare < 0;
                            return false;
                        }
                    },
                    p_t1,
                    p_t2);

                return lt;
            }
        };

        /// <summary>
        /// The instance that is responsible for resolving unbound parameters in the factory.
        /// </summary>
        ParameterResolver m_resolver;

        /// <summary>
        /// The storage for the factories. A map is used to allow for ordering of the factories
        /// based on their Key, which will allow for partial lookup later. It does come at a slight
        /// inefficiency if the full key is known compared to an unordered_map, however for this toy
        /// example it is fine and illustrates the partial lookup solution.
        /// </summary>
        std::map<
            Key,
            std::function<std::unique_ptr<Base>()>,
            HeterogenousTupleLess> m_factories;
    };

}

