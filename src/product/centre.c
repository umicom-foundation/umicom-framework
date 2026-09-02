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

/*
 * Initialise product centre from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_product_centre_create(UmiProductCentre **out_service)
{
    UmiProductCentre *service;
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_service = NULL;
    service = (UmiProductCentre *)calloc(1U, sizeof(*service));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    service->revision = 1U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_product_marketplace_registry_create(&service->marketplace);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_product_metadata_provider_registry_create(&service->providers);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_product_installation_state_registry_create(&service->installations);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_product_update_policy_registry_create(&service->policies);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_product_centre_destroy(service);
        return status;
    }
    *out_service = service;
    return UMI_STATUS_OK;
}

/* Release or reset state held by product centre so the same storage can be reused safely. */
void umi_product_centre_destroy(UmiProductCentre *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return;
    umi_product_update_policy_registry_destroy(service->policies);
    umi_product_installation_state_registry_destroy(service->installations);
    umi_product_metadata_provider_registry_destroy(service->providers);
    umi_product_marketplace_registry_destroy(service->marketplace);
    free(service);
}

/*
 * Provide the product centre snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_product_centre_snapshot(const UmiProductCentre *service, UmiProductCentreSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Provide the product centre marketplace operation used by this module and its client
 * applications.
 */
UmiProductMarketplaceItemRegistry *umi_product_centre_marketplace(UmiProductCentre *service)
{
    return service != NULL ? service->marketplace : NULL;
}

/*
 * Provide the product centre providers operation used by this module and its client
 * applications.
 */
UmiProductMetadataProviderRegistry *umi_product_centre_providers(UmiProductCentre *service)
{
    return service != NULL ? service->providers : NULL;
}

/*
 * Provide the product centre installations operation used by this module and its client
 * applications.
 */
UmiProductInstallationRegistry *umi_product_centre_installations(UmiProductCentre *service)
{
    return service != NULL ? service->installations : NULL;
}

/*
 * Provide the product centre policies operation used by this module and its client
 * applications.
 */
UmiProductUpdatePolicyRegistry *umi_product_centre_policies(UmiProductCentre *service)
{
    return service != NULL ? service->policies : NULL;
}
