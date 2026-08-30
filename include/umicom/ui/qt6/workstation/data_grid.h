/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/data_grid.h
 *
 * PURPOSE:
 *   Render high-density semantic data-grid columns and row capacity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSTATION_DATA_GRID_H
#define UMICOM_UI_QT6_WORKSTATION_DATA_GRID_H

#include "umicom/ui/qt6/adapter.h"
#include "umicom/ui/workstation/data_grid_surface.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_ws_data_grid_descriptor(void);
UmiQt6WidgetHandle umi_qt6_ws_data_grid_create(const UmiWsDataGridSurface *grid);

#ifdef __cplusplus
}
#endif
#endif
