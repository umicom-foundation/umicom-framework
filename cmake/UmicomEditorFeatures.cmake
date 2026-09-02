#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomEditorFeatures.cmake
#
# PURPOSE:
#   Configure the umicom editor features build rules without duplicating product
#   logic.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Umicom Framework editor source inventory.
# Generated feature implementations and Framework-owned editor platforms are
# assembled here before the canonical umicom_editor target is created.
# Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
#-----------------------------------------------------------------------------
set(UMICOM_EDITOR_FEATURE_SOURCES
    src/editor/features/text_buffer.c
    src/editor/features/line_index.c
    src/editor/features/text_position.c
    src/editor/features/text_range.c
    src/editor/features/text_snapshot.c
    src/editor/features/document_identity.c
    src/editor/features/document_metadata.c
    src/editor/features/document_lifecycle.c
    src/editor/features/document_loader.c
    src/editor/features/document_saver.c
    src/editor/features/document_autosave.c
    src/editor/features/document_recovery.c
    src/editor/features/document_encoding.c
    src/editor/features/line_endings.c
    src/editor/features/large_file_mode.c
    src/editor/features/read_only_regions.c
    src/editor/features/virtual_document.c
    src/editor/features/document_conflict.c
    src/editor/features/edit_operation.c
    src/editor/features/edit_transaction.c
    src/editor/features/edit_history.c
    src/editor/features/multi_cursor.c
    src/editor/features/multi_selection.c
    src/editor/features/clipboard.c
    src/editor/features/indentation.c
    src/editor/features/comment_toggle.c
    src/editor/features/bracket_matching.c
    src/editor/features/auto_closing_pairs.c
    src/editor/features/smart_indent.c
    src/editor/features/word_operations.c
    src/editor/features/line_operations.c
    src/editor/features/case_transform.c
    src/editor/features/sort_lines.c
    src/editor/features/snippet.c
    src/editor/features/format_on_type.c
    src/editor/features/literal_search.c
    src/editor/features/replace.c
    src/editor/features/regex_search.c
    src/editor/features/incremental_search.c
    src/editor/features/go_to_line.c
    src/editor/features/go_to_symbol.c
    src/editor/features/go_to_definition.c
    src/editor/features/go_to_reference.c
    src/editor/features/navigation_history.c
    src/editor/features/breadcrumb_navigation.c
    src/editor/features/quick_open.c
    src/editor/features/command_palette.c
    src/editor/features/peek_view.c
    src/editor/features/link_navigation.c
    src/editor/features/editor_viewport.c
    src/editor/features/editor_gutter.c
    src/editor/features/line_numbers.c
    src/editor/features/minimap.c
    src/editor/features/overview_ruler.c
    src/editor/features/word_wrap.c
    src/editor/features/whitespace_rendering.c
    src/editor/features/current_line.c
    src/editor/features/indent_guides.c
    src/editor/features/bracket_guides.c
    src/editor/features/code_folding.c
    src/editor/features/sticky_scroll.c
    src/editor/features/editor_decoration.c
    src/editor/features/semantic_highlighting.c
    src/editor/features/syntax_highlighting.c
    src/editor/features/inlay_hints.c
    src/editor/features/code_lens.c
    src/editor/features/editor_theme.c
    src/editor/features/editor_tab.c
    src/editor/features/editor_group.c
    src/editor/features/editor_split.c
    src/editor/features/preview_editor.c
    src/editor/features/pinned_editor.c
    src/editor/features/recent_editors.c
    src/editor/features/workspace_trust.c
    src/editor/features/workspace_state.c
    src/editor/features/workspace_session.c
    src/editor/features/file_watcher_sync.c
    src/editor/features/file_compare.c
    src/editor/features/three_way_merge.c
    src/editor/features/diff_editor.c
    src/editor/features/save_participants.c
    src/editor/features/working_copy.c
    src/editor/features/hot_exit.c
    src/editor/features/language_binding.c
    src/editor/features/completion_session.c
    src/editor/features/hover.c
    src/editor/features/signature_help.c
    src/editor/features/diagnostic_collection.c
    src/editor/features/quick_fix.c
    src/editor/features/rename.c
    src/editor/features/document_format.c
    src/editor/features/document_symbols.c
    src/editor/features/workspace_symbols.c
    src/editor/features/semantic_tokens.c
    src/editor/features/call_hierarchy.c
    src/editor/features/type_hierarchy.c
    src/editor/features/inline_completion.c
    src/editor/features/linked_editing.c
    src/editor/features/language_status.c
    src/editor/features/problems_navigation.c
    src/editor/features/source_control_decorations.c
    src/editor/features/test_code_lens.c
    src/editor/features/coverage_gutter.c
    src/editor/features/breakpoint_gutter.c
    src/editor/features/inline_debug_values.c
    src/editor/features/task_diagnostics.c
    src/editor/features/code_ownership.c
    src/editor/features/performance_guard.c
    src/editor/features/accessibility.c
    src/editor/features/extension_points.c
    src/editor/features/editor_commands.c
    src/editor/features/editor_context_keys.c
    src/editor/features/editor_keybindings.c
    src/editor/features/remote_document.c
    src/editor/features/collaborative_cursor.c
    src/editor/features/collaborative_edit.c
    src/editor/features/ai_edit_proposal.c
    src/editor/features/audit_trail.c
    src/editor/features/policy_gate.c

    # Workspace search and replacement platform.
    src/editor/workspace_search_pattern.c
    src/editor/workspace_search_exclusion.c
    src/editor/workspace_search_index.c
    src/editor/workspace_search_match.c
    src/editor/workspace_search_query.c
    src/editor/workspace_search_preview.c
    src/editor/workspace_replace_plan.c
    src/editor/workspace_replace_transaction.c
    src/editor/workspace_search_orchestration.c
    src/editor/workspace_search_command.c

    # Completion and inline suggestion platform.
    src/editor/completion_candidate.c
    src/editor/completion_provider.c
    src/editor/completion_query.c
    src/editor/completion_session.c
    src/editor/completion_orchestration.c
    src/editor/inline_suggestion.c
    src/editor/inline_suggestion_session.c
    src/editor/completion_command.c

    # Professional navigation, peek and hierarchy experience.
    src/editor/navigation_provider.c
    src/editor/navigation_query_session.c
    src/editor/navigation_source_preview.c
    src/editor/peek_navigation_session.c
    src/editor/reference_navigation_session.c
    src/editor/hierarchy_navigation_session.c
    src/editor/symbol_navigation_session.c
    src/editor/navigation_experience.c
)

