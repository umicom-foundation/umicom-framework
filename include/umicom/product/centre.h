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

/*
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

/**
 * Represent the product centre data shared with callers of this public contract.
 */
typedef struct UmiProductCentre UmiProductCentre;

/**
 * Represent the product centre snapshot data shared with callers of this public contract.
 */
typedef struct UmiProductCentreSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t marketplace_count;
    size_t providers_count;
    size_t installations_count;
    size_t policies_count;
    uint64_t revision;
} UmiProductCentreSnapshot;

/**
 * Initialise product centre from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_product_centre_create(UmiProductCentre **out_service);
/**
 * Release or reset state held by product centre so the same storage can be reused safely.
 */
void umi_product_centre_destroy(UmiProductCentre *service);
/**
 * Provide the product centre snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_product_centre_snapshot(const UmiProductCentre *service, UmiProductCentreSnapshot *out_snapshot);
/**
 * Provide the product centre marketplace operation used by this module and its client
 * applications.
 */
UmiProductMarketplaceItemRegistry *umi_product_centre_marketplace(UmiProductCentre *service);
/**
 * Provide the product centre providers operation used by this module and its client
 * applications.
 */
UmiProductMetadataProviderRegistry *umi_product_centre_providers(UmiProductCentre *service);
/**
 * Provide the product centre installations operation used by this module and its client
 * applications.
 */
UmiProductInstallationRegistry *umi_product_centre_installations(UmiProductCentre *service);
/**
 * Provide the product centre policies operation used by this module and its client
 * applications.
 */
UmiProductUpdatePolicyRegistry *umi_product_centre_policies(UmiProductCentre *service);

#ifdef __cplusplus
}
#endif
#endif
