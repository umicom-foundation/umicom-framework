/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/tool_rail.h
 *
 * PURPOSE:
 *   Render reusable activity/tool rails for professional workstation applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSTATION_TOOL_RAIL_H
#define UMICOM_UI_QT6_WORKSTATION_TOOL_RAIL_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 ws tool rail descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_ws_tool_rail_descriptor(void);
/**
 * Initialise qt6 ws tool rail from caller-provided values so later operations receive a
 * known state.
 */
UmiQt6WidgetHandle umi_qt6_ws_tool_rail_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
