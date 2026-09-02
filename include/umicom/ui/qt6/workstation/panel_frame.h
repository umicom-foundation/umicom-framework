/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/panel_frame.h
 *
 * PURPOSE:
 *   Render Framework panel chrome around arbitrary Qt6 child content.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSTATION_PANEL_FRAME_H
#define UMICOM_UI_QT6_WORKSTATION_PANEL_FRAME_H

#include "umicom/ui/qt6/adapter.h"
#include "umicom/ui/workstation/panel_chrome.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 ws panel frame descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_ws_panel_frame_descriptor(void);
/**
 * Initialise qt6 ws panel frame from caller-provided values so later operations receive a
 * known state.
 */
UmiQt6WidgetHandle umi_qt6_ws_panel_frame_create(const UmiWsPanelChrome *chrome, UmiQt6WidgetHandle child);

#ifdef __cplusplus
}
#endif
#endif
