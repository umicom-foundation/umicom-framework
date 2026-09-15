#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/workspace_layout_mutation/CMakeLists.txt
# PURPOSE: Compile production workspace mutations with native regression tests.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)

# A small standalone build is available for diagnosis, while the suite links
# its already-created UI target. No replacement implementation is compiled.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomWorkspaceLayoutMutation LANGUAGES C)
    include(CTest)
endif()
if(NOT BUILD_TESTING)
    return()
endif()
if(TARGET umicom-workspace-layout-mutation-test)
    return()
endif()
get_filename_component(_umicom_framework_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
add_executable(umicom-workspace-layout-mutation-test test_layout_mutation.c)
set_target_properties(umicom-workspace-layout-mutation-test PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED ON C_EXTENSIONS OFF)
if(TARGET Umicom::ui)
    target_link_libraries(umicom-workspace-layout-mutation-test PRIVATE Umicom::ui)
elseif(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    target_sources(umicom-workspace-layout-mutation-test PRIVATE
        "${_umicom_framework_root}/src/ui/workspace_layout.c")
    target_include_directories(umicom-workspace-layout-mutation-test PRIVATE
        "${_umicom_framework_root}/include")
else()
    message(FATAL_ERROR "Workspace mutation tests require the existing Umicom::ui target.")
endif()
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom-workspace-layout-mutation-test)
elseif(MSVC)
    target_compile_options(umicom-workspace-layout-mutation-test PRIVATE /W4)
else()
    target_compile_options(umicom-workspace-layout-mutation-test PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-workspace-layout-mutation-test)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-workspace-layout-mutation-test)
endif()
foreach(_case initialisation clone rename group stack placement context locked utf8 journey)
    add_test(NAME "framework.workspace_layout.mutation.${_case}"
        COMMAND umicom-workspace-layout-mutation-test "${_case}")
    set_tests_properties("framework.workspace_layout.mutation.${_case}" PROPERTIES
        LABELS "framework;workspace;layout;mutation;native;regression"
        TIMEOUT 30)
endforeach()
