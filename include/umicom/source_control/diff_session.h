/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_control/diff_session.h
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
#ifndef UMICOM_SOURCE_CONTROL_DIFF_SESSION_H
#define UMICOM_SOURCE_CONTROL_DIFF_SESSION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SOURCE_CONTROL_DIFF_SESSION_CAPACITY 2048U
#define UMI_SOURCE_CONTROL_DIFF_SESSION_API_VERSION 1U

/**
 * Represent the source control diff session snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiSourceControlDiffSessionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char repository_id[128];
    char left_revision[128];
    char right_revision[128];
    char path[1024];
    size_t hunk_count;
    int binary;
    uint64_t revision;
} UmiSourceControlDiffSessionSnapshot;

/**
 * Represent the source control diff session registry data shared with callers of this
 * public contract.
 */
typedef struct UmiSourceControlDiffSessionRegistry UmiSourceControlDiffSessionRegistry;

/**
 * Initialise source control diff session registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_source_control_diff_session_registry_create(UmiSourceControlDiffSessionRegistry **out_registry);
/**
 * Release or reset state held by source control diff session registry so the same storage
 * can be reused safely.
 */
void umi_source_control_diff_session_registry_destroy(UmiSourceControlDiffSessionRegistry *registry);
/**
 * Provide the source control diff session registry upsert operation used by this module
 * and its client applications.
 */
UmiStatus umi_source_control_diff_session_registry_upsert(UmiSourceControlDiffSessionRegistry *registry, const UmiSourceControlDiffSessionSnapshot *item);
/**
 * Remove source control diff session registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_source_control_diff_session_registry_remove(UmiSourceControlDiffSessionRegistry *registry, const char *id);
/**
 * Find source control diff session registry while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_source_control_diff_session_registry_find(const UmiSourceControlDiffSessionRegistry *registry, const char *id, UmiSourceControlDiffSessionSnapshot *out_item);
/**
 * Find source control diff session registry while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_source_control_diff_session_registry_at(const UmiSourceControlDiffSessionRegistry *registry, size_t index, UmiSourceControlDiffSessionSnapshot *out_item);
/**
 * Return the number of records represented by source control diff session registry without
 * changing their state.
 */
size_t umi_source_control_diff_session_registry_count(const UmiSourceControlDiffSessionRegistry *registry);
/**
 * Provide the source control diff session registry revision operation used by this module
 * and its client applications.
 */
uint64_t umi_source_control_diff_session_registry_revision(const UmiSourceControlDiffSessionRegistry *registry);
/**
 * Release or reset state held by source control diff session registry so the same storage
 * can be reused safely.
 */
void umi_source_control_diff_session_registry_clear(UmiSourceControlDiffSessionRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
