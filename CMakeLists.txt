# CMakeList.txt : CMake project for euler-cpp, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.29)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

add_compile_options(/W4)

include(FetchContent)
FetchContent_Declare(
    magnesium
    GIT_REPOSITORY https://github.com/magnesium-cpp/magnesium
    GIT_TAG 57a61f316bd933a02b8c403cd2da3754e7d11161
)
FetchContent_Declare(
    cxxopts
    GIT_REPOSITORY https://github.com/jarro2783/cxxopts
    GIT_TAG c8c932f891a559a099198ec4a1fccf6110d6463a
)
FetchContent_MakeAvailable(magnesium cxxopts)

# These warning options also surface issues from the system headers which are undesirable in the normal build.
# Only enable when doing in-depth linting.
# add_compile_options(/w34365)

project("euler-cpp" DESCRIPTION "A suite of solutions to Project Euler problems."
                    LANGUAGES CXX)

# Add source to this project's executable.
add_executable(
    euler-cpp
    "main.cpp"
    "problems/src/1.cpp"
    "problems/src/2.cpp"
    "problems/src/3.cpp"
    "problems/src/4.cpp"
    "problems/src/31.cpp"
    "problems/src/32.cpp")

target_include_directories(euler-cpp PRIVATE
    problems/inc
    lib/inc)
target_link_libraries(euler-cpp PRIVATE
    magnesium
    cxxopts)

target_compile_features(euler-cpp PRIVATE cxx_std_20)
set_target_properties(euler-cpp PROPERTIES CXX_EXTENSIONS OFF)
