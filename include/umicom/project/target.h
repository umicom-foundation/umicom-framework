/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/target.h
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
#ifndef UMICOM_PROJECT_TARGET_H
#define UMICOM_PROJECT_TARGET_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_TARGET_CAPACITY 1024U
#define UMI_PROJECT_TARGET_API_VERSION 1U

/**
 * Represent the project target snapshot data shared with callers of this public contract.
 */
typedef struct UmiProjectTargetSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char project_id[128];
    char name[256];
    char kind[64];
    char output_uri[1024];
    int enabled;
    int default_target;
    uint64_t revision;
} UmiProjectTargetSnapshot;

/**
 * Represent the project target registry data shared with callers of this public contract.
 */
typedef struct UmiProjectTargetRegistry UmiProjectTargetRegistry;

/**
 * Initialise project target registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_project_target_registry_create(UmiProjectTargetRegistry **out_registry);
/**
 * Release or reset state held by project target registry so the same storage can be reused
 * safely.
 */
void umi_project_target_registry_destroy(UmiProjectTargetRegistry *registry);
/**
 * Provide the project target registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_project_target_registry_upsert(UmiProjectTargetRegistry *registry, const UmiProjectTargetSnapshot *item);
/**
 * Remove project target registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_project_target_registry_remove(UmiProjectTargetRegistry *registry, const char *id);
/**
 * Find project target registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_project_target_registry_find(const UmiProjectTargetRegistry *registry, const char *id, UmiProjectTargetSnapshot *out_item);
/**
 * Find project target registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_project_target_registry_at(const UmiProjectTargetRegistry *registry, size_t index, UmiProjectTargetSnapshot *out_item);
/**
 * Return the number of records represented by project target registry without changing
 * their state.
 */
size_t umi_project_target_registry_count(const UmiProjectTargetRegistry *registry);
/**
 * Provide the project target registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_project_target_registry_revision(const UmiProjectTargetRegistry *registry);
/**
 * Release or reset state held by project target registry so the same storage can be reused
 * safely.
 */
void umi_project_target_registry_clear(UmiProjectTargetRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
