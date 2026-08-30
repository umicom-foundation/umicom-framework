/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/adaptive_service.h
 *
 * PURPOSE:
 *   Expose aggregate adaptive-shell readiness and active-device state to Framework applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_ADAPTIVE_SERVICE_H
#define UMICOM_UI_ADAPTIVE_ADAPTIVE_SERVICE_H
#include "umicom/ui/adaptive/shell_resolver.h"
#include "umicom/ui/adaptive/device_family.h"
#include "umicom/ui/adaptive/viewport_class.h"
#include "umicom/ui/adaptive/density_policy.h"
#include "umicom/ui/adaptive/navigation_pattern.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAdaptiveServiceSnapshot {
    UmiAdaptiveDeviceFamily family;
    UmiDesignSizeClass size_class;
    UmiAdaptiveNavigationPattern navigation;
    UmiDesignDensity density;
    size_t visible_regions;
    int ready;
} UmiAdaptiveServiceSnapshot;
/* Build an aggregate adaptive runtime snapshot from resolved shell state. */
UmiStatus umi_adaptive_service_snapshot(const UmiAdaptiveShellProfile *profile,
                                        UmiAdaptiveViewport viewport,
                                        UmiAdaptiveInputMode input_mode,
                                        UmiAdaptiveServiceSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
