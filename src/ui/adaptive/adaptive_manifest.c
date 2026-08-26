/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/adaptive_manifest.c
 *
 * PURPOSE:
 *   Declare application-wide adaptive shell capabilities and renderer coverage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/adaptive_manifest.h"
#include <string.h>

/* Initialise an adaptive manifest with all current semantic renderer families enabled. */
UmiStatus umi_adaptive_manifest_init(UmiAdaptiveManifest *manifest,
                                     const char *application_id,
                                     const char *shell_profile_id)
{
    UmiStatus status;
    if (manifest == NULL || application_id == NULL || shell_profile_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(manifest, 0, sizeof *manifest);
    status = umi_adaptive_copy_text(manifest->application_id, sizeof manifest->application_id, application_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_adaptive_copy_text(manifest->shell_profile_id, sizeof manifest->shell_profile_id, shell_profile_id);
    if (status != UMI_STATUS_OK) return status;
    manifest->renderer_mask = 0x07U;
    manifest->supports_orientation_change = 1;
    return UMI_STATUS_OK;
}

/* Require stable identities and at least one renderer before the adaptive shell is considered valid. */
int umi_adaptive_manifest_valid(const UmiAdaptiveManifest *manifest)
{
    return manifest != NULL && manifest->application_id[0] != '\0' &&
           manifest->shell_profile_id[0] != '\0' && manifest->renderer_mask != 0U;
}
