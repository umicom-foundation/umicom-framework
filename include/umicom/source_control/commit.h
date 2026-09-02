/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_control/commit.h
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
#ifndef UMICOM_SOURCE_CONTROL_COMMIT_H
#define UMICOM_SOURCE_CONTROL_COMMIT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SOURCE_CONTROL_COMMIT_CAPACITY 2048U
#define UMI_SOURCE_CONTROL_COMMIT_API_VERSION 1U

/**
 * Represent the source control commit snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiSourceControlCommitSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char repository_id[128];
    char hash[128];
    char author[256];
    char email[256];
    char subject[512];
    uint64_t timestamp;
    int head;
    uint64_t revision;
} UmiSourceControlCommitSnapshot;

/**
 * Represent the source control commit registry data shared with callers of this public
 * contract.
 */
typedef struct UmiSourceControlCommitRegistry UmiSourceControlCommitRegistry;

/**
 * Initialise source control commit registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_source_control_commit_registry_create(UmiSourceControlCommitRegistry **out_registry);
/**
 * Release or reset state held by source control commit registry so the same storage can be
 * reused safely.
 */
void umi_source_control_commit_registry_destroy(UmiSourceControlCommitRegistry *registry);
/**
 * Provide the source control commit registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_source_control_commit_registry_upsert(UmiSourceControlCommitRegistry *registry, const UmiSourceControlCommitSnapshot *item);
/**
 * Remove source control commit registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_source_control_commit_registry_remove(UmiSourceControlCommitRegistry *registry, const char *id);
/**
 * Find source control commit registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_source_control_commit_registry_find(const UmiSourceControlCommitRegistry *registry, const char *id, UmiSourceControlCommitSnapshot *out_item);
/**
 * Find source control commit registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_source_control_commit_registry_at(const UmiSourceControlCommitRegistry *registry, size_t index, UmiSourceControlCommitSnapshot *out_item);
/**
 * Return the number of records represented by source control commit registry without
 * changing their state.
 */
size_t umi_source_control_commit_registry_count(const UmiSourceControlCommitRegistry *registry);
/**
 * Provide the source control commit registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_source_control_commit_registry_revision(const UmiSourceControlCommitRegistry *registry);
/**
 * Release or reset state held by source control commit registry so the same storage can be
 * reused safely.
 */
void umi_source_control_commit_registry_clear(UmiSourceControlCommitRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
