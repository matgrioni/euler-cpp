# CMakeList.txt : CMake project for euler-cpp, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.15)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

add_compile_options(/W4)

# These warning options also surface issues from the system headers which are undesirable in the normal build.
# Only enable when doing in-depth linting.
# add_compile_options(/w34365)

project ("euler-cpp")

add_subdirectory(deps/cxxopts)

# Add source to this project's executable.
add_executable (euler-cpp "main.cpp" "31.cpp" "31.hpp" "32.cpp" "32.hpp" "PermuteView.hpp" "Solver.hpp" "1.hpp"   "1.cpp" "2.cpp")

target_include_directories(euler-cpp PRIVATE deps/cxxopts/include)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET euler-cpp PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
