/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/command_bar.h
 *
 * PURPOSE:
 *   Render and parse the unified command/search centre.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSTATION_COMMAND_BAR_H
#define UMICOM_UI_QT6_WORKSTATION_COMMAND_BAR_H

#include "umicom/ui/qt6/adapter.h"
#include "umicom/ui/workstation/command_bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 ws command bar descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_ws_command_bar_descriptor(void);
/**
 * Initialise qt6 ws command bar from caller-provided values so later operations receive a
 * known state.
 */
UmiQt6WidgetHandle umi_qt6_ws_command_bar_create(const char *placeholder);
/**
 * Provide the qt6 ws command bar query operation used by this module and its client
 * applications.
 */
UmiStatus umi_qt6_ws_command_bar_query(UmiQt6WidgetHandle entry, UmiWsCommandBarQuery *out_query);

#ifdef __cplusplus
}
#endif
#endif
