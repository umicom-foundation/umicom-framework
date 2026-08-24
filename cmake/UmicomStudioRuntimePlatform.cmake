#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomStudioRuntimePlatform.cmake
#
# PURPOSE:
#   Attach the final Framework-owned Umicom Studio runtime composition contract
#   to the canonical developer library and register focused regression coverage.
#
# ARCHITECTURE:
#   The generic Application Shell, IDE Integration platform, DocumentCoordinator,
#   runtime Command Registry and AI Developer Experience remain authoritative.
#   This module composes those reusable services into the final thin-Studio
#   runtime: semantic surfaces/layouts, command aliases and activation, document
#   tabs/title synchronization, status/badges, command search, durable layout
#   sessions, contract closure and the minimal toolkit-host adapter.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

if(NOT TARGET umicom_developer)
    message(FATAL_ERROR
        "Studio Runtime requires the canonical umicom_developer target")
endif()

if(NOT TARGET Umicom::document)
    message(FATAL_ERROR
        "Studio Runtime requires the canonical Umicom::document target")
endif()

# The Studio Runtime projects UmiDocumentTextEncoding through the status model
# (umi_document_encoding_text) and directly owns DocumentCoordinator bindings.
# Make that dependency explicit at the target that receives the Studio Runtime
# sources. This closes the static-library link edge on MinGW/GNU linkers instead
# of relying on transitive executable link order.
target_link_libraries(umicom_developer PUBLIC
    Umicom::document
)

target_sources(umicom_developer PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/activation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/activation_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/activation_profiles/ai_approvals.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/activation_profiles/ai_chat.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/activation_profiles/ai_review.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/activation_profiles/ai_tools.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/activation_profiles/ai_validation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/activation_profiles/build.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/activation_profiles/debug.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/activation_profiles/editor.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/activation_profiles/explorer.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/activation_profiles/outline.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/activation_profiles/output.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/activation_profiles/problems.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/activation_profiles/search.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/activation_profiles/source_control.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/activation_profiles/terminal.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/activation_profiles/test_explorer.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/ai_context_sync.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/badge_sync.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/bindings.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/bootstrap.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/close_guard.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/closure.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_alias.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_alias_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_alias_registry.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_aliases/ai_assistant_open.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_aliases/document_save.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_aliases/file_save.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_aliases/layout_manager.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_aliases/layout_restore.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_aliases/layout_save.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_aliases/navigate_back.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_aliases/navigate_forward.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_aliases/view_ai_approvals.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_aliases/view_ai_review.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_aliases/view_ai_tools.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_aliases/view_ai_validation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_aliases/view_build_dashboard.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_aliases/view_debug.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_aliases/view_explorer.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_aliases/view_outline.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_aliases/view_output.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_aliases/view_problems.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_aliases/view_search.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_aliases/view_source_control.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_aliases/view_terminal.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_aliases/view_test_explorer.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_execution.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_search.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/command_state_sync.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/contract.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/document_state.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/document_sync.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/document_tabs.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/host_adapter.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/host_controller.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/host_sync.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/layout_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/layout_preset.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/layout_presets/ai_development.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/layout_presets/debugging.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/layout_presets/default.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/layout_presets/review.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/layout_presets/source_control.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/layout_presets/testing.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/layout_presets/zen.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/layout_session.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/platform.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/runtime_commands.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/selection_router.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/selection_state.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/session_controller.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/status_model.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/status_sync.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/surface_activator.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/surface_binding.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/surface_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/surface_profiles/ai_approvals.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/surface_profiles/ai_chat.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/surface_profiles/ai_review.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/surface_profiles/ai_tools.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/surface_profiles/ai_validation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/surface_profiles/build.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/surface_profiles/debug.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/surface_profiles/editor.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/surface_profiles/explorer.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/surface_profiles/outline.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/surface_profiles/output.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/surface_profiles/problems.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/surface_profiles/search.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/surface_profiles/source_control.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/surface_profiles/terminal.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/surface_profiles/test_explorer.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/view_factory.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/view_helpers.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/view_ids.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/views/close_guard.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/views/command_search.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/views/contract.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/views/document_tabs.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/views/layout_browser.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/views/runtime_overview.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/views/selection.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/views/shell_state.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/views/status.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/studio_runtime/window_title.c"
)

