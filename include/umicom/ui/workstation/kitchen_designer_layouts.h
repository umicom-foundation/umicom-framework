/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/kitchen_designer_layouts.h
 *
 * PURPOSE:
 *   Define Kitchen Designer presets for floor-plan design, 3D review, component catalogues, materials, costing and properties.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_KITCHEN_DESIGNER_LAYOUTS_H
#define UMICOM_UI_WORKSTATION_KITCHEN_DESIGNER_LAYOUTS_H

#include "umicom/ui/workstation/layout_template.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ws kitchen layout design operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_kitchen_layout_design(UmiWsLayoutTemplate *layout);
/**
 * Provide the ws kitchen layout review operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_kitchen_layout_review(UmiWsLayoutTemplate *layout);
/**
 * Provide the ws kitchen layout catalogue operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_kitchen_layout_catalogue(UmiWsLayoutTemplate *layout);

#ifdef __cplusplus
}
#endif

#endif
