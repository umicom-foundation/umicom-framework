#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/windows/DeploymentCommon.cmake
#
# PURPOSE:
#   Share validation and deterministic file-copy rules across Windows staging,
#   installation and setup generation. These helpers also run in focused tests.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

function(umicom_windows_require_identifier value)
    if(NOT value MATCHES "^[A-Za-z0-9][A-Za-z0-9_.-]*$")
        message(FATAL_ERROR "Unsafe or empty deployment identifier: ${value}")
    endif()
endfunction()

# Bracket-quoted generated CMake is deliberately a small, checked grammar.
function(umicom_windows_require_literal value)
    if(value MATCHES "[;\r\n\t]" OR value MATCHES "\\]==\\]")
        message(FATAL_ERROR "Unsupported control/list delimiter in deployment value")
    endif()
endfunction()

function(umicom_windows_require_relative value)
    umicom_windows_require_literal("${value}")
    if(NOT value OR IS_ABSOLUTE "${value}" OR value MATCHES "^[A-Za-z]:" OR
       value MATCHES "(^|[/\\\\])\\.\\.?([/\\\\]|$)" OR
       value MATCHES "[\\\\:*?\"<>|]" OR value MATCHES "(^|/)[^/]*[ .](/|$)")
        message(FATAL_ERROR "Unsafe deployment-relative path: ${value}")
    endif()
    string(TOLOWER "${value}" _lower)
    if(_lower MATCHES "(^|/)(con|prn|aux|nul|com[1-9]|lpt[1-9])([.]|/|$)")
        message(FATAL_ERROR "Reserved Windows filename in deployment path: ${value}")
    endif()
endfunction()

function(umicom_windows_nsis_escape value output)
    umicom_windows_require_literal("${value}")
    string(REPLACE "$" "$$" _escaped "${value}")
    string(REPLACE "\"" "$\\\"" _escaped "${_escaped}")
    set(${output} "${_escaped}" PARENT_SCOPE)
endfunction()

# Copy a single declared payload file, not the contents of an arbitrary build
# directory. Symlinks are rejected to keep the package ownership list concrete.
function(umicom_windows_copy_file source destination)
    if(NOT EXISTS "${source}" OR IS_DIRECTORY "${source}" OR IS_SYMLINK "${source}")
        message(FATAL_ERROR "Runtime file is absent, not regular, or is a symlink: ${source}")
    endif()
    get_filename_component(_parent "${destination}" DIRECTORY)
    file(MAKE_DIRECTORY "${_parent}")
    file(COPY_FILE "${source}" "${destination}" ONLY_IF_DIFFERENT RESULT _copy_result)
    if(NOT _copy_result STREQUAL "0")
        message(FATAL_ERROR "Cannot stage ${source}: ${_copy_result}")
    endif()
endfunction()

function(umicom_windows_require_pe path)
    if(NOT EXISTS "${path}" OR IS_DIRECTORY "${path}" OR IS_SYMLINK "${path}")
        message(FATAL_ERROR "A regular built Windows executable is required: ${path}")
    endif()
    file(SIZE "${path}" _size)
    if(_size LESS 88)
        message(FATAL_ERROR "Truncated Windows PE image: ${path}")
    endif()
    file(READ "${path}" _signature LIMIT 2 HEX)
    if(NOT _signature STREQUAL "4d5a")
        message(FATAL_ERROR "Not a Windows PE image: ${path}")
    endif()
    file(READ "${path}" _offset_hex OFFSET 60 LIMIT 4 HEX)
    string(SUBSTRING "${_offset_hex}" 0 2 _b0)
    string(SUBSTRING "${_offset_hex}" 2 2 _b1)
    string(SUBSTRING "${_offset_hex}" 4 2 _b2)
    string(SUBSTRING "${_offset_hex}" 6 2 _b3)
    math(EXPR _offset "0x${_b3}${_b2}${_b1}${_b0}")
    math(EXPR _minimum "${_offset} + 24")
    if(_offset LESS 64 OR _minimum GREATER _size)
        message(FATAL_ERROR "Invalid PE header offset: ${path}")
    endif()
    file(READ "${path}" _pe_header OFFSET ${_offset} LIMIT 6 HEX)
    string(SUBSTRING "${_pe_header}" 0 8 _pe_signature)
    string(SUBSTRING "${_pe_header}" 8 4 _machine)
    set(_expected_machine "6486")
    if(ARGC GREATER 1)
        set(_expected_machine "${ARGV1}")
    endif()
    if(NOT _pe_signature STREQUAL "50450000" OR NOT _machine STREQUAL _expected_machine)
        message(FATAL_ERROR "Invalid PE signature or wrong architecture: ${path}")
    endif()
endfunction()

# Refuse to recursively clear an unowned directory. Build staging is the only
# tree removed recursively by this backend; the installed tree never is.
function(umicom_windows_reset_owned_directory directory)
    if(EXISTS "${directory}")
        if(NOT EXISTS "${directory}/.umicom-package-work")
            message(FATAL_ERROR "Refusing to clear unowned package directory: ${directory}")
        endif()
        file(READ "${directory}/.umicom-package-work" _owner)
        if(NOT _owner STREQUAL "Umicom Windows package workspace\n")
            message(FATAL_ERROR "Package workspace ownership marker does not match")
        endif()
        file(REMOVE_RECURSE "${directory}")
    endif()
    file(MAKE_DIRECTORY "${directory}")
    file(WRITE "${directory}/.umicom-package-work" "Umicom Windows package workspace\n")
endfunction()

# Check the complete filename map case-insensitively before constructing a
# Windows package on any host. Distinct case variants are a collision too.
function(umicom_windows_validate_unique_paths)
    set(_seen "")
    foreach(_path IN LISTS ARGN)
        umicom_windows_require_relative("${_path}")
        string(TOLOWER "${_path}" _key)
        if(_key IN_LIST _seen)
            message(FATAL_ERROR "Conflicting Windows package path: ${_path}")
        endif()
        list(APPEND _seen "${_key}")
    endforeach()
endfunction()
