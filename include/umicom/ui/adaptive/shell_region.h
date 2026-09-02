/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/shell_region.h
 *
 * PURPOSE:
 *   Describe one adaptive application-shell region and its collapse/overlay capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_SHELL_REGION_H
#define UMICOM_UI_ADAPTIVE_SHELL_REGION_H
#include "umicom/ui/adaptive/responsive_region.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the adaptive shell region data shared with callers of this public contract.
 */
typedef struct UmiAdaptiveShellRegion {
    UmiAdaptiveResponsiveRegion responsive;
    int32_t preferred_extent;
    int collapsible;
    int overlay_capable;
} UmiAdaptiveShellRegion;
/* Initialise a shell region from an existing responsive-region descriptor. */
UmiStatus umi_adaptive_shell_region_init(UmiAdaptiveShellRegion *region,
                                         const UmiAdaptiveResponsiveRegion *responsive,
                                         int32_t preferred_extent,
                                         int collapsible,
                                         int overlay_capable);

#ifdef __cplusplus
}
#endif
#endif
