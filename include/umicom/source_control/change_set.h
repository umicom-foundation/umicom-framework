/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_control/change_set.h
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
#ifndef UMICOM_SOURCE_CONTROL_CHANGE_SET_H
#define UMICOM_SOURCE_CONTROL_CHANGE_SET_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SOURCE_CONTROL_CHANGE_SET_CAPACITY 2048U
#define UMI_SOURCE_CONTROL_CHANGE_SET_API_VERSION 1U

/**
 * Represent the source control change set snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiSourceControlChangeSetSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char repository_id[128];
    char name[256];
    char description[512];
    size_t change_count;
    int active;
    uint64_t revision;
} UmiSourceControlChangeSetSnapshot;

/**
 * Represent the source control change set registry data shared with callers of this public
 * contract.
 */
typedef struct UmiSourceControlChangeSetRegistry UmiSourceControlChangeSetRegistry;

/**
 * Initialise source control change set registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_source_control_change_set_registry_create(UmiSourceControlChangeSetRegistry **out_registry);
/**
 * Release or reset state held by source control change set registry so the same storage
 * can be reused safely.
 */
void umi_source_control_change_set_registry_destroy(UmiSourceControlChangeSetRegistry *registry);
/**
 * Provide the source control change set registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_source_control_change_set_registry_upsert(UmiSourceControlChangeSetRegistry *registry, const UmiSourceControlChangeSetSnapshot *item);
/**
 * Remove source control change set registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_source_control_change_set_registry_remove(UmiSourceControlChangeSetRegistry *registry, const char *id);
/**
 * Find source control change set registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_source_control_change_set_registry_find(const UmiSourceControlChangeSetRegistry *registry, const char *id, UmiSourceControlChangeSetSnapshot *out_item);
/**
 * Find source control change set registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_source_control_change_set_registry_at(const UmiSourceControlChangeSetRegistry *registry, size_t index, UmiSourceControlChangeSetSnapshot *out_item);
/**
 * Return the number of records represented by source control change set registry without
 * changing their state.
 */
size_t umi_source_control_change_set_registry_count(const UmiSourceControlChangeSetRegistry *registry);
/**
 * Provide the source control change set registry revision operation used by this module
 * and its client applications.
 */
uint64_t umi_source_control_change_set_registry_revision(const UmiSourceControlChangeSetRegistry *registry);
/**
 * Release or reset state held by source control change set registry so the same storage
 * can be reused safely.
 */
void umi_source_control_change_set_registry_clear(UmiSourceControlChangeSetRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
