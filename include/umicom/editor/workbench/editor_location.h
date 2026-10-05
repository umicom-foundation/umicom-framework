/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_location.h
 *
 * PURPOSE:
 *   Represent a validated file, line and column editor location.
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
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_LOCATION_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_LOCATION_H

#include "umicom/editor/workbench/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb editor location data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbEditorLocation { char path[UMI_EDITOR_WB_PATH_CAPACITY]; UmiEditorWbPosition position; } UmiEditorWbEditorLocation;
/**
 * Initialise editor wb editor location from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_editor_location_init(UmiEditorWbEditorLocation *location,const char *path,uint32_t line,uint32_t column);
/**
 * Check that editor wb editor location satisfies its contract before another service
 * relies on it.
 */
int umi_editor_wb_editor_location_valid(const UmiEditorWbEditorLocation *location);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_wb_editor_location_archive_encode(const UmiEditorWbEditorLocation *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_wb_editor_location_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorWbEditorLocation *value);

#ifdef __cplusplus
}
#endif
#endif
