/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/dock_overlay.h
 *
 * PURPOSE:
 *   Render docking targets and drop previews without modifying the Framework layout graph directly.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSTATION_DOCK_OVERLAY_H
#define UMICOM_UI_QT6_WORKSTATION_DOCK_OVERLAY_H

#include "umicom/ui/qt6/adapter.h"
#include "umicom/ui/workstation/dock_preview.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 ws dock overlay descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_ws_dock_overlay_descriptor(void);
/**
 * Initialise qt6 ws dock overlay from caller-provided values so later operations receive a
 * known state.
 */
UmiQt6WidgetHandle umi_qt6_ws_dock_overlay_create(UmiQt6WidgetHandle content);
/**
 * Provide the qt6 ws dock overlay set preview operation used by this module and its client
 * applications.
 */
UmiStatus umi_qt6_ws_dock_overlay_set_preview(UmiQt6WidgetHandle overlay, const UmiWsDockPreview *preview, const char *label);

#ifdef __cplusplus
}
#endif
#endif
