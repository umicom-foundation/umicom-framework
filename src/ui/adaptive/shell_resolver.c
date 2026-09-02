/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/shell_resolver.c
 *
 * PURPOSE:
 *   Resolve a shell profile into concrete region presentations for one viewport class.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/shell_resolver.h"
#include <string.h>

/* Resolve active/inactive regions and collapse overlay-capable side surfaces on compact screens. */
UmiStatus umi_adaptive_shell_resolver_resolve(const UmiAdaptiveShellProfile *profile,
                                              UmiDesignSizeClass size_class,
                                              UmiAdaptiveResolvedShell *out_shell)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!umi_adaptive_shell_profile_valid(profile) || out_shell == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(out_shell, 0, sizeof *out_shell);
    out_shell->navigation = profile->navigation;
    out_shell->density = profile->density;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < profile->layout.count; ++index) {
        const UmiAdaptiveShellRegion *region = &profile->layout.regions[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!umi_adaptive_responsive_region_matches(&region->responsive, size_class)) {
            ++out_shell->hidden_regions;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (size_class == UMI_DESIGN_SIZE_COMPACT && region->overlay_capable != 0) {
            ++out_shell->overlay_regions;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            ++out_shell->inline_regions;
        }
    }
    return UMI_STATUS_OK;
}
