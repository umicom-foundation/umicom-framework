/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_control/staging.h
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
#ifndef UMICOM_SOURCE_CONTROL_STAGING_H
#define UMICOM_SOURCE_CONTROL_STAGING_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SOURCE_CONTROL_STAGING_CAPACITY 2048U
#define UMI_SOURCE_CONTROL_STAGING_API_VERSION 1U

/**
 * Represent the source control staging snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiSourceControlStagingSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char repository_id[128];
    char change_id[128];
    char hunk_id[128];
    int staged;
    int partial;
    uint64_t revision;
} UmiSourceControlStagingSnapshot;

/**
 * Represent the source control staging registry data shared with callers of this public
 * contract.
 */
typedef struct UmiSourceControlStagingRegistry UmiSourceControlStagingRegistry;

/**
 * Initialise source control staging registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_source_control_staging_registry_create(UmiSourceControlStagingRegistry **out_registry);
/**
 * Release or reset state held by source control staging registry so the same storage can
 * be reused safely.
 */
void umi_source_control_staging_registry_destroy(UmiSourceControlStagingRegistry *registry);
/**
 * Provide the source control staging registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_source_control_staging_registry_upsert(UmiSourceControlStagingRegistry *registry, const UmiSourceControlStagingSnapshot *item);
/**
 * Remove source control staging registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_source_control_staging_registry_remove(UmiSourceControlStagingRegistry *registry, const char *id);
/**
 * Find source control staging registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_source_control_staging_registry_find(const UmiSourceControlStagingRegistry *registry, const char *id, UmiSourceControlStagingSnapshot *out_item);
/**
 * Find source control staging registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_source_control_staging_registry_at(const UmiSourceControlStagingRegistry *registry, size_t index, UmiSourceControlStagingSnapshot *out_item);
/**
 * Return the number of records represented by source control staging registry without
 * changing their state.
 */
size_t umi_source_control_staging_registry_count(const UmiSourceControlStagingRegistry *registry);
/**
 * Provide the source control staging registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_source_control_staging_registry_revision(const UmiSourceControlStagingRegistry *registry);
/**
 * Release or reset state held by source control staging registry so the same storage can
 * be reused safely.
 */
void umi_source_control_staging_registry_clear(UmiSourceControlStagingRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
