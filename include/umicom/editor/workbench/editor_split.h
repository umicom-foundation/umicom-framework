/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_split.h
 *
 * PURPOSE:
 *   Describe one horizontal or vertical editor split with a bounded ratio.
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
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_SPLIT_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_SPLIT_H

#include "umicom/editor/workbench/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb editor split data shared with callers of this public contract.
 */
typedef struct UmiEditorWbEditorSplit { char split_id[UMI_EDITOR_WB_ID_CAPACITY]; UmiEditorWbOrientation orientation; double ratio; } UmiEditorWbEditorSplit;
/**
 * Initialise editor wb editor split from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_editor_split_init(UmiEditorWbEditorSplit *split,const char *id,UmiEditorWbOrientation orientation,double ratio);
/**
 * Provide the editor wb editor split set ratio operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_wb_editor_split_set_ratio(UmiEditorWbEditorSplit *split,double ratio);
/**
 * Check that editor wb editor split satisfies its contract before another service relies
 * on it.
 */
int umi_editor_wb_editor_split_valid(const UmiEditorWbEditorSplit *split);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_wb_editor_split_archive_encode(const UmiEditorWbEditorSplit *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_wb_editor_split_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorWbEditorSplit *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiEditorWbEditorSplit object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_editor_wb_editor_split_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiEditorWbEditorSplit *workspace, UmiEditorWbEditorSplit *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_editor_wb_editor_split_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiEditorWbEditorSplit *expected, UmiEditorWbEditorSplit *value);


#ifdef __cplusplus
}
#endif
#endif
