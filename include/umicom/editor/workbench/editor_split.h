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

#ifdef __cplusplus
}
#endif
#endif
