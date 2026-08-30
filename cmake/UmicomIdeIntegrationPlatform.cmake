#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomIdeIntegrationPlatform.cmake
#
# PURPOSE:
#   Attach the cross-domain IDE integration platform to the canonical developer
#   library and register focused regression coverage.
#
# ARCHITECTURE:
#   DocumentCoordinator, Problems, Test Platform, Source Control, DAP Runtime,
#   Language Service, AI Developer Experience, Developer Navigation and runtime
#   command registry remain authoritative. This layer only composes them for thin
#   IDE applications and self-hosting workflows.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

if(NOT TARGET umicom_developer)
    message(FATAL_ERROR
        "IDE Integration requires the canonical umicom_developer target")
endif()

target_sources(umicom_developer PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/active_context.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/ai_action_bridge.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/ai_editor_bridge.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/bindings.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/builtin_surfaces.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/command.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/command_context.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/command_registry_bridge.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/command_router.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/cross_navigation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/debug_bridge.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/document_bridge.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/edit_adapter.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/inline_controller.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/inline_executor.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/inline_history.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/language_bridge.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/location.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/navigation_history.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/perspective.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/platform.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/problem_bridge.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/productivity_bridge.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/selection.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/self_host_checker.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/self_host_manifest.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/source_control_bridge.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/surface.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/surface_profiles/ai_approvals.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/surface_profiles/ai_chat.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/surface_profiles/ai_review.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/surface_profiles/ai_tools.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/surface_profiles/ai_validation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/surface_profiles/build.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/surface_profiles/debug.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/surface_profiles/editor.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/surface_profiles/explorer.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/surface_profiles/outline.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/surface_profiles/output.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/surface_profiles/problems.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/surface_profiles/search.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/surface_profiles/source_control.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/surface_profiles/terminal.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/surface_profiles/test_explorer.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/test_bridge.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/uri.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/view_factory.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/view_helpers.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/view_ids.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/views/active_context.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/views/cross_domain.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/views/inline_ai.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/views/navigation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/views/overview.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/views/self_host.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/views/surfaces.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/views/workflow.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/workflow_evaluator.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/workflow_policy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/workflow_profiles/autonomous_ai.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/workflow_profiles/development.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/workflow_profiles/pre_commit.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/workflow_profiles/release.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/workflow_profiles/self_host.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/workflow_report.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ide_integration/workflow_service.c"
)

