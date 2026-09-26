#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomEducationWorkspace.cmake
# PURPOSE: Compose the reusable learning practicum, persistence and project exports.
# AUTHOR: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include(GNUInstallDirs)
include(CMakeParseArguments)
set(_umicom_education_root "${CMAKE_CURRENT_LIST_DIR}/..")
foreach(dependency IN ITEMS Umicom::data Umicom::developer)
    if(NOT TARGET ${dependency})
        message(FATAL_ERROR "Education workspace requires canonical ${dependency}")
    endif()
endforeach()
function(umicom_education_configure_target target)
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${target})
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${target})
    endif()
endfunction()
# The teaching sources are authoritative. Embedding them during the build keeps
# exported projects byte-identical, relocatable and independent of the CWD.
set(_umicom_education_project_files "")
foreach(course IN ITEMS notes assembly framework)
    file(GLOB course_files CONFIGURE_DEPENDS LIST_DIRECTORIES FALSE
        "${_umicom_education_root}/examples/learning/projects/${course}/*")
    list(APPEND _umicom_education_project_files ${course_files})
endforeach()
set(_umicom_education_resources "${CMAKE_CURRENT_BINARY_DIR}/umicom-education-generated/project_resources.c")
add_custom_command(OUTPUT "${_umicom_education_resources}"
    COMMAND "${CMAKE_COMMAND}"
        "-DROOT=${_umicom_education_root}/examples/learning/projects"
        "-DOUTPUT=${_umicom_education_resources}"
        -P "${_umicom_education_root}/cmake/GenerateEducationProjects.cmake"
    DEPENDS ${_umicom_education_project_files}
        "${_umicom_education_root}/cmake/GenerateEducationProjects.cmake"
    VERBATIM)
add_library(umicom_education_workspace STATIC
    "${_umicom_education_root}/src/education_workspace/catalogue.c"
    "${_umicom_education_root}/src/education_workspace/workspace.c"
    "${_umicom_education_root}/src/education_workspace/storage.c"
    "${_umicom_education_root}/src/education_workspace/report.c"
    "${_umicom_education_root}/src/education_workspace/export_project.c"
    "${_umicom_education_resources}")
