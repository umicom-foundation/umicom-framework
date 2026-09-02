/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_service_registry.c
 *
 * PURPOSE:
 *   Verify explicit service ownership, singleton enforcement, lookup,
 *   replaceable providers, unregistration and owned-service destruction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <stdlib.h>

static int destroyed = 0;

/*
 * Exercise destroy service and return a clear result when the behaviour no longer matches
 * its contract.
 */
static void destroy_service(void *service, void *user_data)
{
    int *counter = (int *)user_data;
    free(service);
    *counter += 1;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiServiceRegistry *registry = NULL;
    UmiServiceDescriptor descriptor;
    const UmiServiceDescriptor *found;
    int *service = (int *)calloc(1U, sizeof(*service));

    assert(service != NULL);
    *service = 73;
    assert(umi_service_registry_create(&registry) == UMI_STATUS_OK);

    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.service_id = "umicom.test.service";
    descriptor.service_version = (UmiVersion){1U, 0U, 0U};
    descriptor.provider_module_id = "org.umicom.test";
    descriptor.service = service;
    descriptor.destroy = destroy_service;
    descriptor.destroy_user_data = &destroyed;
    descriptor.flags = UMI_SERVICE_SINGLETON | UMI_SERVICE_OWNED;

    assert(umi_service_registry_register(registry, &descriptor) ==
           UMI_STATUS_OK);
    assert(umi_service_registry_register(registry, &descriptor) ==
           UMI_STATUS_ALREADY_EXISTS);
    assert(umi_service_registry_count(registry) == 1U);

    found = umi_service_registry_find(registry, "umicom.test.service");
    assert(found != NULL);
    assert(*(int *)found->service == 73);

    assert(umi_service_registry_unregister(registry,
                                           "umicom.test.service",
                                           "org.umicom.test") ==
           UMI_STATUS_OK);
    assert(destroyed == 1);
    assert(umi_service_registry_count(registry) == 0U);

    umi_service_registry_destroy(registry);
    assert(destroyed == 1);
    return 0;
}
