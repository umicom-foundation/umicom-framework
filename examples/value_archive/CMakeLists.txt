# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# An installed consumer uses exported targets and public headers only.
cmake_minimum_required(VERSION 3.24)
project(UmicomPortableState LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
find_package(UmicomFramework CONFIG REQUIRED)
add_executable(umicom-portable-state main.c)
target_link_libraries(umicom-portable-state PRIVATE Umicom::platform Umicom::data)
include(CTest)
if(BUILD_TESTING)
    add_test(NAME portable-state-memory COMMAND umicom-portable-state)
endif()
