# -----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/windows/StageNativeRuntime.cmake
# PURPOSE: Resolve and stage one console executable's coherent private runtime.
# EDUCATIONAL OVERVIEW: Validate every resolved source and destination before
# copying. System DLLs stay owned by Windows; conflicting existing private files
# require review instead of silently mixing incompatible runtime versions.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
# -----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.25)
include("${CMAKE_CURRENT_LIST_DIR}/DeploymentCommon.cmake")
foreach(_field IN ITEMS UMI_NATIVE_EXE UMI_NATIVE_RUNTIME UMI_NATIVE_DESTINATION UMI_NATIVE_OBJDUMP)
    if(NOT DEFINED ${_field} OR "${${_field}}" STREQUAL "")
        message(FATAL_ERROR "Missing native runtime field: ${_field}")
    endif()
    umicom_windows_require_literal("${${_field}}")
endforeach()
umicom_windows_require_pe("${UMI_NATIVE_EXE}")
if(NOT IS_DIRECTORY "${UMI_NATIVE_RUNTIME}" OR NOT EXISTS "${UMI_NATIVE_OBJDUMP}")
    message(FATAL_ERROR "Native runtime bin directory and its inspection tool are required")
endif()
file(REAL_PATH "${UMI_NATIVE_RUNTIME}" _runtime)
file(REAL_PATH "${UMI_NATIVE_DESTINATION}" _destination)
file(REAL_PATH "$ENV{SystemRoot}/System32" _system)
string(TOLOWER "${_runtime}" _runtime_key)
string(TOLOWER "${_system}" _system_key)
set(CMAKE_GET_RUNTIME_DEPENDENCIES_PLATFORM "windows+pe")
set(CMAKE_GET_RUNTIME_DEPENDENCIES_TOOL "objdump")
set(CMAKE_GET_RUNTIME_DEPENDENCIES_COMMAND "${UMI_NATIVE_OBJDUMP}")
# CMake reads the dependency tree with the selected binary tool; it never loads
# DLL code. API-set names are contracts provided by Windows, not payload files.
# Keep this scanner process independent of any Conda DLL directories inherited
# from its parent shell. This environment is not written to account settings.
set(_previous_path "$ENV{PATH}")
set(ENV{PATH} "${_runtime};$ENV{SystemRoot}/System32;$ENV{SystemRoot}")
file(GET_RUNTIME_DEPENDENCIES
    EXECUTABLES "${UMI_NATIVE_EXE}"
    DIRECTORIES "${_runtime}"
    RESOLVED_DEPENDENCIES_VAR _resolved
    UNRESOLVED_DEPENDENCIES_VAR _unresolved
    CONFLICTING_DEPENDENCIES_PREFIX _conflicts
    PRE_EXCLUDE_REGEXES "^[Aa][Pp][Ii]-[Mm][Ss]-" "^[Ee][Xx][Tt]-[Mm][Ss]-")
set(ENV{PATH} "${_previous_path}")
if(_unresolved OR _conflicts_FILENAMES)
    message(FATAL_ERROR "Incomplete/conflicting private runtime. Missing: ${_unresolved}; conflicts: ${_conflicts_FILENAMES}")
endif()
list(LENGTH _resolved _count)
if(_count GREATER 256)
    message(FATAL_ERROR "Native runtime exceeds the dependency-count limit")
endif()
set(_sources "")
set(_names "")
foreach(_file IN LISTS _resolved)
    file(REAL_PATH "${_file}" _real)
    string(TOLOWER "${_real}" _key)
    get_filename_component(_name "${_real}" NAME)
    get_filename_component(_parent "${_real}" DIRECTORY)
    string(TOLOWER "${_parent}" _parent_key)
    if(_parent_key STREQUAL _system_key)
        continue()
    endif()
    if(EXISTS "${_system}/${_name}")
        message(FATAL_ERROR "Refusing a private shadow of a Windows system DLL: ${_name}")
    endif()
    # Prefer the declared toolchain copy, even when an old copy is already beside
    # the executable. Hash comparison below refuses unintended runtime upgrades.
    set(_source "${_runtime}/${_name}")
    if(NOT EXISTS "${_source}" OR IS_SYMLINK "${_source}")
        message(FATAL_ERROR "Dependency is not supplied by the selected runtime: ${_name}")
    endif()
    file(REAL_PATH "${_source}" _source_real)
    get_filename_component(_source_parent "${_source_real}" DIRECTORY)
    string(TOLOWER "${_source_parent}" _source_parent_key)
    if(NOT _source_parent_key STREQUAL _runtime_key)
        message(FATAL_ERROR "Runtime dependency redirects outside the selected directory")
    endif()
    umicom_windows_require_relative("${_name}")
    umicom_windows_require_pe("${_source}")
    file(SHA256 "${_source}" _hash)
    file(SHA256 "${_real}" _observed_hash)
    if(NOT _hash STREQUAL _observed_hash)
        message(FATAL_ERROR "Resolved DLL differs from the selected runtime: ${_name}")
    endif()
    if(EXISTS "${_destination}/${_name}")
        if(IS_DIRECTORY "${_destination}/${_name}" OR IS_SYMLINK "${_destination}/${_name}")
            message(FATAL_ERROR "Native runtime destination is not a regular file: ${_name}")
        endif()
        file(SHA256 "${_destination}/${_name}" _existing_hash)
        if(NOT _existing_hash STREQUAL _hash)
            message(FATAL_ERROR "Existing private DLL differs; review before replacement: ${_name}")
        endif()
    endif()
    list(APPEND _names "${_name}")
    list(APPEND _sources "${_source}")
endforeach()
umicom_windows_validate_unique_paths(${_names})
# Validation of the whole copy set precedes every write. Copy failure may leave
# newly created dependencies, but never replaces an unrelated or conflicting file.
foreach(_source IN LISTS _sources)
    get_filename_component(_name "${_source}" NAME)
    umicom_windows_copy_file("${_source}" "${_destination}/${_name}")
endforeach()
message(STATUS "Private native runtime checked: ${UMI_NATIVE_EXE}")
