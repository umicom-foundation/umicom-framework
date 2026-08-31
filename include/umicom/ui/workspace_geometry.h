/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workspace_geometry.h
 *
 * PURPOSE:
 *   Provide shared normalized geometry for toolkit-neutral workspace regions
 *   without making the UI layer depend on an application library.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_WORKSPACE_GEOMETRY_H
#define UMICOM_UI_WORKSPACE_GEOMETRY_H

#include <stdbool.h>

#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiWorkspaceRect {
    double x;
    double y;
    double width;
    double height;
} UmiUiWorkspaceRect;

/* Return whether a placement has a predefined workspace region. */
bool umi_ui_workspace_region_supported(UmiUiPlacement placement);

/* Return deterministic single-monitor geometry for one placement. */
UmiUiWorkspaceRect umi_ui_workspace_region_rect(UmiUiPlacement placement);

#ifdef __cplusplus
}
#endif
#endif
