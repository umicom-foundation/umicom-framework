# Umicom Framework
# File: examples/ai_workflow_memory/CMakeLists.txt
#
# PURPOSE:
# Build the independent native memory planning consumer from the installed SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT


cmake_minimum_required(VERSION 3.24)
project(UmicomWorkflowMemoryExample LANGUAGES C)
# The installed package owns provenance, ABI and resource checks. Keep the
# original minimal discovery for review; it did not bind a selected SDK identity.
#[=[
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS ai_workflow_preflight)
]=]
set(UMICOM_SDK_ROOT "" CACHE PATH "Installed Framework SDK prefix")
if(NOT UMICOM_SDK_ROOT STREQUAL "")
    if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
        message(FATAL_ERROR "UMICOM_SDK_ROOT must be an absolute existing SDK prefix")
    endif()
    find_package(UmicomFramework CONFIG REQUIRED COMPONENTS ai_workflow_preflight
        PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
    set(_workflow_sdk_root "${UMICOM_SDK_ROOT}")
else()
    find_package(UmicomFramework CONFIG REQUIRED COMPONENTS ai_workflow_preflight)
    if(NOT DEFINED UMICOM_FRAMEWORK_INSTALL_PREFIX OR UMICOM_FRAMEWORK_INSTALL_PREFIX STREQUAL "")
        message(FATAL_ERROR "Discovered SDK does not report its installed prefix")
    endif()
    set(_workflow_sdk_root "${UMICOM_FRAMEWORK_INSTALL_PREFIX}")
endif()
if(NOT COMMAND umicom_require_installed_sdk OR NOT COMMAND umicom_require_sdk_environment OR
   NOT COMMAND umicom_apply_application_branding)
    message(FATAL_ERROR "Workflow example requires the installed SDK contract and branding helpers")
endif()
umicom_require_installed_sdk("${_workflow_sdk_root}" Umicom::ai_workflow_preflight)
# The consumer requires this ABI independently of the producer's claim.
umicom_require_sdk_environment(2)
message(STATUS "Workflow example selected installed SDK: ${_workflow_sdk_root}")

add_executable(umicom-workflow-memory main.c)
target_link_libraries(umicom-workflow-memory PRIVATE Umicom::ai_workflow_preflight)
set_target_properties(umicom-workflow-memory PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)

# Use the shared installed SVG/ICO catalogue and Windows metadata template.
umicom_apply_application_branding(TARGET umicom-workflow-memory
    PRODUCT_NAME "Umicom Workflow Memory" INTERNAL_NAME "umicom-workflow-memory"
    APPLICATION_ID "org.umicom.sdk.workflow.memory"
    VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)

include(CTest)
if(BUILD_TESTING)
    add_test(NAME sdk.workflow_memory.complete COMMAND umicom-workflow-memory)
    add_test(NAME sdk.workflow_memory.incomplete COMMAND umicom-workflow-memory --unknown-scratch)
    set_tests_properties(sdk.workflow_memory.complete sdk.workflow_memory.incomplete
        PROPERTIES TIMEOUT 20 LABELS "sdk;workflow;memory")
endif()
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom-workflow-memory)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-workflow-memory)
endif()
