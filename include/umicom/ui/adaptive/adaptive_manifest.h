/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/adaptive_manifest.h
 *
 * PURPOSE:
 *   Declare application-wide adaptive shell capabilities and renderer coverage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_ADAPTIVE_MANIFEST_H
#define UMICOM_UI_ADAPTIVE_ADAPTIVE_MANIFEST_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAdaptiveManifest {
    char application_id[UMI_ADAPTIVE_ID_CAPACITY];
    char shell_profile_id[UMI_ADAPTIVE_ID_CAPACITY];
    uint32_t renderer_mask;
    uint16_t breakpoint_count;
    int supports_orientation_change;
    int supports_multi_window;
} UmiAdaptiveManifest;
/* Initialise an application adaptive manifest. */
UmiStatus umi_adaptive_manifest_init(UmiAdaptiveManifest *manifest,
                                     const char *application_id,
                                     const char *shell_profile_id);
/* Validate minimum manifest identity and renderer coverage before launch. */
int umi_adaptive_manifest_valid(const UmiAdaptiveManifest *manifest);

#ifdef __cplusplus
}
#endif
#endif
