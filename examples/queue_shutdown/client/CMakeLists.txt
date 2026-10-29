# Umicom Foundation | Sammy Hegab | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomShutdownClient LANGUAGES C)
include(CTest)
option(UMICOM_SHUTDOWN_USE_FRAMEWORK_SDK "Use the complete installed Framework package" OFF)
if(UMICOM_SHUTDOWN_USE_FRAMEWORK_SDK)
    if(NOT DEFINED UMICOM_EXPECTED_SDK_PREFIX OR
       NOT IS_ABSOLUTE "${UMICOM_EXPECTED_SDK_PREFIX}" OR
       NOT IS_DIRECTORY "${UMICOM_EXPECTED_SDK_PREFIX}")
        message(FATAL_ERROR "Supply the installed Framework prefix in UMICOM_EXPECTED_SDK_PREFIX")
    endif()
    find_package(UmicomFramework CONFIG REQUIRED COMPONENTS platform
        PATHS "${UMICOM_EXPECTED_SDK_PREFIX}" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
    file(REAL_PATH "${UMICOM_EXPECTED_SDK_PREFIX}" _sdk_prefix)
    file(REAL_PATH "${UmicomFramework_CONFIG}" _sdk_configuration)
    cmake_path(IS_PREFIX _sdk_prefix "${_sdk_configuration}" NORMALIZE _sdk_inside)
    if(NOT _sdk_inside)
        message(FATAL_ERROR "The cached Framework package is outside the requested prefix")
    endif()
    umicom_require_sdk_environment(2)
    umicom_require_installed_sdk("${UMICOM_EXPECTED_SDK_PREFIX}" Umicom::platform)
else()
    # Retain the original focused laboratory package and its default workflow.
    find_package(UmicomQueueShutdown CONFIG REQUIRED)
endif()
add_executable(umicom-shutdown-client main.c lifecycle_cases.c)
target_link_libraries(umicom-shutdown-client PRIVATE Umicom::platform)
set_target_properties(umicom-shutdown-client PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
add_test(NAME installed.shutdown.client COMMAND umicom-shutdown-client)
set_tests_properties(installed.shutdown.client PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77)

# The portable blocking-join cases complement the original nonblocking example.
# They use public installed APIs and do not substitute queue implementations.
foreach(_case IN ITEMS drain cancel-running-and-pending rejected-reusable
        caller-release callback-error worker-self)
    add_test(NAME installed.shutdown.${_case} COMMAND umicom-shutdown-client ${_case})
    set_tests_properties(installed.shutdown.${_case} PROPERTIES TIMEOUT 30)
endforeach()
if(MSVC)
    target_compile_options(umicom-shutdown-client PRIVATE /W4)
else()
    target_compile_options(umicom-shutdown-client PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
