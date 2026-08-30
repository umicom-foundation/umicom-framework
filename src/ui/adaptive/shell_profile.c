/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/shell_profile.c
 *
 * PURPOSE:
 *   Bind a semantic shell layout to device, density and navigation preferences.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/shell_profile.h"
#include <string.h>

/* Compose one profile from canonical layout and design-system density state. */
UmiStatus umi_adaptive_shell_profile_init(UmiAdaptiveShellProfile *profile,
                                          const char *profile_id,
                                          UmiAdaptiveDeviceFamily family,
                                          UmiAdaptiveNavigationPattern navigation,
                                          UmiDesignDensity density,
                                          const UmiAdaptiveShellLayout *layout)
{
    UmiStatus status;
    if (profile == NULL || profile_id == NULL || layout == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(profile, 0, sizeof *profile);
    status = umi_adaptive_copy_text(profile->profile_id, sizeof profile->profile_id, profile_id);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    profile->family = family;
    profile->navigation = navigation;
    profile->density = density;
    profile->layout = *layout;
    return UMI_STATUS_OK;
}

/* Require identity, a primary layout and valid enum domains before rendering. */
int umi_adaptive_shell_profile_valid(const UmiAdaptiveShellProfile *profile)
{
    return profile != NULL && profile->profile_id[0] != '\0' &&
           profile->layout.layout_id[0] != '\0' &&
           profile->family >= UMI_ADAPTIVE_DEVICE_DESKTOP &&
           profile->family <= UMI_ADAPTIVE_DEVICE_FOLDABLE;
}
