# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomSdkComponentConsumer LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
enable_testing()
# Deliberately consume only the installed package, with no add_subdirectory or
# build-tree fallback. Missing archives must fail before a misleading success.
find_package(SdkFixture CONFIG REQUIRED PATHS "${SDK_ROOT}" NO_DEFAULT_PATH)
add_executable(sdk_consumer main.c)
target_link_libraries(sdk_consumer PRIVATE Sdk::sdk_interface)
if(TARGET Sdk::sdk_deferred)
    target_compile_definitions(sdk_consumer PRIVATE SDK_HAS_DEFERRED=1)
endif()
add_test(NAME sdk.component.consumer COMMAND sdk_consumer)
set_tests_properties(sdk.component.consumer PROPERTIES
    ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:${SDK_ROOT}/bin")
