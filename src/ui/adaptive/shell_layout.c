/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/shell_layout.c
 *
 * PURPOSE:
 *   Hold a bounded semantic application-shell layout shared by all frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/shell_layout.h"
#include <string.h>

/* Reset the layout and assign its stable application-level identifier. */
UmiStatus umi_adaptive_shell_layout_init(UmiAdaptiveShellLayout *layout, const char *layout_id)
{
    if (layout == NULL || layout_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(layout, 0, sizeof *layout);
    return umi_adaptive_copy_text(layout->layout_id, sizeof layout->layout_id, layout_id);
}

/* Add a region while preventing duplicate identifiers and capacity overflow. */
UmiStatus umi_adaptive_shell_layout_add(UmiAdaptiveShellLayout *layout,
                                        const UmiAdaptiveShellRegion *region)
{
    if (layout == NULL || region == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_adaptive_shell_layout_find(layout, region->responsive.region_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (layout->count >= UMI_ADAPTIVE_MAX_ITEMS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    layout->regions[layout->count++] = *region;
    return UMI_STATUS_OK;
}

/* Resolve a region by stable identifier for renderer and designer projections. */
const UmiAdaptiveShellRegion *umi_adaptive_shell_layout_find(const UmiAdaptiveShellLayout *layout,
                                                             const char *region_id)
{
    size_t index;
    if (layout == NULL || region_id == NULL) {
        return NULL;
    }
    for (index = 0U; index < layout->count; ++index) {
        if (strcmp(layout->regions[index].responsive.region_id, region_id) == 0) {
            return &layout->regions[index];
        }
    }
    return NULL;
}
