/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/workspace_strip.h
 *
 * PURPOSE:
 *   Render saved workspace/layout switching from the canonical workspace-strip model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSTATION_WORKSPACE_STRIP_H
#define UMICOM_UI_QT6_WORKSTATION_WORKSPACE_STRIP_H

#include "umicom/ui/qt6/adapter.h"
#include "umicom/ui/workstation/workspace_strip.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_ws_workspace_strip_descriptor(void);
UmiQt6WidgetHandle umi_qt6_ws_workspace_strip_create(const UmiWsWorkspaceStrip *strip);

#ifdef __cplusplus
}
#endif
#endif
