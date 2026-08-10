/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/plugin/permission.c
 *
 * PURPOSE:
 *   Compare requested plug-in permissions with explicit product grants and
 *   report the first denial.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/permission.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>
UmiStatus umi_plugin_permissions_evaluate(const UmiPluginManifest *manifest, const UmiPermissionSet *grants, UmiPluginPermissionDecision *out_decision)
{
    size_t i;
    if (manifest == NULL || grants == NULL || out_decision == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_decision, 0, sizeof(*out_decision)); out_decision->requested = manifest->permission_count;
    for (i = 0U; i < manifest->permission_count; ++i) {
        if (umi_permission_set_contains(grants, manifest->permissions[i])) ++out_decision->granted;
        else { ++out_decision->denied; if (out_decision->first_denied[0] == '\0') (void)snprintf(out_decision->first_denied, sizeof(out_decision->first_denied), "%s", manifest->permissions[i]); }
    }
    return out_decision->denied == 0U ? UMI_STATUS_OK : UMI_STATUS_PERMISSION_DENIED;
}
