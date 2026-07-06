#pragma once

#include <iostream>
#include <format>

namespace euler
{
    /// <summary>
    /// EmptyParameterResolver cannot actually resolve anything and is used when no
    /// resolving is necessary. If it needs to be called, then a static assertion will
    /// fail and the program will not compile.
    /// </summary>
    class EmptyParameterResolver
    {
    public:
        template <typename T>
        T operator()(Param<T> p_param)
        {
            static_assert(false, "The empty parameter resolver was called");
        }
    };

    /// <summary>
    /// CinParameterResolver uses iostream functionality to resolve parameters, and therefore
    /// should only be used for types which can be properly read in by 
    /// </summary>
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

}