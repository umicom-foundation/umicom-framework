#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCreativeMediaResources.cmake
#
# PURPOSE:
#   Embed canonical SVG artwork and scoped Media styling without runtime path assumptions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
set(_umicom_media_root "${CMAKE_CURRENT_LIST_DIR}/..")
set(_umicom_media_assets
    "${_umicom_media_root}/resources/brand/umicom-logo-on-dark.svg"
    "${_umicom_media_root}/resources/brand/umicom-icon-on-dark.svg"
    "${_umicom_media_root}/resources/styles/media-workspace.css")
set(_umicom_media_names LOGO ICON STYLE)
foreach(_umicom_media_index RANGE 0 2)
    list(GET _umicom_media_assets ${_umicom_media_index} _umicom_media_asset)
    list(GET _umicom_media_names ${_umicom_media_index} _umicom_media_name)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_umicom_media_asset}")
    file(READ "${_umicom_media_asset}" _umicom_media_hex HEX)
    string(REGEX REPLACE "([0-9a-fA-F][0-9a-fA-F])" "0x\\1,"
        UMICOM_MEDIA_${_umicom_media_name}_BYTES "${_umicom_media_hex}")
endforeach()
set(_umicom_media_generated "${CMAKE_CURRENT_BINARY_DIR}/generated/creative-media")
file(MAKE_DIRECTORY "${_umicom_media_generated}")
configure_file("${_umicom_media_root}/adapters/gtk4/media_workspace_resources.h.in"
    "${_umicom_media_generated}/umicom_media_workspace_resources.h" @ONLY)
target_include_directories(umicom_creative_workspace_gtk4 PRIVATE "${_umicom_media_generated}")
target_sources(umicom_creative_workspace_gtk4 PRIVATE
    "${_umicom_media_root}/adapters/gtk4/media_workspace_style.c")
