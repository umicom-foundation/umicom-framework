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
include("${FRAMEWORK_ROOT}/cmake/UmicomEmbeddedBrandResources.cmake")

# Native header and offline exports can spell the same asset differently.
# A real Ninja build below must accept their single shared configure input.
umicom_brand_resource_initializer("${FRAMEWORK_ROOT}/resources/brand/umicom-icon.svg" _canonical)
umicom_brand_resource_initializer("${FRAMEWORK_ROOT}/cmake/../resources/brand/umicom-icon.svg" _alias)
if(NOT _canonical STREQUAL _alias)
    message(FATAL_ERROR "Aliased asset paths produced different embedded bytes")
endif()

add_executable(native-branding main.c)
set_target_properties(native-branding PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
umicom_apply_application_branding(TARGET native-branding
    PRODUCT_NAME "Umicom Native Branding Fixture" INTERNAL_NAME native-branding
    RESOURCE_ROOT "${FRAMEWORK_ROOT}/resources" NO_RUNTIME_DEPLOYMENT)

# Applications may declare the executable below the parent that applies
# branding. Preserve its existing runtime policy and test source properties in
# the actual compiler directory rather than in the caller's directory.
add_subdirectory(child)
umicom_apply_application_branding(TARGET native-child-branding
    PRODUCT_NAME "Umicom Child Branding Fixture" INTERNAL_NAME native-child-branding
    RESOURCE_ROOT "${FRAMEWORK_ROOT}/resources" BRANDING_ONLY)
get_target_property(_bootstrap_override native-child-branding UMICOM_WINDOWS_RUNTIME_BOOTSTRAP_DISABLED)
get_target_property(_child_deployment native-child-branding UMICOM_WINDOWS_DEPLOYMENT_REGISTERED)
if(NOT _bootstrap_override STREQUAL "FALSE" OR _child_deployment)
    message(FATAL_ERROR "Branding-only mode changed the child's runtime deployment policy")
endif()

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
    set(_child_resource "${CMAKE_BINARY_DIR}/umicom-branding/native-child-branding/native-child-branding.rc")
    get_source_file_property(_child_dependencies "${_child_resource}"
        TARGET_DIRECTORY native-child-branding OBJECT_DEPENDS)
    file(READ "${CMAKE_BINARY_DIR}/umicom-branding/native-branding/native-branding.rc" _resources)
    foreach(_name IN ITEMS umicom.ico umicom-icon-on-dark.svg)
        string(FIND "${_resources}" "${FRAMEWORK_ROOT}/resources/brand/${_name}" _found)
        if(_found EQUAL -1)
            message(FATAL_ERROR "Generated resource does not reference canonical ${_name}")
        endif()
        if(NOT "${FRAMEWORK_ROOT}/resources/brand/${_name}" IN_LIST _child_dependencies)
            message(FATAL_ERROR "The child resource lacks a compile dependency on ${_name}")
        endif()
    endforeach()
endif()
