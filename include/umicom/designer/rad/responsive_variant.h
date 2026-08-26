/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/responsive_variant.h
 *
 * PURPOSE:
 *   Describe per-breakpoint component geometry and visibility overrides.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_RESPONSIVE_VARIANT_H
#define UMICOM_DESIGNER_RAD_RESPONSIVE_VARIANT_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadResponsiveVariant {
    char breakpoint_id[UMI_RAD_ID_CAPACITY];
    UmiRadRect bounds;
    bool visible;
    bool override_geometry;
} UmiRadResponsiveVariant;
UmiStatus umi_rad_responsive_variant_init(UmiRadResponsiveVariant *item);
int umi_rad_responsive_variant_is_valid(const UmiRadResponsiveVariant *item);
#ifdef __cplusplus
}
#endif
#endif
