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

/* BEGINNER NOTE:
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

typedef struct UmiDebugWatchRegistry UmiDebugWatchRegistry;

UmiStatus umi_debug_watch_registry_create(UmiDebugWatchRegistry **out_registry);
void umi_debug_watch_registry_destroy(UmiDebugWatchRegistry *registry);
UmiStatus umi_debug_watch_registry_upsert(UmiDebugWatchRegistry *registry, const UmiDebugWatchSnapshot *item);
UmiStatus umi_debug_watch_registry_remove(UmiDebugWatchRegistry *registry, const char *id);
UmiStatus umi_debug_watch_registry_find(const UmiDebugWatchRegistry *registry, const char *id, UmiDebugWatchSnapshot *out_item);
UmiStatus umi_debug_watch_registry_at(const UmiDebugWatchRegistry *registry, size_t index, UmiDebugWatchSnapshot *out_item);
size_t umi_debug_watch_registry_count(const UmiDebugWatchRegistry *registry);
uint64_t umi_debug_watch_registry_revision(const UmiDebugWatchRegistry *registry);
void umi_debug_watch_registry_clear(UmiDebugWatchRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
