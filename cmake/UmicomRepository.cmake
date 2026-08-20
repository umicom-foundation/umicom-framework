#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomRepositoryR3.cmake
#
# PURPOSE:
#   Integrate Repository Mega Batch R3 with the current Framework targets
#   without duplicating or replacing the established root CMake catalogue.
#
# USAGE:
#   A composition root includes this file after add_subdirectory(framework).
#   The Framework root may also include it after all base targets and the
#   umicom_add_framework_test helper have been declared.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

set(UMICOM_REPOSITORY_R3_FRAMEWORK_ROOT
    "${CMAKE_CURRENT_LIST_DIR}/..")

if(NOT TARGET umicom_application OR NOT TARGET umicom_desktop)
    message(FATAL_ERROR
        "UmicomRepositoryR3.cmake must be included after the Framework "
        "application and desktop targets are created.")
endif()

target_sources(umicom_application PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/runtime_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/application/launcher.c"
)

target_sources(umicom_desktop PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/desktop/application_strip.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/desktop/desk_runtime.c"
)

if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/desk_gtk4.c"
    )
endif()

if(BUILD_TESTING)
    function(umicom_r3_add_test target test_name source)
        if(TARGET "${target}")
            return()
        endif()
        add_executable("${target}" "${UMICOM_REPOSITORY_R3_FRAMEWORK_ROOT}/${source}")
        target_link_libraries("${target}" PRIVATE Umicom::Framework)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
    endfunction()

    umicom_r3_add_test(
        umicom-application-runtime-catalogue-test
        framework.application.runtime_catalogue
        tests/application/test_runtime_catalogue.c
    )
    umicom_r3_add_test(
        umicom-application-launcher-test
        framework.application.launcher
        tests/application/test_launcher.c
    )
    umicom_r3_add_test(
        umicom-desktop-application-strip-test
        framework.desktop.application_strip
        tests/desktop/test_application_strip.c
    )
    umicom_r3_add_test(
        umicom-desk-runtime-test
        framework.desktop.desk_runtime
        tests/desktop/test_desk_runtime.c
    )
endif()

message(STATUS
    "Repository Mega Batch R3: application launcher and Desk runtime enabled")
