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

int umi_application_suite_layout_region_supported(UmiUiPlacement placement)
{
    return placement >= UMI_UI_PLACEMENT_LEFT &&
           placement <= UMI_UI_PLACEMENT_FLOATING;
}

UmiApplicationSuiteLayoutRect umi_application_suite_layout_region_rect(
    UmiUiPlacement placement)
{
    UmiApplicationSuiteLayoutRect rect = {0.0, 0.0, 1.0, 1.0};
    switch (placement) {
    case UMI_UI_PLACEMENT_LEFT:
        rect.x = 0.0; rect.y = 0.0; rect.width = 0.22; rect.height = 0.72;
        break;
    case UMI_UI_PLACEMENT_RIGHT:
        rect.x = 0.76; rect.y = 0.0; rect.width = 0.24; rect.height = 0.72;
        break;
    case UMI_UI_PLACEMENT_TOP:
        rect.x = 0.0; rect.y = 0.0; rect.width = 1.0; rect.height = 0.18;
        break;
    case UMI_UI_PLACEMENT_BOTTOM:
        rect.x = 0.0; rect.y = 0.72; rect.width = 1.0; rect.height = 0.28;
        break;
    case UMI_UI_PLACEMENT_CENTRE:
        rect.x = 0.22; rect.y = 0.0; rect.width = 0.54; rect.height = 0.72;
        break;
    case UMI_UI_PLACEMENT_FLOATING:
        rect.x = 0.20; rect.y = 0.12; rect.width = 0.60; rect.height = 0.64;
        break;
    default:
        break;
    }
    return rect;
}
