/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/data_grid.h
 *
 * PURPOSE:
 *   Render semantic data-grid column definitions for trading tables, databases, assets and operations lists.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_DATA_GRID_H
#define UMICOM_UI_GTK4_WORKSTATION_DATA_GRID_H

#include <gtk/gtk.h>
#include "umicom/ui/workstation/data_grid_surface.h"

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise gtk4 ws data grid from caller-provided values so later operations receive a
 * known state.
 */
GtkWidget *umi_gtk4_ws_data_grid_create(const UmiWsDataGridSurface *grid);


#ifdef __cplusplus
}
#endif
#endif
