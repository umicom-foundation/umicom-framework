/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/conformance_profile.h
 *
 * PURPOSE:
 *   Declare semantic adaptive-layout expectations shared across GTK4, Qt6 and Native Web previews.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_CONFORMANCE_PROFILE_H
#define UMICOM_DESIGNER_ADAPTIVE_CONFORMANCE_PROFILE_H
#include "umicom/designer/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerAdaptiveConformanceProfile {
    UmiRadTargetKind renderer;
    uint32_t required_size_classes_mask;
    uint32_t required_input_modes_mask;
    int safe_area_required;
    int orientation_required;
} UmiDesignerAdaptiveConformanceProfile;
/* Initialise baseline adaptive conformance requirements for one renderer. */
UmiStatus umi_designer_adaptive_conformance_profile_default(
    UmiRadTargetKind renderer,
    UmiDesignerAdaptiveConformanceProfile *out_profile);
/* Return whether a profile covers all four canonical Design-System size classes. */
int umi_designer_adaptive_conformance_profile_complete(
    const UmiDesignerAdaptiveConformanceProfile *profile);

#ifdef __cplusplus
}
#endif
#endif
