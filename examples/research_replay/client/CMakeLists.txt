# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# This consumer uses only the installed SDK; it does not build Framework internals.
cmake_minimum_required(VERSION 3.24)
project(UmicomResearchClient LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS research_replay)
add_executable(umicom-research-client ../main.c ../strategy.c)
target_link_libraries(umicom-research-client PRIVATE Umicom::research_replay)
if(BUILD_TESTING)
    add_test(NAME research.client.demo COMMAND umicom-research-client demo)
endif()
