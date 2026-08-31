/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/suite_layout/geometry.c
 *
 * PURPOSE:
 *   Provide stable single-monitor geometry used to materialise all suite application layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/suite_layout/geometry.h"
#include "umicom/ui/workspace_geometry.h"

int umi_application_suite_layout_region_supported(UmiUiPlacement placement)
{
    /* Keep the established application API as a compatibility wrapper around
     * the lower-level UI geometry service. */
    return umi_ui_workspace_region_supported(placement) ? 1 : 0;
}

UmiApplicationSuiteLayoutRect umi_application_suite_layout_region_rect(
    UmiUiPlacement placement)
{
    UmiUiWorkspaceRect shared = umi_ui_workspace_region_rect(placement);
    UmiApplicationSuiteLayoutRect rect = {
        shared.x,
        shared.y,
        shared.width,
        shared.height
    };
    return rect;
}
