#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/build_configuration/brand_resources/CMakeLists.txt
#
# PURPOSE:
#   Compose multiple resource consumers against one canonical source tree.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomBrandResourceGraph LANGUAGES C)
if(NOT IS_ABSOLUTE "${RESOURCE_FRAMEWORK}" OR
        NOT ORDER MATCHES "^(gtk-first|media-first)$")
    message(FATAL_ERROR "Expected an absolute resource root and a known include order")
endif()

# These targets exercise the real helper contracts without compiling GTK.
# A separate custom target lets Ninja regenerate the resource headers alone.
add_library(brand_first STATIC EXCLUDE_FROM_ALL "${CMAKE_CURRENT_LIST_DIR}/probe.c")
add_library(brand_second STATIC EXCLUDE_FROM_ALL "${CMAKE_CURRENT_LIST_DIR}/probe.c")
add_library(umicom_creative_workspace_gtk4 STATIC EXCLUDE_FROM_ALL
    "${CMAKE_CURRENT_LIST_DIR}/probe.c")

# Other Framework consumers already register canonical paths. Both embedding
# helpers must compose with these entries rather than introduce ../ aliases.
set(_resources
    "${RESOURCE_FRAMEWORK}/resources/brand/umicom-icon.svg"
    "${RESOURCE_FRAMEWORK}/resources/brand/umicom-icon-on-dark.svg"
    "${RESOURCE_FRAMEWORK}/resources/brand/umicom-logo.svg"
    "${RESOURCE_FRAMEWORK}/resources/brand/umicom-logo-on-dark.svg"
    "${RESOURCE_FRAMEWORK}/resources/styles/media-workspace.css")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${_resources})
include("${RESOURCE_FRAMEWORK}/cmake/UmicomGtkBrandResources.cmake")
if(ORDER STREQUAL "media-first")
    include("${RESOURCE_FRAMEWORK}/cmake/UmicomCreativeMediaResources.cmake")
endif()
umicom_embed_gtk_brand(brand_first)
umicom_embed_gtk_brand(brand_second)
if(ORDER STREQUAL "gtk-first")
    include("${RESOURCE_FRAMEWORK}/cmake/UmicomCreativeMediaResources.cmake")
endif()

get_property(_dependencies DIRECTORY PROPERTY CMAKE_CONFIGURE_DEPENDS)
foreach(_dependency IN LISTS _dependencies)
    get_filename_component(_normalized "${_dependency}" ABSOLUTE)
    if(NOT "${_dependency}" STREQUAL "${_normalized}")
        message(FATAL_ERROR "Non-normalized resource dependency: ${_dependency}")
    endif()
endforeach()
foreach(_resource IN LISTS _resources)
    if(NOT _resource IN_LIST _dependencies)
        message(FATAL_ERROR "Resource changes would not trigger regeneration: ${_resource}")
    endif()
endforeach()
add_custom_target(brand-resource-graph DEPENDS
    "${CMAKE_CURRENT_BINARY_DIR}/generated/gtk-brand/umicom_gtk_brand_resources.h"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/creative-media/umicom_media_workspace_resources.h")
