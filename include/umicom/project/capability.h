/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/capability.h
 *
 * PURPOSE:
 *   Define a reusable project-system record used by Studio and future Umicom development products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef UMICOM_PROJECT_CAPABILITY_H
#define UMICOM_PROJECT_CAPABILITY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_CAPABILITY_CAPACITY 1024U
#define UMI_PROJECT_CAPABILITY_API_VERSION 1U

/**
 * Represent the project capability snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectCapabilitySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char project_id[128];
    char capability_id[128];
    char version[128];
    int required;
    int available;
    uint64_t revision;
} UmiProjectCapabilitySnapshot;

/**
 * Represent the project capability registry data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectCapabilityRegistry UmiProjectCapabilityRegistry;

/**
 * Initialise project capability registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_project_capability_registry_create(UmiProjectCapabilityRegistry **out_registry);
/**
 * Release or reset state held by project capability registry so the same storage can be
 * reused safely.
 */
void umi_project_capability_registry_destroy(UmiProjectCapabilityRegistry *registry);
/**
 * Provide the project capability registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_capability_registry_upsert(UmiProjectCapabilityRegistry *registry, const UmiProjectCapabilitySnapshot *item);
/**
 * Remove project capability registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_project_capability_registry_remove(UmiProjectCapabilityRegistry *registry, const char *id);
/**
 * Find project capability registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_project_capability_registry_find(const UmiProjectCapabilityRegistry *registry, const char *id, UmiProjectCapabilitySnapshot *out_item);
/**
 * Find project capability registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_project_capability_registry_at(const UmiProjectCapabilityRegistry *registry, size_t index, UmiProjectCapabilitySnapshot *out_item);
/**
 * Return the number of records represented by project capability registry without changing
 * their state.
 */
size_t umi_project_capability_registry_count(const UmiProjectCapabilityRegistry *registry);
/**
 * Provide the project capability registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_project_capability_registry_revision(const UmiProjectCapabilityRegistry *registry);
/**
 * Release or reset state held by project capability registry so the same storage can be
 * reused safely.
 */
void umi_project_capability_registry_clear(UmiProjectCapabilityRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
