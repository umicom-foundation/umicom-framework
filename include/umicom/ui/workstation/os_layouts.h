/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/os_layouts.h
 *
 * PURPOSE:
 *   Define Umicom OS desktop, systems-development and system-monitor workstation layout presets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_OS_LAYOUTS_H
#define UMICOM_UI_WORKSTATION_OS_LAYOUTS_H

#include "umicom/ui/workstation/layout_template.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ws os layout desktop operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_os_layout_desktop(UmiWsLayoutTemplate *layout);
/**
 * Provide the ws os layout systems operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_os_layout_systems(UmiWsLayoutTemplate *layout);
/**
 * Provide the ws os layout monitor operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_os_layout_monitor(UmiWsLayoutTemplate *layout);

#ifdef __cplusplus
}
#endif

#endif
