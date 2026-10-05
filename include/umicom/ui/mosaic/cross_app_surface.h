/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/cross_app_surface.h
 *
 * PURPOSE:
 *   Identify an application-owned surface without making the application own the docking implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_CROSS_APP_SURFACE_H
#define UMICOM_UI_MOSAIC_CROSS_APP_SURFACE_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic cross app surface data shared with callers of this public
 * contract.
 */
typedef struct UmiUiMosaicCrossAppSurface {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char panel_id[UMI_UI_MOSAIC_ID_CAPACITY];
    UmiUiMosaicApplication application;
    uint16_t row;
    uint16_t column;
    uint16_t row_span;
    uint16_t column_span;
    bool active;
} UmiUiMosaicCrossAppSurface;

/* Initializes one mixed-application mosaic record. */
void umi_ui_mosaic_cross_app_surface_init(UmiUiMosaicCrossAppSurface *value);
/* Places an application panel into a logical mosaic coordinate. */
UmiStatus umi_ui_mosaic_cross_app_surface_place(UmiUiMosaicCrossAppSurface *value, const char *id, const char *panel_id, UmiUiMosaicApplication application, uint16_t row, uint16_t column);
/* Validates identity, ownership and bounded span geometry. */
UmiStatus umi_ui_mosaic_cross_app_surface_validate(const UmiUiMosaicCrossAppSurface *value);
/* Returns the number of logical cells covered by the record. */
uint32_t umi_ui_mosaic_cross_app_surface_area(const UmiUiMosaicCrossAppSurface *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_mosaic_cross_app_surface_archive_encode(const UmiUiMosaicCrossAppSurface *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_mosaic_cross_app_surface_archive_decode(const void *bytes, size_t byte_count,
    UmiUiMosaicCrossAppSurface *value);

#ifdef __cplusplus
}
#endif
#endif
