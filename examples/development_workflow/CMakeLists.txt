# Umicom Framework
# File: examples/development_workflow/CMakeLists.txt
#
# PURPOSE:
# Build the complete Notes workflow and its inspection-only preview against one explicitly selected installed Framework SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

cmake_minimum_required(VERSION 3.25)
project(UmicomDevelopmentWorkflowExample LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
# Use the explicitly selected installed package. The earlier unrestricted lookup
# is retained for review because a cached or registered SDK could select a
# different package than the developer intended.
#[=[
find_package(UmicomFramework CONFIG REQUIRED)
]=]
if(NOT DEFINED UMICOM_EXPECTED_SDK_PREFIX OR
   NOT IS_ABSOLUTE "${UMICOM_EXPECTED_SDK_PREFIX}" OR
   NOT IS_DIRECTORY "${UMICOM_EXPECTED_SDK_PREFIX}")
    message(FATAL_ERROR "Supply UMICOM_EXPECTED_SDK_PREFIX as the installed SDK's absolute directory")
endif()
find_package(UmicomFramework CONFIG REQUIRED
    PATHS "${UMICOM_EXPECTED_SDK_PREFIX}" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
if(NOT DEFINED UmicomFramework_CONFIG OR NOT EXISTS "${UmicomFramework_CONFIG}")
    message(FATAL_ERROR "The selected Framework configuration is not an existing file")
endif()
file(REAL_PATH "${UMICOM_EXPECTED_SDK_PREFIX}" _sdk_prefix)
file(REAL_PATH "${UmicomFramework_CONFIG}" _sdk_configuration)
cmake_path(IS_PREFIX _sdk_prefix "${_sdk_configuration}" NORMALIZE _sdk_inside)
if(NOT _sdk_inside)
    message(FATAL_ERROR "The selected Framework package is outside the requested prefix")
endif()
if(NOT COMMAND umicom_require_sdk_environment OR
   NOT COMMAND umicom_require_installed_sdk_layout OR
   NOT COMMAND umicom_apply_application_branding)
    message(FATAL_ERROR "Reinstall Framework with its SDK metadata, layout and branding helpers")
endif()
umicom_require_sdk_environment(2)
umicom_require_installed_sdk_layout("${UMICOM_EXPECTED_SDK_PREFIX}" _headers Umicom::Framework)
add_executable(umicom-development-workflow-example main.c)
target_link_libraries(umicom-development-workflow-example PRIVATE Umicom::Framework)
enable_testing()
add_test(NAME notes.development_workflow COMMAND umicom-development-workflow-example complete)
set_tests_properties(notes.development_workflow PROPERTIES TIMEOUT 180)

# Apply the installed resource catalogue's SVG artwork and Windows ICO.
if(WIN32)
    enable_language(RC)
endif()
umicom_apply_application_branding(TARGET umicom-development-workflow-example
    PRODUCT_NAME "Umicom Development Workflow Example"
    INTERNAL_NAME "umicom-development-workflow-example")
if(MSVC)
    target_compile_options(umicom-development-workflow-example PRIVATE /W4)
else()
    target_compile_options(umicom-development-workflow-example PRIVATE -Wall -Wextra -Wpedantic)
endif()
foreach(_scenario IN ITEMS release debug relwithdebinfo default-generator empty-configuration escaped usage configuration-overflow)
    add_test(NAME notes.development_workflow.preview.${_scenario}
        COMMAND "${CMAKE_COMMAND}"
            "-DTEST_PROGRAM=$<TARGET_FILE:umicom-development-workflow-example>"
            "-DSCENARIO=${_scenario}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/check_preview.cmake")
    set_tests_properties(notes.development_workflow.preview.${_scenario}
        PROPERTIES TIMEOUT 20 LABELS "sdk;developer;preview")
endforeach()
