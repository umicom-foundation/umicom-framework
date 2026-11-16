# Umicom Framework
# File: examples/model_formats/CMakeLists.txt
#
# PURPOSE:
# Build an independent branded model metadata example against a checked installed Framework SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

cmake_minimum_required(VERSION 3.24)
project(UmicomModelFormatsExample LANGUAGES C)
set(UMICOM_SDK_ROOT "" CACHE PATH "Installed Framework SDK prefix")
if(NOT UMICOM_SDK_ROOT STREQUAL "")
    if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
        message(FATAL_ERROR "UMICOM_SDK_ROOT must be an absolute existing SDK prefix")
    endif()
    find_package(UmicomFramework CONFIG REQUIRED COMPONENTS model_formats
        PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
    set(_model_sdk_root "${UMICOM_SDK_ROOT}")
else()
    find_package(UmicomFramework CONFIG REQUIRED COMPONENTS model_formats)
    if(NOT DEFINED UMICOM_FRAMEWORK_INSTALL_PREFIX OR UMICOM_FRAMEWORK_INSTALL_PREFIX STREQUAL "")
        message(FATAL_ERROR "Discovered SDK does not report its installed prefix")
    endif()
    set(_model_sdk_root "${UMICOM_FRAMEWORK_INSTALL_PREFIX}")
endif()
if(NOT COMMAND umicom_require_installed_sdk OR NOT COMMAND umicom_require_sdk_environment OR
   NOT COMMAND umicom_apply_application_branding)
    message(FATAL_ERROR "This example requires installed SDK provenance, ABI and branding helpers")
endif()
# Validate the dependency closure instead of adding private source include paths.
umicom_require_installed_sdk("${_model_sdk_root}" Umicom::model_formats)
umicom_require_sdk_environment(2)
add_executable(umicom-model-header main.c)
target_link_libraries(umicom-model-header PRIVATE Umicom::model_formats)
set_target_properties(umicom-model-header PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
# The installed resource service owns the approved SVG/ICO catalogue.
umicom_apply_application_branding(TARGET umicom-model-header
    PRODUCT_NAME "Umicom Model Header" INTERNAL_NAME "umicom-model-header"
    APPLICATION_ID "org.umicom.sdk.model-header"
    VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)

# A separate explicit-file example keeps the synthetic header example unchanged.
add_executable(umicom-model-inspect inspect_file.c)
target_link_libraries(umicom-model-inspect PRIVATE Umicom::model_formats)
set_target_properties(umicom-model-inspect PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_application_branding(TARGET umicom-model-inspect
    PRODUCT_NAME "Umicom Model Inspector" INTERNAL_NAME "umicom-model-inspect"
    APPLICATION_ID "org.umicom.sdk.model-inspector"
    VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
