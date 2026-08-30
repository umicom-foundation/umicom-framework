/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/centre.h
 *
 * PURPOSE:
 *   Define an owned software/product centre combining marketplace metadata providers, installations and update policies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract is toolkit-neutral and intended for reuse by Studio,
 * Trader, TMS and future Umicom applications without exposing GUI objects.
 */
#ifndef UMICOM_PRODUCT_CENTRE_H
#define UMICOM_PRODUCT_CENTRE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/product/marketplace.h"
#include "umicom/product/metadata_provider.h"
#include "umicom/product/installation_state.h"
#include "umicom/product/update_policy.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiProductCentre UmiProductCentre;

typedef struct UmiProductCentreSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t marketplace_count;
    size_t providers_count;
    size_t installations_count;
    size_t policies_count;
    uint64_t revision;
} UmiProductCentreSnapshot;

UmiStatus umi_product_centre_create(UmiProductCentre **out_service);
void umi_product_centre_destroy(UmiProductCentre *service);
UmiStatus umi_product_centre_snapshot(const UmiProductCentre *service, UmiProductCentreSnapshot *out_snapshot);
UmiProductMarketplaceItemRegistry *umi_product_centre_marketplace(UmiProductCentre *service);
UmiProductMetadataProviderRegistry *umi_product_centre_providers(UmiProductCentre *service);
UmiProductInstallationRegistry *umi_product_centre_installations(UmiProductCentre *service);
UmiProductUpdatePolicyRegistry *umi_product_centre_policies(UmiProductCentre *service);

#ifdef __cplusplus
}
#endif
#endif
