#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomVcsWorkbenchPlatform.cmake
#
# PURPOSE:
#   Compose the established diff, merge and advanced VCS engines into one
#   operational workbench consumed by thin IDE and trading applications.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

set(UMICOM_VCS_WORKBENCH_FRAMEWORK_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

if(NOT TARGET umicom_developer OR NOT TARGET umicom_vcs)
    message(FATAL_ERROR
        "VCS Workbench Platform requires canonical developer and VCS targets")
endif()

target_sources(umicom_developer PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/vcs/workbench/types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/vcs/workbench/compare_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/vcs/workbench/difference_map.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/vcs/workbench/directory_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/vcs/workbench/merge_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/vcs/workbench/partial_stage_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/vcs/workbench/review_session.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/vcs/workbench/command_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/vcs/workbench/runtime.c"
)
target_link_libraries(umicom_developer PUBLIC Umicom::vcs)

if(BUILD_TESTING)
    function(umicom_add_vcs_workbench_test target test_name source)
        if(TARGET "${target}")
            return()
        endif()
        add_executable(
            "${target}"
            "${UMICOM_VCS_WORKBENCH_FRAMEWORK_ROOT}/${source}"
        )
        target_link_libraries("${target}" PRIVATE Umicom::Framework)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
        set_tests_properties(
            "${test_name}"
            PROPERTIES LABELS "framework;vcs-workbench;source-control;diff-merge"
        )
    endfunction()

    umicom_add_vcs_workbench_test(
        umicom-vcs-workbench-types-test
        framework.vcs_workbench.types
        tests/vcs_workbench/test_types.c)
    umicom_add_vcs_workbench_test(
        umicom-vcs-workbench-compare-model-test
        framework.vcs_workbench.compare.model
        tests/vcs_workbench/test_compare_model.c)
    umicom_add_vcs_workbench_test(
        umicom-vcs-workbench-difference-map-test
        framework.vcs_workbench.difference.map
        tests/vcs_workbench/test_difference_map.c)
    umicom_add_vcs_workbench_test(
        umicom-vcs-workbench-directory-model-test
        framework.vcs_workbench.directory.model
        tests/vcs_workbench/test_directory_model.c)
    umicom_add_vcs_workbench_test(
        umicom-vcs-workbench-merge-model-test
        framework.vcs_workbench.merge.model
        tests/vcs_workbench/test_merge_model.c)
    umicom_add_vcs_workbench_test(
        umicom-vcs-workbench-partial-stage-model-test
        framework.vcs_workbench.partial.stage.model
        tests/vcs_workbench/test_partial_stage_model.c)
    umicom_add_vcs_workbench_test(
        umicom-vcs-workbench-review-session-test
        framework.vcs_workbench.review.session
        tests/vcs_workbench/test_review_session.c)
    umicom_add_vcs_workbench_test(
        umicom-vcs-workbench-command-catalogue-test
        framework.vcs_workbench.command.catalogue
        tests/vcs_workbench/test_command_catalogue.c)
    umicom_add_vcs_workbench_test(
        umicom-vcs-workbench-runtime-test
        framework.vcs_workbench.runtime
        tests/vcs_workbench/test_runtime.c)
endif()
