#pragma once

#include <compare>
#include <cstdint>
#include <format>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "Collections.hpp"
#include "Types.hpp"

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
        template <typename Fn, typename... SchemaSpecs>
        using SchemaExecResultT = std::invoke_result_t<Fn, typename SchemaSpecs::Type...>;

        /// <summary>
        /// 
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <typeparam name="ParameterResolver"></typeparam>
        /// <typeparam name="...SchemaSpecs"></typeparam>
        /// <param name="p_resolver"></param>
        /// <param name="...p_schemaSpecs"></param>
        /// <returns></returns>
        template <typename ParameterResolver, typename Fn, typename... SchemaSpecs>
        auto CreateExecutable(
            ParameterResolver& p_resolver,
            Fn&& p_fn,
            SchemaSpecs&&... p_schemaSpecs) -> std::function<SchemaExecResultT<Fn, SchemaSpecs...>()>
        {
            if constexpr (sizeof...(p_schemaSpecs) == 0)
            {
                return std::forward<Fn>(p_fn);
            }
            else
            {
                auto resolve = [&p_resolver](auto&& p_spec) -> decltype(auto)
                {
                    if constexpr (mg::is_instance_v<decltype(p_spec), Bind>)
                    {
                        return p_spec.m_value;
                    }
                    else
                    {
                        return p_resolver(p_spec);
                    }
                };

                auto exec = [
                    resolve,
                    fn = std::forward<Fn>(p_fn),
                    ...specs = std::forward<SchemaSpecs>(p_schemaSpecs)]() mutable -> SchemaExecResultT<Fn, SchemaSpecs...>
                {
                    return fn(resolve(specs)...);
                };

                return exec;
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
    /// <returns>The arguments perfectly forwarded and packed in a tuple.</returns>
    template <typename... Ts>
    decltype(auto) K(Ts&&... p_ts) noexcept
    {
        return std::forward_as_tuple(std::forward<Ts>(p_ts)...);
    }

    template <typename... Ts>
    decltype(auto) S(Ts&&... p_ts) noexcept
    {
        return std::forward_as_tuple(std::forward<Ts>(p_ts)...);
    }

    template <typename T>
    struct DynamicRegistration{};

    /// <summary>
    /// The idea behind a KeyedSchemaRouter is to use a key which is associated with some schema whose parameters are
    /// fixed at compile time or resolved dynamically according to some policy. This combination of key and schema is
    /// then associated with some executable code which this will route to as necessary.
    /// </summary>
    /// <typeparam name="Key">The key type that the different schema are stored under.</typeparam>
    /// <typeparam name="R">The type that this router will return when its keyed logic is executed.</typeparam>
    /// <typeparam name="ParameterResolver">The type of object that is used to resolve unbound parameters.</typeparam>
    template <typename Key, typename R, typename ParameterResolver, template <typename> typename RegistrationPolicy = DynamicRegistration>
    class KeyedSchemaRouter
    {
    public:
        /// <summary>
        /// Create a new KeyedSchemaRouter with the resolver provided.
        /// </summary>
        /// <param name="p_resolver">The resolver to use for unbound parameters.</param>
        KeyedSchemaRouter(const ParameterResolver& p_resolver = ParameterResolver())
            : m_resolver(p_resolver)
        { }

        /// <summary>
        /// Add (register) a new function on this router. This adds an association between the provided key and the
        /// specified schema for the creation of the concrete type.
        /// </summary>
        /// <typeparam name="LookupKey">The key to add this schema instance under.</typeparam>
        /// <typeparam name="Fn">The function that this is routing to.</typeparam>
        /// <typeparam name="...SchemaSpecs">The types of the parameters that define the schema.</typeparam>
        /// <param name="p_key">The actual key instance to add an entry for.</param>
        /// <param name="...p_schemaSpecs">The different schema specifications (one of Param, Bind, etc) which
        /// detail how this type should be created at runtime for this key.</param>
        /// <returns>This router instance to allow for a builder interface.</returns>
        template <typename LookupKey, typename Fn, typename... SchemaSpecs>
        KeyedSchemaRouter& Add(LookupKey&& p_key, Fn&& p_fn, SchemaSpecs&&... p_schemaSpecs)
        {
            auto exec = detail::CreateExecutable(
                m_resolver,
                std::forward<Fn>(p_fn),
                std::forward<SchemaSpecs>(p_schemaSpecs)...);

            // Use emplace over try_emplace for the following reasons:
            //   * Insertion failures are expected to be very rare and cause an exception anyway so the unnecessary cost
            //     of construction in this case is acceptable.
            //   * This allows for immovable and uncopyable types to be used in the key and to be directly constructed
            //     in-place in the map.
            auto [_, inserted] = m_execs.emplace(
                std::piecewise_construct,
                std::forward<LookupKey>(p_key),
                std::forward_as_tuple(std::move(exec)));
            if (!inserted)
            {
                throw std::runtime_error("Key already exists on the router.");
            }

            return *this;
        }

        template <typename T, typename LookupKey, typename... SchemaSpecs>
        KeyedSchemaRouter& Register(LookupKey&& p_key, SchemaSpecs&&... p_schemaSpecs)
        {
            return Add(
                std::forward<LookupKey>(p_key),
                RegistrationPolicy<T>{},
                std::forward<SchemaSpecs>(p_schemaSpecs)...);
        }

        template <auto V, typename LookupKey, typename... SchemaSpecs>
        KeyedSchemaRouter& Register(LookupKey&& p_key, SchemaSpecs&&... p_schemaSpecs)
        {
            return Add(
                std::forward<LookupKey>(p_key),
                RegistrationPolicy<V>{},
                std::forward<SchemaSpecs>(p_schemaSpecs)...);
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

            auto [start, end] = m_execs.equal_range(std::forward<LookupKey>(p_key));

            for (auto it = start; it != end; ++it)
            {
                std::apply(p_fn, it->first);
            }
        }

        /// <summary>
        /// Route to an executable instance previously registered. The lookup key is heterogenous and comparisons will
        /// be done transparently at a per element level. If the key does not exist then an exception is thrown.
        /// Otherwise, the executable logic is returned.
        /// </summary>
        /// <typeparam name="LookupKey">The type of the passed in key. Should not usually need to be specified.</typeparam>
        /// <param name="p_key">The key to use for looking up the registered executable.</param>
        /// <returns>The executable logic that this key routes to.</returns>
        template <typename LookupKey>
        std::function<R()> Route(LookupKey&& p_key)
        {
            // The executable map considers partial keys as equal to the full key, so this is to protect against
            // a tuple that is a partial match of a key in the map from being considered found in the map.
            if constexpr (std::tuple_size_v<LookupKey> != std::tuple_size_v<Key>)
            {
                throw std::runtime_error("The executable could not be found for the given key.");
            }

            auto it = m_execs.find(std::forward<LookupKey>(p_key));
            if (it == m_execs.end())
            {
                throw std::runtime_error("The executable could not be found for the given key.");
            }

            return it->second;
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
                mg::iter_zipped_tuples(
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
        /// The instance that is responsible for resolving unbound parameters in the executables.
        /// </summary>
        ParameterResolver m_resolver;

        /// <summary>
        /// The storage for the executables. A map is used to allow for ordering of the executables
        /// based on their Key, which will allow for partial lookup later. It does come at a slight
        /// inefficiency if the full key is known compared to an unordered_map, however for this toy
        /// example it is fine and illustrates the partial lookup solution.
        /// </summary>
        std::map<
            Key,
            std::function<R()>,
            HeterogenousTupleLess> m_execs;
    };
}

