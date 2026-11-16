# Umicom Framework
# File: examples/comfy_node_pages/CMakeLists.txt
#
# PURPOSE:
#   Build an independent branded C node browser from an explicit installed Framework SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

cmake_minimum_required(VERSION 3.24)
project(UmicomNodePagesSdkExample LANGUAGES C)
set(UMICOM_SDK_ROOT "" CACHE PATH "Selected installed Framework SDK prefix")
if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
    message(FATAL_ERROR "Set UMICOM_SDK_ROOT to an absolute existing installed SDK prefix")
endif()
find_package(UmicomFramework 0.9 CONFIG REQUIRED COMPONENTS comfy_protocol platform
    PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
foreach(_helper umicom_require_installed_sdk umicom_require_sdk_environment
                umicom_apply_application_branding umicom_apply_warnings umicom_apply_sanitizers)
    if(NOT COMMAND ${_helper})
        message(FATAL_ERROR "Selected SDK lacks required helper: ${_helper}")
    endif()
endforeach()
umicom_require_installed_sdk("${UMICOM_SDK_ROOT}" Umicom::comfy_protocol Umicom::platform)
umicom_require_sdk_environment(2)
add_executable(umicom-sdk-comfy-node-pages main.c)
target_link_libraries(umicom-sdk-comfy-node-pages PRIVATE Umicom::comfy_protocol Umicom::platform)
set_target_properties(umicom-sdk-comfy-node-pages PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES
    C_EXTENSIONS NO RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
umicom_apply_warnings(umicom-sdk-comfy-node-pages)
umicom_apply_sanitizers(umicom-sdk-comfy-node-pages)
umicom_apply_application_branding(TARGET umicom-sdk-comfy-node-pages
    PRODUCT_NAME "Umicom Node Browser SDK Example" INTERNAL_NAME "umicom-sdk-comfy-node-pages"
    APPLICATION_ID "org.umicom.sdk.comfy-node-pages" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
include(CTest)
if(BUILD_TESTING)
    if(NOT COMMAND umicom_add_comfy_node_pages_command_tests)
        message(FATAL_ERROR "Use an SDK containing the node browsing qualification helper")
    endif()
    umicom_add_comfy_node_pages_command_tests(umicom-sdk-comfy-node-pages sdk.comfy-node-pages)
endif()
