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

/*
 * Provide the application suite layout region supported operation used by this module and
 * its client applications.
 */
int umi_application_suite_layout_region_supported(UmiUiPlacement placement)
{
    /* Keep the established application API as a compatibility wrapper around
     * the lower-level UI geometry service. */
    return umi_ui_workspace_region_supported(placement) ? 1 : 0;
}

/* Return shared normalized geometry through the established suite API. */
UmiApplicationSuiteLayoutRect umi_application_suite_layout_region_rect(
    UmiUiPlacement placement)
{
    /* Convert the shared UI rectangle by value so the application layer does
     * not expose or own the lower-level geometry structure. */
    UmiUiWorkspaceRect shared = umi_ui_workspace_region_rect(placement);
    UmiApplicationSuiteLayoutRect rect = {
        shared.x,
        shared.y,
        shared.width,
        shared.height
    };
    return rect;
}
