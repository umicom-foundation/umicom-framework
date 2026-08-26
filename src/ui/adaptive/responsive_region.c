/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/responsive_region.c
 *
 * PURPOSE:
 *   Define size-class aware semantic shell regions with priority-based presentation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/responsive_region.h"
#include <string.h>

/* Store one responsive region after validating identity and class ordering. */
UmiStatus umi_adaptive_responsive_region_init(UmiAdaptiveResponsiveRegion *region,
                                              const char *region_id,
                                              UmiAdaptiveRegionRole role,
                                              UmiDesignSizeClass minimum_class,
                                              UmiDesignSizeClass maximum_class,
                                              uint32_t priority)
{
    UmiStatus status;
    if (region == NULL || region_id == NULL || minimum_class > maximum_class) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(region, 0, sizeof *region);
    status = umi_adaptive_copy_text(region->region_id, sizeof region->region_id, region_id);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    region->role = role;
    region->minimum_class = minimum_class;
    region->maximum_class = maximum_class;
    region->priority = priority;
    return UMI_STATUS_OK;
}

/* Match the requested size class against the region's inclusive class interval. */
int umi_adaptive_responsive_region_matches(const UmiAdaptiveResponsiveRegion *region,
                                           UmiDesignSizeClass size_class)
{
    if (region == NULL) {
        return 0;
    }
    return size_class >= region->minimum_class && size_class <= region->maximum_class;
}
