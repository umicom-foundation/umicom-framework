/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/binding.h
 *
 * PURPOSE:
 *   Define model-to-view bindings that can be shared by multiple frontend adapters.
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
#ifndef UMICOM_FRONTEND_BINDING_H
#define UMICOM_FRONTEND_BINDING_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FRONTEND_BINDING_CAPACITY 2048U

/**
 * Represent the frontend binding snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiFrontendBindingSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char source_path[256];
    char target_widget_id[128];
    char target_property[128];
    char converter[128];
    int two_way;
    int enabled;
    uint64_t revision;
} UmiFrontendBindingSnapshot;

/**
 * Represent the frontend binding registry data shared with callers of this public
 * contract.
 */
typedef struct UmiFrontendBindingRegistry UmiFrontendBindingRegistry;

/**
 * Initialise frontend binding registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_frontend_binding_registry_create(UmiFrontendBindingRegistry **out_registry);
/**
 * Release or reset state held by frontend binding registry so the same storage can be
 * reused safely.
 */
void umi_frontend_binding_registry_destroy(UmiFrontendBindingRegistry *registry);
/**
 * Provide the frontend binding registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_frontend_binding_registry_upsert(UmiFrontendBindingRegistry *registry, const UmiFrontendBindingSnapshot *item);
/**
 * Remove frontend binding registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_frontend_binding_registry_remove(UmiFrontendBindingRegistry *registry, const char *id);
/**
 * Find frontend binding registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_frontend_binding_registry_find(const UmiFrontendBindingRegistry *registry, const char *id, UmiFrontendBindingSnapshot *out_item);
/**
 * Find frontend binding registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_frontend_binding_registry_at(const UmiFrontendBindingRegistry *registry, size_t index, UmiFrontendBindingSnapshot *out_item);
/**
 * Return the number of records represented by frontend binding registry without changing
 * their state.
 */
size_t umi_frontend_binding_registry_count(const UmiFrontendBindingRegistry *registry);
/**
 * Provide the frontend binding registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_frontend_binding_registry_revision(const UmiFrontendBindingRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
