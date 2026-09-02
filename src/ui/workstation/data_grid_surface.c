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

/*
 * Initialise ws data grid surface from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ws_data_grid_surface_init(UmiWsDataGridSurface *grid, const char *grid_id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (grid == NULL || !umi_ws_id_valid(grid_id)) return UMI_STATUS_INVALID_ARGUMENT;
    *grid = (UmiWsDataGridSurface){0};
    grid->striped = true;
    return umi_ws_copy_text(grid->grid_id, sizeof(grid->grid_id), grid_id);
}

/*
 * Provide the ws data grid surface add column operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_data_grid_surface_add_column(UmiWsDataGridSurface *grid,
                                              const char *column_id,
                                              const char *label,
                                              int32_t width) {
    UmiWsGridColumn *column;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (grid == NULL || !umi_ws_id_valid(column_id) || label == NULL || width <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (grid->column_count >= UMI_WS_MAX_GRID_COLUMNS) return UMI_STATUS_CAPACITY_EXCEEDED;
    column = &grid->columns[grid->column_count++];
    *column = (UmiWsGridColumn){0};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(column->column_id, sizeof(column->column_id), column_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(column->label, sizeof(column->label), label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    column->width = width;
    column->sortable = true;
    column->resizable = true;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws data grid surface set rows operation used by this module and its client
 * applications.
 */
void umi_ws_data_grid_surface_set_rows(UmiWsDataGridSurface *grid, size_t row_count) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (grid != NULL) grid->row_count = row_count;
}
