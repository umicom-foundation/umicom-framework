/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/profile_health.c
 *
 * PURPOSE:
 *   Implement DAP adapter executable health reporting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/profile_health.h"
#include <stdio.h>
#include <string.h>

/*
 * Provide the debug runtime profile health probe operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_profile_health_probe(
    const UmiDebugAdapterProfile *profile,
    UmiDebugRuntimeProfileHealth *out_health)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || out_health == NULL ||
        profile->id[0] == '\0' || profile->executable[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_health, 0, sizeof(*out_health));
    (void)snprintf(out_health->profile_id, sizeof(out_health->profile_id),
                   "%s", profile->id);
    (void)snprintf(out_health->executable, sizeof(out_health->executable),
                   "%s", profile->executable);

    status = umi_toolchain_find_on_path(
        profile->executable,
        out_health->resolved_path,
        sizeof(out_health->resolved_path));
    out_health->status = status;
    out_health->available = status == UMI_STATUS_OK;
    return UMI_STATUS_OK;
}
