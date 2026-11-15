#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/local_models/sdk_consumer/CMakeLists.txt
#
# PURPOSE:
#   Verify that an installed Framework SDK exports the native model service and its transitive dependencies.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.24)
project(UmicomModelConsumer LANGUAGES C)
# Preserve the former unrestricted discovery for review. The checked selection
# below retains discovery while refusing mixed or incompatible SDK inputs.
#[=[
find_package(UmicomFramework CONFIG REQUIRED)
]=]
# A selected installation is a contract, not merely a package-search hint.
# Keep discovery available for existing CMAKE_PREFIX_PATH users; always validate
# the discovered package, and compare an explicit root even when a cache exists.
set(UMICOM_SDK_ROOT "" CACHE PATH "Installed Framework SDK prefix")
if(NOT UMICOM_SDK_ROOT STREQUAL "")
    if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
        message(FATAL_ERROR "UMICOM_SDK_ROOT must be an absolute existing SDK prefix")
    endif()
    find_package(UmicomFramework CONFIG REQUIRED COMPONENTS local_models
        PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
    set(_model_sdk_root "${UMICOM_SDK_ROOT}")
else()
    find_package(UmicomFramework CONFIG REQUIRED COMPONENTS local_models)
    if(NOT DEFINED UMICOM_FRAMEWORK_INSTALL_PREFIX OR
       UMICOM_FRAMEWORK_INSTALL_PREFIX STREQUAL "")
        message(FATAL_ERROR "Discovered SDK does not report its installed prefix")
    endif()
    set(_model_sdk_root "${UMICOM_FRAMEWORK_INSTALL_PREFIX}")
    message(STATUS "Model consumer uses auto-discovered Framework SDK: ${_model_sdk_root}")
endif()
if(NOT COMMAND umicom_require_installed_sdk OR NOT COMMAND umicom_require_sdk_environment)
    message(FATAL_ERROR "Model consumer requires installed SDK contract helpers; reinstall a current SDK")
endif()
umicom_require_installed_sdk("${_model_sdk_root}" Umicom::local_models)
# This consumer's required C ABI is independent of the producer's declaration.
umicom_require_sdk_environment(2)
message(STATUS "Model consumer selected installed SDK: ${_model_sdk_root}")
add_executable(umicom-model-consumer main.c)
# Keep ownership exercises in the same installed consumer and transitive export
# boundary. They must not borrow private Framework headers or source objects.
target_sources(umicom-model-consumer PRIVATE provider_ownership.c provider_ownership.h)
target_link_libraries(umicom-model-consumer PRIVATE Umicom::local_models)
set_target_properties(umicom-model-consumer PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
enable_testing()
add_test(NAME local_models.sdk_catalogue COMMAND umicom-model-consumer)

# The installed SDK supplies the approved SVG/ICO catalogue and native icon metadata.
umicom_apply_application_branding(TARGET umicom-model-consumer
    PRODUCT_NAME "Umicom Model SDK Example" INTERNAL_NAME "umicom-model-consumer"
    APPLICATION_ID "org.umicom.sdk.models" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
