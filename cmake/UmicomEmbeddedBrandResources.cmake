#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomEmbeddedBrandResources.cmake
#
# PURPOSE:
#   Keep approved artwork and other compiled resources tied to their original source bytes.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)

# Generate a byte initializer directly from the checked source. Split long
# rows so large assets also stay within C translation line limits.
function(umicom_resource_initializer asset output)
    # Normalize the asset path before it becomes a configure input. Independent
    # consumers can reach the same master through different directory spellings,
    # while Ninja assigns that master a single canonical graph identity.
    get_filename_component(_asset "${asset}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    if(NOT EXISTS "${_asset}" OR IS_DIRECTORY "${_asset}")
        message(FATAL_ERROR "Required Umicom resource is missing: ${_asset}")
    endif()
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_asset}")
    file(READ "${_asset}" _hex HEX)
    string(REGEX REPLACE "([0-9a-fA-F][0-9a-fA-F])" "0x\\1," _bytes "${_hex}")
    set(_byte "0x[0-9a-fA-F][0-9a-fA-F],")
    string(REPEAT "${_byte}" 16 _row)
    string(REGEX REPLACE "(${_row})" "\\1\n" _initializer "${_bytes}")
    set(${output} "${_initializer}" PARENT_SCOPE)
endfunction()

# Retain the existing brand entry point so current consumers keep the same
# source contract while browser modules share exact byte generation.
function(umicom_brand_resource_initializer asset output)
    umicom_resource_initializer("${asset}" _brand_bytes)
    set(${output} "${_brand_bytes}" PARENT_SCOPE)
endfunction()
