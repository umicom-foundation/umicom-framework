/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/shell_profile.h
 *
 * PURPOSE:
 *   Bind a semantic shell layout to device, density and navigation preferences.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_SHELL_PROFILE_H
#define UMICOM_UI_ADAPTIVE_SHELL_PROFILE_H
#include "umicom/ui/adaptive/shell_layout.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAdaptiveShellProfile {
    char profile_id[UMI_ADAPTIVE_ID_CAPACITY];
    UmiAdaptiveDeviceFamily family;
    UmiAdaptiveNavigationPattern navigation;
    UmiDesignDensity density;
    UmiAdaptiveShellLayout layout;
} UmiAdaptiveShellProfile;
/* Initialise a reusable adaptive shell profile. */
UmiStatus umi_adaptive_shell_profile_init(UmiAdaptiveShellProfile *profile,
                                          const char *profile_id,
                                          UmiAdaptiveDeviceFamily family,
                                          UmiAdaptiveNavigationPattern navigation,
                                          UmiDesignDensity density,
                                          const UmiAdaptiveShellLayout *layout);
/* Validate a profile before use by a runtime renderer. */
int umi_adaptive_shell_profile_valid(const UmiAdaptiveShellProfile *profile);

#ifdef __cplusplus
}
#endif
#endif
