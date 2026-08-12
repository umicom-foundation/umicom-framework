/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/update_policy.h
 *
 * PURPOSE:
 *   Define update-channel and trust policy independently from package providers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

typedef struct UmiProductUpdatePolicyRegistry UmiProductUpdatePolicyRegistry;

UmiStatus umi_product_update_policy_registry_create(UmiProductUpdatePolicyRegistry **out_registry);
void umi_product_update_policy_registry_destroy(UmiProductUpdatePolicyRegistry *registry);
UmiStatus umi_product_update_policy_registry_upsert(UmiProductUpdatePolicyRegistry *registry, const UmiProductUpdatePolicySnapshot *item);
UmiStatus umi_product_update_policy_registry_remove(UmiProductUpdatePolicyRegistry *registry, const char *id);
UmiStatus umi_product_update_policy_registry_find(const UmiProductUpdatePolicyRegistry *registry, const char *id, UmiProductUpdatePolicySnapshot *out_item);
UmiStatus umi_product_update_policy_registry_at(const UmiProductUpdatePolicyRegistry *registry, size_t index, UmiProductUpdatePolicySnapshot *out_item);
size_t umi_product_update_policy_registry_count(const UmiProductUpdatePolicyRegistry *registry);
uint64_t umi_product_update_policy_registry_revision(const UmiProductUpdatePolicyRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
