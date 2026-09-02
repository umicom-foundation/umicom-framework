/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/studio_layouts.h
 *
 * PURPOSE:
 *   Define Framework-owned Studio workbench presets for welcome, coding, debugging, design, diff/merge, testing, database, AI and focus workflows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_STUDIO_LAYOUTS_H
#define UMICOM_UI_WORKSTATION_STUDIO_LAYOUTS_H

#include "umicom/ui/workstation/layout_template.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ws studio layout welcome operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_studio_layout_welcome(UmiWsLayoutTemplate *layout);
/**
 * Provide the ws studio layout code operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_studio_layout_code(UmiWsLayoutTemplate *layout);
/**
 * Provide the ws studio layout debug operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_studio_layout_debug(UmiWsLayoutTemplate *layout);
/**
 * Provide the ws studio layout designer operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_studio_layout_designer(UmiWsLayoutTemplate *layout);
/**
 * Provide the ws studio layout diff merge operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_studio_layout_diff_merge(UmiWsLayoutTemplate *layout);
/**
 * Provide the ws studio layout testing operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_studio_layout_testing(UmiWsLayoutTemplate *layout);
/**
 * Provide the ws studio layout database operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_studio_layout_database(UmiWsLayoutTemplate *layout);
/**
 * Provide the ws studio layout ai development operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_studio_layout_ai_development(UmiWsLayoutTemplate *layout);
/**
 * Provide the ws studio layout ai teacher operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_studio_layout_ai_teacher(UmiWsLayoutTemplate *layout);
/**
 * Provide the ws studio layout focus operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_studio_layout_focus(UmiWsLayoutTemplate *layout);

#ifdef __cplusplus
}
#endif

#endif
