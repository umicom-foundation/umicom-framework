/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/conformance_profile.h
 *
 * PURPOSE:
 *   Aggregate design-token, accessibility, contrast, responsive and semantic-component conformance evidence.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_CONFORMANCE_PROFILE_H
#define INCLUDE_UMICOM_UI_DESIGN_CONFORMANCE_PROFILE_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the design conformance profile data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignConformanceProfile { uint32_t token_checks; uint32_t token_failures; uint32_t accessibility_checks; uint32_t accessibility_failures; uint32_t contrast_checks; uint32_t contrast_failures; uint32_t responsive_checks; uint32_t responsive_failures; uint32_t component_checks; uint32_t component_failures; } UmiDesignConformanceProfile;
/* Record one pass/fail result for a named design conformance category. */
UmiStatus umi_design_conformance_record(UmiDesignConformanceProfile *profile, const char *category, int passed);
/* Calculate a normalized [0,1] conformance score across all recorded checks. */
double umi_design_conformance_score(const UmiDesignConformanceProfile *profile);
/* Return one only when every recorded design-system conformance check passed. */
int umi_design_conformance_passes(const UmiDesignConformanceProfile *profile);

#ifdef __cplusplus
}
#endif

#endif
