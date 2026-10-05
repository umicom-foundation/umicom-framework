/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/mosaic_activation.h
 *
 * PURPOSE:
 *   Define toolkit-neutral mosaic activation contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_MOSAIC_ACTIVATION_H
#define UMICOM_UI_MOSAIC_MOSAIC_ACTIVATION_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic mosaic activation data shared with callers of this public
 * contract.
 */
typedef struct UmiUiMosaicMosaicActivation {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char panel_id[UMI_UI_MOSAIC_ID_CAPACITY];
    UmiUiMosaicApplication application;
    uint16_t row;
    uint16_t column;
    uint16_t row_span;
    uint16_t column_span;
    bool active;
} UmiUiMosaicMosaicActivation;

/* Initializes one mixed-application mosaic record. */
void umi_ui_mosaic_mosaic_activation_init(UmiUiMosaicMosaicActivation *value);
/* Places an application panel into a logical mosaic coordinate. */
UmiStatus umi_ui_mosaic_mosaic_activation_place(UmiUiMosaicMosaicActivation *value, const char *id, const char *panel_id, UmiUiMosaicApplication application, uint16_t row, uint16_t column);
/* Validates identity, ownership and bounded span geometry. */
UmiStatus umi_ui_mosaic_mosaic_activation_validate(const UmiUiMosaicMosaicActivation *value);
/* Returns the number of logical cells covered by the record. */
uint32_t umi_ui_mosaic_mosaic_activation_area(const UmiUiMosaicMosaicActivation *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_mosaic_mosaic_activation_archive_encode(const UmiUiMosaicMosaicActivation *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_mosaic_mosaic_activation_archive_decode(const void *bytes, size_t byte_count,
    UmiUiMosaicMosaicActivation *value);

#ifdef __cplusplus
}
#endif
#endif
