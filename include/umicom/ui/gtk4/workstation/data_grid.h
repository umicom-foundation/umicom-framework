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

GtkWidget *umi_gtk4_ws_data_grid_create(const UmiWsDataGridSurface *grid);

#endif
