/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/conformance_profile.c
 *
 * PURPOSE:
 *   Declare semantic adaptive-layout expectations shared across GTK4, Qt6 and Native Web previews.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/conformance_profile.h"
#include <string.h>

/* Require all canonical size classes and pointer/touch semantics for production renderers. */
UmiStatus umi_designer_adaptive_conformance_profile_default(
    UmiRadTargetKind renderer,
    UmiDesignerAdaptiveConformanceProfile *out_profile)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(out_profile==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_profile,0,sizeof *out_profile);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(renderer<UMI_RAD_TARGET_ABSTRACT||renderer>UMI_RAD_TARGET_NATIVE_WEB)return UMI_STATUS_INVALID_ARGUMENT;
    out_profile->renderer=renderer;
    out_profile->required_size_classes_mask=0x0FU;
    out_profile->required_input_modes_mask=0x07U;
    out_profile->safe_area_required=1;
    out_profile->orientation_required=1;
    return UMI_STATUS_OK;
}
/* Treat the four canonical size-class bits as the minimum cross-frontend coverage gate. */
int umi_designer_adaptive_conformance_profile_complete(
    const UmiDesignerAdaptiveConformanceProfile *profile)
{
    return profile!=NULL&&(profile->required_size_classes_mask&0x0FU)==0x0FU;
}
