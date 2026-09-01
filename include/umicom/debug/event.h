/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/event.h
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
#ifndef UMICOM_DEBUG_EVENT_H
#define UMICOM_DEBUG_EVENT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_EVENT_CAPACITY 2048U
#define UMI_DEBUG_EVENT_API_VERSION 1U

typedef struct UmiDebugEventSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char session_id[128];
    char kind[128];
    char detail[1024];
    uint64_t timestamp;
    int important;
    uint64_t revision;
} UmiDebugEventSnapshot;

typedef struct UmiDebugEventRegistry UmiDebugEventRegistry;

UmiStatus umi_debug_event_registry_create(UmiDebugEventRegistry **out_registry);
void umi_debug_event_registry_destroy(UmiDebugEventRegistry *registry);
UmiStatus umi_debug_event_registry_upsert(UmiDebugEventRegistry *registry, const UmiDebugEventSnapshot *item);
UmiStatus umi_debug_event_registry_remove(UmiDebugEventRegistry *registry, const char *id);
UmiStatus umi_debug_event_registry_find(const UmiDebugEventRegistry *registry, const char *id, UmiDebugEventSnapshot *out_item);
UmiStatus umi_debug_event_registry_at(const UmiDebugEventRegistry *registry, size_t index, UmiDebugEventSnapshot *out_item);
size_t umi_debug_event_registry_count(const UmiDebugEventRegistry *registry);
uint64_t umi_debug_event_registry_revision(const UmiDebugEventRegistry *registry);
void umi_debug_event_registry_clear(UmiDebugEventRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
