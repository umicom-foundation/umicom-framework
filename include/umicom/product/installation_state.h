/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/product/installation_state.h
 *
 * PURPOSE:
 *   Define installation, verification and rollback state for reusable product delivery workflows.
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
#ifndef UMICOM_PRODUCT_INSTALLATION_STATE_H
#define UMICOM_PRODUCT_INSTALLATION_STATE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PRODUCT_INSTALLATION_STATE_CAPACITY 1024U

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

typedef struct UmiProductInstallationRegistry UmiProductInstallationRegistry;

UmiStatus umi_product_installation_state_registry_create(UmiProductInstallationRegistry **out_registry);
void umi_product_installation_state_registry_destroy(UmiProductInstallationRegistry *registry);
UmiStatus umi_product_installation_state_registry_upsert(UmiProductInstallationRegistry *registry, const UmiProductInstallationSnapshot *item);
UmiStatus umi_product_installation_state_registry_remove(UmiProductInstallationRegistry *registry, const char *id);
UmiStatus umi_product_installation_state_registry_find(const UmiProductInstallationRegistry *registry, const char *id, UmiProductInstallationSnapshot *out_item);
UmiStatus umi_product_installation_state_registry_at(const UmiProductInstallationRegistry *registry, size_t index, UmiProductInstallationSnapshot *out_item);
UmiStatus umi_product_installation_state_registry_set_state(
    UmiProductInstallationRegistry *registry,
    const char *id,
    int state,
    int verified,
    int rollback_available);
size_t umi_product_installation_state_registry_count(const UmiProductInstallationRegistry *registry);
uint64_t umi_product_installation_state_registry_revision(const UmiProductInstallationRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
