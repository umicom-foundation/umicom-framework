/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_control/tag.h
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
#ifndef UMICOM_SOURCE_CONTROL_TAG_H
#define UMICOM_SOURCE_CONTROL_TAG_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SOURCE_CONTROL_TAG_CAPACITY 2048U
#define UMI_SOURCE_CONTROL_TAG_API_VERSION 1U

/**
 * Represent the source control tag snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiSourceControlTagSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char repository_id[128];
    char name[256];
    char target[128];
    char message[512];
    int annotated;
    uint64_t revision;
} UmiSourceControlTagSnapshot;

/**
 * Represent the source control tag registry data shared with callers of this public
 * contract.
 */
typedef struct UmiSourceControlTagRegistry UmiSourceControlTagRegistry;

/**
 * Initialise source control tag registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_source_control_tag_registry_create(UmiSourceControlTagRegistry **out_registry);
/**
 * Release or reset state held by source control tag registry so the same storage can be
 * reused safely.
 */
void umi_source_control_tag_registry_destroy(UmiSourceControlTagRegistry *registry);
/**
 * Provide the source control tag registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_source_control_tag_registry_upsert(UmiSourceControlTagRegistry *registry, const UmiSourceControlTagSnapshot *item);
/**
 * Remove source control tag registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_source_control_tag_registry_remove(UmiSourceControlTagRegistry *registry, const char *id);
/**
 * Find source control tag registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_source_control_tag_registry_find(const UmiSourceControlTagRegistry *registry, const char *id, UmiSourceControlTagSnapshot *out_item);
/**
 * Find source control tag registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_source_control_tag_registry_at(const UmiSourceControlTagRegistry *registry, size_t index, UmiSourceControlTagSnapshot *out_item);
/**
 * Return the number of records represented by source control tag registry without changing
 * their state.
 */
size_t umi_source_control_tag_registry_count(const UmiSourceControlTagRegistry *registry);
/**
 * Provide the source control tag registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_source_control_tag_registry_revision(const UmiSourceControlTagRegistry *registry);
/**
 * Release or reset state held by source control tag registry so the same storage can be
 * reused safely.
 */
void umi_source_control_tag_registry_clear(UmiSourceControlTagRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
