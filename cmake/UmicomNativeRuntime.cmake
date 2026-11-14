# -----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomNativeRuntime.cmake
# PURPOSE: Stage private runtime DLLs for console helpers without GTK resources.
# EDUCATIONAL OVERVIEW: Reuse shared deployment validation and copy primitives.
# Only the declared toolchain runtime supplies non-system dependencies; neither
# the Python environment nor the Windows account PATH is modified.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
# -----------------------------------------------------------------------------
include_guard(GLOBAL)
include("${CMAKE_CURRENT_LIST_DIR}/windows/DeploymentCommon.cmake")
include(CMakeParseArguments)

# Register staging beside a built target and at its explicit install destination.
# Registration is opt-in and does not change any existing GUI deployment rule.
function(umicom_prepare_native_runtime)
    if(NOT WIN32)
        return()
    endif()
    cmake_parse_arguments(NATIVE "" "TARGET;RUNTIME_DIR;DESTINATION" "" ${ARGN})
    if(NATIVE_UNPARSED_ARGUMENTS OR NOT TARGET "${NATIVE_TARGET}")
        message(FATAL_ERROR "Native runtime registration needs an existing target")
    endif()
    umicom_windows_require_identifier("${NATIVE_TARGET}")
    umicom_windows_require_relative("${NATIVE_DESTINATION}")
    umicom_windows_require_literal("${NATIVE_RUNTIME_DIR}")
    if(NOT CMAKE_HOST_WIN32)
        message(FATAL_ERROR "Native runtime deployment needs a Windows host; turn the application staging option off for cross-compilation")
    endif()
    if(NOT IS_DIRECTORY "${NATIVE_RUNTIME_DIR}")
        message(FATAL_ERROR "Select the coherent toolchain bin directory used to link this target")
    endif()
    get_target_property(_type "${NATIVE_TARGET}" TYPE)
    get_target_property(_registered "${NATIVE_TARGET}" UMICOM_NATIVE_RUNTIME_REGISTERED)
    if(NOT _type STREQUAL "EXECUTABLE")
        message(FATAL_ERROR "Private runtime registration requires an executable")
    endif()
    if(_registered)
        return()
    endif()
    find_program(_umi_native_objdump NAMES objdump.exe objdump llvm-objdump.exe
        PATHS "${NATIVE_RUNTIME_DIR}" NO_DEFAULT_PATH NO_CACHE REQUIRED)
    set(_script "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/windows/StageNativeRuntime.cmake")
    # The finished executable is scanned only after linking. An ALL refresh also
    # repairs missing staged DLLs when a later build does not relink that executable.
    add_custom_command(TARGET "${NATIVE_TARGET}" POST_BUILD
        COMMAND "${CMAKE_COMMAND}" "-DUMI_NATIVE_EXE=$<TARGET_FILE:${NATIVE_TARGET}>"
            "-DUMI_NATIVE_RUNTIME=${NATIVE_RUNTIME_DIR}" "-DUMI_NATIVE_OBJDUMP=${_umi_native_objdump}"
            "-DUMI_NATIVE_DESTINATION=$<TARGET_FILE_DIR:${NATIVE_TARGET}>" -P "${_script}" VERBATIM)
    add_custom_target("${NATIVE_TARGET}-private-runtime" ALL
        COMMAND "${CMAKE_COMMAND}" "-DUMI_NATIVE_EXE=$<TARGET_FILE:${NATIVE_TARGET}>"
            "-DUMI_NATIVE_RUNTIME=${NATIVE_RUNTIME_DIR}" "-DUMI_NATIVE_OBJDUMP=${_umi_native_objdump}"
            "-DUMI_NATIVE_DESTINATION=$<TARGET_FILE_DIR:${NATIVE_TARGET}>" -P "${_script}"
        DEPENDS "${NATIVE_TARGET}" VERBATIM)
    # This install rule follows install(TARGETS), preserving a custom tools/native
    # location rather than assuming every executable is installed under bin.
    install(CODE "
set(UMI_NATIVE_EXE [==[$<TARGET_FILE:${NATIVE_TARGET}>]==])
set(UMI_NATIVE_RUNTIME [==[${NATIVE_RUNTIME_DIR}]==])
set(UMI_NATIVE_OBJDUMP [==[${_umi_native_objdump}]==])
set(UMI_NATIVE_DESTINATION \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${NATIVE_DESTINATION}\")
include([==[${_script}]==])
")
    set_property(TARGET "${NATIVE_TARGET}" PROPERTY UMICOM_NATIVE_RUNTIME_REGISTERED TRUE)
endfunction()
