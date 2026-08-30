/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/capability_builtin.c
 *
 * PURPOSE:
 *   Register the complete Framework document capability inventory in a stable order.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/capabilities.h"

typedef const UmiDocumentCapabilityDescriptor *(*DocumentCapabilityGetter)(void);

static const DocumentCapabilityGetter BUILTIN_CAPABILITIES[] = {
    umi_document_capability_untitled_document,
    umi_document_capability_open_document,
    umi_document_capability_close_document,
    umi_document_capability_reopen_document,
    umi_document_capability_clone_document,
    umi_document_capability_read_only_document,
    umi_document_capability_large_document,
    umi_document_capability_virtual_document,
    umi_document_capability_resource_uri,
    umi_document_capability_native_path,
    umi_document_capability_path_canonicalization,
    umi_document_capability_content_fingerprint,
    umi_document_capability_file_stat,
    umi_document_capability_display_name,
    umi_document_capability_language_identity,
    umi_document_capability_mime_identity,
    umi_document_capability_local_file_provider,
    umi_document_capability_memory_provider,
    umi_document_capability_remote_provider,
    umi_document_capability_archive_provider,
    umi_document_capability_ssh_provider,
    umi_document_capability_webdav_provider,
    umi_document_capability_cloud_provider,
    umi_document_capability_custom_provider,
    umi_document_capability_utf8_encoding,
    umi_document_capability_utf8_bom_encoding,
    umi_document_capability_utf16le_encoding,
    umi_document_capability_utf16be_encoding,
    umi_document_capability_encoding_validation,
    umi_document_capability_encoding_detection,
    umi_document_capability_encoding_conversion,
    umi_document_capability_bom_policy,
    umi_document_capability_atomic_save,
    umi_document_capability_save_as,
    umi_document_capability_save_participant,
    umi_document_capability_backup_copy,
    umi_document_capability_permission_preservation,
    umi_document_capability_line_ending_policy,
    umi_document_capability_final_newline_policy,
    umi_document_capability_trailing_whitespace_policy,
    umi_document_capability_autosave,
    umi_document_capability_recovery_journal,
    umi_document_capability_hot_exit,
    umi_document_capability_crash_recovery,
    umi_document_capability_backup_rotation,
    umi_document_capability_orphan_cleanup,
    umi_document_capability_session_restore,
    umi_document_capability_document_snapshot,
    umi_document_capability_external_change_detection,
    umi_document_capability_dirty_conflict,
    umi_document_capability_three_way_conflict,
    umi_document_capability_overwrite_resolution,
    umi_document_capability_reload_resolution,
    umi_document_capability_compare_resolution,
    umi_document_capability_file_lock,
    umi_document_capability_merge_resolution,
    umi_document_capability_new_command,
    umi_document_capability_open_command,
    umi_document_capability_save_command,
    umi_document_capability_save_as_command,
    umi_document_capability_close_command,
    umi_document_capability_close_all_command,
    umi_document_capability_revert_command,
    umi_document_capability_rename_command,
    umi_document_capability_find_command,
    umi_document_capability_replace_command,
    umi_document_capability_go_to_line_command,
    umi_document_capability_go_to_offset_command,
    umi_document_capability_next_match_command,
    umi_document_capability_previous_match_command,
    umi_document_capability_recent_documents,
    umi_document_capability_document_history,
    umi_document_capability_workspace_trust,
    umi_document_capability_symlink_policy,
    umi_document_capability_path_traversal_policy,
    umi_document_capability_file_size_limit,
    umi_document_capability_binary_file_policy,
    umi_document_capability_write_permission_policy,
    umi_document_capability_document_audit,
    umi_document_capability_sensitive_content_redaction,
    umi_document_capability_memory_mapping,
    umi_document_capability_streaming_loader,
    umi_document_capability_incremental_fingerprint,
    umi_document_capability_asynchronous_load,
    umi_document_capability_asynchronous_save,
    umi_document_capability_content_cache,
    umi_document_capability_chunked_transport,
    umi_document_capability_io_cancellation,
    umi_document_capability_ui_document_model,
    umi_document_capability_headless_document_host,
    umi_document_capability_gtk4_document_host,
    umi_document_capability_lsp_document_sync,
    umi_document_capability_vcs_diff_bridge,
    umi_document_capability_document_test_fixture,
    umi_document_capability_provider_extension_point,
    umi_document_capability_document_telemetry,
};

UmiStatus umi_document_capability_catalog_register_builtins(
    UmiDocumentCapabilityCatalog *catalog)
{
    size_t index;
    UmiStatus status;
    if (catalog == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < sizeof(BUILTIN_CAPABILITIES) / sizeof(BUILTIN_CAPABILITIES[0]); ++index) {
        status = umi_document_capability_catalog_upsert(catalog, BUILTIN_CAPABILITIES[index]());
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}
