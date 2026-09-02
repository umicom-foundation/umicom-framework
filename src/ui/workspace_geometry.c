/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workspace_geometry.c
 *
 * PURPOSE:
 *   Implement reusable normalized workspace-region geometry for every thin
 *   application and frontend adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workspace_geometry.h"

/*
 * Provide the ui workspace region supported operation used by this module and its client
 * applications.
 */
bool umi_ui_workspace_region_supported(UmiUiPlacement placement)
{
    return placement >= UMI_UI_PLACEMENT_LEFT &&
           placement <= UMI_UI_PLACEMENT_FLOATING;
}

/*
 * Provide the ui workspace region rect operation used by this module and its client
 * applications.
 */
UmiUiWorkspaceRect umi_ui_workspace_region_rect(UmiUiPlacement placement)
{
    UmiUiWorkspaceRect rectangle = {0.0, 0.0, 1.0, 1.0};

    /* These proportions provide a readable default.  Saved user layouts may
     * replace them without changing the semantic placement name. */
    switch (placement) {
        case UMI_UI_PLACEMENT_LEFT:
            rectangle.x = 0.0;
            rectangle.y = 0.0;
            rectangle.width = 0.22;
            rectangle.height = 0.72;
            break;
        case UMI_UI_PLACEMENT_RIGHT:
            rectangle.x = 0.76;
            rectangle.y = 0.0;
            rectangle.width = 0.24;
            rectangle.height = 0.72;
            break;
        case UMI_UI_PLACEMENT_TOP:
            rectangle.x = 0.0;
            rectangle.y = 0.0;
            rectangle.width = 1.0;
            rectangle.height = 0.18;
            break;
        case UMI_UI_PLACEMENT_BOTTOM:
            rectangle.x = 0.0;
            rectangle.y = 0.72;
            rectangle.width = 1.0;
            rectangle.height = 0.28;
            break;
        case UMI_UI_PLACEMENT_CENTRE:
            rectangle.x = 0.22;
            rectangle.y = 0.0;
            rectangle.width = 0.54;
            rectangle.height = 0.72;
            break;
        case UMI_UI_PLACEMENT_FLOATING:
            rectangle.x = 0.20;
            rectangle.y = 0.12;
            rectangle.width = 0.60;
            rectangle.height = 0.64;
            break;
        default:
            /* Unknown values retain a safe full-workspace rectangle. */
            break;
    }
    return rectangle;
}
