# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Independent consumer: every Framework header and library comes from the SDK.
cmake_minimum_required(VERSION 3.24)
project(UmicomInstalledSdkQualification LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(UMICOM_SDK_ROOT "" CACHE PATH "Explicit complete Framework installation to qualify")
if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
    message(FATAL_ERROR "Supply an absolute existing UMICOM_SDK_ROOT installation prefix")
endif()
# The selected prefix is mandatory. Do not fall back to a source checkout or
# another registered SDK if the requested installation is missing a component.
find_package(UmicomFramework 0.9 CONFIG REQUIRED COMPONENTS base platform document
    PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
if(NOT COMMAND umicom_require_installed_sdk)
    message(FATAL_ERROR "This SDK lacks installed-consumer qualification; rebuild and install the current SDK")
endif()
umicom_require_installed_sdk("${UMICOM_SDK_ROOT}" Umicom::platform Umicom::document)

# Reuse the existing public Notes exercise. There is no copied implementation
# of editing, undo/redo, ownership, saving or document conflict handling.
file(REAL_PATH "${UMICOM_SDK_ROOT}" _prefix)
set(_notes "${UMICOM_FRAMEWORK_EXAMPLES_ROOT}/editor_workflow/edit_commands.c")
_umicom_require_sdk_file("${_prefix}" "${_notes}" "Installed Notes example")
add_executable(umicom-sdk-notes "${_notes}")
target_link_libraries(umicom-sdk-notes PRIVATE Umicom::document)
set(_workspace "${UMICOM_FRAMEWORK_EXAMPLES_ROOT}/value_archive/workspace.c")
_umicom_require_sdk_file("${_prefix}" "${_workspace}" "Installed workspace decoder example")
add_executable(umicom-sdk-workspace "${_workspace}")
target_link_libraries(umicom-sdk-workspace PRIVATE Umicom::platform)
foreach(_target IN ITEMS umicom-sdk-notes umicom-sdk-workspace)
    if(MSVC)
        target_compile_options("${_target}" PRIVATE /W4 /WX)
    else()
        target_compile_options("${_target}" PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endforeach()
include(CTest)
if(BUILD_TESTING)
    add_test(NAME sdk.notes.edit_history_save COMMAND umicom-sdk-notes)
    add_test(NAME sdk.workspace.scratch_ownership COMMAND umicom-sdk-workspace)
    set_tests_properties(sdk.notes.edit_history_save sdk.workspace.scratch_ownership
        PROPERTIES TIMEOUT 60 LABELS "sdk;installed;consumer")
endif()
install(TARGETS umicom-sdk-notes umicom-sdk-workspace RUNTIME DESTINATION bin)
