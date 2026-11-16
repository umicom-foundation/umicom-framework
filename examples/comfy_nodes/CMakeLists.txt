# Umicom Framework
# File: examples/comfy_nodes/CMakeLists.txt
#
# PURPOSE:
# Build an independent offline ComfyUI node catalogue example from a selected installed Framework SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT


cmake_minimum_required(VERSION 3.24)
project(UmicomComfyNodesSdkExample LANGUAGES C)
set(UMICOM_SDK_ROOT "" CACHE PATH "Selected installed Framework SDK prefix")
if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
    message(FATAL_ERROR "Set UMICOM_SDK_ROOT to an absolute existing installed Framework SDK prefix")
endif()
find_package(UmicomFramework 0.9 CONFIG REQUIRED COMPONENTS comfy_protocol
    PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
if(NOT COMMAND umicom_require_installed_sdk OR NOT COMMAND umicom_require_sdk_environment OR
   NOT COMMAND umicom_apply_application_branding)
    message(FATAL_ERROR "ComfyNodes example requires current installed SDK identity and branding helpers")
endif()
# Validate the selected package origin and ABI through its shared services.
# The consumer includes public headers and never compiles Framework sources.
umicom_require_installed_sdk("${UMICOM_SDK_ROOT}" Umicom::comfy_protocol)
umicom_require_sdk_environment(2)
add_executable(umicom-sdk-comfy-nodes main.c)
target_link_libraries(umicom-sdk-comfy-nodes PRIVATE Umicom::comfy_protocol)
set_target_properties(umicom-sdk-comfy-nodes PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
umicom_apply_application_branding(TARGET umicom-sdk-comfy-nodes
    PRODUCT_NAME "Umicom ComfyNodes SDK Example" INTERNAL_NAME "umicom-sdk-comfy-nodes"
    APPLICATION_ID "org.umicom.sdk.comfy-nodes" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
include(CTest)
if(BUILD_TESTING)
    add_test(NAME sdk.comfy-nodes.node-observation COMMAND "$<TARGET_FILE:umicom-sdk-comfy-nodes>")
    set_tests_properties(sdk.comfy-nodes.node-observation PROPERTIES
        TIMEOUT 20 LABELS "sdk;ai;comfy-nodes;example")
endif()


# The typed example uses the same installed component and branding contract.
add_executable(umicom-sdk-comfy-numeric-input numeric.c)
target_link_libraries(umicom-sdk-comfy-numeric-input PRIVATE Umicom::comfy_protocol)
set_target_properties(umicom-sdk-comfy-numeric-input PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
umicom_apply_application_branding(TARGET umicom-sdk-comfy-numeric-input
    PRODUCT_NAME "Umicom Numeric Node SDK Example" INTERNAL_NAME "umicom-sdk-comfy-numeric-input"
    APPLICATION_ID "org.umicom.sdk.comfy-numeric-input" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
if(BUILD_TESTING)
    add_test(NAME sdk.comfy-nodes.numeric-input COMMAND "$<TARGET_FILE:umicom-sdk-comfy-numeric-input>")
    set_tests_properties(sdk.comfy-nodes.numeric-input PROPERTIES
        TIMEOUT 20 LABELS "sdk;comfy;numeric;example")
endif()


# Choice observations use the same installed catalogue and brand resources.
add_executable(umicom-sdk-comfy-choice-input choices.c)
target_link_libraries(umicom-sdk-comfy-choice-input PRIVATE Umicom::comfy_protocol)
set_target_properties(umicom-sdk-comfy-choice-input PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
umicom_apply_application_branding(TARGET umicom-sdk-comfy-choice-input
    PRODUCT_NAME "Umicom Choice Node SDK Example" INTERNAL_NAME "umicom-sdk-comfy-choice-input"
    APPLICATION_ID "org.umicom.sdk.comfy-choice-input" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
if(BUILD_TESTING)
    add_test(NAME sdk.comfy-nodes.choice-input COMMAND "$<TARGET_FILE:umicom-sdk-comfy-choice-input>")
    set_tests_properties(sdk.comfy-nodes.choice-input PROPERTIES
        TIMEOUT 20 LABELS "sdk;comfy;choices;example")
endif()


# Literal observations link only the selected installed shared component.
add_executable(umicom-sdk-comfy-literal-input literals.c)
target_link_libraries(umicom-sdk-comfy-literal-input PRIVATE Umicom::comfy_protocol)
set_target_properties(umicom-sdk-comfy-literal-input PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
umicom_apply_application_branding(TARGET umicom-sdk-comfy-literal-input
    PRODUCT_NAME "Umicom Literal Node SDK Example" INTERNAL_NAME "umicom-sdk-comfy-literal-input"
    APPLICATION_ID "org.umicom.sdk.comfy-literal-input" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
if(BUILD_TESTING)
    add_test(NAME sdk.comfy-nodes.literal-input COMMAND "$<TARGET_FILE:umicom-sdk-comfy-literal-input>")
    set_tests_properties(sdk.comfy-nodes.literal-input PROPERTIES
        TIMEOUT 20 LABELS "sdk;comfy;literal;example")
endif()
