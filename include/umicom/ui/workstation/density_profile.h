/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/density_profile.h
 *
 * PURPOSE:
 *   Define semantic spacing, row height and hit-target metrics for Compact, Comfortable and Touch workstation densities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_DENSITY_PROFILE_H
#define UMICOM_UI_WORKSTATION_DENSITY_PROFILE_H

#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsDensityProfile {
    UmiWsDensity density;
    int32_t spacing;
    int32_t panel_header_height;
    int32_t toolbar_height;
    int32_t row_height;
    int32_t hit_target;
} UmiWsDensityProfile;

UmiWsDensityProfile umi_ws_density_profile_make(UmiWsDensity density);

#ifdef __cplusplus
}
#endif

#endif