# -----------------------------------------------------------------------------
# Professional diagnostics, Problems and SARIF platform.
#
# This inventory is loaded after the foundational diagnostics target exists.
# Registering the vertical slice here preserves the current modular build
# arrangement without adding another top-level build script or duplicating the
# authoritative target definition.
# -----------------------------------------------------------------------------
if(NOT TARGET umicom_diagnostics)
    message(FATAL_ERROR
        "Professional diagnostics sources require the umicom_diagnostics target")
endif()

target_sources(umicom_diagnostics PRIVATE
    src/diagnostics/diagnostic_provider.c
    src/diagnostics/severity_policy.c
    src/diagnostics/suppression.c
    src/diagnostics/baseline.c
    src/diagnostics/problem_model.c
    src/diagnostics/sarif.c
    src/diagnostics/orchestration.c
    src/diagnostics/command.c
)


# -----------------------------------------------------------------------------
# Advanced debugging platform.
#
# This inventory file is included before the canonical umicom_debug target is
# declared.  Defer the additive source registration until the directory's main
# CMakeLists has created that target.  This keeps the existing root build file
# authoritative and avoids adding another build script or replacing any target.
# -----------------------------------------------------------------------------
function(umicom_register_advanced_debugging_sources)
    # Load the dependency only when the parent build has not already provided its target.
    if(NOT TARGET umicom_debug)
        message(FATAL_ERROR
            "Advanced debugging sources require the umicom_debug target")
    endif()

    target_sources(umicom_debug PRIVATE
        src/debug/advanced_breakpoint.c
        src/debug/adapter_contract.c
        src/debug/thread_inspector.c
        src/debug/register_bank.c
        src/debug/memory_view.c
        src/debug/disassembly_view.c
        src/debug/inspection_session.c
        src/debug/advanced_debugging.c
    )
endfunction()

cmake_language(DEFER CALL umicom_register_advanced_debugging_sources)
