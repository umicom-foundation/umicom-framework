/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime_legacy_service_descriptor.c
 *
 * PURPOSE:
 *   Preserve compatibility for externally owned service descriptors that do
 *   not provide an optional destruction callback.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/service_registry.h"

#include <assert.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiServiceRegistry *registry = NULL;
    UmiServiceDescriptor descriptor;
    const UmiServiceDescriptor *stored;
    int service = 42;

    assert(umi_service_registry_create(&registry) == UMI_STATUS_OK);
    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.service_id = "legacy.service";
    descriptor.service_version = (UmiVersion){1U, 0U, 0U};
    descriptor.provider_module_id = "legacy.provider";
    descriptor.service = &service;
    descriptor.destroy = NULL;
    descriptor.destroy_user_data = NULL;

    assert(umi_service_registry_register(registry, &descriptor) == UMI_STATUS_OK);
    stored = umi_service_registry_find(registry, descriptor.service_id);
    assert(stored != NULL);
    assert(stored->service == &service);
    umi_service_registry_destroy(registry);
    return 0;
}