add_library(Umicom::education_workspace ALIAS umicom_education_workspace)
set_target_properties(umicom_education_workspace PROPERTIES EXPORT_NAME education_workspace)
target_include_directories(umicom_education_workspace PUBLIC
    $<BUILD_INTERFACE:${_umicom_education_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_education_workspace PUBLIC Umicom::data Umicom::developer)
umicom_education_configure_target(umicom_education_workspace)
install(TARGETS umicom_education_workspace EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
install(DIRECTORY "${_umicom_education_root}/include/umicom/education_workspace"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom" COMPONENT Framework)
foreach(course IN ITEMS notes assembly framework)
    install(DIRECTORY "${_umicom_education_root}/examples/learning/projects/${course}"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/learning/projects" COMPONENT Learning)
endforeach()
if(TARGET Umicom::ui_gtk4)
    add_library(umicom_education_workspace_gtk4 STATIC
        "${_umicom_education_root}/adapters/gtk4/education_workspace_panel_gtk4.c"
        "${_umicom_education_root}/adapters/gtk4/education_workspace_commands_gtk4.c"
        "${_umicom_education_root}/adapters/gtk4/education_workspace_application_gtk4.c")
    add_library(Umicom::education_workspace_gtk4 ALIAS umicom_education_workspace_gtk4)
    set_target_properties(umicom_education_workspace_gtk4 PROPERTIES EXPORT_NAME education_workspace_gtk4)
    target_link_libraries(umicom_education_workspace_gtk4 PUBLIC
        Umicom::education_workspace Umicom::ui_gtk4)
    umicom_education_configure_target(umicom_education_workspace_gtk4)
    install(TARGETS umicom_education_workspace_gtk4 EXPORT UmicomFrameworkTargets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
    install(FILES "${_umicom_education_root}/include/umicom/ui/gtk4/education_workspace.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
endif()
if(BUILD_TESTING)
    # Compile the same canonical source files that learners export. These
    # results are CTest evidence, not automatically credited quiz progress.
    # The stock test deliberately uses the C reference here; the exported
    # project independently selects and verifies native Assembly when supported.
    add_executable(umicom-education-notes-checks
        "${_umicom_education_root}/examples/learning/projects/notes/notes.c"
        "${_umicom_education_root}/examples/learning/projects/notes/test_notes.c")
    add_executable(umicom-education-stock-reference-checks
        "${_umicom_education_root}/examples/learning/projects/assembly/stock_total.c"
        "${_umicom_education_root}/examples/learning/projects/assembly/test_total.c")
    add_executable(umicom-education-framework-notes-checks
        "${_umicom_education_root}/examples/learning/projects/framework/test_storage.c")
    target_link_libraries(umicom-education-framework-notes-checks PRIVATE Umicom::data)
    foreach(practice IN ITEMS notes stock-reference framework-notes)
        umicom_education_configure_target("umicom-education-${practice}-checks")
        add_test(NAME "framework.education_workspace.practice.${practice}"
            COMMAND "umicom-education-${practice}-checks")
        set_tests_properties("framework.education_workspace.practice.${practice}" PROPERTIES
            TIMEOUT 30 LABELS "framework;education-workspace;practical-project")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("umicom-education-${practice}-checks")
        endif()
    endforeach()
    add_executable(umicom-education-workspace-test
        "${_umicom_education_root}/tests/education_workspace/test_workspace.c")
    target_link_libraries(umicom-education-workspace-test PRIVATE Umicom::education_workspace)
    umicom_education_configure_target(umicom-education-workspace-test)
    include("${_umicom_education_root}/tests/education_workspace/cases.cmake")
    foreach(case IN LISTS UMICOM_EDUCATION_WORKSPACE_CASES)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/education-tests/${case}")
        add_test(NAME "framework.education_workspace.${case}"
            COMMAND umicom-education-workspace-test "${case}")
        set_tests_properties("framework.education_workspace.${case}" PROPERTIES
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/education-tests/${case}"
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;education-workspace;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-education-workspace-test)
    endif()
    add_executable(umicom-education-output-fixture
        "${_umicom_education_root}/tests/education_workspace/output_fixture.c")
    target_link_libraries(umicom-education-output-fixture PRIVATE Umicom::education_workspace)
    umicom_education_configure_target(umicom-education-output-fixture)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-education-output-fixture)
    endif()
    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(Python3_Interpreter_FOUND)
        add_test(NAME framework.education_workspace.independent_output
            COMMAND "${Python3_EXECUTABLE}"
                "${_umicom_education_root}/tests/education_workspace/check_output.py"
                $<TARGET_FILE:umicom-education-output-fixture>
                "${_umicom_education_root}/examples/learning/projects")
        set_tests_properties(framework.education_workspace.independent_output PROPERTIES
            TIMEOUT 60 LABELS "framework;education-workspace;export;regression")
    else()
        message(STATUS "Education independent output check not registered: Python3 interpreter unavailable")
    endif()
    if(TARGET Umicom::education_workspace_gtk4)
        add_executable(umicom-education-workspace-gtk-test
            "${_umicom_education_root}/tests/education_workspace/test_panel_gtk4.c")
        target_link_libraries(umicom-education-workspace-gtk-test PRIVATE Umicom::education_workspace_gtk4)
        umicom_education_configure_target(umicom-education-workspace-gtk-test)
        foreach(case workflow unsaved_note retained_controls)
            add_test(NAME "framework.education_workspace.gtk.${case}"
                COMMAND umicom-education-workspace-gtk-test "${case}")
            set_tests_properties("framework.education_workspace.gtk.${case}" PROPERTIES
                TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;education-workspace;gtk4;regression")
        endforeach()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-education-workspace-gtk-test)
        endif()
    endif()
endif()
# The module selects the shared capability. No lesson, score or persistence
# implementation is copied into a consuming application.
function(umicom_attach_education_workspace)
    cmake_parse_arguments(PARSE_ARGV 0 UMI "" "NAME;TARGET" "")
    if(UMI_UNPARSED_ARGUMENTS OR NOT UMI_NAME STREQUAL "education" OR
       NOT UMI_TARGET OR NOT TARGET "${UMI_TARGET}")
        message(FATAL_ERROR "Education attachment requires NAME education and an existing TARGET")
    endif()
    get_target_property(attached "${UMI_TARGET}" UMICOM_EDUCATION_WORKSPACE_ATTACHED)
    if(attached)
        message(FATAL_ERROR "Education workspace already attached to ${UMI_TARGET}")
    endif()
    set_target_properties("${UMI_TARGET}" PROPERTIES UMICOM_EDUCATION_WORKSPACE_ATTACHED TRUE)
    target_link_libraries("${UMI_TARGET}" PUBLIC Umicom::education_workspace)
    add_executable(umicom-education-workspace-example
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../examples/education_workspace_main.c")
    target_link_libraries(umicom-education-workspace-example PRIVATE Umicom::education_workspace)
    umicom_education_configure_target(umicom-education-workspace-example)
    install(TARGETS umicom-education-workspace-example
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT Learning)
    if(BUILD_TESTING)
        add_test(NAME education.workspace.example COMMAND umicom-education-workspace-example)
        set_tests_properties(education.workspace.example PROPERTIES TIMEOUT 30 LABELS "education;example")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-education-workspace-example)
        endif()
    endif()
endfunction()
install(FILES "${_umicom_education_root}/docs/learning/EDUCATION_WORKSPACE.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Learning)
unset(_umicom_education_root)
unset(_umicom_education_resources)
unset(_umicom_education_project_files)
