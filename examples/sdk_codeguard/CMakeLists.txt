#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/sdk_codeguard/CMakeLists.txt
#
# PURPOSE:
#   Build a native CodeGuard example against an explicitly selected installed SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomSdkCodeGuard VERSION 1.0.0 LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(UMICOM_SDK_ROOT "" CACHE PATH "Installed Framework SDK prefix")
if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
    message(FATAL_ERROR "UMICOM_SDK_ROOT must name an absolute existing installed SDK prefix")
endif()
find_package(UmicomFramework 0.9 CONFIG REQUIRED COMPONENTS codeguard
    PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
umicom_require_installed_sdk("${UMICOM_SDK_ROOT}" Umicom::codeguard)
umicom_require_sdk_environment(2)
add_executable(umicom-sdk-codeguard main.c)
target_link_libraries(umicom-sdk-codeguard PRIVATE Umicom::codeguard)
# The package owns SVG/ICO staging and the native Windows resource template.
umicom_apply_application_branding(TARGET umicom-sdk-codeguard
    PRODUCT_NAME "Umicom CodeGuard SDK Example" INTERNAL_NAME "umicom-sdk-codeguard"
    APPLICATION_ID "org.umicom.sdk.codeguard" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)

# Background execution uses the same installed package and branding service.
add_executable(umicom-sdk-codeguard-background background.c)
target_link_libraries(umicom-sdk-codeguard-background PRIVATE Umicom::codeguard)
umicom_apply_application_branding(TARGET umicom-sdk-codeguard-background
    PRODUCT_NAME "Umicom Background CodeGuard SDK Example"
    INTERNAL_NAME "umicom-sdk-codeguard-background"
    APPLICATION_ID "org.umicom.sdk.codeguard.background"
    VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)

# Current-context review uses the package-owned job and retained-report services.
add_executable(umicom-sdk-codeguard-review review.c)
target_link_libraries(umicom-sdk-codeguard-review PRIVATE Umicom::codeguard)
umicom_apply_application_branding(TARGET umicom-sdk-codeguard-review
    PRODUCT_NAME "Umicom CodeGuard Review SDK Example"
    INTERNAL_NAME "umicom-sdk-codeguard-review"
    APPLICATION_ID "org.umicom.sdk.codeguard.review"
    VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)

# Owned report projection demonstrates the same package boundary used by a UI.
add_executable(umicom-sdk-codeguard-report report.c)
target_link_libraries(umicom-sdk-codeguard-report PRIVATE Umicom::codeguard)
umicom_apply_application_branding(TARGET umicom-sdk-codeguard-report
    PRODUCT_NAME "Umicom CodeGuard Report SDK Example"
    INTERNAL_NAME "umicom-sdk-codeguard-report"
    APPLICATION_ID "org.umicom.sdk.codeguard.report"
    VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)

# Exclusive export shares installed native output and the approved SVG/ICO resources.
add_executable(umicom-sdk-codeguard-export export.c)
target_link_libraries(umicom-sdk-codeguard-export PRIVATE Umicom::codeguard)
umicom_apply_application_branding(TARGET umicom-sdk-codeguard-export
    PRODUCT_NAME "Umicom CodeGuard Export SDK Example"
    INTERNAL_NAME "umicom-sdk-codeguard-export"
    APPLICATION_ID "org.umicom.sdk.codeguard.export"
    VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
