/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/data_grid_surface.c
 *
 * PURPOSE:
 *   Implement semantic data-grid columns for market tables, databases, asset metadata, properties and operational lists.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/data_grid_surface.h"

UmiStatus umi_ws_data_grid_surface_init(UmiWsDataGridSurface *grid, const char *grid_id) {
    if (grid == NULL || !umi_ws_id_valid(grid_id)) return UMI_STATUS_INVALID_ARGUMENT;
    *grid = (UmiWsDataGridSurface){0};
    grid->striped = true;
    return umi_ws_copy_text(grid->grid_id, sizeof(grid->grid_id), grid_id);
}

UmiStatus umi_ws_data_grid_surface_add_column(UmiWsDataGridSurface *grid,
                                              const char *column_id,
                                              const char *label,
                                              int32_t width) {
    UmiWsGridColumn *column;
    if (grid == NULL || !umi_ws_id_valid(column_id) || label == NULL || width <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (grid->column_count >= UMI_WS_MAX_GRID_COLUMNS) return UMI_STATUS_CAPACITY_EXCEEDED;
    column = &grid->columns[grid->column_count++];
    *column = (UmiWsGridColumn){0};
    if (umi_ws_copy_text(column->column_id, sizeof(column->column_id), column_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_ws_copy_text(column->label, sizeof(column->label), label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    column->width = width;
    column->sortable = true;
    column->resizable = true;
    return UMI_STATUS_OK;
}

void umi_ws_data_grid_surface_set_rows(UmiWsDataGridSurface *grid, size_t row_count) {
    if (grid != NULL) grid->row_count = row_count;
}
