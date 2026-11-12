#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/build_configuration/native_branding/CMakeLists.txt
#
# PURPOSE:
#   Verify native-tool branding does not require GTK runtime deployment.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.25)
project(UmicomNativeBrandingFixture LANGUAGES C)
if(NOT DEFINED FRAMEWORK_ROOT)
    message(FATAL_ERROR "FRAMEWORK_ROOT must identify the source checkout")
endif()
if(WIN32)
    enable_language(RC)
endif()
# Leave the global deployment default enabled: this target must opt out locally.
set(UMICOM_WINDOWS_DEPLOY_RUNTIME ON CACHE BOOL "Fixture default")
include("${FRAMEWORK_ROOT}/cmake/UmicomApplicationBranding.cmake")
add_executable(native-branding main.c)
set_target_properties(native-branding PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
umicom_apply_application_branding(TARGET native-branding
    PRODUCT_NAME "Umicom Native Branding Fixture" INTERNAL_NAME native-branding
    RESOURCE_ROOT "${FRAMEWORK_ROOT}/resources" NO_RUNTIME_DEPLOYMENT)
get_target_property(_registered native-branding UMICOM_WINDOWS_DEPLOYMENT_REGISTERED)
if(_registered)
    message(FATAL_ERROR "Native resource-only branding registered runtime deployment")
endif()
get_target_property(_sources native-branding SOURCES)
foreach(_source IN LISTS _sources)
    if(_source MATCHES "windows_runtime_(bootstrap|cache)[.]c$")
        message(FATAL_ERROR "Native branding injected GTK runtime bootstrap source")
    endif()
endforeach()
if(WIN32)
    file(READ "${CMAKE_BINARY_DIR}/umicom-branding/native-branding/native-branding.rc" _resources)
    foreach(_name IN ITEMS umicom.ico umicom-icon-on-dark.svg)
        string(FIND "${_resources}" "${FRAMEWORK_ROOT}/resources/brand/${_name}" _found)
        if(_found EQUAL -1)
            message(FATAL_ERROR "Generated resource does not reference canonical ${_name}")
        endif()
    endforeach()
endif()
