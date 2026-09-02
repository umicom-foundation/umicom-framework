/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/scope.h
 *
 * PURPOSE:
 *   Define a DAP-friendly but adapter-neutral debugger record for native and future Umicom runtimes.
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
#ifndef UMICOM_DEBUG_SCOPE_H
#define UMICOM_DEBUG_SCOPE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_SCOPE_CAPACITY 2048U
#define UMI_DEBUG_SCOPE_API_VERSION 1U

/**
 * Represent the debug scope snapshot data shared with callers of this public contract.
 */
typedef struct UmiDebugScopeSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char frame_id[128];
    char name[256];
    uint64_t variables_reference;
    int expensive;
    int32_t order;
    uint64_t revision;
} UmiDebugScopeSnapshot;

/**
 * Represent the debug scope registry data shared with callers of this public contract.
 */
typedef struct UmiDebugScopeRegistry UmiDebugScopeRegistry;

/**
 * Initialise debug scope registry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_debug_scope_registry_create(UmiDebugScopeRegistry **out_registry);
/**
 * Release or reset state held by debug scope registry so the same storage can be reused
 * safely.
 */
void umi_debug_scope_registry_destroy(UmiDebugScopeRegistry *registry);
/**
 * Provide the debug scope registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_scope_registry_upsert(UmiDebugScopeRegistry *registry, const UmiDebugScopeSnapshot *item);
/**
 * Remove debug scope registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_debug_scope_registry_remove(UmiDebugScopeRegistry *registry, const char *id);
/**
 * Find debug scope registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_scope_registry_find(const UmiDebugScopeRegistry *registry, const char *id, UmiDebugScopeSnapshot *out_item);
/**
 * Find debug scope registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_scope_registry_at(const UmiDebugScopeRegistry *registry, size_t index, UmiDebugScopeSnapshot *out_item);
/**
 * Return the number of records represented by debug scope registry without changing their
 * state.
 */
size_t umi_debug_scope_registry_count(const UmiDebugScopeRegistry *registry);
/**
 * Provide the debug scope registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_debug_scope_registry_revision(const UmiDebugScopeRegistry *registry);
/**
 * Release or reset state held by debug scope registry so the same storage can be reused
 * safely.
 */
void umi_debug_scope_registry_clear(UmiDebugScopeRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
