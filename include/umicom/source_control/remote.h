/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_control/remote.h
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
#ifndef UMICOM_SOURCE_CONTROL_REMOTE_H
#define UMICOM_SOURCE_CONTROL_REMOTE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SOURCE_CONTROL_REMOTE_CAPACITY 2048U
#define UMI_SOURCE_CONTROL_REMOTE_API_VERSION 1U

/**
 * Represent the source control remote snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiSourceControlRemoteSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char repository_id[128];
    char name[128];
    char fetch_url[1024];
    char push_url[1024];
    int default_remote;
    uint64_t revision;
} UmiSourceControlRemoteSnapshot;

/**
 * Represent the source control remote registry data shared with callers of this public
 * contract.
 */
typedef struct UmiSourceControlRemoteRegistry UmiSourceControlRemoteRegistry;

/**
 * Initialise source control remote registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_source_control_remote_registry_create(UmiSourceControlRemoteRegistry **out_registry);
/**
 * Release or reset state held by source control remote registry so the same storage can be
 * reused safely.
 */
void umi_source_control_remote_registry_destroy(UmiSourceControlRemoteRegistry *registry);
/**
 * Provide the source control remote registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_source_control_remote_registry_upsert(UmiSourceControlRemoteRegistry *registry, const UmiSourceControlRemoteSnapshot *item);
/**
 * Remove source control remote registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_source_control_remote_registry_remove(UmiSourceControlRemoteRegistry *registry, const char *id);
/**
 * Find source control remote registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_source_control_remote_registry_find(const UmiSourceControlRemoteRegistry *registry, const char *id, UmiSourceControlRemoteSnapshot *out_item);
/**
 * Find source control remote registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_source_control_remote_registry_at(const UmiSourceControlRemoteRegistry *registry, size_t index, UmiSourceControlRemoteSnapshot *out_item);
/**
 * Return the number of records represented by source control remote registry without
 * changing their state.
 */
size_t umi_source_control_remote_registry_count(const UmiSourceControlRemoteRegistry *registry);
/**
 * Provide the source control remote registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_source_control_remote_registry_revision(const UmiSourceControlRemoteRegistry *registry);
/**
 * Release or reset state held by source control remote registry so the same storage can be
 * reused safely.
 */
void umi_source_control_remote_registry_clear(UmiSourceControlRemoteRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
