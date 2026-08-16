#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomWorkspaceSearchFeatures.cmake
#
# PURPOSE:
#   Add Batch 93 workspace-search, replacement-preview and transactional replace
#   sources and focused tests to the existing Framework editor target.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------

if(NOT TARGET umicom_editor)
    message(FATAL_ERROR
        "UmicomWorkspaceSearchFeatures.cmake must be included after umicom_editor is created")
endif()

target_sources(umicom_editor PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/editor/workspace_search_pattern.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/editor/workspace_search_exclusion.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/editor/workspace_search_index.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/editor/workspace_search_match.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/editor/workspace_search_query.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/editor/workspace_search_preview.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/editor/workspace_replace_plan.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/editor/workspace_replace_transaction.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/editor/workspace_search_orchestration.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/editor/workspace_search_command.c"
)

if(BUILD_TESTING)
    set(UMICOM_WORKSPACE_SEARCH_TESTS
        workspace_search_pattern
        workspace_search_pattern_fuzz
        workspace_search_exclusion
        workspace_search_index
        workspace_search_query
        workspace_search_preview
        workspace_replace_plan
        workspace_replace_transaction
        workspace_search_orchestration
        workspace_search_command
    )

    foreach(UMICOM_WORKSPACE_SEARCH_TEST IN LISTS UMICOM_WORKSPACE_SEARCH_TESTS)
        set(UMICOM_WORKSPACE_SEARCH_TEST_TARGET
            "umicom_test_${UMICOM_WORKSPACE_SEARCH_TEST}")
        add_executable(${UMICOM_WORKSPACE_SEARCH_TEST_TARGET}
            "${CMAKE_CURRENT_LIST_DIR}/../tests/test_${UMICOM_WORKSPACE_SEARCH_TEST}.c")
        target_link_libraries(${UMICOM_WORKSPACE_SEARCH_TEST_TARGET}
            PRIVATE Umicom::editor)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(${UMICOM_WORKSPACE_SEARCH_TEST_TARGET})
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(${UMICOM_WORKSPACE_SEARCH_TEST_TARGET})
        endif()
        add_test(
            NAME "framework.editor.${UMICOM_WORKSPACE_SEARCH_TEST}"
            COMMAND ${UMICOM_WORKSPACE_SEARCH_TEST_TARGET})
        set_tests_properties(
            "framework.editor.${UMICOM_WORKSPACE_SEARCH_TEST}"
            PROPERTIES LABELS "framework;editor;workspace-search;batch-93")
    endforeach()
endif()
