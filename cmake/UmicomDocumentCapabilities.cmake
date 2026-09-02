#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomDocumentCapabilities.cmake
#
# PURPOSE:
#   Configure the umicom document capabilities build rules without duplicating
#   product logic.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Umicom Framework document capability source inventory.
# Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
#-----------------------------------------------------------------------------
set(UMICOM_DOCUMENT_CAPABILITY_SOURCES
    src/document/capabilities/untitled_document.c
    src/document/capabilities/open_document.c
    src/document/capabilities/close_document.c
    src/document/capabilities/reopen_document.c
    src/document/capabilities/clone_document.c
    src/document/capabilities/read_only_document.c
    src/document/capabilities/large_document.c
    src/document/capabilities/virtual_document.c
    src/document/capabilities/resource_uri.c
    src/document/capabilities/native_path.c
    src/document/capabilities/path_canonicalization.c
    src/document/capabilities/content_fingerprint.c
    src/document/capabilities/file_stat.c
    src/document/capabilities/display_name.c
    src/document/capabilities/language_identity.c
    src/document/capabilities/mime_identity.c
    src/document/capabilities/local_file_provider.c
    src/document/capabilities/memory_provider.c
    src/document/capabilities/remote_provider.c
    src/document/capabilities/archive_provider.c
    src/document/capabilities/ssh_provider.c
    src/document/capabilities/webdav_provider.c
    src/document/capabilities/cloud_provider.c
    src/document/capabilities/custom_provider.c
    src/document/capabilities/utf8_encoding.c
    src/document/capabilities/utf8_bom_encoding.c
    src/document/capabilities/utf16le_encoding.c
    src/document/capabilities/utf16be_encoding.c
    src/document/capabilities/encoding_validation.c
    src/document/capabilities/encoding_detection.c
    src/document/capabilities/encoding_conversion.c
    src/document/capabilities/bom_policy.c
    src/document/capabilities/atomic_save.c
    src/document/capabilities/save_as.c
    src/document/capabilities/save_participant.c
    src/document/capabilities/backup_copy.c
    src/document/capabilities/permission_preservation.c
    src/document/capabilities/line_ending_policy.c
    src/document/capabilities/final_newline_policy.c
    src/document/capabilities/trailing_whitespace_policy.c
    src/document/capabilities/autosave.c
    src/document/capabilities/recovery_journal.c
    src/document/capabilities/hot_exit.c
    src/document/capabilities/crash_recovery.c
    src/document/capabilities/backup_rotation.c
    src/document/capabilities/orphan_cleanup.c
    src/document/capabilities/session_restore.c
    src/document/capabilities/document_snapshot.c
    src/document/capabilities/external_change_detection.c
    src/document/capabilities/dirty_conflict.c
    src/document/capabilities/three_way_conflict.c
    src/document/capabilities/overwrite_resolution.c
    src/document/capabilities/reload_resolution.c
    src/document/capabilities/compare_resolution.c
    src/document/capabilities/file_lock.c
    src/document/capabilities/merge_resolution.c
    src/document/capabilities/new_command.c
    src/document/capabilities/open_command.c
    src/document/capabilities/save_command.c
    src/document/capabilities/save_as_command.c
    src/document/capabilities/close_command.c
    src/document/capabilities/close_all_command.c
    src/document/capabilities/revert_command.c
    src/document/capabilities/rename_command.c
    src/document/capabilities/find_command.c
    src/document/capabilities/replace_command.c
    src/document/capabilities/go_to_line_command.c
    src/document/capabilities/go_to_offset_command.c
    src/document/capabilities/next_match_command.c
    src/document/capabilities/previous_match_command.c
    src/document/capabilities/recent_documents.c
    src/document/capabilities/document_history.c
    src/document/capabilities/workspace_trust.c
    src/document/capabilities/symlink_policy.c
    src/document/capabilities/path_traversal_policy.c
    src/document/capabilities/file_size_limit.c
    src/document/capabilities/binary_file_policy.c
    src/document/capabilities/write_permission_policy.c
    src/document/capabilities/document_audit.c
    src/document/capabilities/sensitive_content_redaction.c
    src/document/capabilities/memory_mapping.c
    src/document/capabilities/streaming_loader.c
    src/document/capabilities/incremental_fingerprint.c
    src/document/capabilities/asynchronous_load.c
    src/document/capabilities/asynchronous_save.c
    src/document/capabilities/content_cache.c
    src/document/capabilities/chunked_transport.c
    src/document/capabilities/io_cancellation.c
    src/document/capabilities/ui_document_model.c
    src/document/capabilities/headless_document_host.c
    src/document/capabilities/gtk4_document_host.c
    src/document/capabilities/lsp_document_sync.c
    src/document/capabilities/vcs_diff_bridge.c
    src/document/capabilities/document_test_fixture.c
    src/document/capabilities/provider_extension_point.c
    src/document/capabilities/document_telemetry.c
)
