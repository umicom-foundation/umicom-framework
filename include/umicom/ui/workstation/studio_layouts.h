/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/studio_layouts.h
 *
 * PURPOSE:
 *   Define Framework-owned Studio workbench presets for welcome, coding, debugging, design, diff/merge, testing, database, AI and focus workflows.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_STUDIO_LAYOUTS_H
#define UMICOM_UI_WORKSTATION_STUDIO_LAYOUTS_H

#include "umicom/ui/workstation/layout_template.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ws_studio_layout_welcome(UmiWsLayoutTemplate *layout);
UmiStatus umi_ws_studio_layout_code(UmiWsLayoutTemplate *layout);
UmiStatus umi_ws_studio_layout_debug(UmiWsLayoutTemplate *layout);
UmiStatus umi_ws_studio_layout_designer(UmiWsLayoutTemplate *layout);
UmiStatus umi_ws_studio_layout_diff_merge(UmiWsLayoutTemplate *layout);
UmiStatus umi_ws_studio_layout_testing(UmiWsLayoutTemplate *layout);
UmiStatus umi_ws_studio_layout_database(UmiWsLayoutTemplate *layout);
UmiStatus umi_ws_studio_layout_ai_development(UmiWsLayoutTemplate *layout);
UmiStatus umi_ws_studio_layout_ai_teacher(UmiWsLayoutTemplate *layout);
UmiStatus umi_ws_studio_layout_focus(UmiWsLayoutTemplate *layout);

#ifdef __cplusplus
}
#endif

#endif
