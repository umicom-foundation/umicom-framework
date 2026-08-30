/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime_legacy_capability_descriptor.c
 *
 * PURPOSE:
 *   Preserve metadata-only capability registration where a capability declares
 *   availability without exposing a concrete provider pointer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/capability_registry.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiCapabilityRegistry *registry = NULL;
    UmiCapabilityDescriptor descriptor;
    const UmiCapabilityDescriptor *stored;

    assert(umi_capability_registry_create(&registry) == UMI_STATUS_OK);
    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.capability_id = "legacy.capability";
    descriptor.capability_version = (UmiVersion){1U, 0U, 0U};
    descriptor.provider_module_id = "legacy.provider";
    descriptor.provider = NULL;
    descriptor.flags = UMI_CAPABILITY_OPTIONAL;

    assert(umi_capability_registry_register(registry, &descriptor) ==
           UMI_STATUS_OK);
    stored = umi_capability_registry_find(registry, descriptor.capability_id);
    assert(stored != NULL);
    assert(stored->provider == NULL);
    umi_capability_registry_destroy(registry);
    return 0;
}
