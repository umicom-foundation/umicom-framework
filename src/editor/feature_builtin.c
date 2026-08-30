/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/feature_builtin.c
 *
 * PURPOSE:
 *   Register the complete built-in editor feature inventory in deterministic
 *   priority order through one product-neutral composition boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/features.h"

typedef const UmiEditorFeatureDescriptor *(*FeatureGetter)(void);

static const FeatureGetter BUILTIN_FEATURES[] = {
    umi_editor_feature_text_buffer,
    umi_editor_feature_line_index,
    umi_editor_feature_text_position,
    umi_editor_feature_text_range,
    umi_editor_feature_text_snapshot,
    umi_editor_feature_document_identity,
    umi_editor_feature_document_metadata,
    umi_editor_feature_document_lifecycle,
    umi_editor_feature_document_loader,
    umi_editor_feature_document_saver,
    umi_editor_feature_document_autosave,
    umi_editor_feature_document_recovery,
    umi_editor_feature_document_encoding,
    umi_editor_feature_line_endings,
    umi_editor_feature_large_file_mode,
    umi_editor_feature_read_only_regions,
    umi_editor_feature_virtual_document,
    umi_editor_feature_document_conflict,
    umi_editor_feature_edit_operation,
    umi_editor_feature_edit_transaction,
    umi_editor_feature_edit_history,
    umi_editor_feature_multi_cursor,
    umi_editor_feature_multi_selection,
    umi_editor_feature_clipboard,
    umi_editor_feature_indentation,
    umi_editor_feature_comment_toggle,
    umi_editor_feature_bracket_matching,
    umi_editor_feature_auto_closing_pairs,
    umi_editor_feature_smart_indent,
    umi_editor_feature_word_operations,
    umi_editor_feature_line_operations,
    umi_editor_feature_case_transform,
    umi_editor_feature_sort_lines,
    umi_editor_feature_snippet,
    umi_editor_feature_format_on_type,
    umi_editor_feature_literal_search,
    umi_editor_feature_replace,
    umi_editor_feature_regex_search,
    umi_editor_feature_incremental_search,
    umi_editor_feature_go_to_line,
    umi_editor_feature_go_to_symbol,
    umi_editor_feature_go_to_definition,
    umi_editor_feature_go_to_reference,
    umi_editor_feature_navigation_history,
    umi_editor_feature_breadcrumb_navigation,
    umi_editor_feature_quick_open,
    umi_editor_feature_command_palette,
    umi_editor_feature_peek_view,
    umi_editor_feature_link_navigation,
    umi_editor_feature_editor_viewport,
    umi_editor_feature_editor_gutter,
    umi_editor_feature_line_numbers,
    umi_editor_feature_minimap,
    umi_editor_feature_overview_ruler,
    umi_editor_feature_word_wrap,
    umi_editor_feature_whitespace_rendering,
    umi_editor_feature_current_line,
    umi_editor_feature_indent_guides,
    umi_editor_feature_bracket_guides,
    umi_editor_feature_code_folding,
    umi_editor_feature_sticky_scroll,
    umi_editor_feature_editor_decoration,
    umi_editor_feature_semantic_highlighting,
    umi_editor_feature_syntax_highlighting,
    umi_editor_feature_inlay_hints,
    umi_editor_feature_code_lens,
    umi_editor_feature_editor_theme,
    umi_editor_feature_editor_tab,
    umi_editor_feature_editor_group,
    umi_editor_feature_editor_split,
    umi_editor_feature_preview_editor,
    umi_editor_feature_pinned_editor,
    umi_editor_feature_recent_editors,
    umi_editor_feature_workspace_trust,
    umi_editor_feature_workspace_state,
    umi_editor_feature_workspace_session,
    umi_editor_feature_file_watcher_sync,
    umi_editor_feature_file_compare,
    umi_editor_feature_three_way_merge,
    umi_editor_feature_diff_editor,
    umi_editor_feature_save_participants,
    umi_editor_feature_working_copy,
    umi_editor_feature_hot_exit,
    umi_editor_feature_language_binding,
    umi_editor_feature_completion_session,
    umi_editor_feature_hover,
    umi_editor_feature_signature_help,
    umi_editor_feature_diagnostic_collection,
    umi_editor_feature_quick_fix,
    umi_editor_feature_rename,
    umi_editor_feature_document_format,
    umi_editor_feature_document_symbols,
    umi_editor_feature_workspace_symbols,
    umi_editor_feature_semantic_tokens,
    umi_editor_feature_call_hierarchy,
    umi_editor_feature_type_hierarchy,
    umi_editor_feature_inline_completion,
    umi_editor_feature_linked_editing,
    umi_editor_feature_language_status,
    umi_editor_feature_problems_navigation,
    umi_editor_feature_source_control_decorations,
    umi_editor_feature_test_code_lens,
    umi_editor_feature_coverage_gutter,
    umi_editor_feature_breakpoint_gutter,
    umi_editor_feature_inline_debug_values,
    umi_editor_feature_task_diagnostics,
    umi_editor_feature_code_ownership,
    umi_editor_feature_performance_guard,
    umi_editor_feature_accessibility,
    umi_editor_feature_extension_points,
    umi_editor_feature_editor_commands,
    umi_editor_feature_editor_context_keys,
    umi_editor_feature_editor_keybindings,
    umi_editor_feature_remote_document,
    umi_editor_feature_collaborative_cursor,
    umi_editor_feature_collaborative_edit,
    umi_editor_feature_ai_edit_proposal,
    umi_editor_feature_audit_trail,
    umi_editor_feature_policy_gate,
};

UmiStatus umi_editor_feature_catalog_register_builtins(
    UmiEditorFeatureCatalog *catalog)
{
    size_t index;

    if (catalog == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U;
         index < sizeof(BUILTIN_FEATURES) / sizeof(BUILTIN_FEATURES[0]);
         ++index) {
        UmiStatus status = umi_editor_feature_catalog_upsert(
            catalog, BUILTIN_FEATURES[index]());
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}
