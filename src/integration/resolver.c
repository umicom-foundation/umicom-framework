/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/resolver.c
 *
 * PURPOSE:
 *   Implement the corresponding public Suite and Inter-Application Runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/resolver.h"

#include <stdio.h>
#include <string.h>

static bool dependency_satisfied(
    const UmiIntegrationRegistry *registry,
    const UmiIntegrationDependency *dependency)
{
    if (dependency->target_type == UMI_INTEGRATION_TARGET_APPLICATION) {
        const UmiIntegrationRegistryEntry *entry =
            umi_integration_registry_find_const(registry, dependency->target);
        return entry != NULL && entry->application.enabled;
    }
    return umi_integration_registry_find_capability(
        registry, dependency->target) != NULL;
}

UmiStatus umi_integration_resolve(
    const UmiIntegrationRegistry *registry,
    const UmiIntegrationDependency *dependencies,
    size_t dependency_count,
    UmiIntegrationResolution *out_resolution)
{
    size_t index;
    if (registry == NULL || out_resolution == NULL ||
        (dependencies == NULL && dependency_count != 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_resolution, 0, sizeof(*out_resolution));
    for (index = 0U; index < dependency_count; ++index) {
        bool satisfied = dependency_satisfied(registry, &dependencies[index]);
        if (dependencies[index].kind == UMI_INTEGRATION_DEPENDENCY_REQUIRED) {
            if (satisfied) {
                ++out_resolution->satisfied_required;
            } else {
                ++out_resolution->missing_required;
                if (out_resolution->first_missing_required[0] == '\0') {
                    (void)snprintf(
                        out_resolution->first_missing_required,
                        sizeof(out_resolution->first_missing_required),
                        "%s",
                        dependencies[index].target
                    );
                }
            }
        } else {
            if (satisfied) {
                ++out_resolution->satisfied_optional;
            } else {
                ++out_resolution->missing_optional;
            }
        }
    }
    return out_resolution->missing_required == 0U
        ? UMI_STATUS_OK
        : UMI_STATUS_UNAVAILABLE;
}
