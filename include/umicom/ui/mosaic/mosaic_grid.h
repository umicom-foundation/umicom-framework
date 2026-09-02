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

#ifdef __cplusplus
}
#endif
#endif
