/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/mosaic_grid.h
 *
 * PURPOSE:
 *   Define toolkit-neutral mosaic grid contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_MOSAIC_GRID_H
#define UMICOM_UI_MOSAIC_MOSAIC_GRID_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic mosaic grid data shared with callers of this public contract.
 */
typedef struct UmiUiMosaicMosaicGrid {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char panel_id[UMI_UI_MOSAIC_ID_CAPACITY];
    UmiUiMosaicApplication application;
    uint16_t row;
    uint16_t column;
    uint16_t row_span;
    uint16_t column_span;
    bool active;
} UmiUiMosaicMosaicGrid;

/* Initializes one mixed-application mosaic record. */
void umi_ui_mosaic_mosaic_grid_init(UmiUiMosaicMosaicGrid *value);
/* Places an application panel into a logical mosaic coordinate. */
UmiStatus umi_ui_mosaic_mosaic_grid_place(UmiUiMosaicMosaicGrid *value, const char *id, const char *panel_id, UmiUiMosaicApplication application, uint16_t row, uint16_t column);
/* Validates identity, ownership and bounded span geometry. */
UmiStatus umi_ui_mosaic_mosaic_grid_validate(const UmiUiMosaicMosaicGrid *value);
/* Returns the number of logical cells covered by the record. */
uint32_t umi_ui_mosaic_mosaic_grid_area(const UmiUiMosaicMosaicGrid *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_mosaic_mosaic_grid_archive_encode(const UmiUiMosaicMosaicGrid *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_mosaic_mosaic_grid_archive_decode(const void *bytes, size_t byte_count,
    UmiUiMosaicMosaicGrid *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ui_mosaic_mosaic_grid_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiUiMosaicMosaicGrid *expected, UmiUiMosaicMosaicGrid *value);


#ifdef __cplusplus
}
#endif
#endif
