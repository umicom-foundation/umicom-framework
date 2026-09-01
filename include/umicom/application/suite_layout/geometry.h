/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/geometry.h
 *
 * PURPOSE:
 *   Map canonical UI placement values to deterministic normalized workspace geometry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_GEOMETRY_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_GEOMETRY_H

#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Normalized rectangle used to place one layout region on any screen size.
 *
 * Values are fractions of the available workspace. For example, a width of
 * `0.25` uses one quarter of the available horizontal space.
 */
typedef struct UmiApplicationSuiteLayoutRect {
    double x;
    double y;
    double width;
    double height;
} UmiApplicationSuiteLayoutRect;

/**
 * Returns the canonical rectangle for one supported placement.
 *
 * @param placement Toolkit-neutral region requested by a layout recipe.
 * @return Normalized geometry. Unsupported placements return the shared
 *         fallback rectangle and can be detected with the support function.
 *
 * @see umi_application_suite_layout_region_supported
 */
UmiApplicationSuiteLayoutRect umi_application_suite_layout_region_rect(
    UmiUiPlacement placement);
/**
 * Reports whether canonical geometry exists for a placement.
 *
 * @param placement Toolkit-neutral region to examine.
 * @return Non-zero when the region can be projected, otherwise zero.
 */
int umi_application_suite_layout_region_supported(UmiUiPlacement placement);

#ifdef __cplusplus
}
#endif
#endif
