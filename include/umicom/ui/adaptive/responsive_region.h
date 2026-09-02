/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/responsive_region.h
 *
 * PURPOSE:
 *   Define size-class aware semantic shell regions with priority-based presentation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_RESPONSIVE_REGION_H
#define UMICOM_UI_ADAPTIVE_RESPONSIVE_REGION_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the adaptive responsive region data shared with callers of this public
 * contract.
 */
typedef struct UmiAdaptiveResponsiveRegion {
    char region_id[UMI_ADAPTIVE_ID_CAPACITY];
    UmiAdaptiveRegionRole role;
    UmiDesignSizeClass minimum_class;
    UmiDesignSizeClass maximum_class;
    uint32_t priority;
} UmiAdaptiveResponsiveRegion;
/* Initialise one responsive shell region with an inclusive size-class range. */
UmiStatus umi_adaptive_responsive_region_init(UmiAdaptiveResponsiveRegion *region,
                                              const char *region_id,
                                              UmiAdaptiveRegionRole role,
                                              UmiDesignSizeClass minimum_class,
                                              UmiDesignSizeClass maximum_class,
                                              uint32_t priority);
/* Return whether the region participates in the requested size class. */
int umi_adaptive_responsive_region_matches(const UmiAdaptiveResponsiveRegion *region,
                                           UmiDesignSizeClass size_class);

#ifdef __cplusplus
}
#endif
#endif
