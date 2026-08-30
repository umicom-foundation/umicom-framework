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

/* BEGINNER NOTE:
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

typedef struct UmiDebugScopeRegistry UmiDebugScopeRegistry;

UmiStatus umi_debug_scope_registry_create(UmiDebugScopeRegistry **out_registry);
void umi_debug_scope_registry_destroy(UmiDebugScopeRegistry *registry);
UmiStatus umi_debug_scope_registry_upsert(UmiDebugScopeRegistry *registry, const UmiDebugScopeSnapshot *item);
UmiStatus umi_debug_scope_registry_remove(UmiDebugScopeRegistry *registry, const char *id);
UmiStatus umi_debug_scope_registry_find(const UmiDebugScopeRegistry *registry, const char *id, UmiDebugScopeSnapshot *out_item);
UmiStatus umi_debug_scope_registry_at(const UmiDebugScopeRegistry *registry, size_t index, UmiDebugScopeSnapshot *out_item);
size_t umi_debug_scope_registry_count(const UmiDebugScopeRegistry *registry);
uint64_t umi_debug_scope_registry_revision(const UmiDebugScopeRegistry *registry);
void umi_debug_scope_registry_clear(UmiDebugScopeRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
