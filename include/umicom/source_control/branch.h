/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_control/branch.h
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
#ifndef UMICOM_SOURCE_CONTROL_BRANCH_H
#define UMICOM_SOURCE_CONTROL_BRANCH_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SOURCE_CONTROL_BRANCH_CAPACITY 2048U
#define UMI_SOURCE_CONTROL_BRANCH_API_VERSION 1U

/**
 * Represent the source control branch snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiSourceControlBranchSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char repository_id[128];
    char name[256];
    char upstream[256];
    char head[128];
    int current;
    int remote;
    uint64_t revision;
} UmiSourceControlBranchSnapshot;

/**
 * Represent the source control branch registry data shared with callers of this public
 * contract.
 */
typedef struct UmiSourceControlBranchRegistry UmiSourceControlBranchRegistry;

/**
 * Initialise source control branch registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_source_control_branch_registry_create(UmiSourceControlBranchRegistry **out_registry);
/**
 * Release or reset state held by source control branch registry so the same storage can be
 * reused safely.
 */
void umi_source_control_branch_registry_destroy(UmiSourceControlBranchRegistry *registry);
/**
 * Provide the source control branch registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_source_control_branch_registry_upsert(UmiSourceControlBranchRegistry *registry, const UmiSourceControlBranchSnapshot *item);
/**
 * Remove source control branch registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_source_control_branch_registry_remove(UmiSourceControlBranchRegistry *registry, const char *id);
/**
 * Find source control branch registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_source_control_branch_registry_find(const UmiSourceControlBranchRegistry *registry, const char *id, UmiSourceControlBranchSnapshot *out_item);
/**
 * Find source control branch registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_source_control_branch_registry_at(const UmiSourceControlBranchRegistry *registry, size_t index, UmiSourceControlBranchSnapshot *out_item);
/**
 * Return the number of records represented by source control branch registry without
 * changing their state.
 */
size_t umi_source_control_branch_registry_count(const UmiSourceControlBranchRegistry *registry);
/**
 * Provide the source control branch registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_source_control_branch_registry_revision(const UmiSourceControlBranchRegistry *registry);
/**
 * Release or reset state held by source control branch registry so the same storage can be
 * reused safely.
 */
void umi_source_control_branch_registry_clear(UmiSourceControlBranchRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
