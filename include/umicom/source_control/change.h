/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_control/change.h
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
#ifndef UMICOM_SOURCE_CONTROL_CHANGE_H
#define UMICOM_SOURCE_CONTROL_CHANGE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SOURCE_CONTROL_CHANGE_CAPACITY 2048U
#define UMI_SOURCE_CONTROL_CHANGE_API_VERSION 1U

/**
 * Represent the source control change snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiSourceControlChangeSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char repository_id[128];
    char uri[1024];
    char status[64];
    char old_uri[1024];
    int staged;
    int conflict;
    uint64_t revision;
} UmiSourceControlChangeSnapshot;

/**
 * Represent the source control change registry data shared with callers of this public
 * contract.
 */
typedef struct UmiSourceControlChangeRegistry UmiSourceControlChangeRegistry;

/**
 * Initialise source control change registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_source_control_change_registry_create(UmiSourceControlChangeRegistry **out_registry);
/**
 * Release or reset state held by source control change registry so the same storage can be
 * reused safely.
 */
void umi_source_control_change_registry_destroy(UmiSourceControlChangeRegistry *registry);
/**
 * Provide the source control change registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_source_control_change_registry_upsert(UmiSourceControlChangeRegistry *registry, const UmiSourceControlChangeSnapshot *item);
/**
 * Remove source control change registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_source_control_change_registry_remove(UmiSourceControlChangeRegistry *registry, const char *id);
/**
 * Find source control change registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_source_control_change_registry_find(const UmiSourceControlChangeRegistry *registry, const char *id, UmiSourceControlChangeSnapshot *out_item);
/**
 * Find source control change registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_source_control_change_registry_at(const UmiSourceControlChangeRegistry *registry, size_t index, UmiSourceControlChangeSnapshot *out_item);
/**
 * Return the number of records represented by source control change registry without
 * changing their state.
 */
size_t umi_source_control_change_registry_count(const UmiSourceControlChangeRegistry *registry);
/**
 * Provide the source control change registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_source_control_change_registry_revision(const UmiSourceControlChangeRegistry *registry);
/**
 * Release or reset state held by source control change registry so the same storage can be
 * reused safely.
 */
void umi_source_control_change_registry_clear(UmiSourceControlChangeRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
