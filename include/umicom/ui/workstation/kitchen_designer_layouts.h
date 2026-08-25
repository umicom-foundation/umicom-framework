/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/kitchen_designer_layouts.h
 *
 * PURPOSE:
 *   Define Kitchen Designer presets for floor-plan design, 3D review, component catalogues, materials, costing and properties.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_KITCHEN_DESIGNER_LAYOUTS_H
#define UMICOM_UI_WORKSTATION_KITCHEN_DESIGNER_LAYOUTS_H

#include "umicom/ui/workstation/layout_template.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ws_kitchen_layout_design(UmiWsLayoutTemplate *layout);
UmiStatus umi_ws_kitchen_layout_review(UmiWsLayoutTemplate *layout);
UmiStatus umi_ws_kitchen_layout_catalogue(UmiWsLayoutTemplate *layout);

#ifdef __cplusplus
}
#endif

#endif
