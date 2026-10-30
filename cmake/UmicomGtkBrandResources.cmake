#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomGtkBrandResources.cmake
#
# PURPOSE:
#   Embed canonical SVG artwork without runtime path assumptions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
function(umicom_embed_gtk_brand target)
    set(_root "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/..")
    foreach(_kind IN ITEMS LOGO ICON LOGO_DARK ICON_DARK)
        if(_kind STREQUAL "LOGO")
            set(_file umicom-logo.svg)
        elseif(_kind STREQUAL "ICON")
            set(_file umicom-icon.svg)
        elseif(_kind STREQUAL "LOGO_DARK")
            set(_file umicom-logo-on-dark.svg)
        else()
            set(_file umicom-icon-on-dark.svg)
        endif()
        set(_asset "${_root}/resources/brand/${_file}")
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_asset}")
        file(READ "${_asset}" _hex HEX)
        string(REGEX REPLACE "([0-9a-fA-F][0-9a-fA-F])" "0x\\1,"
            UMICOM_BRAND_${_kind}_BYTES "${_hex}")
    endforeach()
    set(_generated "${CMAKE_CURRENT_BINARY_DIR}/generated/gtk-brand")
    file(MAKE_DIRECTORY "${_generated}")
    configure_file("${_root}/adapters/gtk4/brand_resources.h.in"
        "${_generated}/umicom_gtk_brand_resources.h" @ONLY)
    target_include_directories(${target} PRIVATE "${_generated}")
endfunction()
