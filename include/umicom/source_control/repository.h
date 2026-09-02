/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_control/repository.h
 *
 * PURPOSE:
 *   Define a provider-neutral source-control workspace record above the low-level VCS adapter boundary.
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
#ifndef UMICOM_SOURCE_CONTROL_REPOSITORY_H
#define UMICOM_SOURCE_CONTROL_REPOSITORY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SOURCE_CONTROL_REPOSITORY_CAPACITY 2048U
#define UMI_SOURCE_CONTROL_REPOSITORY_API_VERSION 1U

/**
 * Represent the source control repository snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiSourceControlRepositorySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char root_uri[1024];
    char provider[64];
    char branch[256];
    char head[128];
    int clean;
    int detached;
    uint64_t revision;
} UmiSourceControlRepositorySnapshot;

/**
 * Represent the source control repository registry data shared with callers of this public
 * contract.
 */
typedef struct UmiSourceControlRepositoryRegistry UmiSourceControlRepositoryRegistry;

/**
 * Initialise source control repository registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_source_control_repository_registry_create(UmiSourceControlRepositoryRegistry **out_registry);
/**
 * Release or reset state held by source control repository registry so the same storage
 * can be reused safely.
 */
void umi_source_control_repository_registry_destroy(UmiSourceControlRepositoryRegistry *registry);
/**
 * Provide the source control repository registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_source_control_repository_registry_upsert(UmiSourceControlRepositoryRegistry *registry, const UmiSourceControlRepositorySnapshot *item);
/**
 * Remove source control repository registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_source_control_repository_registry_remove(UmiSourceControlRepositoryRegistry *registry, const char *id);
/**
 * Find source control repository registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_source_control_repository_registry_find(const UmiSourceControlRepositoryRegistry *registry, const char *id, UmiSourceControlRepositorySnapshot *out_item);
/**
 * Find source control repository registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_source_control_repository_registry_at(const UmiSourceControlRepositoryRegistry *registry, size_t index, UmiSourceControlRepositorySnapshot *out_item);
/**
 * Return the number of records represented by source control repository registry without
 * changing their state.
 */
size_t umi_source_control_repository_registry_count(const UmiSourceControlRepositoryRegistry *registry);
/**
 * Provide the source control repository registry revision operation used by this module
 * and its client applications.
 */
uint64_t umi_source_control_repository_registry_revision(const UmiSourceControlRepositoryRegistry *registry);
/**
 * Release or reset state held by source control repository registry so the same storage
 * can be reused safely.
 */
void umi_source_control_repository_registry_clear(UmiSourceControlRepositoryRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
