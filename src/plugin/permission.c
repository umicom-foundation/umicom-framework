/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/permission.c
 *
 * PURPOSE:
 *   Compare requested plug-in permissions with explicit product grants and
 *   report the first denial.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/permission.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>
/*
 * Provide the plugin permissions evaluate operation used by this module and its client
 * applications.
 */
UmiStatus umi_plugin_permissions_evaluate(const UmiPluginManifest *manifest, const UmiPermissionSet *grants, UmiPluginPermissionDecision *out_decision)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL || grants == NULL || out_decision == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_decision, 0, sizeof(*out_decision)); out_decision->requested = manifest->permission_count;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < manifest->permission_count; ++i) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_permission_set_contains(grants, manifest->permissions[i])) ++out_decision->granted;
        /* Use this fallback path when the earlier condition does not apply. */
        else { ++out_decision->denied; /* Apply this branch only when its contract condition is satisfied. */ if (out_decision->first_denied[0] == '\0') (void)snprintf(out_decision->first_denied, sizeof(out_decision->first_denied), "%s", manifest->permissions[i]); }
    }
    return out_decision->denied == 0U ? UMI_STATUS_OK : UMI_STATUS_PERMISSION_DENIED;
}
