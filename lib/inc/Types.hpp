#pragma once

#include <type_traits>

namespace mg
{
    namespace detail
    {
        /// <summary>
        /// False case on implementation when checking if a class is an instantiation of a template.
        /// </summary>
        /// <typeparam>The type that is being checked for.</typeparam>
        /// <template>The template that is being checked against.</template>
        template <typename, template <typename...> typename>
        struct is_instance_impl : public std::false_type {};

        /// <summary>
        /// True case on implementation when checking if a class is an instantiation of a template.
        /// </summary>
        /// <template name="U">The template that is being checked against.</template>
        /// <typeparam name="...Ts">The types that instantiate the template that is being checked against.</typeparam>
        template <template <typename...> typename U, typename... Ts>
        struct is_instance_impl<U<Ts...>, U> : public std::true_type {};
    }

    /// <summary>
    /// Check if a type is an instantiation of a template. The check is independent of reference, const, and volatile
    /// modifiers.
    /// </summary>
    /// <typeparam name="T">The type to check as an instantiation of a template.</typeparam>
    /// <template name="U">The template to check against.</template>
    template <typename T, template <typename...> typename U>
    using is_instance = detail::is_instance_impl<std::remove_cvref_t<T>, U>;

    /// <summary>
    /// The value alias for the template instantiation check.
    /// </summary>
    /// <typeparam name="T">The type to check as an instantiation of a template.</typeparam>
    /// <template name="U">The template to check against.</template>
    template <typename T, template <typename ...> typename U>
    constexpr bool is_instance_v = is_instance<T, U>::value;
}