/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/installation_state.h
 *
 * PURPOSE:
 *   Define installation, verification and rollback state for reusable product delivery workflows.
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
#ifndef UMICOM_PRODUCT_INSTALLATION_STATE_H
#define UMICOM_PRODUCT_INSTALLATION_STATE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PRODUCT_INSTALLATION_STATE_CAPACITY 1024U

/**
 * Represent the product installation snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiProductInstallationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char product_id[128];
    char version[64];
    char install_root[1024];
    char channel[64];
    uint64_t installed_at;
    int state;
    int verified;
    int rollback_available;
    uint64_t revision;
} UmiProductInstallationSnapshot;

/**
 * Represent the product installation registry data shared with callers of this public
 * contract.
 */
typedef struct UmiProductInstallationRegistry UmiProductInstallationRegistry;

/**
 * Initialise product installation state registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_product_installation_state_registry_create(UmiProductInstallationRegistry **out_registry);
/**
 * Release or reset state held by product installation state registry so the same storage
 * can be reused safely.
 */
void umi_product_installation_state_registry_destroy(UmiProductInstallationRegistry *registry);
/**
 * Provide the product installation state registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_product_installation_state_registry_upsert(UmiProductInstallationRegistry *registry, const UmiProductInstallationSnapshot *item);
/**
 * Remove product installation state registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_product_installation_state_registry_remove(UmiProductInstallationRegistry *registry, const char *id);
/**
 * Find product installation state registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_product_installation_state_registry_find(const UmiProductInstallationRegistry *registry, const char *id, UmiProductInstallationSnapshot *out_item);
/**
 * Find product installation state registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_product_installation_state_registry_at(const UmiProductInstallationRegistry *registry, size_t index, UmiProductInstallationSnapshot *out_item);
/**
 * Provide the product installation state registry set state operation used by this module
 * and its client applications.
 */
UmiStatus umi_product_installation_state_registry_set_state(
    UmiProductInstallationRegistry *registry,
    const char *id,
    int state,
    int verified,
    int rollback_available);
/**
 * Return the number of records represented by product installation state registry without
 * changing their state.
 */
size_t umi_product_installation_state_registry_count(const UmiProductInstallationRegistry *registry);
/**
 * Provide the product installation state registry revision operation used by this module
 * and its client applications.
 */
uint64_t umi_product_installation_state_registry_revision(const UmiProductInstallationRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
