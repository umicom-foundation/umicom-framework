#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomQualificationInputs.cmake
# Purpose: Associate private headers and runtime resources with build owners.
# Author and Organisation: Sammy Hegab, Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

# Call after creating the real local target. Paths are relative to the calling
# directory, like target_sources. Missing files remain visible as unresolved
# inventory rows; declaration does not assert that a generated output exists.
function(umicom_target_qualification_inputs target)
    cmake_parse_arguments(PARSE_ARGV 1 INPUT "" "" "FILES")
    if(NOT "${INPUT_UNPARSED_ARGUMENTS}" STREQUAL "" OR
            NOT "${INPUT_KEYWORDS_MISSING_VALUES}" STREQUAL "" OR "${INPUT_FILES}" STREQUAL "")
        message(FATAL_ERROR "Qualification inputs require FILES and no unknown arguments: ${target}")
    endif()
    if(NOT TARGET "${target}")
        message(FATAL_ERROR "Unknown qualification input owner: ${target}")
    endif()
    get_property(_alias TARGET "${target}" PROPERTY ALIASED_TARGET)
    get_property(_imported TARGET "${target}" PROPERTY IMPORTED)
    if(NOT "${_alias}" STREQUAL "" OR _imported)
        message(FATAL_ERROR "Qualification inputs require a real local target: ${target}")
    endif()
    set(_accepted "")
    foreach(_input IN LISTS INPUT_FILES)
        if(_input STREQUAL "" OR _input MATCHES "\\$<" OR _input MATCHES "[;\r\n\t]")
            message(FATAL_ERROR "Qualification input must be a literal file path: ${target}")
        endif()
        cmake_path(ABSOLUTE_PATH _input BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" NORMALIZE OUTPUT_VARIABLE _absolute)
        if(IS_DIRECTORY "${_absolute}")
            message(FATAL_ERROR "Qualification input cannot be a directory: ${_absolute}")
        endif()
        list(APPEND _accepted "${_absolute}")
    endforeach()
    get_property(_existing TARGET "${target}" PROPERTY UMICOM_QUALIFICATION_INPUTS)
    list(APPEND _existing ${_accepted})
    list(REMOVE_DUPLICATES _existing)
    # This is qualification ownership, not a compiler dependency scanner. Keep
    # SOURCES and custom commands unchanged; the owner defines real build recipes.
    set_property(TARGET "${target}" PROPERTY UMICOM_QUALIFICATION_INPUTS "${_existing}")
endfunction()
