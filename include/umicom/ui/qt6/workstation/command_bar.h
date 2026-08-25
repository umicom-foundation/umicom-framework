/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/command_bar.h
 *
 * PURPOSE:
 *   Render and parse the unified command/search centre.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSTATION_COMMAND_BAR_H
#define UMICOM_UI_QT6_WORKSTATION_COMMAND_BAR_H

#include "umicom/ui/qt6/adapter.h"
#include "umicom/ui/workstation/command_bar.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_ws_command_bar_descriptor(void);
UmiQt6WidgetHandle umi_qt6_ws_command_bar_create(const char *placeholder);
UmiStatus umi_qt6_ws_command_bar_query(UmiQt6WidgetHandle entry, UmiWsCommandBarQuery *out_query);

#ifdef __cplusplus
}
#endif
#endif
