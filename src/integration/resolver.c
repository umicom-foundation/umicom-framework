/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/resolver.c
 *
 * PURPOSE:
 *   Implement the corresponding public Suite and Inter-Application Runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/resolver.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the dependency satisfied operation used by this module and its client
 * applications.
 */
static bool dependency_satisfied(
    const UmiIntegrationRegistry *registry,
    const UmiIntegrationDependency *dependency)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (dependency->target_type == UMI_INTEGRATION_TARGET_APPLICATION) {
        const UmiIntegrationRegistryEntry *entry =
            umi_integration_registry_find_const(registry, dependency->target);
        return entry != NULL && entry->application.enabled;
    }
    return umi_integration_registry_find_capability(
        registry, dependency->target) != NULL;
}

/*
 * Provide the integration resolve operation used by this module and its client
 * applications.
 */
UmiStatus umi_integration_resolve(
    const UmiIntegrationRegistry *registry,
    const UmiIntegrationDependency *dependencies,
    size_t dependency_count,
    UmiIntegrationResolution *out_resolution)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_resolution == NULL ||
        (dependencies == NULL && dependency_count != 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_resolution, 0, sizeof(*out_resolution));
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < dependency_count; ++index) {
        bool satisfied = dependency_satisfied(registry, &dependencies[index]);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (dependencies[index].kind == UMI_INTEGRATION_DEPENDENCY_REQUIRED) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (satisfied) {
                ++out_resolution->satisfied_required;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                ++out_resolution->missing_required;
                /* Apply this branch only when its contract condition is satisfied. */
                if (out_resolution->first_missing_required[0] == '\0') {
                    (void)snprintf(
                        out_resolution->first_missing_required,
                        sizeof(out_resolution->first_missing_required),
                        "%s",
                        dependencies[index].target
                    );
                }
            }
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            /* Apply this branch only when its contract condition is satisfied. */
            if (satisfied) {
                ++out_resolution->satisfied_optional;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                ++out_resolution->missing_optional;
            }
        }
    }
    return out_resolution->missing_required == 0U
        ? UMI_STATUS_OK
        : UMI_STATUS_UNAVAILABLE;
}
