/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/mosaic_workspace.h
 *
 * PURPOSE:
 *   Describe a mixed-application workspace whose cells can host Studio, Trader, TMS, LLM and other panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_MOSAIC_WORKSPACE_H
#define UMICOM_UI_MOSAIC_MOSAIC_WORKSPACE_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic mosaic workspace data shared with callers of this public
 * contract.
 */
typedef struct UmiUiMosaicMosaicWorkspace {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char panel_id[UMI_UI_MOSAIC_ID_CAPACITY];
    UmiUiMosaicApplication application;
    uint16_t row;
    uint16_t column;
    uint16_t row_span;
    uint16_t column_span;
    bool active;
} UmiUiMosaicMosaicWorkspace;

/* Initializes one mixed-application mosaic record. */
void umi_ui_mosaic_mosaic_workspace_init(UmiUiMosaicMosaicWorkspace *value);
/* Places an application panel into a logical mosaic coordinate. */
UmiStatus umi_ui_mosaic_mosaic_workspace_place(UmiUiMosaicMosaicWorkspace *value, const char *id, const char *panel_id, UmiUiMosaicApplication application, uint16_t row, uint16_t column);
/* Validates identity, ownership and bounded span geometry. */
UmiStatus umi_ui_mosaic_mosaic_workspace_validate(const UmiUiMosaicMosaicWorkspace *value);
/* Returns the number of logical cells covered by the record. */
uint32_t umi_ui_mosaic_mosaic_workspace_area(const UmiUiMosaicMosaicWorkspace *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_mosaic_mosaic_workspace_archive_encode(const UmiUiMosaicMosaicWorkspace *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_mosaic_mosaic_workspace_archive_decode(const void *bytes, size_t byte_count,
    UmiUiMosaicMosaicWorkspace *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiUiMosaicMosaicWorkspace object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_ui_mosaic_mosaic_workspace_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiUiMosaicMosaicWorkspace *workspace, UmiUiMosaicMosaicWorkspace *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ui_mosaic_mosaic_workspace_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiUiMosaicMosaicWorkspace *expected, UmiUiMosaicMosaicWorkspace *value);


#ifdef __cplusplus
}
#endif
#endif
