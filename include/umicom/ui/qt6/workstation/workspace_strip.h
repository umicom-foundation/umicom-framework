/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/workspace_strip.h
 *
 * PURPOSE:
 *   Render saved workspace/layout switching from the canonical workspace-strip model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSTATION_WORKSPACE_STRIP_H
#define UMICOM_UI_QT6_WORKSTATION_WORKSPACE_STRIP_H

#include "umicom/ui/qt6/adapter.h"
#include "umicom/ui/workstation/workspace_strip.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 ws workspace strip descriptor operation used by this module and its
 * client applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_ws_workspace_strip_descriptor(void);
/**
 * Initialise qt6 ws workspace strip from caller-provided values so later operations
 * receive a known state.
 */
UmiQt6WidgetHandle umi_qt6_ws_workspace_strip_create(const UmiWsWorkspaceStrip *strip);

#ifdef __cplusplus
}
#endif
#endif
