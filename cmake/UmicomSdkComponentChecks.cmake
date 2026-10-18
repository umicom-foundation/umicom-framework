# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Tiny real-library fixtures test install/export behavior on the build host.
include_guard(GLOBAL)
if(NOT BUILD_TESTING)
    return()
endif()
if(CMAKE_CROSSCOMPILING)
    message(STATUS "SDK component execution requires native host qualification")
    return()
endif()
set(_sdk_component_cases selective spaced-prefix relocated-prefix shared release
    duplicate-registration deferred-registration full-install missing-library missing-installed-library
    missing-target imported-target alias-target executable-target object-target empty-registration name-collision)
set(_sdk_error_missing-target "Framework SDK target does not exist")
set(_sdk_error_imported-target "Framework SDK requires an owned library")
set(_sdk_error_alias-target "Framework SDK requires an owned library")
set(_sdk_error_executable-target "Framework SDK target is not an installable library")
set(_sdk_error_object-target "Framework SDK target is not an installable library")
set(_sdk_error_empty-registration "Framework SDK registration requires library targets")
set(_sdk_error_name-collision "Framework SDK build target name is already in use")
foreach(_case IN LISTS _sdk_component_cases)
    add_test(NAME "framework.sdk.component.${_case}"
        COMMAND "${CMAKE_COMMAND}"
            "-DTEST_CASE=${_case}" "-DCONFIG=$<CONFIG>"
            "-DGENERATOR=${CMAKE_GENERATOR}"
            "-DGENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}"
            "-DGENERATOR_TOOLSET=${CMAKE_GENERATOR_TOOLSET}"
            "-DMAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}"
            "-DCC=${CMAKE_C_COMPILER}" "-DCTEST_BIN=${CMAKE_CTEST_COMMAND}"
            "-DSOURCE_ROOT=${CMAKE_CURRENT_LIST_DIR}/../tests/sdk_component"
            "-DOUTPUT_ROOT=${CMAKE_CURRENT_BINARY_DIR}/sdk-component/${_case}"
            "-DSDK_HELPER=${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake"
            "-DEXPECTED_ERROR=${_sdk_error_${_case}}"
            -P "${CMAKE_CURRENT_LIST_DIR}/../tests/sdk_component/check.cmake")
    set_tests_properties("framework.sdk.component.${_case}" PROPERTIES
        TIMEOUT 600 PROCESSORS 2 LABELS "framework;sdk;installation;regression")
endforeach()
