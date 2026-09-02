/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/watch.h
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
#ifndef UMICOM_DEBUG_WATCH_H
#define UMICOM_DEBUG_WATCH_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_WATCH_CAPACITY 2048U
#define UMI_DEBUG_WATCH_API_VERSION 1U

/**
 * Represent the debug watch snapshot data shared with callers of this public contract.
 */
typedef struct UmiDebugWatchSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char session_id[128];
    char expression[1024];
    char value[1024];
    char type[256];
    int enabled;
    int valid;
    uint64_t revision;
} UmiDebugWatchSnapshot;

/**
 * Represent the debug watch registry data shared with callers of this public contract.
 */
typedef struct UmiDebugWatchRegistry UmiDebugWatchRegistry;

/**
 * Initialise debug watch registry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_debug_watch_registry_create(UmiDebugWatchRegistry **out_registry);
/**
 * Release or reset state held by debug watch registry so the same storage can be reused
 * safely.
 */
void umi_debug_watch_registry_destroy(UmiDebugWatchRegistry *registry);
/**
 * Provide the debug watch registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_watch_registry_upsert(UmiDebugWatchRegistry *registry, const UmiDebugWatchSnapshot *item);
/**
 * Remove debug watch registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_debug_watch_registry_remove(UmiDebugWatchRegistry *registry, const char *id);
/**
 * Find debug watch registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_watch_registry_find(const UmiDebugWatchRegistry *registry, const char *id, UmiDebugWatchSnapshot *out_item);
/**
 * Find debug watch registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_watch_registry_at(const UmiDebugWatchRegistry *registry, size_t index, UmiDebugWatchSnapshot *out_item);
/**
 * Return the number of records represented by debug watch registry without changing their
 * state.
 */
size_t umi_debug_watch_registry_count(const UmiDebugWatchRegistry *registry);
/**
 * Provide the debug watch registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_debug_watch_registry_revision(const UmiDebugWatchRegistry *registry);
/**
 * Release or reset state held by debug watch registry so the same storage can be reused
 * safely.
 */
void umi_debug_watch_registry_clear(UmiDebugWatchRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
