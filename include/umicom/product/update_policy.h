/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/update_policy.h
 *
 * PURPOSE:
 *   Define update-channel and trust policy independently from package providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_PRODUCT_UPDATE_POLICY_H
#define UMICOM_PRODUCT_UPDATE_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PRODUCT_UPDATE_POLICY_CAPACITY 1024U

/**
 * Represent the product update policy snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiProductUpdatePolicySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char product_id[128];
    char channel[64];
    char allowed_range[128];
    int automatic;
    int security_only;
    int allow_prerelease;
    int require_signature;
    uint64_t revision;
} UmiProductUpdatePolicySnapshot;

/**
 * Represent the product update policy registry data shared with callers of this public
 * contract.
 */
typedef struct UmiProductUpdatePolicyRegistry UmiProductUpdatePolicyRegistry;

/**
 * Initialise product update policy registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_product_update_policy_registry_create(UmiProductUpdatePolicyRegistry **out_registry);
/**
 * Release or reset state held by product update policy registry so the same storage can be
 * reused safely.
 */
void umi_product_update_policy_registry_destroy(UmiProductUpdatePolicyRegistry *registry);
/**
 * Provide the product update policy registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_product_update_policy_registry_upsert(UmiProductUpdatePolicyRegistry *registry, const UmiProductUpdatePolicySnapshot *item);
/**
 * Remove product update policy registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_product_update_policy_registry_remove(UmiProductUpdatePolicyRegistry *registry, const char *id);
/**
 * Find product update policy registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_product_update_policy_registry_find(const UmiProductUpdatePolicyRegistry *registry, const char *id, UmiProductUpdatePolicySnapshot *out_item);
/**
 * Find product update policy registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_product_update_policy_registry_at(const UmiProductUpdatePolicyRegistry *registry, size_t index, UmiProductUpdatePolicySnapshot *out_item);
/**
 * Return the number of records represented by product update policy registry without
 * changing their state.
 */
size_t umi_product_update_policy_registry_count(const UmiProductUpdatePolicyRegistry *registry);
/**
 * Provide the product update policy registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_product_update_policy_registry_revision(const UmiProductUpdatePolicyRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
