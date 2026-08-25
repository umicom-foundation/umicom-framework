/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/data_grid_surface.h
 *
 * PURPOSE:
 *   Define semantic data-grid columns for market tables, databases, asset metadata, properties and operational lists.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_DATA_GRID_SURFACE_H
#define UMICOM_UI_WORKSTATION_DATA_GRID_SURFACE_H

#include <stdbool.h>
#include <stddef.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsGridColumn {
    char column_id[UMI_UI_ID_CAPACITY];
    char label[UMI_UI_TEXT_CAPACITY];
    int32_t width;
    bool sortable;
    bool resizable;
} UmiWsGridColumn;

typedef struct UmiWsDataGridSurface {
    char grid_id[UMI_UI_ID_CAPACITY];
    UmiWsGridColumn columns[UMI_WS_MAX_GRID_COLUMNS];
    size_t column_count;
    size_t row_count;
    bool striped;
    bool dense;
} UmiWsDataGridSurface;

UmiStatus umi_ws_data_grid_surface_init(UmiWsDataGridSurface *grid, const char *grid_id);
UmiStatus umi_ws_data_grid_surface_add_column(UmiWsDataGridSurface *grid,
                                              const char *column_id,
                                              const char *label,
                                              int32_t width);
void umi_ws_data_grid_surface_set_rows(UmiWsDataGridSurface *grid, size_t row_count);

#ifdef __cplusplus
}
#endif

#endif
