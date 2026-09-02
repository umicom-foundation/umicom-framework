/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/layout_geometry.h
 *
 * PURPOSE:
 *   Define portable geometry transforms for DPI scaling, monitor clamping and saved workstation layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_LAYOUT_GEOMETRY_H
#define UMICOM_UI_WORKSTATION_LAYOUT_GEOMETRY_H

#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ws layout geometry clamp operation used by this module and its client
 * applications.
 */
UmiUiRect umi_ws_layout_geometry_clamp(UmiUiRect bounds, UmiUiRect work_area);
/**
 * Provide the ws layout geometry scale operation used by this module and its client
 * applications.
 */
UmiUiRect umi_ws_layout_geometry_scale(UmiUiRect bounds, int32_t from_scale_percent, int32_t to_scale_percent);
/**
 * Provide the ws layout geometry area operation used by this module and its client
 * applications.
 */
int32_t umi_ws_layout_geometry_area(UmiUiRect bounds);

#ifdef __cplusplus
}
#endif

#endif
