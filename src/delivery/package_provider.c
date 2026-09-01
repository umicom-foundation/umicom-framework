/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/package_provider.c
 *
 * PURPOSE:
 *   Implement a provider interface for packaging technologies and invoke providers through a stable C contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Providers keep external packaging tools behind one interface so product code never shells out directly.
 */

#include "umicom/delivery/package_provider.h"
#include "delivery_internal.h"
#include <string.h>

UmiStatus umi_package_provider_init(UmiPackageProvider *provider,
                                    const char *provider_id,
                                    UmiPackageFormat format,
                                    void *instance,
                                    UmiPackageCreateFn create)
{
    UmiStatus status;
    if (provider == NULL || provider_id == NULL || create == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(provider, 0, sizeof(*provider));
    status = umi_delivery_copy_text(provider->provider_id,
                                    sizeof(provider->provider_id), provider_id);
    if (status != UMI_STATUS_OK) return status;
    provider->format = format;
    provider->instance = instance;
    provider->create = create;
    return UMI_STATUS_OK;
}

UmiStatus umi_package_provider_create(const UmiPackageProvider *provider,
                                      const UmiPackageSpec *spec,
                                      UmiPackageResult *result)
{
    if (provider == NULL || spec == NULL || result == NULL || provider->create == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (provider->format != spec->format) return UMI_STATUS_INVALID_ARGUMENT;
    return provider->create(provider->instance, spec, result);
}
