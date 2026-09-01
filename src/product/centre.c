/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/centre.c
 *
 * PURPOSE:
 *   Implement an owned software/product centre combining marketplace metadata providers, installations and update policies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation owns only its documented child models. Product-specific
 * business data and toolkit objects remain outside this reusable Framework layer.
 */
#include "umicom/product/centre.h"

#include <stdlib.h>
#include <string.h>

struct UmiProductCentre {
    UmiProductMarketplaceItemRegistry *marketplace;
    UmiProductMetadataProviderRegistry *providers;
    UmiProductInstallationRegistry *installations;
    UmiProductUpdatePolicyRegistry *policies;
    uint64_t revision;
};

UmiStatus umi_product_centre_create(UmiProductCentre **out_service)
{
    UmiProductCentre *service;
    UmiStatus status = UMI_STATUS_OK;
    if (out_service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_service = NULL;
    service = (UmiProductCentre *)calloc(1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    service->revision = 1U;
    if (status == UMI_STATUS_OK) status = umi_product_marketplace_registry_create(&service->marketplace);
    if (status == UMI_STATUS_OK) status = umi_product_metadata_provider_registry_create(&service->providers);
    if (status == UMI_STATUS_OK) status = umi_product_installation_state_registry_create(&service->installations);
    if (status == UMI_STATUS_OK) status = umi_product_update_policy_registry_create(&service->policies);
    if (status != UMI_STATUS_OK) {
        umi_product_centre_destroy(service);
        return status;
    }
    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_product_centre_destroy(UmiProductCentre *service)
{
    if (service == NULL) return;
    umi_product_update_policy_registry_destroy(service->policies);
    umi_product_installation_state_registry_destroy(service->installations);
    umi_product_metadata_provider_registry_destroy(service->providers);
    umi_product_marketplace_registry_destroy(service->marketplace);
    free(service);
}

UmiStatus umi_product_centre_snapshot(const UmiProductCentre *service, UmiProductCentreSnapshot *out_snapshot)
{
    if (service == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = 1U;
    out_snapshot->marketplace_count = umi_product_marketplace_registry_count(service->marketplace);
    out_snapshot->providers_count = umi_product_metadata_provider_registry_count(service->providers);
    out_snapshot->installations_count = umi_product_installation_state_registry_count(service->installations);
    out_snapshot->policies_count = umi_product_update_policy_registry_count(service->policies);
    out_snapshot->revision = service->revision;
    return UMI_STATUS_OK;
}

UmiProductMarketplaceItemRegistry *umi_product_centre_marketplace(UmiProductCentre *service)
{
    return service != NULL ? service->marketplace : NULL;
}

UmiProductMetadataProviderRegistry *umi_product_centre_providers(UmiProductCentre *service)
{
    return service != NULL ? service->providers : NULL;
}

UmiProductInstallationRegistry *umi_product_centre_installations(UmiProductCentre *service)
{
    return service != NULL ? service->installations : NULL;
}

UmiProductUpdatePolicyRegistry *umi_product_centre_policies(UmiProductCentre *service)
{
    return service != NULL ? service->policies : NULL;
}
