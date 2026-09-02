/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/reference.h
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
#ifndef UMICOM_PROJECT_REFERENCE_H
#define UMICOM_PROJECT_REFERENCE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_REFERENCE_CAPACITY 1024U
#define UMI_PROJECT_REFERENCE_API_VERSION 1U

/**
 * Represent the project reference snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectReferenceSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char project_id[128];
    char target_project_id[128];
    char kind[64];
    int required;
    int available;
    uint64_t revision;
} UmiProjectReferenceSnapshot;

/**
 * Represent the project reference registry data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectReferenceRegistry UmiProjectReferenceRegistry;

/**
 * Initialise project reference registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_project_reference_registry_create(UmiProjectReferenceRegistry **out_registry);
/**
 * Release or reset state held by project reference registry so the same storage can be
 * reused safely.
 */
void umi_project_reference_registry_destroy(UmiProjectReferenceRegistry *registry);
/**
 * Provide the project reference registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_reference_registry_upsert(UmiProjectReferenceRegistry *registry, const UmiProjectReferenceSnapshot *item);
/**
 * Remove project reference registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_project_reference_registry_remove(UmiProjectReferenceRegistry *registry, const char *id);
/**
 * Find project reference registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_project_reference_registry_find(const UmiProjectReferenceRegistry *registry, const char *id, UmiProjectReferenceSnapshot *out_item);
/**
 * Find project reference registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_project_reference_registry_at(const UmiProjectReferenceRegistry *registry, size_t index, UmiProjectReferenceSnapshot *out_item);
/**
 * Return the number of records represented by project reference registry without changing
 * their state.
 */
size_t umi_project_reference_registry_count(const UmiProjectReferenceRegistry *registry);
/**
 * Provide the project reference registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_project_reference_registry_revision(const UmiProjectReferenceRegistry *registry);
/**
 * Release or reset state held by project reference registry so the same storage can be
 * reused safely.
 */
void umi_project_reference_registry_clear(UmiProjectReferenceRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
