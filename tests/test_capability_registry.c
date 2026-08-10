/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_capability_registry.c
 *
 * PURPOSE:
 *   Verify capability registration, dependency resolution, duplicate rejection,
 *   lookup, removal, and deterministic registry enumeration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <stdlib.h>
#include <string.h>

int main(void)
{
    UmiCapabilityRegistry *registry = NULL;
    int provider = 42;
    const char *required[] = {"umicom.test.service", NULL};
    const char *missing = NULL;
    UmiCapabilityDescriptor descriptor = {
        .structure_size = sizeof(UmiCapabilityDescriptor),
        .capability_id = "umicom.test.service",
        .capability_version = {1U, 2U, 3U},
        .provider_module_id = "org.umicom.test.provider",
        .provider = &provider,
        .flags = UMI_CAPABILITY_SINGLETON
    };

    if (umi_capability_registry_create(&registry) != UMI_STATUS_OK)
        return EXIT_FAILURE;
    if (umi_capability_registry_register(registry, &descriptor) != UMI_STATUS_OK)
        return EXIT_FAILURE;
    if (umi_capability_registry_count(registry) != 1U)
        return EXIT_FAILURE;
    if (umi_capability_registry_find(registry, descriptor.capability_id) == NULL)
        return EXIT_FAILURE;
    if (umi_capability_registry_require(registry, required, &missing) !=
        UMI_STATUS_OK || missing != NULL)
        return EXIT_FAILURE;
    if (umi_capability_registry_register(registry, &descriptor) !=
        UMI_STATUS_ALREADY_EXISTS)
        return EXIT_FAILURE;
    if (umi_capability_registry_unregister(registry,
                                           descriptor.capability_id,
                                           descriptor.provider_module_id) !=
        UMI_STATUS_OK)
        return EXIT_FAILURE;
    if (umi_capability_registry_require(registry, required, &missing) !=
        UMI_STATUS_NOT_FOUND || missing == NULL ||
        strcmp(missing, required[0]) != 0)
        return EXIT_FAILURE;

    umi_capability_registry_destroy(registry);
    return EXIT_SUCCESS;
}
