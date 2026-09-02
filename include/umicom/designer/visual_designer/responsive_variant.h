/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/responsive_variant.h
 *
 * PURPOSE:
 *   Describe per-breakpoint component geometry and visibility overrides.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_RESPONSIVE_VARIANT_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_RESPONSIVE_VARIANT_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer responsive variant data shared with callers of this public contract.
 */
typedef struct UmiRadResponsiveVariant {
    char breakpoint_id[UMI_RAD_ID_CAPACITY];
    UmiRadRect bounds;
    bool visible;
    bool override_geometry;
} UmiRadResponsiveVariant;
/**
 * Initialise visual designer responsive variant from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_rad_responsive_variant_init(UmiRadResponsiveVariant *item);
/**
 * Check that visual designer responsive variant satisfies its contract before another service relies
 * on it.
 */
int umi_rad_responsive_variant_is_valid(const UmiRadResponsiveVariant *item);
#ifdef __cplusplus
}
#endif
#endif
