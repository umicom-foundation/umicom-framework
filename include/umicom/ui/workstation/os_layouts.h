/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/os_layouts.h
 *
 * PURPOSE:
 *   Define Umicom OS desktop, systems-development and system-monitor workstation layout presets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_OS_LAYOUTS_H
#define UMICOM_UI_WORKSTATION_OS_LAYOUTS_H

#include "umicom/ui/workstation/layout_template.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ws_os_layout_desktop(UmiWsLayoutTemplate *layout);
UmiStatus umi_ws_os_layout_systems(UmiWsLayoutTemplate *layout);
UmiStatus umi_ws_os_layout_monitor(UmiWsLayoutTemplate *layout);

#ifdef __cplusplus
}
#endif

#endif
