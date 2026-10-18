/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_reopen.h
 *
 * PURPOSE:
 *   Build a reopen request from recently-closed editor evidence.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_REOPEN_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_REOPEN_H

#include "umicom/editor/workbench/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb editor reopen data shared with callers of this public contract.
 */
typedef struct UmiEditorWbEditorReopen { char resource[UMI_EDITOR_WB_PATH_CAPACITY]; char group_id[UMI_EDITOR_WB_ID_CAPACITY]; UmiEditorWbOpenMode mode; bool force; } UmiEditorWbEditorReopen;
/**
 * Initialise editor wb editor reopen from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_editor_reopen_init(UmiEditorWbEditorReopen *state,const char *resource,const char *group_id);
/**
 * Check that editor wb editor reopen satisfies its contract before another service relies
 * on it.
 */
int umi_editor_wb_editor_reopen_valid(const UmiEditorWbEditorReopen *state);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_wb_editor_reopen_archive_encode(const UmiEditorWbEditorReopen *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_wb_editor_reopen_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorWbEditorReopen *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiEditorWbEditorReopen object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_editor_wb_editor_reopen_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiEditorWbEditorReopen *workspace, UmiEditorWbEditorReopen *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_editor_wb_editor_reopen_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiEditorWbEditorReopen *expected, UmiEditorWbEditorReopen *value);


#ifdef __cplusplus
}
#endif
#endif
