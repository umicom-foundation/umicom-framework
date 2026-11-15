#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/sdk_about/CMakeLists.txt
#
# PURPOSE:
#   Build a native About example against an explicitly selected installed SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomSdkAbout VERSION 1.0.0 LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(UMICOM_SDK_ROOT "" CACHE PATH "Installed Framework SDK prefix")
if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
    message(FATAL_ERROR "UMICOM_SDK_ROOT must name an absolute existing installed SDK prefix")
endif()
find_package(UmicomFramework 0.9 CONFIG REQUIRED COMPONENTS ui_gtk4 product
    PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
umicom_require_installed_sdk("${UMICOM_SDK_ROOT}" Umicom::ui_gtk4 Umicom::product)
umicom_require_sdk_environment(2)
add_executable(umicom-sdk-about main.c)
target_link_libraries(umicom-sdk-about PRIVATE Umicom::ui_gtk4)
# The package owns SVG/ICO staging and the native Windows resource template.
umicom_apply_application_branding(TARGET umicom-sdk-about
    PRODUCT_NAME "Umicom SDK Example" INTERNAL_NAME "umicom-sdk-about"
    APPLICATION_ID "org.umicom.sdk.about" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
