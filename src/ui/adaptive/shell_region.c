/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/shell_region.c
 *
 * PURPOSE:
 *   Describe one adaptive application-shell region and its collapse/overlay capabilities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/shell_region.h"

/* Compose responsive visibility with shell-specific extent/collapse semantics. */
UmiStatus umi_adaptive_shell_region_init(UmiAdaptiveShellRegion *region,
                                         const UmiAdaptiveResponsiveRegion *responsive,
                                         int32_t preferred_extent,
                                         int collapsible,
                                         int overlay_capable)
{
    if (region == NULL || responsive == NULL || preferred_extent < 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    region->responsive = *responsive;
    region->preferred_extent = preferred_extent;
    region->collapsible = collapsible != 0;
    region->overlay_capable = overlay_capable != 0;
    return UMI_STATUS_OK;
}