if(BUILD_TESTING)
    add_executable(
        umicom-ide-integration-ai-editor-bridge-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_ai_editor_bridge.c"
    )
    target_link_libraries(umicom-ide-integration-ai-editor-bridge-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-ai-editor-bridge-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-ai-editor-bridge-test)
    endif()
    add_test(NAME framework.ide_integration.ai-editor-bridge COMMAND umicom-ide-integration-ai-editor-bridge-test)

    add_executable(
        umicom-ide-integration-command-catalogue-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_command_catalogue.c"
    )
    target_link_libraries(umicom-ide-integration-command-catalogue-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-command-catalogue-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-command-catalogue-test)
    endif()
    add_test(NAME framework.ide_integration.command-catalogue COMMAND umicom-ide-integration-command-catalogue-test)

    add_executable(
        umicom-ide-integration-command-context-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_command_context.c"
    )
    target_link_libraries(umicom-ide-integration-command-context-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-command-context-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-command-context-test)
    endif()
    add_test(NAME framework.ide_integration.command-context COMMAND umicom-ide-integration-command-context-test)

    add_executable(
        umicom-ide-integration-edit-adapter-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_edit_adapter.c"
    )
    target_link_libraries(umicom-ide-integration-edit-adapter-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-edit-adapter-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-edit-adapter-test)
    endif()
    add_test(NAME framework.ide_integration.edit-adapter COMMAND umicom-ide-integration-edit-adapter-test)

    add_executable(
        umicom-ide-integration-inline-controller-conflict-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_inline_controller_conflict.c"
    )
    target_link_libraries(umicom-ide-integration-inline-controller-conflict-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-inline-controller-conflict-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-inline-controller-conflict-test)
    endif()
    add_test(NAME framework.ide_integration.inline-controller-conflict COMMAND umicom-ide-integration-inline-controller-conflict-test)

    add_executable(
        umicom-ide-integration-inline-history-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_inline_history.c"
    )
    target_link_libraries(umicom-ide-integration-inline-history-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-inline-history-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-inline-history-test)
    endif()
    add_test(NAME framework.ide_integration.inline-history COMMAND umicom-ide-integration-inline-history-test)

    add_executable(
        umicom-ide-integration-language-bridge-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_language_bridge.c"
    )
    target_link_libraries(umicom-ide-integration-language-bridge-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-language-bridge-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-language-bridge-test)
    endif()
    add_test(NAME framework.ide_integration.language-bridge COMMAND umicom-ide-integration-language-bridge-test)

    add_executable(
        umicom-ide-integration-location-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_location.c"
    )
    target_link_libraries(umicom-ide-integration-location-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-location-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-location-test)
    endif()
    add_test(NAME framework.ide_integration.location COMMAND umicom-ide-integration-location-test)

    add_executable(
        umicom-ide-integration-navigation-history-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_navigation_history.c"
    )
    target_link_libraries(umicom-ide-integration-navigation-history-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-navigation-history-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-navigation-history-test)
    endif()
    add_test(NAME framework.ide_integration.navigation-history COMMAND umicom-ide-integration-navigation-history-test)

    add_executable(
        umicom-ide-integration-perspective-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_perspective.c"
    )
    target_link_libraries(umicom-ide-integration-perspective-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-perspective-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-perspective-test)
    endif()
    add_test(NAME framework.ide_integration.perspective COMMAND umicom-ide-integration-perspective-test)

    add_executable(
        umicom-ide-integration-problem-bridge-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_problem_bridge.c"
    )
    target_link_libraries(umicom-ide-integration-problem-bridge-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-problem-bridge-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-problem-bridge-test)
    endif()
    add_test(NAME framework.ide_integration.problem-bridge COMMAND umicom-ide-integration-problem-bridge-test)

    add_executable(
        umicom-ide-integration-selection-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_selection.c"
    )
    target_link_libraries(umicom-ide-integration-selection-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-selection-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-selection-test)
    endif()
    add_test(NAME framework.ide_integration.selection COMMAND umicom-ide-integration-selection-test)

    add_executable(
        umicom-ide-integration-self-host-checker-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_self_host_checker.c"
    )
    target_link_libraries(umicom-ide-integration-self-host-checker-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-self-host-checker-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-self-host-checker-test)
    endif()
    add_test(NAME framework.ide_integration.self-host-checker COMMAND umicom-ide-integration-self-host-checker-test)

    add_executable(
        umicom-ide-integration-self-host-manifest-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_self_host_manifest.c"
    )
    target_link_libraries(umicom-ide-integration-self-host-manifest-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-self-host-manifest-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-self-host-manifest-test)
    endif()
    add_test(NAME framework.ide_integration.self-host-manifest COMMAND umicom-ide-integration-self-host-manifest-test)

    add_executable(
        umicom-ide-integration-source-control-target-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_source_control_target.c"
    )
    target_link_libraries(umicom-ide-integration-source-control-target-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-source-control-target-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-source-control-target-test)
    endif()
    add_test(NAME framework.ide_integration.source-control-target COMMAND umicom-ide-integration-source-control-target-test)

    add_executable(
        umicom-ide-integration-surface-manifest-closure-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_surface_manifest_closure.c"
    )
    target_link_libraries(umicom-ide-integration-surface-manifest-closure-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-surface-manifest-closure-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-surface-manifest-closure-test)
    endif()
    add_test(NAME framework.ide_integration.surface-manifest-closure COMMAND umicom-ide-integration-surface-manifest-closure-test)

    add_executable(
        umicom-ide-integration-surface-profile-ai-approvals-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_surface_profile_ai_approvals.c"
    )
    target_link_libraries(umicom-ide-integration-surface-profile-ai-approvals-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-surface-profile-ai-approvals-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-surface-profile-ai-approvals-test)
    endif()
    add_test(NAME framework.ide_integration.surface-profile-ai-approvals COMMAND umicom-ide-integration-surface-profile-ai-approvals-test)

    add_executable(
        umicom-ide-integration-surface-profile-ai-chat-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_surface_profile_ai_chat.c"
    )
    target_link_libraries(umicom-ide-integration-surface-profile-ai-chat-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-surface-profile-ai-chat-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-surface-profile-ai-chat-test)
    endif()
    add_test(NAME framework.ide_integration.surface-profile-ai-chat COMMAND umicom-ide-integration-surface-profile-ai-chat-test)

    add_executable(
        umicom-ide-integration-surface-profile-ai-review-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_surface_profile_ai_review.c"
    )
    target_link_libraries(umicom-ide-integration-surface-profile-ai-review-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-surface-profile-ai-review-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-surface-profile-ai-review-test)
    endif()
    add_test(NAME framework.ide_integration.surface-profile-ai-review COMMAND umicom-ide-integration-surface-profile-ai-review-test)

    add_executable(
        umicom-ide-integration-surface-profile-ai-tools-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_surface_profile_ai_tools.c"
    )
    target_link_libraries(umicom-ide-integration-surface-profile-ai-tools-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-surface-profile-ai-tools-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-surface-profile-ai-tools-test)
    endif()
    add_test(NAME framework.ide_integration.surface-profile-ai-tools COMMAND umicom-ide-integration-surface-profile-ai-tools-test)

    add_executable(
        umicom-ide-integration-surface-profile-ai-validation-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_surface_profile_ai_validation.c"
    )
    target_link_libraries(umicom-ide-integration-surface-profile-ai-validation-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-surface-profile-ai-validation-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-surface-profile-ai-validation-test)
    endif()
    add_test(NAME framework.ide_integration.surface-profile-ai-validation COMMAND umicom-ide-integration-surface-profile-ai-validation-test)

    add_executable(
        umicom-ide-integration-surface-profile-build-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_surface_profile_build.c"
    )
    target_link_libraries(umicom-ide-integration-surface-profile-build-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-surface-profile-build-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-surface-profile-build-test)
    endif()
    add_test(NAME framework.ide_integration.surface-profile-build COMMAND umicom-ide-integration-surface-profile-build-test)

    add_executable(
        umicom-ide-integration-surface-profile-debug-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_surface_profile_debug.c"
    )
    target_link_libraries(umicom-ide-integration-surface-profile-debug-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-surface-profile-debug-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-surface-profile-debug-test)
    endif()
    add_test(NAME framework.ide_integration.surface-profile-debug COMMAND umicom-ide-integration-surface-profile-debug-test)

    add_executable(
        umicom-ide-integration-surface-profile-editor-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_surface_profile_editor.c"
    )
    target_link_libraries(umicom-ide-integration-surface-profile-editor-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-surface-profile-editor-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-surface-profile-editor-test)
    endif()
    add_test(NAME framework.ide_integration.surface-profile-editor COMMAND umicom-ide-integration-surface-profile-editor-test)

    add_executable(
        umicom-ide-integration-surface-profile-explorer-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_surface_profile_explorer.c"
    )
    target_link_libraries(umicom-ide-integration-surface-profile-explorer-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-surface-profile-explorer-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-surface-profile-explorer-test)
    endif()
    add_test(NAME framework.ide_integration.surface-profile-explorer COMMAND umicom-ide-integration-surface-profile-explorer-test)

    add_executable(
        umicom-ide-integration-surface-profile-outline-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_surface_profile_outline.c"
    )
    target_link_libraries(umicom-ide-integration-surface-profile-outline-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-surface-profile-outline-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-surface-profile-outline-test)
    endif()
    add_test(NAME framework.ide_integration.surface-profile-outline COMMAND umicom-ide-integration-surface-profile-outline-test)

    add_executable(
        umicom-ide-integration-surface-profile-output-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_surface_profile_output.c"
    )
    target_link_libraries(umicom-ide-integration-surface-profile-output-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-surface-profile-output-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-surface-profile-output-test)
    endif()
    add_test(NAME framework.ide_integration.surface-profile-output COMMAND umicom-ide-integration-surface-profile-output-test)

    add_executable(
        umicom-ide-integration-surface-profile-problems-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_surface_profile_problems.c"
    )
    target_link_libraries(umicom-ide-integration-surface-profile-problems-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-surface-profile-problems-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-surface-profile-problems-test)
    endif()
    add_test(NAME framework.ide_integration.surface-profile-problems COMMAND umicom-ide-integration-surface-profile-problems-test)

    add_executable(
        umicom-ide-integration-surface-profile-search-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_surface_profile_search.c"
    )
    target_link_libraries(umicom-ide-integration-surface-profile-search-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-surface-profile-search-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-surface-profile-search-test)
    endif()
    add_test(NAME framework.ide_integration.surface-profile-search COMMAND umicom-ide-integration-surface-profile-search-test)

    add_executable(
        umicom-ide-integration-surface-profile-source-control-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_surface_profile_source_control.c"
    )
    target_link_libraries(umicom-ide-integration-surface-profile-source-control-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-surface-profile-source-control-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-surface-profile-source-control-test)
    endif()
    add_test(NAME framework.ide_integration.surface-profile-source-control COMMAND umicom-ide-integration-surface-profile-source-control-test)

    add_executable(
        umicom-ide-integration-surface-profile-terminal-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_surface_profile_terminal.c"
    )
    target_link_libraries(umicom-ide-integration-surface-profile-terminal-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-surface-profile-terminal-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-surface-profile-terminal-test)
    endif()
    add_test(NAME framework.ide_integration.surface-profile-terminal COMMAND umicom-ide-integration-surface-profile-terminal-test)

    add_executable(
        umicom-ide-integration-surface-profile-test-explorer-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_surface_profile_test_explorer.c"
    )
    target_link_libraries(umicom-ide-integration-surface-profile-test-explorer-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-surface-profile-test-explorer-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-surface-profile-test-explorer-test)
    endif()
    add_test(NAME framework.ide_integration.surface-profile-test-explorer COMMAND umicom-ide-integration-surface-profile-test-explorer-test)

    add_executable(
        umicom-ide-integration-surface-registry-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_surface_registry.c"
    )
    target_link_libraries(umicom-ide-integration-surface-registry-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-surface-registry-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-surface-registry-test)
    endif()
    add_test(NAME framework.ide_integration.surface-registry COMMAND umicom-ide-integration-surface-registry-test)

    add_executable(
        umicom-ide-integration-test-bridge-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_test_bridge.c"
    )
    target_link_libraries(umicom-ide-integration-test-bridge-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-test-bridge-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-test-bridge-test)
    endif()
    add_test(NAME framework.ide_integration.test-bridge COMMAND umicom-ide-integration-test-bridge-test)

    add_executable(
        umicom-ide-integration-types-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_types.c"
    )
    target_link_libraries(umicom-ide-integration-types-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-types-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-types-test)
    endif()
    add_test(NAME framework.ide_integration.types COMMAND umicom-ide-integration-types-test)

    add_executable(
        umicom-ide-integration-uri-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_uri.c"
    )
    target_link_libraries(umicom-ide-integration-uri-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-uri-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-uri-test)
    endif()
    add_test(NAME framework.ide_integration.uri COMMAND umicom-ide-integration-uri-test)

    add_executable(
        umicom-ide-integration-view-ids-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_view_ids.c"
    )
    target_link_libraries(umicom-ide-integration-view-ids-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-view-ids-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-view-ids-test)
    endif()
    add_test(NAME framework.ide_integration.view-ids COMMAND umicom-ide-integration-view-ids-test)

    add_executable(
        umicom-ide-integration-workflow-evaluator-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_workflow_evaluator.c"
    )
    target_link_libraries(umicom-ide-integration-workflow-evaluator-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-workflow-evaluator-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-workflow-evaluator-test)
    endif()
    add_test(NAME framework.ide_integration.workflow-evaluator COMMAND umicom-ide-integration-workflow-evaluator-test)

    add_executable(
        umicom-ide-integration-workflow-policy-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_workflow_policy.c"
    )
    target_link_libraries(umicom-ide-integration-workflow-policy-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-workflow-policy-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-workflow-policy-test)
    endif()
    add_test(NAME framework.ide_integration.workflow-policy COMMAND umicom-ide-integration-workflow-policy-test)

    add_executable(
        umicom-ide-integration-workflow-profile-autonomous-ai-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_workflow_profile_autonomous_ai.c"
    )
    target_link_libraries(umicom-ide-integration-workflow-profile-autonomous-ai-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-workflow-profile-autonomous-ai-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-workflow-profile-autonomous-ai-test)
    endif()
    add_test(NAME framework.ide_integration.workflow-profile-autonomous-ai COMMAND umicom-ide-integration-workflow-profile-autonomous-ai-test)

    add_executable(
        umicom-ide-integration-workflow-profile-development-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_workflow_profile_development.c"
    )
    target_link_libraries(umicom-ide-integration-workflow-profile-development-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-workflow-profile-development-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-workflow-profile-development-test)
    endif()
    add_test(NAME framework.ide_integration.workflow-profile-development COMMAND umicom-ide-integration-workflow-profile-development-test)

    add_executable(
        umicom-ide-integration-workflow-profile-pre-commit-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_workflow_profile_pre_commit.c"
    )
    target_link_libraries(umicom-ide-integration-workflow-profile-pre-commit-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-workflow-profile-pre-commit-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-workflow-profile-pre-commit-test)
    endif()
    add_test(NAME framework.ide_integration.workflow-profile-pre-commit COMMAND umicom-ide-integration-workflow-profile-pre-commit-test)

    add_executable(
        umicom-ide-integration-workflow-profile-release-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_workflow_profile_release.c"
    )
    target_link_libraries(umicom-ide-integration-workflow-profile-release-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-workflow-profile-release-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-workflow-profile-release-test)
    endif()
    add_test(NAME framework.ide_integration.workflow-profile-release COMMAND umicom-ide-integration-workflow-profile-release-test)

    add_executable(
        umicom-ide-integration-workflow-profile-self-host-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_workflow_profile_self_host.c"
    )
    target_link_libraries(umicom-ide-integration-workflow-profile-self-host-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-workflow-profile-self-host-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-workflow-profile-self-host-test)
    endif()
    add_test(NAME framework.ide_integration.workflow-profile-self-host COMMAND umicom-ide-integration-workflow-profile-self-host-test)

    add_executable(
        umicom-ide-integration-workflow-report-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ide_integration/test_workflow_report.c"
    )
    target_link_libraries(umicom-ide-integration-workflow-report-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ide-integration-workflow-report-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ide-integration-workflow-report-test)
    endif()
    add_test(NAME framework.ide_integration.workflow-report COMMAND umicom-ide-integration-workflow-report-test)

endif()

message(STATUS
    "Umicom cross-domain IDE integration, inline AI and self-host workflow enabled")