if(BUILD_TESTING)
    add_executable(
        umicom-studio-runtime-activation-catalogue-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_activation_catalogue.c"
    )
    target_link_libraries(umicom-studio-runtime-activation-catalogue-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-activation-catalogue-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-activation-catalogue-test)
    endif()
    add_test(NAME framework.studio_runtime.activation-catalogue COMMAND umicom-studio-runtime-activation-catalogue-test)

    add_executable(
        umicom-studio-runtime-activation-profile-ai-approvals-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_activation_profile_ai_approvals.c"
    )
    target_link_libraries(umicom-studio-runtime-activation-profile-ai-approvals-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-activation-profile-ai-approvals-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-activation-profile-ai-approvals-test)
    endif()
    add_test(NAME framework.studio_runtime.activation-profile-ai-approvals COMMAND umicom-studio-runtime-activation-profile-ai-approvals-test)

    add_executable(
        umicom-studio-runtime-activation-profile-ai-chat-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_activation_profile_ai_chat.c"
    )
    target_link_libraries(umicom-studio-runtime-activation-profile-ai-chat-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-activation-profile-ai-chat-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-activation-profile-ai-chat-test)
    endif()
    add_test(NAME framework.studio_runtime.activation-profile-ai-chat COMMAND umicom-studio-runtime-activation-profile-ai-chat-test)

    add_executable(
        umicom-studio-runtime-activation-profile-ai-review-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_activation_profile_ai_review.c"
    )
    target_link_libraries(umicom-studio-runtime-activation-profile-ai-review-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-activation-profile-ai-review-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-activation-profile-ai-review-test)
    endif()
    add_test(NAME framework.studio_runtime.activation-profile-ai-review COMMAND umicom-studio-runtime-activation-profile-ai-review-test)

    add_executable(
        umicom-studio-runtime-activation-profile-ai-tools-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_activation_profile_ai_tools.c"
    )
    target_link_libraries(umicom-studio-runtime-activation-profile-ai-tools-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-activation-profile-ai-tools-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-activation-profile-ai-tools-test)
    endif()
    add_test(NAME framework.studio_runtime.activation-profile-ai-tools COMMAND umicom-studio-runtime-activation-profile-ai-tools-test)

    add_executable(
        umicom-studio-runtime-activation-profile-ai-validation-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_activation_profile_ai_validation.c"
    )
    target_link_libraries(umicom-studio-runtime-activation-profile-ai-validation-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-activation-profile-ai-validation-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-activation-profile-ai-validation-test)
    endif()
    add_test(NAME framework.studio_runtime.activation-profile-ai-validation COMMAND umicom-studio-runtime-activation-profile-ai-validation-test)

    add_executable(
        umicom-studio-runtime-activation-profile-build-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_activation_profile_build.c"
    )
    target_link_libraries(umicom-studio-runtime-activation-profile-build-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-activation-profile-build-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-activation-profile-build-test)
    endif()
    add_test(NAME framework.studio_runtime.activation-profile-build COMMAND umicom-studio-runtime-activation-profile-build-test)

    add_executable(
        umicom-studio-runtime-activation-profile-debug-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_activation_profile_debug.c"
    )
    target_link_libraries(umicom-studio-runtime-activation-profile-debug-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-activation-profile-debug-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-activation-profile-debug-test)
    endif()
    add_test(NAME framework.studio_runtime.activation-profile-debug COMMAND umicom-studio-runtime-activation-profile-debug-test)

    add_executable(
        umicom-studio-runtime-activation-profile-editor-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_activation_profile_editor.c"
    )
    target_link_libraries(umicom-studio-runtime-activation-profile-editor-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-activation-profile-editor-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-activation-profile-editor-test)
    endif()
    add_test(NAME framework.studio_runtime.activation-profile-editor COMMAND umicom-studio-runtime-activation-profile-editor-test)

    add_executable(
        umicom-studio-runtime-activation-profile-explorer-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_activation_profile_explorer.c"
    )
    target_link_libraries(umicom-studio-runtime-activation-profile-explorer-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-activation-profile-explorer-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-activation-profile-explorer-test)
    endif()
    add_test(NAME framework.studio_runtime.activation-profile-explorer COMMAND umicom-studio-runtime-activation-profile-explorer-test)

    add_executable(
        umicom-studio-runtime-activation-profile-outline-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_activation_profile_outline.c"
    )
    target_link_libraries(umicom-studio-runtime-activation-profile-outline-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-activation-profile-outline-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-activation-profile-outline-test)
    endif()
    add_test(NAME framework.studio_runtime.activation-profile-outline COMMAND umicom-studio-runtime-activation-profile-outline-test)

    add_executable(
        umicom-studio-runtime-activation-profile-output-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_activation_profile_output.c"
    )
    target_link_libraries(umicom-studio-runtime-activation-profile-output-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-activation-profile-output-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-activation-profile-output-test)
    endif()
    add_test(NAME framework.studio_runtime.activation-profile-output COMMAND umicom-studio-runtime-activation-profile-output-test)

    add_executable(
        umicom-studio-runtime-activation-profile-problems-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_activation_profile_problems.c"
    )
    target_link_libraries(umicom-studio-runtime-activation-profile-problems-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-activation-profile-problems-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-activation-profile-problems-test)
    endif()
    add_test(NAME framework.studio_runtime.activation-profile-problems COMMAND umicom-studio-runtime-activation-profile-problems-test)

    add_executable(
        umicom-studio-runtime-activation-profile-search-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_activation_profile_search.c"
    )
    target_link_libraries(umicom-studio-runtime-activation-profile-search-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-activation-profile-search-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-activation-profile-search-test)
    endif()
    add_test(NAME framework.studio_runtime.activation-profile-search COMMAND umicom-studio-runtime-activation-profile-search-test)

    add_executable(
        umicom-studio-runtime-activation-profile-source-control-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_activation_profile_source_control.c"
    )
    target_link_libraries(umicom-studio-runtime-activation-profile-source-control-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-activation-profile-source-control-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-activation-profile-source-control-test)
    endif()
    add_test(NAME framework.studio_runtime.activation-profile-source-control COMMAND umicom-studio-runtime-activation-profile-source-control-test)

    add_executable(
        umicom-studio-runtime-activation-profile-terminal-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_activation_profile_terminal.c"
    )
    target_link_libraries(umicom-studio-runtime-activation-profile-terminal-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-activation-profile-terminal-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-activation-profile-terminal-test)
    endif()
    add_test(NAME framework.studio_runtime.activation-profile-terminal COMMAND umicom-studio-runtime-activation-profile-terminal-test)

    add_executable(
        umicom-studio-runtime-activation-profile-test-explorer-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_activation_profile_test_explorer.c"
    )
    target_link_libraries(umicom-studio-runtime-activation-profile-test-explorer-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-activation-profile-test-explorer-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-activation-profile-test-explorer-test)
    endif()
    add_test(NAME framework.studio_runtime.activation-profile-test-explorer COMMAND umicom-studio-runtime-activation-profile-test-explorer-test)

    add_executable(
        umicom-studio-runtime-ai-context-sync-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_ai_context_sync.c"
    )
    target_link_libraries(umicom-studio-runtime-ai-context-sync-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-ai-context-sync-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-ai-context-sync-test)
    endif()
    add_test(NAME framework.studio_runtime.ai-context-sync COMMAND umicom-studio-runtime-ai-context-sync-test)

    add_executable(
        umicom-studio-runtime-bindings-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_bindings.c"
    )
    target_link_libraries(umicom-studio-runtime-bindings-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-bindings-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-bindings-test)
    endif()
    add_test(NAME framework.studio_runtime.bindings COMMAND umicom-studio-runtime-bindings-test)

    add_executable(
        umicom-studio-runtime-close-guard-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_close_guard.c"
    )
    target_link_libraries(umicom-studio-runtime-close-guard-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-close-guard-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-close-guard-test)
    endif()
    add_test(NAME framework.studio_runtime.close-guard COMMAND umicom-studio-runtime-close-guard-test)

    add_executable(
        umicom-studio-runtime-command-alias-ai-assistant-open-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_ai_assistant_open.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-ai-assistant-open-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-ai-assistant-open-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-ai-assistant-open-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-ai-assistant-open COMMAND umicom-studio-runtime-command-alias-ai-assistant-open-test)

    add_executable(
        umicom-studio-runtime-command-alias-catalogue-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_catalogue.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-catalogue-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-catalogue-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-catalogue-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-catalogue COMMAND umicom-studio-runtime-command-alias-catalogue-test)

    add_executable(
        umicom-studio-runtime-command-alias-document-save-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_document_save.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-document-save-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-document-save-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-document-save-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-document-save COMMAND umicom-studio-runtime-command-alias-document-save-test)

    add_executable(
        umicom-studio-runtime-command-alias-file-save-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_file_save.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-file-save-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-file-save-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-file-save-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-file-save COMMAND umicom-studio-runtime-command-alias-file-save-test)

    add_executable(
        umicom-studio-runtime-command-alias-layout-manager-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_layout_manager.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-layout-manager-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-layout-manager-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-layout-manager-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-layout-manager COMMAND umicom-studio-runtime-command-alias-layout-manager-test)

    add_executable(
        umicom-studio-runtime-command-alias-layout-restore-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_layout_restore.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-layout-restore-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-layout-restore-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-layout-restore-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-layout-restore COMMAND umicom-studio-runtime-command-alias-layout-restore-test)

    add_executable(
        umicom-studio-runtime-command-alias-layout-save-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_layout_save.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-layout-save-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-layout-save-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-layout-save-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-layout-save COMMAND umicom-studio-runtime-command-alias-layout-save-test)

    add_executable(
        umicom-studio-runtime-command-alias-navigate-back-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_navigate_back.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-navigate-back-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-navigate-back-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-navigate-back-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-navigate-back COMMAND umicom-studio-runtime-command-alias-navigate-back-test)

    add_executable(
        umicom-studio-runtime-command-alias-navigate-forward-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_navigate_forward.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-navigate-forward-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-navigate-forward-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-navigate-forward-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-navigate-forward COMMAND umicom-studio-runtime-command-alias-navigate-forward-test)

    add_executable(
        umicom-studio-runtime-command-alias-registry-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_registry.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-registry-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-registry-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-registry-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-registry COMMAND umicom-studio-runtime-command-alias-registry-test)

    add_executable(
        umicom-studio-runtime-command-alias-view-ai-approvals-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_view_ai_approvals.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-view-ai-approvals-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-view-ai-approvals-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-view-ai-approvals-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-view-ai-approvals COMMAND umicom-studio-runtime-command-alias-view-ai-approvals-test)

    add_executable(
        umicom-studio-runtime-command-alias-view-ai-review-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_view_ai_review.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-view-ai-review-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-view-ai-review-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-view-ai-review-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-view-ai-review COMMAND umicom-studio-runtime-command-alias-view-ai-review-test)

    add_executable(
        umicom-studio-runtime-command-alias-view-ai-tools-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_view_ai_tools.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-view-ai-tools-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-view-ai-tools-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-view-ai-tools-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-view-ai-tools COMMAND umicom-studio-runtime-command-alias-view-ai-tools-test)

    add_executable(
        umicom-studio-runtime-command-alias-view-ai-validation-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_view_ai_validation.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-view-ai-validation-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-view-ai-validation-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-view-ai-validation-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-view-ai-validation COMMAND umicom-studio-runtime-command-alias-view-ai-validation-test)

    add_executable(
        umicom-studio-runtime-command-alias-view-build-dashboard-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_view_build_dashboard.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-view-build-dashboard-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-view-build-dashboard-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-view-build-dashboard-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-view-build-dashboard COMMAND umicom-studio-runtime-command-alias-view-build-dashboard-test)

    add_executable(
        umicom-studio-runtime-command-alias-view-debug-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_view_debug.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-view-debug-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-view-debug-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-view-debug-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-view-debug COMMAND umicom-studio-runtime-command-alias-view-debug-test)

    add_executable(
        umicom-studio-runtime-command-alias-view-explorer-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_view_explorer.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-view-explorer-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-view-explorer-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-view-explorer-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-view-explorer COMMAND umicom-studio-runtime-command-alias-view-explorer-test)

    add_executable(
        umicom-studio-runtime-command-alias-view-outline-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_view_outline.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-view-outline-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-view-outline-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-view-outline-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-view-outline COMMAND umicom-studio-runtime-command-alias-view-outline-test)

    add_executable(
        umicom-studio-runtime-command-alias-view-output-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_view_output.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-view-output-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-view-output-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-view-output-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-view-output COMMAND umicom-studio-runtime-command-alias-view-output-test)

    add_executable(
        umicom-studio-runtime-command-alias-view-problems-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_view_problems.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-view-problems-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-view-problems-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-view-problems-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-view-problems COMMAND umicom-studio-runtime-command-alias-view-problems-test)

    add_executable(
        umicom-studio-runtime-command-alias-view-search-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_view_search.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-view-search-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-view-search-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-view-search-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-view-search COMMAND umicom-studio-runtime-command-alias-view-search-test)

    add_executable(
        umicom-studio-runtime-command-alias-view-source-control-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_view_source_control.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-view-source-control-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-view-source-control-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-view-source-control-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-view-source-control COMMAND umicom-studio-runtime-command-alias-view-source-control-test)

    add_executable(
        umicom-studio-runtime-command-alias-view-terminal-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_view_terminal.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-view-terminal-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-view-terminal-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-view-terminal-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-view-terminal COMMAND umicom-studio-runtime-command-alias-view-terminal-test)

    add_executable(
        umicom-studio-runtime-command-alias-view-test-explorer-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_alias_view_test_explorer.c"
    )
    target_link_libraries(umicom-studio-runtime-command-alias-view-test-explorer-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-alias-view-test-explorer-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-alias-view-test-explorer-test)
    endif()
    add_test(NAME framework.studio_runtime.command-alias-view-test-explorer COMMAND umicom-studio-runtime-command-alias-view-test-explorer-test)

    add_executable(
        umicom-studio-runtime-command-search-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_command_search.c"
    )
    target_link_libraries(umicom-studio-runtime-command-search-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-command-search-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-command-search-test)
    endif()
    add_test(NAME framework.studio_runtime.command-search COMMAND umicom-studio-runtime-command-search-test)

    add_executable(
        umicom-studio-runtime-contract-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_contract.c"
    )
    target_link_libraries(umicom-studio-runtime-contract-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-contract-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-contract-test)
    endif()
    add_test(NAME framework.studio_runtime.contract COMMAND umicom-studio-runtime-contract-test)

    add_executable(
        umicom-studio-runtime-contract-closure-missing-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_contract_closure_missing.c"
    )
    target_link_libraries(umicom-studio-runtime-contract-closure-missing-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-contract-closure-missing-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-contract-closure-missing-test)
    endif()
    add_test(NAME framework.studio_runtime.contract-closure-missing COMMAND umicom-studio-runtime-contract-closure-missing-test)

    add_executable(
        umicom-studio-runtime-document-tabs-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_document_tabs.c"
    )
    target_link_libraries(umicom-studio-runtime-document-tabs-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-document-tabs-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-document-tabs-test)
    endif()
    add_test(NAME framework.studio_runtime.document-tabs COMMAND umicom-studio-runtime-document-tabs-test)

    add_executable(
        umicom-studio-runtime-host-adapter-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_host_adapter.c"
    )
    target_link_libraries(umicom-studio-runtime-host-adapter-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-host-adapter-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-host-adapter-test)
    endif()
    add_test(NAME framework.studio_runtime.host-adapter COMMAND umicom-studio-runtime-host-adapter-test)

    add_executable(
        umicom-studio-runtime-layout-catalogue-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_layout_catalogue.c"
    )
    target_link_libraries(umicom-studio-runtime-layout-catalogue-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-layout-catalogue-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-layout-catalogue-test)
    endif()
    add_test(NAME framework.studio_runtime.layout-catalogue COMMAND umicom-studio-runtime-layout-catalogue-test)

    add_executable(
        umicom-studio-runtime-layout-preset-ai-development-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_layout_preset_ai_development.c"
    )
    target_link_libraries(umicom-studio-runtime-layout-preset-ai-development-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-layout-preset-ai-development-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-layout-preset-ai-development-test)
    endif()
    add_test(NAME framework.studio_runtime.layout-preset-ai-development COMMAND umicom-studio-runtime-layout-preset-ai-development-test)

    add_executable(
        umicom-studio-runtime-layout-preset-apply-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_layout_preset_apply.c"
    )
    target_link_libraries(umicom-studio-runtime-layout-preset-apply-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-layout-preset-apply-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-layout-preset-apply-test)
    endif()
    add_test(NAME framework.studio_runtime.layout-preset-apply COMMAND umicom-studio-runtime-layout-preset-apply-test)

    add_executable(
        umicom-studio-runtime-layout-preset-debugging-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_layout_preset_debugging.c"
    )
    target_link_libraries(umicom-studio-runtime-layout-preset-debugging-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-layout-preset-debugging-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-layout-preset-debugging-test)
    endif()
    add_test(NAME framework.studio_runtime.layout-preset-debugging COMMAND umicom-studio-runtime-layout-preset-debugging-test)

    add_executable(
        umicom-studio-runtime-layout-preset-default-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_layout_preset_default.c"
    )
    target_link_libraries(umicom-studio-runtime-layout-preset-default-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-layout-preset-default-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-layout-preset-default-test)
    endif()
    add_test(NAME framework.studio_runtime.layout-preset-default COMMAND umicom-studio-runtime-layout-preset-default-test)

    add_executable(
        umicom-studio-runtime-layout-preset-review-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_layout_preset_review.c"
    )
    target_link_libraries(umicom-studio-runtime-layout-preset-review-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-layout-preset-review-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-layout-preset-review-test)
    endif()
    add_test(NAME framework.studio_runtime.layout-preset-review COMMAND umicom-studio-runtime-layout-preset-review-test)

    add_executable(
        umicom-studio-runtime-layout-preset-source-control-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_layout_preset_source_control.c"
    )
    target_link_libraries(umicom-studio-runtime-layout-preset-source-control-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-layout-preset-source-control-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-layout-preset-source-control-test)
    endif()
    add_test(NAME framework.studio_runtime.layout-preset-source-control COMMAND umicom-studio-runtime-layout-preset-source-control-test)

    add_executable(
        umicom-studio-runtime-layout-preset-testing-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_layout_preset_testing.c"
    )
    target_link_libraries(umicom-studio-runtime-layout-preset-testing-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-layout-preset-testing-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-layout-preset-testing-test)
    endif()
    add_test(NAME framework.studio_runtime.layout-preset-testing COMMAND umicom-studio-runtime-layout-preset-testing-test)

    add_executable(
        umicom-studio-runtime-layout-preset-zen-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_layout_preset_zen.c"
    )
    target_link_libraries(umicom-studio-runtime-layout-preset-zen-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-layout-preset-zen-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-layout-preset-zen-test)
    endif()
    add_test(NAME framework.studio_runtime.layout-preset-zen COMMAND umicom-studio-runtime-layout-preset-zen-test)

    add_executable(
        umicom-studio-runtime-layout-session-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_layout_session.c"
    )
    target_link_libraries(umicom-studio-runtime-layout-session-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-layout-session-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-layout-session-test)
    endif()
    add_test(NAME framework.studio_runtime.layout-session COMMAND umicom-studio-runtime-layout-session-test)

    add_executable(
        umicom-studio-runtime-navigation-activation-precedence-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_navigation_activation_precedence.c"
    )
    target_link_libraries(umicom-studio-runtime-navigation-activation-precedence-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-navigation-activation-precedence-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-navigation-activation-precedence-test)
    endif()
    add_test(NAME framework.studio_runtime.navigation-activation-precedence COMMAND umicom-studio-runtime-navigation-activation-precedence-test)

    add_executable(
        umicom-studio-runtime-runtime-command-inventory-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_runtime_command_inventory.c"
    )
    target_link_libraries(umicom-studio-runtime-runtime-command-inventory-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-runtime-command-inventory-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-runtime-command-inventory-test)
    endif()
    add_test(NAME framework.studio_runtime.runtime-command-inventory COMMAND umicom-studio-runtime-runtime-command-inventory-test)

    add_executable(
        umicom-studio-runtime-selection-router-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_selection_router.c"
    )
    target_link_libraries(umicom-studio-runtime-selection-router-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-selection-router-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-selection-router-test)
    endif()
    add_test(NAME framework.studio_runtime.selection-router COMMAND umicom-studio-runtime-selection-router-test)

    add_executable(
        umicom-studio-runtime-selection-state-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_selection_state.c"
    )
    target_link_libraries(umicom-studio-runtime-selection-state-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-selection-state-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-selection-state-test)
    endif()
    add_test(NAME framework.studio_runtime.selection-state COMMAND umicom-studio-runtime-selection-state-test)

    add_executable(
        umicom-studio-runtime-status-model-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_status_model.c"
    )
    target_link_libraries(umicom-studio-runtime-status-model-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-status-model-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-status-model-test)
    endif()
    add_test(NAME framework.studio_runtime.status-model COMMAND umicom-studio-runtime-status-model-test)

    add_executable(
        umicom-studio-runtime-surface-activation-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_surface_activation.c"
    )
    target_link_libraries(umicom-studio-runtime-surface-activation-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-surface-activation-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-surface-activation-test)
    endif()
    add_test(NAME framework.studio_runtime.surface-activation COMMAND umicom-studio-runtime-surface-activation-test)

    add_executable(
        umicom-studio-runtime-surface-catalogue-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_surface_catalogue.c"
    )
    target_link_libraries(umicom-studio-runtime-surface-catalogue-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-surface-catalogue-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-surface-catalogue-test)
    endif()
    add_test(NAME framework.studio_runtime.surface-catalogue COMMAND umicom-studio-runtime-surface-catalogue-test)

    add_executable(
        umicom-studio-runtime-surface-install-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_surface_install.c"
    )
    target_link_libraries(umicom-studio-runtime-surface-install-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-surface-install-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-surface-install-test)
    endif()
    add_test(NAME framework.studio_runtime.surface-install COMMAND umicom-studio-runtime-surface-install-test)

    add_executable(
        umicom-studio-runtime-surface-profile-ai-approvals-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_surface_profile_ai_approvals.c"
    )
    target_link_libraries(umicom-studio-runtime-surface-profile-ai-approvals-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-surface-profile-ai-approvals-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-surface-profile-ai-approvals-test)
    endif()
    add_test(NAME framework.studio_runtime.surface-profile-ai-approvals COMMAND umicom-studio-runtime-surface-profile-ai-approvals-test)

    add_executable(
        umicom-studio-runtime-surface-profile-ai-chat-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_surface_profile_ai_chat.c"
    )
    target_link_libraries(umicom-studio-runtime-surface-profile-ai-chat-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-surface-profile-ai-chat-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-surface-profile-ai-chat-test)
    endif()
    add_test(NAME framework.studio_runtime.surface-profile-ai-chat COMMAND umicom-studio-runtime-surface-profile-ai-chat-test)

    add_executable(
        umicom-studio-runtime-surface-profile-ai-review-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_surface_profile_ai_review.c"
    )
    target_link_libraries(umicom-studio-runtime-surface-profile-ai-review-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-surface-profile-ai-review-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-surface-profile-ai-review-test)
    endif()
    add_test(NAME framework.studio_runtime.surface-profile-ai-review COMMAND umicom-studio-runtime-surface-profile-ai-review-test)

    add_executable(
        umicom-studio-runtime-surface-profile-ai-tools-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_surface_profile_ai_tools.c"
    )
    target_link_libraries(umicom-studio-runtime-surface-profile-ai-tools-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-surface-profile-ai-tools-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-surface-profile-ai-tools-test)
    endif()
    add_test(NAME framework.studio_runtime.surface-profile-ai-tools COMMAND umicom-studio-runtime-surface-profile-ai-tools-test)

    add_executable(
        umicom-studio-runtime-surface-profile-ai-validation-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_surface_profile_ai_validation.c"
    )
    target_link_libraries(umicom-studio-runtime-surface-profile-ai-validation-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-surface-profile-ai-validation-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-surface-profile-ai-validation-test)
    endif()
    add_test(NAME framework.studio_runtime.surface-profile-ai-validation COMMAND umicom-studio-runtime-surface-profile-ai-validation-test)

    add_executable(
        umicom-studio-runtime-surface-profile-build-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_surface_profile_build.c"
    )
    target_link_libraries(umicom-studio-runtime-surface-profile-build-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-surface-profile-build-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-surface-profile-build-test)
    endif()
    add_test(NAME framework.studio_runtime.surface-profile-build COMMAND umicom-studio-runtime-surface-profile-build-test)

    add_executable(
        umicom-studio-runtime-surface-profile-debug-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_surface_profile_debug.c"
    )
    target_link_libraries(umicom-studio-runtime-surface-profile-debug-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-surface-profile-debug-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-surface-profile-debug-test)
    endif()
    add_test(NAME framework.studio_runtime.surface-profile-debug COMMAND umicom-studio-runtime-surface-profile-debug-test)

    add_executable(
        umicom-studio-runtime-surface-profile-editor-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_surface_profile_editor.c"
    )
    target_link_libraries(umicom-studio-runtime-surface-profile-editor-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-surface-profile-editor-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-surface-profile-editor-test)
    endif()
    add_test(NAME framework.studio_runtime.surface-profile-editor COMMAND umicom-studio-runtime-surface-profile-editor-test)

    add_executable(
        umicom-studio-runtime-surface-profile-explorer-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_surface_profile_explorer.c"
    )
    target_link_libraries(umicom-studio-runtime-surface-profile-explorer-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-surface-profile-explorer-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-surface-profile-explorer-test)
    endif()
    add_test(NAME framework.studio_runtime.surface-profile-explorer COMMAND umicom-studio-runtime-surface-profile-explorer-test)

    add_executable(
        umicom-studio-runtime-surface-profile-outline-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_surface_profile_outline.c"
    )
    target_link_libraries(umicom-studio-runtime-surface-profile-outline-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-surface-profile-outline-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-surface-profile-outline-test)
    endif()
    add_test(NAME framework.studio_runtime.surface-profile-outline COMMAND umicom-studio-runtime-surface-profile-outline-test)

    add_executable(
        umicom-studio-runtime-surface-profile-output-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_surface_profile_output.c"
    )
    target_link_libraries(umicom-studio-runtime-surface-profile-output-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-surface-profile-output-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-surface-profile-output-test)
    endif()
    add_test(NAME framework.studio_runtime.surface-profile-output COMMAND umicom-studio-runtime-surface-profile-output-test)

    add_executable(
        umicom-studio-runtime-surface-profile-problems-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_surface_profile_problems.c"
    )
    target_link_libraries(umicom-studio-runtime-surface-profile-problems-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-surface-profile-problems-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-surface-profile-problems-test)
    endif()
    add_test(NAME framework.studio_runtime.surface-profile-problems COMMAND umicom-studio-runtime-surface-profile-problems-test)

    add_executable(
        umicom-studio-runtime-surface-profile-search-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_surface_profile_search.c"
    )
    target_link_libraries(umicom-studio-runtime-surface-profile-search-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-surface-profile-search-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-surface-profile-search-test)
    endif()
    add_test(NAME framework.studio_runtime.surface-profile-search COMMAND umicom-studio-runtime-surface-profile-search-test)

    add_executable(
        umicom-studio-runtime-surface-profile-source-control-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_surface_profile_source_control.c"
    )
    target_link_libraries(umicom-studio-runtime-surface-profile-source-control-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-surface-profile-source-control-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-surface-profile-source-control-test)
    endif()
    add_test(NAME framework.studio_runtime.surface-profile-source-control COMMAND umicom-studio-runtime-surface-profile-source-control-test)

    add_executable(
        umicom-studio-runtime-surface-profile-terminal-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_surface_profile_terminal.c"
    )
    target_link_libraries(umicom-studio-runtime-surface-profile-terminal-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-surface-profile-terminal-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-surface-profile-terminal-test)
    endif()
    add_test(NAME framework.studio_runtime.surface-profile-terminal COMMAND umicom-studio-runtime-surface-profile-terminal-test)

    add_executable(
        umicom-studio-runtime-surface-profile-test-explorer-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_surface_profile_test_explorer.c"
    )
    target_link_libraries(umicom-studio-runtime-surface-profile-test-explorer-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-surface-profile-test-explorer-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-surface-profile-test-explorer-test)
    endif()
    add_test(NAME framework.studio_runtime.surface-profile-test-explorer COMMAND umicom-studio-runtime-surface-profile-test-explorer-test)

    add_executable(
        umicom-studio-runtime-types-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_types.c"
    )
    target_link_libraries(umicom-studio-runtime-types-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-types-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-types-test)
    endif()
    add_test(NAME framework.studio_runtime.types COMMAND umicom-studio-runtime-types-test)

    add_executable(
        umicom-studio-runtime-view-ids-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_view_ids.c"
    )
    target_link_libraries(umicom-studio-runtime-view-ids-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-view-ids-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-view-ids-test)
    endif()
    add_test(NAME framework.studio_runtime.view-ids COMMAND umicom-studio-runtime-view-ids-test)

    add_executable(
        umicom-studio-runtime-view-ids-unique-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_view_ids_unique.c"
    )
    target_link_libraries(umicom-studio-runtime-view-ids-unique-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-view-ids-unique-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-view-ids-unique-test)
    endif()
    add_test(NAME framework.studio_runtime.view-ids-unique COMMAND umicom-studio-runtime-view-ids-unique-test)

    add_executable(
        umicom-studio-runtime-window-title-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_window_title.c"
    )
    target_link_libraries(umicom-studio-runtime-window-title-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-studio-runtime-window-title-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-studio-runtime-window-title-test)
    endif()
    add_test(NAME framework.studio_runtime.window-title COMMAND umicom-studio-runtime-window-title-test)


    # Link-closure regression: this test intentionally links only
    # Umicom::developer. The public Umicom::document dependency above must make
    # the document encoding/line-ending symbols resolvable on static MinGW/GNU
    # linkers.
    add_executable(
        umicom-studio-runtime-document-link-closure-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/studio_runtime/test_document_link_closure.c"
    )
    target_link_libraries(
        umicom-studio-runtime-document-link-closure-test
        PRIVATE
            Umicom::developer
    )
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(
            umicom-studio-runtime-document-link-closure-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(
            umicom-studio-runtime-document-link-closure-test)
    endif()
    add_test(
        NAME framework.studio_runtime.document-link-closure
        COMMAND umicom-studio-runtime-document-link-closure-test
    )
endif()

message(STATUS
    "Umicom final Framework-owned Studio runtime composition enabled")
